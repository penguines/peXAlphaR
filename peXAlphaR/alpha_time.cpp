#include "alpha_time.h"

void aTime::setTime(time_t time, const int& utc){
	a_time* tmp = gmtime(&time);
	tmCopy(tmp, &(this->m_time));
	this->m_utc = utc;
}

void aTime::setTime(const a_time& time, const int& utc){
	tmCopy(&time, &(this->m_time));
	this->m_utc = utc;
}

void aTime::setTime(const int& year, const int& month, const int& day, const int& wday, const int& hour, const int& min, const int& sec, const int& utc){
	this->setYear(year);
	this->setMonth(month);
	this->setMDay(day);
	this->setWDay(wday);
	this->setHour(hour);
	this->setMin(min);
	this->setSec(sec);
	this->setUTC(utc);
}

void aTime::setADYear(const int& yr){
	this->m_time.tm_year = yr - 1900;
}

int aTime::ADYear() const{
	return (this->m_time.tm_year + 1900);
}

void aTime::setYear(const int& yr){
	this->m_time.tm_year = yr + 70;
}

int aTime::year() const{
	return (this->m_time.tm_year - 70);
}

void aTime::setMonth(const int& mon){
	this->m_time.tm_mon = mon - 1;
}

int aTime::month() const{
	return (this->m_time.tm_mon + 1);
}

void aTime::setWDay(const int& wday){
	this->m_time.tm_wday = (wday % 7);
}

int aTime::wday() const{
	if (this->m_time.tm_wday == 0) {
		return WDAY_SUNDAY;
	}
	else {
		return this->m_time.tm_wday;
	}
}

void aTime::setMDay(const int& mday){
	this->m_time.tm_mday = mday;
}

int aTime::mday() const{
	return this->m_time.tm_mday;
}

void aTime::setYDay(const int& yday){
	this->m_time.tm_yday = yday;
}

int aTime::yday() const{
	return this->m_time.tm_yday;
}

void aTime::setIsdst(const int& isdst){
	this->m_time.tm_isdst = isdst;
}

int aTime::isdst() const{
	return this->m_time.tm_isdst;
}

void aTime::setSec(const int& sec){
	this->m_time.tm_sec = sec;
}

int aTime::sec() const{
	return this->m_time.tm_sec;
}

void aTime::setMin(const int& min){
	this->m_time.tm_min = min;
}

int aTime::mint() const{
	return this->m_time.tm_min;
}

void aTime::setHour(const int& hour){
	this->m_time.tm_hour = hour;
}

int aTime::hour() const{
	return this->m_time.tm_hour;
}

a_time& aTime::tm_time(){
	return this->m_time;
}

const a_time& aTime::tm_time() const{
	return static_cast<const a_time&>(this->m_time);
}

time_t aTime::_gmtime64() const{
	a_time tm_tmp;
	tmCopy(this->m_time, tm_tmp);
	return _mkgmtime(&tm_tmp);
}

time_t aTime::_localtime64() const{
	a_time tm_tmp;
	tmCopy(this->m_time, tm_tmp);
	return mktime(&tm_tmp);
}

time_t aTime::time64() const{
	return (this->_gmtime64() - this->m_utc * 3600LL);
}

void aTime::setUTC(int utc){
	this->m_utc = utc;
}

int aTime::utc() const{
	return static_cast<const int&>(this->m_utc);
}

void aTime::printTime(std::string& str){
	char buf[32];
	sprintf(buf, "%d/%02d/%02d %02d:%02d:%02d", this->ADYear(), this->month(), this->mday(), this->hour(), this->mint(), this->sec());
	str.assign(buf);
}

aTime& aTime::copy(const aTime& src){
	if (this == &src) {
		return *this;
	}
	tmCopy(&src.tm_time(), &(this->m_time));
	this->m_utc = src.utc();
	return *this;
}

aTime& aTime::operator=(const aTime& a_tm){
	return this->copy(a_tm);
}

aTime& aTime::operator+=(time_t secs){
	time_t cur_tmp = _mkgmtime(&(this->m_time));
	cur_tmp += secs;
	this->m_time = *gmtime(&cur_tmp);
	return *this;
}

aTime& aTime::operator+=(const aTime& a_tm){
	return this->operator+=(a_tm._gmtime64());
}

bool aTime::operator==(const aTime& other){
	return (this->m_time == other.tm_time() &&\
			this->m_utc  == other.utc());
}

bool aTime::operator>(const aTime& other){
	return (this->time64() > other.time64());
}

bool aTime::operator<(const aTime& other){
	return (this->time64() < other.time64());
}

bool aTime::operator>=(const aTime& other){
	return (this->time64() >= other.time64());
}

bool aTime::operator<=(const aTime& other){
	return (this->time64() <= other.time64());
}

aTime::aTime(){
	this->m_time.tm_sec = 0;
	this->m_time.tm_min = 0;
	this->m_time.tm_hour = 0;
	this->m_time.tm_mday = 1;
	this->m_time.tm_wday = 0;
	this->m_time.tm_yday = 0;
	this->m_time.tm_mon = 0;
	this->m_time.tm_year = 70;
	this->m_time.tm_isdst = 0;
	this->m_utc = 0;
}

aTime::aTime(time_t time, const int& utc){
	this->setTime(time, utc);
}

aTime::aTime(const a_time& time, const int& utc){
	this->setTime(time, utc);
}

aTime::aTime(const int& year, const int& month, const int& day, const int& wday, const int& hour, const int& min, const int& sec, const int& utc){
	this->setTime(year, month, day, wday, hour, min, sec, utc);
}

aTime::~aTime(){

}

time_t operator-(const aTime& _left, const aTime& _right){
	return (_left.time64() - _right.time64());
}

bool operator==(const a_time& _tm1, const a_time& _tm2){
	return (_tm1.tm_year  == _tm2.tm_year &&\
			_tm1.tm_mon	  == _tm2.tm_mon &&\
			_tm1.tm_mday  == _tm2.tm_mday &&\
			_tm1.tm_hour  == _tm2.tm_hour &&\
			_tm1.tm_min   == _tm2.tm_min &&\
			_tm1.tm_sec   == _tm2.tm_sec &&\
			_tm1.tm_wday  == _tm2.tm_wday &&\
			_tm1.tm_yday  == _tm2.tm_yday &&\
			_tm1.tm_isdst == _tm2.tm_isdst);
}

void tmCopy(const a_time* src, a_time* dst){
	dst->tm_sec = src->tm_sec;
	dst->tm_min = src->tm_min;
	dst->tm_hour = src->tm_hour;
	dst->tm_mday = src->tm_mday;
	dst->tm_wday = src->tm_wday;
	dst->tm_yday = src->tm_yday;
	dst->tm_mon = src->tm_mon;
	dst->tm_year = src->tm_year;
	dst->tm_isdst = src->tm_isdst;
}

void tmCopy(const a_time& src, a_time& dst){
	tmCopy(&src, &dst);
}