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
const char Monitored<int>::type_name[] PROGMEM = "int";

template<>
const char Monitored<unsigned int>::type_name[] PROGMEM = "uint";

template<>
const char Monitored<long>::type_name[] PROGMEM = "long";

template<>
const char Monitored<unsigned long>::type_name[] PROGMEM = "ulong";

template<>
const char Monitored<float>::type_name[] PROGMEM = "float";

template<>
const char Monitored<double>::type_name[] PROGMEM = "double";
