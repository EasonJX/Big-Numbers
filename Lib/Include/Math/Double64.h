#pragma once

#include <Math.h>

int    getExpo10(   double x);
inline bool   isnan(double x) {
  return std::isnan(x);
}
inline bool   isinf(double x) {
  return std::isinf(x);
}
bool   isPInfinity( double x);
bool   isNInfinity( double x);

inline bool   isnan(float x) {
  return std::isnan(x);
}
inline bool   isinf(float  x) {
  return std::isinf(x);
}
bool   isPInfinity( float  x);
bool   isNInfinity( float  x);

#define FLT_NAN  std::numeric_limits<float>::quiet_NaN()
#define FLT_PINF std::numeric_limits<float>::infinity()
#define FLT_NINF (-FLT_PINF)
#define DBL_NAN  std::numeric_limits<double>::quiet_NaN()
#define DBL_PINF std::numeric_limits<double>::infinity()
#define DBL_NINF (-DBL_PINF)

inline double dsign(double x) {
  return (x < 0) ? -1 : (x > 0) ? 1 : 0;
}
inline double dmax(double x1, double x2) {
  return (x1 > x2) ? x1 : x2;
}
inline double dmin(double x1, double x2) {
  return (x1 < x2) ? x1 : x2;
}
inline int dmax(int x1, int x2) {
  return (x1 > x2) ? x1 : x2;
}
inline int dmin(int x1, int x2) {
  return (x1 < x2) ? x1 : x2;
}
inline UINT dmax(UINT x1, UINT x2) {
  return (x1 > x2) ? x1 : x2;
}
inline UINT dmin(UINT x1, UINT x2) {
  return (x1 < x2) ? x1 : x2;
}

double mypow(             double x, double y);
double root(              double x, double y);
double cot(               double x);
double acot(              double x);
// calculate both cos and sin. c:inout c, s:out
extern "C" {
  void   sincos(double &c, double &s);
  double exp10(double x);
};

inline double getDouble(float x) {
  return x;
}
inline float getFloat(float x) {
  return x;
}
inline int getInt(float x) {
  return (int)x;
}
inline UINT getUint(float x) {
  return (UINT)x;
}
inline long getLong(float x) {
  return getInt(x);
}
inline ULONG getUlong(float x) {
  return getUint(x);
}
inline INT64 getInt64(float x) {
  return (INT64)x;
}
inline UINT64 getUint64(float x) {
  return (UINT64)x;
}

inline double getDouble(double x) {
  return x;
}
inline float getFloat(double x) {
  return (float)x;
}
inline int getInt(double x) {
  return (int)x;
}
inline UINT getUint(double x) {
  return (UINT)x;
}
inline long getLong(double x) {
  return getInt(x);
}
inline ULONG getUlong(double x) {
  return getUint(x);
}
inline INT64 getInt64(double x) {
  return (INT64)x;
}
inline UINT64 getUint64(double x) {
  return (UINT64)x;
}

inline bool isFloat(float x) {
  return true;
}
inline bool isInt64(float x) {
  return x == getInt64(x);
}
inline bool isInt(float x) {
  return x == getInt(x);
}

inline bool isFloat(double x) {
  return x == (float)x;
}
inline bool isInt64(double x) {
  return x == getInt64(x);
}
inline bool isInt(double x) {
  return x == getInt(x);
}


inline UINT getSignificand(float x) {
  return (UINT)(((*((ULONG*)(&(x)))) & 0x7fffff) | 0x800000);
}

inline int getExpo2(float x) {
  return (int)((((*((ULONG*)(&(x)))) >> 23) & 0xff) - 0x7f);
}

inline BYTE getSign(float x) {
  return (((BYTE*)(&x))[3]) & 0x80;
}
inline UINT64 getSignificand(double x) {
  return (((*((UINT64*)(&(x)))) & 0xfffffffffffffui64) | 0x10000000000000ui64);
}

inline int getExpo2(double x) {
  return ((int)((((*((UINT64*)(&(x)))) >> 52) & 0x7ff) - 0x3ff));
}

inline BYTE getSign(double x) {
  return (((BYTE*)(&x))[7]) & 0x80;
}
