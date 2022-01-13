#include "math.h"

float acosf(float x){ return acos(x); }
float acoshf(float x){ return acosh(x); }

float asinf(float x){ return asin(x); }
float asinhf(float x){ return asinf(x); }

float atanf(float x){ return atan(x); }
float atanhf(float x){ return atanh(x); }

float atan2f(float x, float y){ return atan2(x,y); }

float cbrtf(float x){ return cbrt(x); }

float ceilf(float x){ return ceil(x); }

float copysignf(float x, float y) { return copysign(x, y); }

double copysignl(long double x, long double y){ return copysign(x, y); }

float cosf(float x){ return cos(x); }
float coshf(float x){ return cosh(x); }

float erff(float x){ return erf(x); }
float erfcf(float x){ return erfc(x); }

float expf(float x){ return exp(x); }
//float exp2f(float x){ return exp2(x); }
float expm1f(float x){ return expm1(x); }

float fabsf(float x){ return fabs(x); }

float finitef(float x){ return finite(x); }

float floorf(float x){ return floor(x); }

float fmodf(float x, float y){ return fmod(x, y); }

float hypotf(float x, float y){ return hypot(x, y); }

int ilogbf(float x){ return ilogb(x); }
int ilogbl(long double x){ return ilogb(x); }

float j0f(float x){ return j0(x); }
float j1f(float x){ return j1(x); }
float jnf(int n, float x){ return jn(n,x); }

float y0f(float x){ return y0(x); }
float y1f(float x){ return y1(x); }
float ynf(int n, float x){ return yn(n, x); }

float lgammaf(float x){ return lgamma(x); }
float gammaf(float x){ return gamma(x); }
//float tgammaf(float x){ return tgamma(x); }

float lgammaf_r(float x, int *sign){ return lgamma_r(x, sign); }
float gammaf_r(float x, int *sign){ return gamma_r(x, sign); }

float logf(float x){ return log(x); }
float log10f(float x) { return log10(x); }
float log1pf(float x) { return log1p(x); }
//float log2f(float x) { return log2(x); }

//float nanf(const char *tagp) { return nan(tagp); }
//long double nanl(const char *tagp) { return nan(tagp); }

float nextafterf(float x, float y) { return nextafter(x, y); }
long double nextafterl(long double x, long double y)
{ return nextafter(x, y); }

float powf(float x, float y) { return pow(x, y); }

float remainderf(float x, float y) { return remainder(x, y); }
//float remquof(float x, float y, int *quo) { return remquo(x, y, quo); }

float rintf(float x){ return rint(x); }

float scalbnf(float x, int n){ return scalbn(x, n); }
long double scalbnl(long double x, int n){ return scalbn(x, n); }

float sinf(float x){ return sin(x); }
float sinhf(float x){ return sinh(x); }

float sqrtf(float x){ return sqrt(x); }

float tanf(float x){ return tan(x); }
float tanhf(float x){ return tanh(x); }

//float truncf(float x){ return trunc(x); }
