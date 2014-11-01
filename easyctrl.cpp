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
void Monitored<int>::format(Stream &stream) {
	stream.print(this->value);
}

template<>
void Monitored<int>::parse(const char *data) {
	this->value = atoi(data);
}

template<>
const char Monitored<int>::type_name[] PROGMEM = "int";
