#include "pch.h"

using namespace std;

// Calculates a^r mod n
BigInt powmod(const BigInt &a, const BigInt &r, const BigInt &n, bool verbose) {
  DigitPool *pool = a.getDigitPool();

  BigInt p = pool->_1();
  BigInt tmpa(a);
  BigInt tmpr(r, pool);

  int i = 1;
  StreamParameters param(20);
  if(verbose) {
    tcout << param << "a" << " " << param << "r" << " " << param << "p" << endl;
  }
  while(!tmpr.isZero()) {
    if(verbose) {
      tcout << param << tmpa << " " << param << tmpr << " " << param << p << endl;
    }

    if(isOdd(tmpr)) {
      p = (p * tmpa) % n;
      --tmpr;
    } else {
      tmpa = (tmpa * tmpa) % n;
      tmpr.divide2();
    }
  }
  if(verbose) {
    tcout << param << tmpa << " " << param << tmpr << " " << param << p << endl;
  }
  return p;
}
