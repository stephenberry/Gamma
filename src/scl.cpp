/*	Gamma - Generic processing library
	See COPYRIGHT file for authors and license information */

#include <ctype.h>
#include <string.h>
#include "scl.h"
#include "MacroD.h"

namespace gam{
namespace scl{

char base10To36(int v){
	if(within(v, 0, 9)) return '0' + v;
	if(within(v,10,35)) return 'a' + v - 10;
	return '0';
}

int base36To10(char v){
	v = tolower(v);
	if(within(v, '0', '9')) return v - '0';
	if(within(v, 'a', 'z')) return v - 'a' + 10;
	return 0;	// non-alphanumeric
}


float clipMag(float value, float min, float max){
	union {float f; ULONG i;} v;
	v.f = value;
	ULONG sign = v.i & 0x80000000;
	v.i |= 0x80000000;
	v.f = clip(v.f, max, min);
	v.i |= sign;
	return v.f;
}


double freq(const char * note){

	char c = *note++;
	if(within(c, 'a', 'g')){
		c -= 97;

		static char r[7] = {9,11,0,2,4,5,7};
		char result = r[(unsigned)c];
		
		c = *note++;
		     if(c == '+'){ result++; c = *note; }
		else if(c == '-'){ result--; c = *note; }
		else if(c == ' '){ c = *note; }
		
		return ::pow(2., (double)(result + (c-48)*12) / 12.) * 8.1757989157741;		
	}
	return 0.;
}


double laguerre(int n, int k, double x) {
	double res = 1, bin = 1;
	
	for(int i=n; i>=1; --i){
		bin = bin * (k+i) / (n + 1 - i);
		res = bin - x * res / i;
	}
	return res;
}


double legendre(int l, int m, double t){

	if(l<0){ /*printf("l=%d. l must be non-negative.\n");*/ return 0; }
	if(m<-l || m>l){ /*printf("m=%d. m must be -l <= m <= l.\n");*/ return 0; }

	// compute P_l^m(x) by the recurrence relation
	//		(l-m)P_l^m(x) = x(2l-1)P_{l-1}^m(x) - (l+m-1)P_{l-2}^m(x)
	// with 
	//		P_m^m(x) = (-1)^m (2m-1)!! (1-x)^{m/2}, 
	//		P_{m+1}^m(x) = x(2m+1) P_m^m(x).

	double P = 0;
	double cs = cos(t);
	double sn = sin(t);
	int mm = scl::abs(m);			/*   mm = |m|   */
	double y1 = 1.;
	
	for(int i=1; i<=mm; ++i)
		y1 *= -((i<<1) - 1) * sn;
	
	if(l==mm) P = y1;

	else{
		double y = ((mm<<1) + 1) * cs * y1;
		if(l==(mm+1)) P = y;

		else{
			double c = (mm<<1) - 1;
			for(int k=mm+2; k<=l; ++k){
				double y2 = y1;
				y1 = y;
				double d = c / (k - mm);
				y = (2. + d) * cs * y1 - (1. + d) * y2;
			}
			P = y;
		}
	}

	// In the case that m<0, 
	// compute P_n^{-|m|}(x) by the formula 
	//		P_l^{-|m|}(x) = (-1)^{|m|}((l-|m|)!/(l+|m|)!)^{1/2} P_l^{|m|}(x). 
	if(m<0){
		for(int i=l-mm+1; i<=l+mm; ++i) P *= 1. / i;
		if(scl::odd(mm)) P = -P;
	}

	return P;
}


void colorHSV(float r, float g, float b, float &h, float &s, float &v){

	float min = r < g ? (r < b ? r : b) : (g < b ? g : b);
	float max = r > g ? (r > b ? r : b) : (g > b ? g : b);

	v = max;					// v
	float delta = max - min;	// delta RGB value

	if ( delta != 0.f && max != 0.f ){		// chromatic data...
		s = delta / max;		// s
		
		float hl;
		if     ( r == max )	hl =       ( g - b ) / delta;	// between yellow & magenta
		else if( g == max )	hl = 2.f + ( b - r ) / delta;	// between cyan & yellow
		else				hl = 4.f + ( r - g ) / delta;	// between magenta & cyan

		if( hl < 0.f ) hl += 6.f;

		h = hl * 0.166666667f;
	}
	else{				// this is a gray, no chroma...
	   h = 0.f;
	   s = 0.f;
	}
}


void colorRGB(float h, float s, float v, float &r, float &g, float &b){
	
	if( s == 0.f ) {
		r = g = b = v;	// achromatic (grey)
		return;
	}
	
	h *= 6.f;
										// sector 0 to 5
	unsigned int i = (unsigned int)(h);	// integer part of h
	float f = h - (float)i;				// fractional part of h
	float p = v * ( 1.f - s );
		
	float q;	// depends on hue section
	if(i & 1U)	q = v * ( 1.f - s * f );			// odd
	else		q = v * ( 1.f - s * ( 1.f - f ) );	// even

	switch( i ) {
		case 0: r = v; g = q; b = p; break;
		case 1:	r = q; g = v; b = p; break;
		case 2:	r = p; g = v; b = q; break;
		case 3:	r = p; g = q; b = v; break;
		case 4: r = q; g = p; b = v; break;
		default:r = v; g = p; b = q; break;
	}
}


/*

exponent will not be modified
must quantize fraction

1. if	exponent is greater than 01111110, continue
   else return 0
2. zero last n bits in fraction

range		exp				zero lsb
[1/2, 1)	01111110 (126)	>= 32
[1, 2)		01111111 (127)	23
[2, 4)		10000000 (128)	22
[4, 8)		10000001 (129)	21

0 01111110 11111111111111111111111	just under 1

0 01111110 Fffffffffffffffffffffff	[1/2, 1)

0 01111111 Fffffffffffffffffffffff	[1, 2)

0 10000000 Fffffffffffffffffffffff	[2, 4)

0 01111111 10000000000000000000000	1.5
0 01111111 00000000000000000000000	1

0 10000000 01000000000000000000000	2.5
0 10000000 00000000000000000000000	2

0 10000000 11000000000000000000000	3.5
0 10000000 10000000000000000000000	3

0 00000000 00000000000000000000000	[0, 1)	huge
1 11111111 00000000000000000000000	[1, 2)	0
1 11111111 10000000000000000000000	[2, 4)	1
1 11111111 11000000000000000000000	[4, 8)	2

0 00000000 11111111111111111111111
1 11111111 00000000000000000000000

*/


// branchless
//float SclOp::fastFloor(float value){
//	ULONG valueU = *(ULONG *)&value;
////	ULONG shift = 23U - ((valueU >> 23U & 0xff) - 127U);
////	valueU = valueU >> shift << shift;
//	ULONG shift = (valueU >> 23U & 0xff) - 127U;
//	valueU &= ~(~(0xff800000 >> shift));
//	//printBinary((ULONG)~(0x7FFFFF >> shift)); printf("\n");
//	return *(float *)&valueU;
//}

//float SclOp::fastFloor(float value){
//	union { float f; ULONG u; } word;
//	word.f = value;
//
//	ULONG bits = word.u;
//	bits += 0x800000;
//
//	if(bits & 0x40000000){	// mag outside [0, 1)
//		int shift = 23U - (bits >> 23U & 0x7F);
//		bits = word.u;
//		bits = bits >> shift << shift;
//		word.u = bits;
//		return word.f;
//	}
//	else{
//		return 0.f;
//	}
//}

//inline float SclOp::truncFast(float value){
//
//	ULONG valueU = *(ULONG *)&value;
//	
//	// Is value outside [0, 1) ?
//	if((valueU & 0x7fffffff) > 0x3f7fffff){
//		ULONG shift = 150UL - (valueU >> 23UL & 0x7f);
//		valueU = valueU >> shift << shift;
//		return *(float *)&valueU;
//	}
//	
//	return 0.f;
//}

} // end namespace scl
} // end namespace gam

#include "MacroU.h"

