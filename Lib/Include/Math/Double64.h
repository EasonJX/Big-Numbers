#pragma once

#include <Math.h>

int    getExpo10(         double x);
bool   isNan(             double x);
bool   isPInfinity(       double x);
bool   isNInfinity(       double x);
bool   isInfinity(        double x);

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

double mypow(             double x, double y);
double root(              double x, double y);
double cot(               double x);
double acot(              double x);
// calculate both cos and sin. c:inout c, s:out
extern "C" {
  void   sincos(double &c, double &s);
  double exp10(double x);
};

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

inline bool isFloat(double x) {
  return x == (float)x;
}
inline bool isInt64(double x) {
  return x == getInt64(x);
}
inline bool isInt(double x) {
  return x == getInt(x);
}
