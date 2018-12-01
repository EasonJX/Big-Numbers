#pragma once

#include "MyUtil.h"
#include <Math.h>

inline char   getChar(  float  v) { return (char  )v;         }
inline char   getChar(  double v) { return (char  )v;         }
inline char   getUchar( float  v) { return (UCHAR )v;         }
inline char   getUchar( double v) { return (UCHAR )v;         }
inline int    getShort( float  v) { return (short )v;         }
inline int    getShort( double v) { return (short )v;         }
inline UINT   getUshort(float  v) { return (USHORT)v;         }
inline UINT   getUshort(double v) { return (USHORT)v;         }
inline int    getInt(   float  v) { return (int   )v;         }
inline int    getInt(   double v) { return (int   )v;         }
inline UINT   getUint(  float  v) { return (UINT  )v;         }
inline UINT   getUint(  double v) { return (UINT  )v;         }
inline long   getLong(  float  v) { return getInt( v);        }
inline long   getLong(  double v) { return getInt( v);        }
inline ULONG  getUlong( float  v) { return getUint(v);        }
inline ULONG  getUlong( double v) { return getUint(v);        }
inline INT64  getInt64( float  v) { return (INT64 )v;         }
inline INT64  getInt64( double v) { return (INT64 )v;         }
inline UINT64 getUint64(float  v) { return (UINT64)v;         }
inline UINT64 getUint64(double v) { return (UINT64)v;         }
inline float  getFloat( float  v) { return v;                 }
inline float  getFloat( double v) { return (float )v;         }
inline double getDouble(float  v) { return v;                 }
inline double getDouble(double v) { return v;                 }

inline bool   isChar(   char   v) { return true;              }
inline bool   isChar(   UCHAR  v) { return v <= CHAR_MAX;     }
inline bool   isChar(   short  v) { return v == (char)v;      }
inline bool   isChar(   USHORT v) { return v == (char)v;      }
inline bool   isChar(   int    v) { return v == (char)v;      }
inline bool   isChar(   UINT   v) { return v == (char)v;      }
inline bool   isChar(   long   v) { return v == (char)v;      }
inline bool   isChar(   ULONG  v) { return v == (char)v;      }
inline bool   isChar(   INT64  v) { return v == (char)v;      }
inline bool   isChar(   UINT64 v) { return v == (char)v;      }
inline bool   isChar(   float  v) { return v == getChar(v);   }
inline bool   isChar(   double v) { return v == getChar(v);   }

inline bool   isUchar(  char   v) { return v >= 0;            }
inline bool   isUchar(  UCHAR  v) { return true;              }
inline bool   isUchar(  short  v) { return v == (UCHAR)v;     }
inline bool   isUchar(  USHORT v) { return v == (UCHAR)v;     }
inline bool   isUchar(  int    v) { return v == (UCHAR)v;     }
inline bool   isUchar(  UINT   v) { return v == (UCHAR)v;     }
inline bool   isUchar(  long   v) { return v == (UCHAR)v;     }
inline bool   isUchar(  ULONG  v) { return v == (UCHAR)v;     }
inline bool   isUchar(  INT64  v) { return v == (UCHAR)v;     }
inline bool   isUchar(  UINT64 v) { return v == (UCHAR)v;     }
inline bool   isUchar(  float  v) { return isfinite(v) && (v == getUchar(v));  }
inline bool   isUchar(  double v) { return isfinite(v) && (v == getUchar(v));  }

inline bool   isShort(  short  v) { return true;              }
inline bool   isShort(  USHORT v) { return v <= SHRT_MAX;     }
inline bool   isShort(  int    v) { return v == (short)v;     }
inline bool   isShort(  UINT   v) { return v == (short)v;     }
inline bool   isShort(  long   v) { return v == (short)v;     }
inline bool   isShort(  ULONG  v) { return v == (short)v;     }
inline bool   isShort(  INT64  v) { return v == (short)v;     }
inline bool   isShort(  UINT64 v) { return v == (short)v;     }
inline bool   isShort(  float  v) { return isfinite(v) && (v == getShort(v));  }
inline bool   isShort(  double v) { return isfinite(v) && (v == getShort(v));  }

inline bool   isUshort( short  v) { return v >= 0;            }
inline bool   isUshort( USHORT v) { return true;              }
inline bool   isUshort( int    v) { return v == (USHORT)v;    }
inline bool   isUshort( UINT   v) { return v == (USHORT)v;    }
inline bool   isUshort( long   v) { return v == (USHORT)v;    }
inline bool   isUshort( ULONG  v) { return v == (USHORT)v;    }
inline bool   isUshort( INT64  v) { return v == (USHORT)v;    }
inline bool   isUshort( UINT64 v) { return v == (USHORT)v;    }
inline bool   isUshort( float  v) { return isfinite(v) && (v == getUshort(v)); }
inline bool   isUshort( double v) { return isfinite(v) && (v == getUshort(v)); }

inline bool   isInt(    int    v) { return true;              }
inline bool   isInt(    UINT   v) { return v <= INT_MAX;      }
inline bool   isInt(    long   v) { return true;              }
inline bool   isInt(    ULONG  v) { return v <= INT_MAX;      }
inline bool   isInt(    INT64  v) { return v == (int)v;       }
inline bool   isInt(    UINT64 v) { return v == (int)v;       }
inline bool   isInt(    float  v) { return isfinite(v) && (v == getInt(v));    }
inline bool   isInt(    double v) { return isfinite(v) && (v == getInt(v));    }

inline bool   isUint(   int    v) { return v >= 0;            }
inline bool   isUint(   UINT   v) { return true;              }
inline bool   isUint(   long   v) { return v >= 0;            }
inline bool   isUint(   ULONG  v) { return true;              }
inline bool   isUint(   INT64  v) { return v == (UINT)v;      }
inline bool   isUint(   UINT64 v) { return v == (UINT)v;      }
inline bool   isUint(   float  v) { return isfinite(v) && (v == getUint(v));   }
inline bool   isUint(   double v) { return isfinite(v) && (v == getUint(v));   }

inline bool   isInt64(  INT64  v) { return true;              }
inline bool   isInt64(  UINT64 v) { return v <= INT64_MAX;    }
inline bool   isInt64(  float  v) { return isfinite(v) && (v == getInt64(v));  }
inline bool   isInt64(  double v) { return isfinite(v) && (v == getInt64(v));  }

inline bool   isUint64( INT64  v) { return v >= 0;            }
inline bool   isUint64( UINT64 v) { return true;              }
inline bool   isUint64( float  v) { return isfinite(v) && (v == getUint64(v)); }
inline bool   isUint64( double v) { return isfinite(v) && (v == getUint64(v)); }

inline bool   isFloat(  float  v) { return true;              }
inline bool   isFloat(  double v) { return !isnormal(v) || (v == getFloat(v));  }


inline bool   isnan(    float  v) { return std::isnan(v);    }
inline bool   isnan(    double v) { return std::isnan(v);    }
inline bool   isinf(    float  v) { return std::isinf(v);    }
inline bool   isinf(    double v) { return std::isinf(v);    }

inline bool   isPInfinity(float  v) { return _fpclass(v) == _FPCLASS_PINF; }
inline bool   isPInfinity(double v) { return _fpclass(v) == _FPCLASS_PINF; }
inline bool   isNInfinity(float  v) { return _fpclass(v) == _FPCLASS_NINF; }
inline bool   isNInfinity(double v) { return _fpclass(v) == _FPCLASS_NINF; }

inline bool   isPow2(   int    v) { return (v & -v) == v;    }
inline bool   isPow2(   INT64  v) { return (v & -v) == v;    }

inline bool   isOdd(    int    v) { return (v & 1) != 0;     }
inline bool   isEven(   int    v) { return (v & 1) == 0;     }
inline bool   isOdd(    UINT   v) { return (v & 1) != 0;     }
inline bool   isEven(   UINT   v) { return (v & 1) == 0;     }
inline bool   isOdd(    long   v) { return (v & 1) != 0;     }
inline bool   isEven(   long   v) { return (v & 1) == 0;     }
inline bool   isOdd(    ULONG  v) { return (v & 1) != 0;     }
inline bool   isEven(   ULONG  v) { return (v & 1) == 0;     }
inline bool   isOdd(    INT64  v) { return (v & 1) != 0;     }
inline bool   isEven(   INT64  v) { return (v & 1) == 0;     }
inline bool   isOdd(    UINT64 v) { return (v & 1) != 0;     }
inline bool   isEven(   UINT64 v) { return (v & 1) == 0;     }

// 5-rounding
double round(   double v, int dec);
double trunc(   double v, int dec=0);
double fraction(double v);
float  fraction(float  v);

// return (v > 0) ? +1 : (v < 0) ? -1 : 0
inline int sign(int           v) { return v < 0 ? -1 : v > 0 ? 1 : 0; }
// return (v > 0) ? +1 : (v < 0) ? -1 : 0
inline int sign(const INT64  &v) { return v < 0 ? -1 : v > 0 ? 1 : 0; }
// return (v > 0) ? +1 : (v < 0) ? -1 : 0
inline int sign(float         v) { return v < 0 ? -1 : v > 0 ? 1 : 0; }
// return (v > 0) ? +1 : (v < 0) ? -1 : 0
inline int sign(const double &v) { return v < 0 ? -1 : v > 0 ? 1 : 0; }

// return v*v
inline UINT  sqr( int           v) { return v * v; }
// return v*v
inline float sqr( float         v) { return v * v; }
// return v*v
inline double sqr(const double &v) { return v * v; }

extern const double M_PI;
extern const double M_E;

#define RAD2GRAD(r) ((r) / M_PI * 180.0)
#define GRAD2RAD(g) ((g) / 180.0 * M_PI)

#define PERCENT( n,total) ((total)?((double)(n)*100 /(total)):100.0 )
#define PERMILLE(n,total) ((total)?((double)(n)*1000/(total)):1000.0)

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif
#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

// return x if x is in I = [min(x1, x2); max(x1, x2)] else the endpoint of I nearest to x
short   minMax(short   x, short   x1, short   x2);
USHORT  minMax(USHORT  x, USHORT  x1, USHORT  x2);
int     minMax(int     x, int     x1, int     x2);
UINT    minMax(UINT    x, UINT    x1, UINT    x2);
INT64   minMax(INT64   x, INT64   x1, INT64   x2);
UINT64  minMax(UINT64  x, UINT64  x1, UINT64  x2);
float   minMax(float   x, float   x1, float   x2);
double  minMax(double  x, double  x1, double  x2);

#define FLT_NAN  std::numeric_limits<float>::quiet_NaN()
#define FLT_PINF std::numeric_limits<float>::infinity()
#define FLT_NINF (-FLT_PINF)
#define DBL_NAN  std::numeric_limits<double>::quiet_NaN()
#define DBL_PINF std::numeric_limits<double>::infinity()
#define DBL_NINF (-DBL_PINF)

inline double dsign(double x           ) { return (x < 0) ? -1 : (x > 0) ? 1 : 0; }
inline int    dmin(int    x1, int    x2) { return (x1 < x2) ? x1 : x2;}
inline int    dmax(int    x1, int    x2) { return (x1 > x2) ? x1 : x2;}
inline UINT   dmin(UINT   x1, UINT   x2) { return (x1 < x2) ? x1 : x2;}
inline UINT   dmax(UINT   x1, UINT   x2) { return (x1 > x2) ? x1 : x2;}
inline INT64  dmin(INT64  x1, INT64  x2) { return (x1 < x2) ? x1 : x2;}
inline INT64  dmax(INT64  x1, INT64  x2) { return (x1 > x2) ? x1 : x2;}
inline UINT64 dmin(UINT64 x1, UINT64 x2) { return (x1 < x2) ? x1 : x2;}
inline UINT64 dmax(UINT64 x1, UINT64 x2) { return (x1 > x2) ? x1 : x2;}
inline float  dmin(float  x1, float  x2) { return (x1 < x2) ? x1 : x2;}
inline float  dmax(float  x1, float  x2) { return (x1 > x2) ? x1 : x2;}
inline double dmin(double x1, double x2) { return (x1 < x2) ? x1 : x2;}
inline double dmax(double x1, double x2) { return (x1 > x2) ? x1 : x2;}

inline UINT   getSignificand(float  x) { return       (((*((UINT  *)(&(x))))  & 0x7fffff           ) | 0x800000);             }
inline int    getExpo2(      float  x) { return (int)((((*((UINT  *)(&(x)))) >> 23) & 0xff ) - 0x7f);                         }
inline BYTE   getSign(       float  x) { return          (((BYTE  *)(&x))[3]) & 0x80;                                         }
inline UINT64 getSignificand(double x) { return       (((*((UINT64*)(&(x))))  & 0xfffffffffffffui64) | 0x10000000000000ui64); }
inline int    getExpo2(      double x) { return (int)((((*((UINT64*)(&(x)))) >> 52) & 0x7ff) - 0x3ff);                        }
inline BYTE   getSign(       double x) { return          (((BYTE  *)(&x))[7]) & 0x80;                                         }
