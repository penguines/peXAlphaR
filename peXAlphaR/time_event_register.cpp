#include "time_event_register.h"

timeEventRegister::timeEventRegister(){

}

timeEventRegister::~timeEventRegister(){
	this->m_args.clear();
	this->m_events.clear();
	this->m_regfuncs.clear();
}

void timeEventRegister::append(void(*reg_func)(timeEventList&), void* arg){
	this->m_regfuncs.emplace_back(reg_func);
	this->m_args.emplace_back(arg);
}

int timeEventRegister::registerEvents(){
	int cnt = 0;
	for (auto iter = this->m_regfuncs.begin(); iter != this->m_regfuncs.end(); iter++) {
		(*iter)(this->m_events);
		cnt++;
	}
	return cnt;
}

int timeEventRegister::executeEvents(const aTime& time){
	int cnt = 0;
	for (int i = 0; i < m_events.size(); i++) {
		if (m_events[i].trig(time, m_args[i]) > 0) {
			cnt++;
			PRINTLOG("Event %s has been triggered.\n", m_events[i].tag.name.c_str());
		}
	}
	return cnt;
}

size_t timeEventRegister::eventSize(){
	return m_events.size();
}

int timeEventRegister::generateHelp(std::string& help_str, int max_permission, int event_type){

	return 0;
}

timeEventList& timeEventRegister::getEvents(){
	return this->m_events;
}
