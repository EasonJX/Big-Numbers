#pragma once

#include "MyUtil.h"
#include "Packer.h"
#include <time.h>

typedef enum {
  MONDAY
 ,TUESDAY
 ,WEDNESDAY
 ,THURSDAY
 ,FRIDAY
 ,SATURDAY
 ,SUNDAY
} WeekDay;

typedef enum {
  TMILLISECOND
 ,TSECOND
 ,TMINUTE
 ,THOUR
 ,TDAYOFMONTH
 ,TDAYOFYEAR
 ,TWEEK
 ,TMONTH
 ,TYEAR
} TimeComponent;

extern const String ddMMyy;
extern const String ddMMyyyy;
extern const String ddMM;
extern const String MMyyyy;
extern const String MMyy;
extern const String yyyy;
extern const String yy;
extern const String hhmm;
extern const String hhmmss;
extern const String hhmmssSSS;
extern const String ddMMyyyyhhmm;
extern const String ddMMyyyyhhmmss;
extern const String ddMMyyyyhhmmssSSS;

class Date {
private:
  int m_factor;
  void init(const TCHAR *src);
  Date(int factor);
  static void checkFactor(INT64 factor);
  static int getDATE0Factor();
public:
  static const short s_ydaynl[];
  static const short s_ydayl[];
  static const TCHAR *s_daynames[];
  // Calculate factor from day,month,year
  static int  getFactor(int day, int month, int year);
  static int  getFactor(time_t tt);

  // Is year a leapYear
  static inline bool isLeapYear(int year) {
    return ((year%4==0) && (year%100 != 0)) || (year%400 == 0);
  }

  // Number of days of month in the given year
  static int  getDaysInMonth(int year, int month);
  // Number of days in year
  static inline int  getDaysInYear(int year) {
    return isLeapYear(year) ? 366 : 365;
  }
  // Number of weeks in year
  static int  getWeeksInYear(int year);
  // Get the date of easter in the given year in the gregorian calender
  static Date getEaster(     int year);
  static bool dateValidate(  int day, int month, int year);
  // return (year<currentyear%100+30)?(year+2000) : (year<100)?(year+1900):year;
  static int  adjustYear100( int year);

  Date();
  Date(int day, int month, int year);
  inline explicit Date(const TCHAR *src) {
    init(src);
  }
  inline explicit Date(const String &src) {
    init(src.cstr());
  }
  inline explicit Date(time_t tt) {
    m_factor = getFactor(tt);
  }
  explicit Date(double d);                              // ie type DATE
  inline double getDATE() const {
    return m_factor - getDATE0Factor();
  }
  Date     operator+(int days) const;
  Date     operator-(int days) const;
  // Add days to this
  Date    &operator+=(int days);
  // Subtract days from this
  Date    &operator-=(int days);
  // Prefix form. Add 1 day
  Date    &operator++() { return operator+=(1); }
  // Prefix form. Subtract 1 day
  Date    &operator--() { return operator-=(1); }
  // Postfix form. Add 1 day
  Date     operator++(int);
  // Postfix form. Subtract 1 day
  Date     operator--(int);
  // Distance in days
  inline int operator-(const Date &r) const {
    return m_factor - r.m_factor;
  }

  Date &add(TimeComponent c, int count);
  int   get(TimeComponent c);
  Date &set(TimeComponent c, int value);
  static int diff(const Date &from, const Date &to, TimeComponent c = TDAYOFMONTH);
  friend inline int dateCmp(const Date &l, const Date &r) {
    return l.m_factor - r.m_factor;
  }
  inline bool operator==(const Date &r) const {
    return m_factor == r.m_factor;
  }
  inline bool operator!=(const Date &r) const {
    return m_factor != r.m_factor;
  }
  inline bool operator< (const Date &r) const {
    return m_factor < r.m_factor;
  }
  inline bool operator<=(const Date &r) const {
    return m_factor <= r.m_factor;
  }
  inline bool operator> (const Date &r) const {
    return m_factor > r.m_factor;
  }
  inline bool operator>=(const Date &r) const {
    return m_factor >= r.m_factor;
  }
  void getDMY(int &day, int &month, int &year) const;
  int getYear()        const;
  int getMonth()       const;
  int getDayOfMonth()  const;
  int getDayOfYear()   const;
  inline WeekDay getWeekDay() const {
    return (WeekDay)((m_factor - 2) % 7);
  }
  int getWeek()        const;
  inline bool isLeapYear()    const {
    return isLeapYear(getYear());
  }

  inline int getFactor() const {
    return m_factor;
  }

  inline ULONG hashCode() const {
    return m_factor;
  }

  static const Date &getMinDate();
  static const Date &getMaxDate();

  static inline INT64 getMinFactor() {
    return getMinDate().m_factor;
  }

  static inline INT64 getMaxFactor() {
    return getMaxDate().m_factor;
  }

  TCHAR *tostr(TCHAR *dst, const String &format = ddMMyyyy) const;
  inline String toString(const String &format = ddMMyyyy) const {
    TCHAR tmp[1024];
    return tostr(tmp, format);
  }
  friend class Timestamp;
  friend Packer &operator<<(Packer &p, const Date &d);
  friend Packer &operator>>(Packer &p,       Date &d);
};

class Time {
private:
  int m_factor;
  // msec:milliseconds from midnight
  inline explicit Time(double msec) {
    m_factor = (int)msec;
  }
  void init(const TCHAR *src);
  static int getFactor(time_t tt);
  inline Time(int factor) {
    checkFactor(factor);
    m_factor = factor;
  }
  static void throwInvalidFactor(INT64 factor);
  static inline void checkFactor(INT64 factor) {
    if((UINT64)factor >= (UINT)getMaxFactor()) throwInvalidFactor(factor);
  }

public:

  static inline int getFactor(int hour, int minute, int second, int millisecond) {
    return ((hour * 60 + minute) * 60 + second) * 1000 + millisecond;
  }
  static bool timeValidate(int hour, int minute, int second, int millisecond=0);

  Time();
  Time(int hour, int minute, int second, int millisecond = 0);
  inline explicit Time(const String &src) {
    init(src.cstr());
  }
  inline explicit Time(const TCHAR *src) {
    init(src);
  }
  inline explicit Time(time_t t) {
    m_factor = getFactor(t);
  }
  Time  operator+( int seconds) const;
  inline Time  operator-(int seconds) const {
    return *this + (-seconds);
  }
  inline Time &operator+=(int seconds) {
    return *this = *this + seconds;
  }
  inline Time &operator-=(int seconds) {
    return *this = *this - seconds;
  }
  // Prefix form. Add 1 second
  Time &operator++() { return operator+=(1); }
  // Prefix form. Subtract 1 second
  Time &operator--() { return operator-=(1); }
  // Postfix form. Add 1 second
  Time  operator++(int);
  // Postfix form. Subtract 1 second
  Time  operator--(int);
  // Distance in seconds
  inline int   operator-(const Time &r) const {
    return (m_factor - r.m_factor) / 1000;
  }
  Time &add(TimeComponent c, int count);
  int   get(TimeComponent c);
  Time &set(TimeComponent c, int value);
  friend inline int timeCmp(const Time &l, const Time &r) {
    return l.m_factor - r.m_factor;
  }
  inline bool operator==(const Time &r) const {
    return m_factor == r.m_factor;
  }
  inline bool operator!=(const Time &r) const {
    return m_factor != r.m_factor;
  }
  inline bool operator< (const Time &r) const {
    return m_factor < r.m_factor;
  }
  inline bool operator<=(const Time &r) const {
    return m_factor <= r.m_factor;
  }
  inline bool operator> (const Time &r) const {
    return m_factor > r.m_factor;
  }
  inline bool operator>=(const Time &r) const {
    return m_factor >= r.m_factor;
  }
  void getHMS(int &hour, int &minute, int &second, int &millisecond) const;
  inline int getHour() const {
    return m_factor / (3600000);
  }
  inline int getMinute() const {
    return (m_factor / 60000) % 60;
  }
  inline int getSecond() const {
    return (m_factor / 1000) % 60;
  }
  inline int getMilliSecond() const {
    return m_factor % 1000;
  }
  inline ULONG hashCode() const {
    return m_factor;
  }

  static const Time &getMinTime();
  static const Time &getMaxTime();

  static inline int getMinFactor() {
    return 0;
  }

  static inline int getMaxFactor() {
    return getMaxTime().m_factor + 1;
  }

  TCHAR *tostr(TCHAR *dst, const String &format = hhmm) const;
  inline String toString(const String &format = hhmm) const {
    TCHAR tmp[1024];
    return tostr(tmp, format);
  }
  friend class Timestamp;
  friend Packer &operator<<(Packer &p, const Time &t);
  friend Packer &operator>>(Packer &p,       Time &t);
};

class Timestamp {
private:
  INT64 m_factor;
  void init(const TCHAR *src);
  static void checkFactor(INT64 factor);
public:
  static inline INT64 getFactor(const Date &d, const Time &t) {
    return (__int64)d.m_factor * Time::getMaxFactor() + t.m_factor;
  }

  Timestamp();
  inline Timestamp(int day, int month, int year, int hour, int minute, int second = 0, int millisecond = 0) {
    m_factor = getFactor(Date(day, month, year), Time(hour, minute, second, millisecond));
  }
  inline Timestamp(const Date &d, const Time &t) {
    m_factor = getFactor(d, t);
  }
  inline explicit Timestamp(const String &src) {
    init(src.cstr());
  }
  inline explicit Timestamp(const TCHAR *src) {
    init(src);
  }
  inline Timestamp(const Date &d) {
    m_factor = (INT64)d.m_factor * Time::getMaxFactor();
  }
  Timestamp(const SYSTEMTIME &st);
  inline explicit Timestamp(time_t t) {
    m_factor = getFactor(Date(t), Time(t));
  }
  explicit Timestamp(double d); // ie type DATE
  double getDATE() const;
  // Add count days
  Timestamp  operator+( int count) const;
  // Subtract count days
  Timestamp  operator-( int count) const;
  // Add count days
  inline Timestamp &operator+=(int count) {
    return *this = *this + count;
  }
  // Subtract count days
  inline Timestamp &operator-=(int count) {
    return *this = *this - count;
  }
  // Prefix form. Add 1 day
  inline Timestamp &operator++() {
    return operator+=(1);
  }
  // Prefix form. Subtract 1 day
  inline Timestamp &operator--() {
    return operator-=(1);
  }
  // Postfix form. Add 1 day
  Timestamp  operator++(int);
  // Postfix form. Subtract 1 day
  Timestamp  operator--(int);
  // Difference in days
  inline double operator-(const Timestamp &r) const {
    return ((double)m_factor - r.m_factor) / Time::getMaxFactor();
  }
  friend double diff(const Timestamp &from, const Timestamp &to, TimeComponent c = TDAYOFMONTH);
  Timestamp &add(TimeComponent c, int count);
  int        get(TimeComponent c);
  Timestamp &set(TimeComponent c, int value);
  friend inline int timestampCmp(const Timestamp &l, const Timestamp &r) {
    return sign(l.m_factor - r.m_factor);
  }
  inline bool operator==(const Timestamp &r) const {
    return m_factor == r.m_factor;
  }
  inline bool operator!=(const Timestamp &r) const {
    return m_factor != r.m_factor;
  }
  inline bool operator< (const Timestamp &r) const {
    return m_factor < r.m_factor;
  }
  inline bool operator<=(const Timestamp &r) const {
    return m_factor <= r.m_factor;
  }
  inline bool operator> (const Timestamp &r) const {
    return m_factor > r.m_factor;
  }
  inline bool operator>=(const Timestamp &r) const {
    return m_factor >= r.m_factor;
  }
  inline int getYear() const {
    return getDate().getYear();
  }
  inline int getMonth() const {
    return getDate().getMonth();
  }
  inline int getWeek() const {
    return getDate().getWeek();
  }
  inline int getDayOfMonth()  const {
    return getDate().getDayOfMonth();
  }
  inline int getHour() const {
    return getTime().getHour();
  }
  inline int getDayOfYear() const {
    return getDate().getDayOfYear();
  }
  inline int getMinute() const {
    return getTime().getMinute();
  }
  inline int getSecond() const {
    return getTime().getSecond();
  }
  inline int getMilliSecond() const {
    return getTime().getMilliSecond();
  }
  inline WeekDay getWeekDay() const {
    return getDate().getWeekDay();
  }
  inline void getDMY(int &day, int &month, int &year) const {
    getDate().getDMY(day, month, year);
  }
  inline void getHMS(int &hour, int &minute, int &second, int &millisecond) const {
    getTime().getHMS(hour, minute, second, millisecond);
  }
  inline Date getDate() const {
    return Date((int)(m_factor/Time::getMaxFactor()));
  }
  inline Time getTime() const {
    return Time((int)(m_factor % Time::getMaxFactor()));
  }
  time_t gettime_t() const;

  operator SYSTEMTIME() const;

  inline ULONG hashCode() const {
    return uint64Hash(m_factor);
  }

  static const Timestamp &getMinTimestamp();
  static const Timestamp &getMaxTimestamp();

  static inline INT64 getMinFactor() {
    return getMinTimestamp().m_factor;
  }

  static inline INT64 getMaxFactor() {
    return getMaxTimestamp().m_factor;
  }

  TCHAR *tostr(TCHAR *dst, const String &DateFormat = ddMMyyyyhhmm) const;
  inline String toString(const String &format = ddMMyyyyhhmm) const {
    TCHAR tmp[1024];
    return tostr(tmp, format);
  }
  static String cctime(); // cctime(getSystemTime());
  static String cctime(time_t tt);
  static time_t getSystemTime();
  friend Packer &operator<<(Packer &p, const Timestamp &ts);
  friend Packer &operator>>(Packer &p,       Timestamp &ts);
};
