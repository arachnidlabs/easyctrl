#include <string.h>
#include <avr/pgmspace.h>
#include "Stream.h"

#ifndef EASYCTRL_H
#define EASYCTRL_H

#define LINE_BUFFER_SIZE 64

class MonitoredBase {
    friend class Easyctrl_Class;
protected:
    const char *name;
    MonitoredBase *next;
    bool dirty;
    bool writeable;

    void describe(Stream &stream) {
        stream.print(F(".field "));
        stream.print(this->name);
        stream.print(F(" "));
        this->printTypeName(stream);
        if(this->writeable) {
            stream.println(F(" rw"));
        } else {
            stream.println(F(" ro"));
        }
    }

    void init(const char *name, bool writeable);
    virtual void parse(const char *data) = 0;
    virtual void format(Stream &stream) = 0;
    virtual void printTypeName(Stream &stream) = 0;

    void read(const char *data) {
        this->parse(data);
        this->dirty = false;
    }

    void write(Stream &stream) {
        if(this->dirty) {
            stream.print(F(".value "));
            stream.print(this->name);
            stream.print(F(" "));
            this->format(stream);
            stream.println();
            this->dirty = false;
        }
    }

public:
    void changed() {
        this->dirty = true;
    }
};

class Easyctrl_Class {
    friend class MonitoredBase;
private:
    const char *name;
    Stream *stream;
    MonitoredBase *first;
    char line_buffer[LINE_BUFFER_SIZE];
    int buffer_idx;

    void writeManifest();

    MonitoredBase *findVariable(const char *name) {
        for(MonitoredBase *i = this->first; i != NULL; i = i->next) {
            if(strcmp(name, i->name) == 0) {
                return i;
            }
        }
        return NULL;
    }

    void handleCommand(char *data) {
        char *cmd = strsep(&data, " ");
        if(strcasecmp_P(cmd, PSTR(".set")) == 0) {
            char *name = strsep(&data, " ");
            MonitoredBase *variable = findVariable(name);
            if(variable != NULL) {
                if(!variable->writeable) {
                    this->stream->println(F(".err Read-only variable"));
                } else {
                    variable->read(data);
                }
            } else {
                this->stream->println(F(".err Unknown variable"));
            }
        } else {
            this->stream->println(F(".err Unknown command"));
        }
    }

    void readCommands() {
        int c;
        while((c = this->stream->read()) >= 0) {
            if(buffer_idx < LINE_BUFFER_SIZE) {
                line_buffer[buffer_idx++] = c;
            }

            if(c == '\n') {
                if(buffer_idx < LINE_BUFFER_SIZE) {
                    line_buffer[buffer_idx] = '\0';
                    handleCommand(line_buffer);
                }
                buffer_idx = 0;
            }
        }
    }

    void writeChanges() {
        for(MonitoredBase *i = this->first; i != NULL; i = i->next) {
            i->write(*this->stream);
        }
    }
public:
    Easyctrl_Class() : stream(NULL), first(NULL), buffer_idx(0) { }

    void begin(const char *name, Stream &stream) {
        this->name = name;
        this->stream = &stream;
        this->writeManifest();
    }

    void update() {
        this->readCommands();
        this->writeChanges();
    }
};

extern Easyctrl_Class Easyctrl;

template<class T>
class Monitored : public MonitoredBase {
private:
    T value;
protected:
    virtual void format(Stream &stream);
    virtual void parse(const char *data);
    virtual void printTypeName(Stream &stream);
public:
    Monitored(const char *name, T value) {
        this->value = value;
        this->dirty = true;
        init(name, true);
    }

    Monitored(const char *name, T value, bool writeable) {
        this->value = value;
        this->dirty = true;
        init(name, writeable);
    }

    Monitored(const char *name) {
        this->dirty = false;
        init(name, true);
    }

    Monitored<T>& operator=(T value) {
        this->dirty = (this->value != value);
        this->value = value;
        return *this;
    }

    operator T() {
        return this->value;
    }

    const char *getName() {
        return this->name;
    }
};

template<int size>
class MonitoredBuffer : public MonitoredBase {
private:
    char buf[size];
protected:
    virtual void format(Stream &stream) {
        for(char *c = buf; *c != '\0' && c - buf < size; c++) {
            switch(*c) {
            case '\n':
                stream.print(F("\\n"));
                break;
            case '\r':
                stream.print(F("\\r"));
                break;
            case '\\':
                stream.print(F("\\\\"));
                break;
            default:
                if(*c < 0) {
                    stream.print(F("\\x"));
                    stream.print((unsigned char)*c, HEX);
                } else {
                    stream.print(*c);
                }
                break;
            }
        }
    }

    virtual void parse(const char *data) {
        int idx = 0;
        for(const char *c = data; *c != '\0' && *c != '\n' && c - data < size - 1; c++) {
            if(*c == '\\') {
                c++;
                switch(*c) {
                case 'n':
                    buf[idx++] = '\n';
                    break;
                case 'r':
                    buf[idx++] = '\r';
                    break;
                case '\\':
                    buf[idx++] = '\\';
                    break;
                case 'x':
                    buf[idx++] = strtol((const char[]){c[1], c[2], '\0'}, NULL, 16);
                    c += 2;
                    break;
                }
            } else {
                buf[idx++] = *c;
            }
        }
        buf[idx] = '\0';
    }

    virtual void printTypeName(Stream &stream) {
        stream.print(F("char["));
        stream.print(size);
        stream.print(F("]"));
    }

public:
    MonitoredBuffer(const char *name) {
        this->dirty = false;
        init(name, true);
    }

    MonitoredBuffer(const char *name, bool writeable) {
        this->dirty = false;
        init(name, writeable);
    }

    MonitoredBuffer& operator=(const MonitoredBuffer &value) {
        strncpy(this->buf, value.buf, size);
        this->dirty = true;
    }

    MonitoredBuffer& operator=(const char *value) {
        strncpy(this->buf, value, size);
        this->dirty = true;
    }

    MonitoredBuffer& operator=(const __FlashStringHelper *value) {
        strncpy_P(this->buf, reinterpret_cast<PGM_P>(value), size);
        this->dirty = true;
        return *this;
    }

    operator char*() {
        return this->buf;
    }

    char operator[](int i) const {
        return this->buf[i];
    }

    char& operator [](int i) {
        return this->buf[i];
    }
};

// Formatter for any type supported by stream.print
template<class T>
void Monitored<T>::format(Stream &stream) {
    stream.print(this->value);
}

#endif
