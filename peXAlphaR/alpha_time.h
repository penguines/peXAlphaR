#ifndef ALPHA_TIME_H
#define ALPHA_TIME_H

#include <time.h>
#include <cstdint>

typedef struct tm a_time;

enum aTimeMon{ MONTH_JAN = 1, MONTH_FEB = 2, MONTH_MAR = 3, MONTH_APR = 4, MONTH_MAY = 5, MONTH_JUN = 6,
				MONTH_JUL = 7, MONTH_AUG = 8, MONTH_SEPT = 9, MONTH_OCT = 10, MONTH_NOV = 11, MONTH_DEC = 12 };
enum aTimeWDay{ WDAY_MONDAY = 1, WDAY_TUESDAY = 2, WDAY_WEDNESDAY = 3, WDAY_THRUSDAY = 4, WDAY_FRIDAY = 5, 
				WDAY_SATURDAY = 6, WDAY_SUNDAY = 7};

//Time as local time
class aTime {
public:
	void setTime(time_t time, const int& utc = 0);
	void setTime(const a_time& time, const int& utc = 0);
	void setTime(
		const int& year,
		const int& month,
		const int& day,
		const int& wday,
		const int& hour,
		const int& min,
		const int& sec,
		const int& utc = 0);

	//A.D. Year, not smaller than 1970.
	void setADYear(const int& yr);
	//A.D. Year.
	int ADYear() const;

	//Years since 1970.
	void setYear(const int& yr);
	//Years since 1970.
	int year() const;

	//Month begin from 1(Jan) to 12(Dec).
	void setMonth(const int& mon);
	//Month begin from 1(Jan) to 12(Dec).
	int month() const;

	//Day of week begin from 1(Monday) to 7(Sunday).
	void setWDay(const int& wday);
	//Day of week begin from 1(Monday) to 7(Sunday).
	int wday() const;

	//Date in month, 1~31.
	void setMDay(const int& mday);
	//Date in month, 1~31.
	int mday() const;

	//Days in a year since Jan.1st(0~365).
	void setYDay(const int& yday);
	//Days in a year since Jan.1st(0~365).
	int yday() const;

	void setIsdst(const int& isdst);
	int isdst() const;

	void setSec(const int& sec);
	int sec() const;

	void setMin(const int& min);
	int mint() const;

	void setHour(const int& hour);
	int hour() const;

	//Time as struct tm(UTC time)
	a_time& tm_time();
	//Time as struct tm(UTC time)
	const a_time& tm_time() const;
	//time_t(UTC time)
	time_t _gmtime64() const;
	//time_t(Local time)
	time_t _localtime64() const;
	//time_t(Calculate by gmtime & utc)
	time_t time64() const;

	//Set local time zone.
	void setUTC(int utc);
	//Local time zone.
	int utc() const;

	
	aTime& copy(const aTime& src);
	aTime& operator=(const aTime& a_tm);
	aTime& operator+=(time_t secs);
	//ignore time zone(consider utc = 0)
	aTime& operator+=(const aTime& a_tm);

	bool operator==(const aTime& other);
	bool operator>(const aTime& other);
	bool operator<(const aTime& other);
	bool operator>=(const aTime& other);
	bool operator<=(const aTime& other);

	aTime();
	aTime(time_t time, const int& utc = 0);
	aTime(const a_time& time, const int& utc = 0);
	aTime(
		const int& year,
		const int& month,
		const int& day,
		const int& wday,
		const int& hour,
		const int& min,
		const int& sec,
		const int& utc = 0);

	~aTime();
private:
	a_time m_time;
	int m_utc;
};

time_t operator-(const aTime& _left, const aTime& _right);
bool operator==(const a_time& _tm1, const a_time& _tm2);

void tmCopy(const a_time* src, a_time* dst);
void tmCopy(const a_time& src, a_time& dst);

#endif