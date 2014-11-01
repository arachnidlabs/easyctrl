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

    void init(const char *name, bool writeable);
    virtual void parse(const char *data) = 0;
    virtual void format(Stream &stream) = 0;
    virtual void describe(Stream &stream) = 0;

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
    static const char type_name[] PROGMEM;
protected:
    virtual void format(Stream &stream);
    virtual void parse(const char *data);

    virtual void describe(Stream &stream) {
        stream.print(F(".field "));
        stream.print(this->name);
        stream.print(F(" "));
        stream.print(reinterpret_cast<const __FlashStringHelper *>(this->type_name));
        if(this->writeable) {
            stream.println(F(" rw"));
        } else {
            stream.println(F(" ro"));
        }
    }
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
        this->value = value;
        this->dirty = true;
    }

    operator T() {
        return this->value;
    }

    const char *getName() {
        return this->name;
    }
};

#endif
