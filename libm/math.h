/*
 * math.h
 *
 * PSXSDK
 * modified and stripped down from fdlibm
 */

#ifndef _MATH_H
#define _MATH_H

/* @(#)math.h 1.5 2010/06/16 */
/*
 * ====================================================
 * Copyright (C) 2004 by Sun Microsystems, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#define __HI(x) *(1+(int*)&x)
#define __LO(x) *(int*)&x
#define __HIp(x) *(1+(int*)x)
#define __LOp(x) *(int*)x

#ifdef __STDC__
#define	__P(p)	p
#else
#define	__P(p)	()
#endif

/*
 * ANSI/POSIX
 */

extern int signgam;

#define	MAXFLOAT	((float)3.40282346638528860e+38)

enum fdversion {fdlibm_ieee = -1, fdlibm_svid, fdlibm_xopen, fdlibm_posix};

#define _LIB_VERSION_TYPE enum fdversion
#define _LIB_VERSION _fdlib_version  

/* if global variable _LIB_VERSION is not desirable, one may 
 * change the following to be a constant by: 
 *	#define _LIB_VERSION_TYPE const enum version
 * In that case, after one initializes the value _LIB_VERSION (see
 * s_lib_version.c) during compile time, it cannot be modified
 * in the middle of a program
 */ 
extern  _LIB_VERSION_TYPE  _LIB_VERSION;

#define _IEEE_  fdlibm_ieee
#define _SVID_  fdlibm_svid
#define _XOPEN_ fdlibm_xopen
#define _POSIX_ fdlibm_posix

struct exception {
	int type;
	char *name;
	double arg1;
	double arg2;
	double retval;
};

#define	HUGE		MAXFLOAT

/* 
 * set X_TLOSS = pi*2**52, which is possibly defined in <values.h>
 * (one may replace the following line by "#include <values.h>")
 */

#define X_TLOSS		1.41484755040568800000e+16 

#define	DOMAIN		1
#define	SING		2
#define	OVERFLOW	3
#define	UNDERFLOW	4
#define	TLOSS		5
#define	PLOSS		6

/*
 * ANSI/POSIX
 */
extern double acos __P((double));
extern double asin __P((double));
extern double atan __P((double));
extern double atan2 __P((double, double));
extern double cos __P((double));
extern double sin __P((double));
extern double tan __P((double));

extern double cosh __P((double));
extern double sinh __P((double));
extern double tanh __P((double));

extern double exp __P((double));
extern double frexp __P((double, int *));
extern double ldexp __P((double, int));
extern double log __P((double));
extern double log10 __P((double));
extern double modf __P((double, double *));

extern double pow __P((double, double));
extern double sqrt __P((double));

extern double ceil __P((double));
extern double fabs __P((double));
extern double floor __P((double));
extern double fmod __P((double, double));

extern double erf __P((double));
extern double erfc __P((double));
extern double gamma __P((double));
extern double hypot __P((double, double));
extern int isnan __P((double));
extern int finite __P((double));
extern double j0 __P((double));
extern double j1 __P((double));
extern double jn __P((int, double));
extern double lgamma __P((double));
extern double y0 __P((double));
extern double y1 __P((double));
extern double yn __P((int, double));

extern double acosh __P((double));
extern double asinh __P((double));
extern double atanh __P((double));
extern double cbrt __P((double));
extern double logb __P((double));
extern double nextafter __P((double, double));
extern double remainder __P((double, double));
#ifdef _SCALB_INT
extern double scalb __P((double, int));
#else
extern double scalb __P((double, double));
#endif

extern int matherr __P((struct exception *));

/*
 * IEEE Test Vector
 */
extern double significand __P((double));

/*
 * Functions callable from C, intended to support IEEE arithmetic.
 */
extern double copysign __P((double, double));
extern int ilogb __P((double));
extern double rint __P((double));
extern double scalbn __P((double, int));

/*
 * BSD math library entry points
 */
extern double expm1 __P((double));
extern double log1p __P((double));

/*
 * Reentrant version of gamma & lgamma; passes signgam back by reference
 * as the second argument; user must allocate space for signgam.
 */
#ifdef _REENTRANT
extern double gamma_r __P((double, int *));
extern double lgamma_r __P((double, int *));
#endif	/* _REENTRANT */

/* ieee style elementary functions */
extern double __ieee754_sqrt __P((double));			
extern double __ieee754_acos __P((double));			
extern double __ieee754_acosh __P((double));			
extern double __ieee754_log __P((double));			
extern double __ieee754_atanh __P((double));			
extern double __ieee754_asin __P((double));			
extern double __ieee754_atan2 __P((double,double));			
extern double __ieee754_exp __P((double));
extern double __ieee754_cosh __P((double));
extern double __ieee754_fmod __P((double,double));
extern double __ieee754_pow __P((double,double));
extern double __ieee754_lgamma_r __P((double,int *));
extern double __ieee754_gamma_r __P((double,int *));
extern double __ieee754_lgamma __P((double));
extern double __ieee754_gamma __P((double));
extern double __ieee754_log10 __P((double));
extern double __ieee754_sinh __P((double));
extern double __ieee754_hypot __P((double,double));
extern double __ieee754_j0 __P((double));
extern double __ieee754_j1 __P((double));
extern double __ieee754_y0 __P((double));
extern double __ieee754_y1 __P((double));
extern double __ieee754_jn __P((int,double));
extern double __ieee754_yn __P((int,double));
extern double __ieee754_remainder __P((double,double));
extern int    __ieee754_rem_pio2 __P((double,double*));
#ifdef _SCALB_INT
extern double __ieee754_scalb __P((double,int));
#else
extern double __ieee754_scalb __P((double,double));
#endif

/* fdlibm kernel function */
extern double __kernel_standard __P((double,double,int));	
extern double __kernel_sin __P((double,double,int));
extern double __kernel_cos __P((double,double));
extern double __kernel_tan __P((double,double,int));
extern int    __kernel_rem_pio2 __P((double*,double*,int,int,int,const int*));

/* the defines below are from NetBSD's math.h - which is based on fdlibm as well */

#define	M_E		2.7182818284590452354	/* e */
#define	M_LOG2E		1.4426950408889634074	/* log 2e */
#define	M_LOG10E	0.43429448190325182765	/* log 10e */
#define	M_LN2		0.69314718055994530942	/* log e2 */
#define	M_LN10		2.30258509299404568402	/* log e10 */
#define	M_PI		3.14159265358979323846	/* pi */
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#define	M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#define	M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

// definitions for wrappers

float acosf(float x);; // acos(x); }
float acoshf(float x); // acosh(x); }

float asinf(float x); // asin(x); }
float asinhf(float x); // asinf(x); }

float atanf(float x); // atan(x); }
float atanhf(float x); // atanh(x); }

float atan2f(float x, float y); // atan2(x,y); }

float cbrtf(float x); // cbrt(x); }

float ceilf(float x); // ceil(x); }

float copysignf(float x, float y) ; // copysign(x, y); }

double copysignl(long double x, long double y); // copysign(x, y); }

float cosf(float x); // cos(x); }
float coshf(float x); // cosh(x); }

float erff(float x); // erf(x); }
float erfcf(float x); // erfc(x); }

float expf(float x); // exp(x); }
//float exp2f(float x); // exp2(x); }
float expm1f(float x); // expm1(x); }

float fabsf(float x); // fabs(x); }

float finitef(float x); // finite(x); }

float floorf(float x); // floor(x); }

float fmodf(float x, float y); // fmod(x, y); }

float hypotf(float x, float y); // hypot(x, y); }

int ilogbf(float x); // ilogb(x); }
int ilogbl(long double x); // ilogb(x); }

float j0f(float x); // j0(x); }
float j1f(float x); // j1(x); }
float jnf(int n, float x); // jn(n,x); }

float y0f(float x); // y0(x); }
float y1f(float x); // y1(x); }
float ynf(int n, float x); // yn(n, x); }

float lgammaf(float x); // lgamma(x); }
float gammaf(float x); // gamma(x); }
//float tgammaf(float x); // tgamma(x); }

float lgammaf_r(float x, int *sign); // lgamma_r(x, sign); }
float gammaf_r(float x, int *sign); // gamma_r(x, sign); }

float logf(float x); // log(x); }
float log10f(float x) ; // log10(x); }
float log1pf(float x) ; // log1p(x); }
//float log2f(float x) ; // log2(x); }

//float nanf(const char *tagp) ; // nan(tagp); }
//long double nanl(const char *tagp) ; // nan(tagp); }

float nextafterf(float x, float y) ; // nextafter(x, y); }
long double nextafterl(long double x, long double y)
; // nextafter(x, y); }

float powf(float x, float y) ; // pow(x, y); }

float remainderf(float x, float y) ; // remainder(x, y); }
//float remquof(float x, float y, int *quo) ; // remquo(x, y, quo); }

float rintf(float x); // rint(x); }

float scalbnf(float x, int n); // scalbn(x, n); }
long double scalbnl(long double x, int n); // scalbn(x, n); }

float sinf(float x); // sin(x); }
float sinhf(float x); // sinh(x); }

float sqrtf(float x); // sqrt(x); }

float tanf(float x); // tan(x); }
float tanhf(float x); // tanh(x); }

//float truncf(float x); // trunc(x); }


#endif
