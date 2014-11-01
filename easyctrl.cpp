#include "easyctrl.h"

Easyctrl_Class Easyctrl;

void MonitoredBase::init(const char *name, bool writeable) {
	this->name = name;
	this->writeable = writeable;

	// Add it to the linked list of known monitored variables
    this->next = Easyctrl.first;
    Easyctrl.first = this;
}

void Easyctrl_Class::writeManifest()  {
    stream->print(F(".fields "));
    stream->println(this->name);
    for(MonitoredBase *i = this->first; i != NULL; i = i->next) {
        i->describe(*this->stream);
    }
    stream->println(F(".endfields"));
}

template<>
void Monitored<int>::parse(const char *data) {
    this->value = strtol(data, NULL, 0);
}

template<>
void Monitored<int>::printTypeName(Stream &stream) {
	stream.print(F("int"));
}

template<>
void Monitored<unsigned int>::parse(const char *data) {
    this->value = strtoul(data, NULL, 0);
}

template<>
void Monitored<unsigned int>::printTypeName(Stream &stream) {
	stream.print(F("uint"));
}

template<>
void Monitored<long>::parse(const char *data) {
    this->value = strtol(data, NULL, 0);
}

template<>
void Monitored<long>::printTypeName(Stream &stream) {
	stream.print(F("long"));
}

template<>
void Monitored<unsigned long>::parse(const char *data) {
    this->value = strtoul(data, NULL, 0);
}

template<>
void Monitored<unsigned long>::printTypeName(Stream &stream) {
	stream.print(F("ulong"));
}

template<>
void Monitored<float>::parse(const char *data) {
    this->value = strtod(data, NULL);
}

template<>
void Monitored<float>::printTypeName(Stream &stream) {
	stream.print(F("float"));
}

template<>
void Monitored<double>::parse(const char *data) {
    this->value = strtod(data, NULL);
}

template<>
void Monitored<double>::printTypeName(Stream &stream) {
	stream.print(F("double"));
}
