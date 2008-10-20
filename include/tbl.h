#ifndef GAMMA_TBL_H_INC
#define GAMMA_TBL_H_INC

/*	Gamma - Generic processing library
	See COPYRIGHT file for authors and license information */
 
#include <stdlib.h>
#include "arr.h"
#include "mem.h"
#include "scl.h"
#include "Constants.h"

#include "MacroD.h"

namespace gam{


/// Various window types.
class WinType{
public:

	/// Window types.
	enum type{ 	
		Bartlett,			/**< Bartlett (Triangle)*/
		Blackman,			/**< Blackman */
		BlackmanHarris,		/**< Blackman-Harris */
		Hamming,			/**< Hamming */
		Hann,				/**< von Hann */
		Welch,				/**< Welch */
		Nyquist,			/**< Nyquist */
		Rectangle			/**< Rectangle (1s) */
	};
	
	/// Returns human readable string of window type.
	inline static const char * string(WinType::type type){
		#define CS(name) case WinType::name: return #name;
		switch(type){
			CS(Bartlett) CS(Blackman) CS(BlackmanHarris) CS(Hamming) CS(Hann)
			CS(Welch) CS(Nyquist) CS(Rectangle)
			default: return "Unknown";
		}
		#undef CS
	}
	
};

/// Table functions
namespace tbl{

/// Fills array with arbitrary # of periods of cosine and sine waves.

/// Cosine starts at dst[0], sine starts at dst[len].
///
TEM void cosSin(T * dst, ULONG len, double periods);

/// Fills array with one period of a cosine wave.
TEM void cosine(T * dst, ULONG len);

/// Fills table with section of an exponential decay.

///		Values are normalized to descend from 1 to 0. \n
///		Negative 'order' curves downward and positive 'order' curves upward. \n
///		If 'order' is 0, you get a line.
TEM void decay(T * dst, ULONG len, double order);

/// Fills array with one period of a sine wave.
TEM void sine(T * dst, ULONG len);

/// Fills array with arbitrary phase and length sinusoid.
TEM void sinusoid(T * dst, ULONG len, double phase, double periods);

// Max harmonics
// [len, -1, 0, -1, ..., 0, -1]
// Max harmonics - 1
// [len - 1, 0, -1, 0, ..., -1, 0]
TEM void impulseSum(T * dst, ULONG len);
	
/// Sums band-limited impulse wave into array.

/// The waveform includes harmonics in the range [hrmLo, hrmHi].
/// The amplitude of the waveform will not be normalized.
/// The ideal waveform shape is [4, -1, 0, -1, 0, -1, 0, -1 ]
TEM void impulseSum(T * dst, ULONG len, ULONG hrmLo, ULONG hrmHi);

/// Sums band-limited saw wave into array.

/// The waveform includes harmonics in the range [hrmLo, hrmHi].
/// The ideal waveform shape is [1, 0.75, 0.5, 0.25, 0, -0.25, -0.5, -0.75]
TEM void sawSum(T * dst, ULONG len, ULONG hrmLo, ULONG hrmHi);

/// Sums band-limited square wave into array.

/// The waveform includes harmonics in the range [hrmLo, hrmHi].
///	The ideal waveform shape is [ 1, 1, 1, 1, -1, -1, -1, -1].
TEM void squareSum(T * dst, ULONG len, ULONG hrmLo, ULONG hrmHi);

/// Sums band-limited triangle wave into array.
	
/// The waveform includes harmonics in the range [hrmLo, hrmHi].
///	The ideal waveform shape is [ 0, 0.5, 1, 0.5, 0, -0.5, -1, -0.5].
TEM void triangleSum(T * dst, ULONG len, ULONG hrmLo, ULONG hrmHi);

/// 
TEM void multiWave(T * dst, ULONG len, ULONG order, void (* func)(T *, ULONG, ULONG, ULONG));

/// Returns maximum number of harmonics that will fit in array.
ULONG maxHarmonics(ULONG len);

/// Fills array with polynomial a0 + a1 i + a2 i^2.
TEM void poly(T * dst, ULONG len, T a0, T a1, T a2);

/// Fills array with specified window type.
TEM void window			(T * dst, ULONG len, WinType::type type);
TEM void bartlett		(T * dst, ULONG len); ///< Fills array with Bartlett window.
TEM void blackman		(T * dst, ULONG len); ///< Fills array with Blackman window.
TEM void blackmanHarris	(T * dst, ULONG len); ///< Fills array with Blackman-Harris window.
TEM void hamming		(T * dst, ULONG len); ///< Fills array with Hamming window.
TEM void hann			(T * dst, ULONG len); ///< Fills array with von Hann window.
TEM void welch			(T * dst, ULONG len); ///< Fills array with Welch window.
TEM void rectangle		(T * dst, ULONG len); ///< Fills array with Rectangle window.
TEM void nyquist		(T * dst, ULONG len); ///< Fills array with Nyquist window.


//
// Accessing
//

// Return value from a table with the first half of a dq-symmetric 
// waveform.  The table size must be a power of two.
//
//	'table':	first half of waveform
//	'fbits':	= 31 - (# bits in table)
//	'phase':	phase of lookup (full waveform period is [0, 2^32))
//
//	'phase' bit format (b = 'fbits'):
//	32:			sign bit (0 = positive, 1 = negative)
//	[31, b]:	phase integer part
//	[ b, 0]:	phase fractional part
float atH(const float * src, ULONG fbits, ULONG phase);

// Return value from a table with the first quarter of a dbqp-symmetric 
// waveform.  The table size must be a power of two plus one.
//
//	'table':	first quarter of waveform
//	'fbits':	= 30 - (# bits in table)
//	'phase':	phase of lookup (full waveform period is [0, 2^32))
//
//	'phase' bit format (b = 'fbits'):
//	32:			sign bit (0 = positive, 1 = negative)
//	31:			direction bit (0 = forward, 1 = backward)
//	[30, b]:	phase integer part
//	[ b, 0]:	phase fractional part
float atQ(const float * src, ULONG fbits, ULONG phase);

/// Return fractional part of integer phasor as float in [0, 1).

///	'bits' is the effective size of the lookup table. \n
///	Note: the fraction only has 24-bits of precision.
float fraction(ULONG bits, ULONG phase);

// Get interpolation data from integer phasor.
//float getIpol2(ULONG bits, ULONG phase, ULONG &i, ULONG &ip1);

/// Returns phase increment factor.

///	Multiply by desired frequency in Hz to get integer phase increment.
///
float phaseIncFactor(double framesPerSec);

//
//	Printing
//

/// Prints array as hexidecimal values.
void printHexArray(float * table, ULONG len, ULONG valuesPerLine);




// Implementation_______________________________________________________________

TEM void cosSin(T * dst, ULONG len, double periods){
	T * cos = dst;
	T * sin = dst + len;
	double radInc = (periods * M_2PI) / (double)len;
	double cos1 = cos(radInc);
	double sin1 = sin(radInc);
	double cs = 1.;
	double sn = 0.;

	LOOP_P(len,
		*cos++ = cs; *sin++ = sn;
		scl::mulComplex(cs, sn, cos1, sin1);
	)
}

TEM void cosine(T * dst, ULONG len){
	double inc = M_2PI / (double)len;
	double phs = inc;
	len >>= 1;
	
	T * dst2 = dst + len;
	
	*dst++  = (T) 1;
	*dst2++ = (T)-1;

	len -= 1;
	LOOP_P(len,
		T val = (T)cos(phs);
		*dst++  =  val;
		*dst2++ = -val;
		phs += inc;
	)
}

TEM void decay(T * dst, ULONG len, double order){
	double final = pow(2., order);
	double lambda = log(final) / (float)len;
	double time = 1.;
	double scale = 1. / (1. - final);
	double offset = -final;
	*dst++ = (T)1;
	len -= 1;
	LOOP_P(len,
		*dst++ = (T)(exp(lambda * time) + offset) * scale;
		time++;
	)
}

TEM void sine(T * dst, ULONG len){
	double inc = M_2PI / (double)len;
	double phs = inc;
	len >>= 1;
	
	T * dst2 = dst + len;
	
	*dst++  = (T)0;
	*dst2++ = (T)0;
	
	len -= 1;
	LOOP_P(len,
		T val = (T)sin(phs);
		*dst++  =  val;
		*dst2++ = -val;
		phs += inc;
	)
}

// VERY accurate, but not so fast
TEM void sinusoid(T * dst, ULONG len, double phase, double periods){
	
	double inc = M_2PI * periods / (double)len;

	for(double i = 0.; i < (double)len; i++){
		*dst++ = (T)sin(inc * i + phase);
	}
}

TEM void poly(T * dst, ULONG len, T a0, T a1, T a2){
	LOOP(len, *dst++ = scl::poly((T)i, a0, a1, a2); )
}

TEM void impulseSum(T * dst, ULONG len){
	ULONG harmonics = (len>>1) - 1;
	*dst++ = (T)harmonics;
	*dst++ = (T)0;
	LOOP(harmonics,
		*dst++ = (T)-1;
		*dst++ = (T) 0;
	)
}

TEM void impulseSum(T * dst, ULONG len, ULONG hrmLo, ULONG hrmHi){
	double inc = M_2PI / (double)len;
	ULONG hLen = len >> 1;
	
	for(ULONG k = hrmLo; k <= hrmHi; ++k){
		double phaseInc = (double)k * inc;
		double phs = 0.;
		
		T * dst1 = dst;
		
		LOOP(hLen + 1, 
			*dst1++ += (T)(cos(phs));
			phs += phaseInc;
		)
	}
	
	// Extrapolate remaining from [db] symmetry
	mem::mirrorR(dst + 1, len - 1);
}

TEM void sawSum(T * dst, ULONG len, ULONG hrmLo, ULONG hrmHi){

	static const double sawFactor = 2.0 / M_PI;
	double inc = M_2PI / (double)len;
	ULONG hLen = len >> 1;
	
	dst++;
	
	for(ULONG i = hrmLo; i <= hrmHi; ++i){
		double h = (double)i;
		double phaseInc = h * inc;
		double phs = phaseInc;
		double amp = sawFactor / h;
		
		T * dst1 = dst;
		
		for(ULONG j=1; j<hLen; ++j){
			*dst1++ += (T)(amp * sin(phs));
			phs += phaseInc;
		}
	}
	
	// Extrapolate remaining from [dp] symmetry
	arr::mirror_dp(dst, len-1);	
}

TEM void squareSum(T * dst, ULONG len, ULONG hrmLo, ULONG hrmHi){

	static const double sqrFactor = 4.0 / M_PI;
	double inc = M_2PI / (double)len;
	ULONG qLen = len >> 2;
	
	dst++;
	
	hrmLo |= 1;	// next highest odd if even
	
	// Calculate first quadrant
	for(ULONG i = hrmLo; i <= hrmHi; i+=2){

		double h = (double)i;
		double phaseInc = h * inc;
		double phs = phaseInc;
		double amp = sqrFactor / h;
		
		T * dst1 = dst;
		
		for(ULONG j=1; j<=qLen; ++j){
			*dst1++ += (T)(amp * sin(phs));
			phs += phaseInc;
		}
	}

	// Extrapolate remaining from [dbqp] symmetry	
	mem::mirrorR(dst, (len >> 1) - 1);
	arr::mirror_dq(--dst, len);
}


TEM void triangleSum(T * dst, ULONG len, ULONG hrmLo, ULONG hrmHi){

	static const double triFactor = 8.0 / (M_PI * M_PI);
	double inc = M_2PI / (double)len;
	ULONG qLen = len >> 2;
	
	dst++;
	
	hrmLo |= 1;	// next highest odd if even
	
	double factor = hrmLo & 0x2 ? -triFactor : triFactor;
	
	// Calculate first quadrant
	for(ULONG i = hrmLo; i <= hrmHi; i+=2){

		double h = (double)i;
		double phaseInc = h * inc;
		double phs = phaseInc;
		double amp = factor / (h * h);
		factor = -factor;
		
		T * dst1 = dst;
		
		for(ULONG j=1; j<=qLen; ++j){
			*dst1++ += (T)(amp * sin(phs));
			phs += phaseInc;
		}
	}

	// Extrapolate remaining from [dbqp] symmetry	
	mem::mirrorR(dst, (len >> 1) - 1);
	arr::mirror_dq(--dst, len);
}

/*	1	1
	2	2
	3	4
	5	8
	9	16	
*/
TEM void multiWave(T * dst, ULONG len, ULONG order, void (* func)(T *, ULONG, ULONG, ULONG)){

	dst += len * (order - 1);

	func(dst, len, 1, 1);
	
	ULONG hrmLo = 2;
	ULONG hrmHi = 2;

	for(ULONG o=0; o<order-1; o++){
	
		T * dstPrev = dst;
		dst -= len;
		mem::copy(dst, dstPrev, len);
		func(dst, len, hrmLo, hrmHi);
	
		hrmLo = hrmHi + 1;
		hrmHi <<= 1;
	}
}

TEM void window(T * dst, ULONG len, WinType::type type){
	switch(type){
		case WinType::Bartlett:			bartlett(dst, len);			break;
		case WinType::Blackman:			blackman(dst, len);			break;
		case WinType::BlackmanHarris:	blackmanHarris(dst, len);	break;
		case WinType::Hamming:			hamming(dst, len);			break;
		case WinType::Hann:				hann(dst, len);				break;
		case WinType::Welch:			welch(dst, len);			break;
		case WinType::Nyquist:			nyquist(dst, len);			break;
		default:						rectangle(dst, len);
	};
}

#define SYM_WIN(period, phs0, eqn) \
	double inc = period / (double)(len);\
	double phs = phs0;\
	T * dst2 = dst + len - 1;\
	*dst++ = (T)eqn;\
	LOOP(len >> 1,\
		phs += inc;\
		T val = (T)eqn;\
		*dst++  = val;\
		*dst2-- = val;\
	)
	
TEM void bartlett      (T * dst, ULONG len){ SYM_WIN(2.   , 0., phs) }
TEM void blackman      (T * dst, ULONG len){ SYM_WIN(M_2PI, 0., scl::blackman(phs)) }
TEM void blackmanHarris(T * dst, ULONG len){ SYM_WIN(M_2PI, 0., scl::blackmanHarris(phs)) }
TEM void hamming       (T * dst, ULONG len){ SYM_WIN(M_2PI, 0., scl::hamming(phs)) }
TEM void hann          (T * dst, ULONG len){ SYM_WIN(M_2PI, 0., scl::hann(phs)) }
TEM void welch         (T * dst, ULONG len){ SYM_WIN(2.   ,-1., scl::welch(phs)) }

#undef SYM_WIN

TEM void rectangle(T * dst, ULONG len){ mem::set(dst, gen::val((T)1), len); }

TEM void nyquist(T * dst, ULONG len){
	len >>= 1;
	LOOP_P(len,
		*dst++ = (T) 1;
		*dst++ = (T)-1;
	)
}



//void TblOp::decay(float * dst, ULONG len, float order){
//	float final = pow(2.f, order);
//	float lambda = log(final) / (float)len;
//	float time = 1.f;
//	float scale = 1.f / (1.f - final);
//	float offset = -final;
//	*dst++ = 1.f;
//	LOOP(len - 1,
//		*dst++ = (exp(lambda * time) + offset) * scale;
//		time++;
//	)
//}

inline ULONG maxHarmonics(ULONG len){ return len>>1; }


// Return value from a table containing the first quarter of a sine wave.
// The table size must be a power of two.
//
//	'qsin':		first quarter of a sine wave
//	'bits':		= 30 - (# bits in table)
//	'mask':		= (table size) - 1
//	'phase':	phase of lookup (2^32 is one period of the sine wave)
//
//	'phase' bit format (b = 'bits'):
//	32:			sign bit (0 = positive, 1 = negative)
//	31:			direction bit (0 = forward, 1 = backward)
//	[30, b]:	phase integer part
//	[ b, 0]:	phase fractional part

//inline float TblOp::atQ(float * qsin, ULONG bits, ULONG mask, ULONG phase){
//	union{float f; ULONG i;} val;
//	ULONG sign = phase & 0x80000000;
//	ULONG dir  = phase & 0x40000000;
//
//	phase >>= bits;			// integer part of index
//	
//	// Check direction and reverse index
//	if(dir){
//		phase = -phase; /* index = (len - index);  */
//		if((phase & mask) == 0){
//			val.i = 0x3f800000 | sign;
//			return val.f;
//		}
//	}
//	
//	val.f = qsin[phase & mask];
//	val.i |= sign;	// sign bit
//	return val.f;
//}

// 1 branch
//inline float TblOp::atQ(float * qsin, ULONG bits, ULONG mask, ULONG phase){
//	union{float f; ULONG i;} val;
//	ULONG sign = phase & 0x80000000;
//	ULONG dir  = (phase & 0x40000000) >> 30;	
//
//	phase >>= bits;	// integer part of index
//	
//	/*
//	ULONG phase2[2];
//	phase2[0] = phase;
//	phase2[1] = -phase;
//	phase = phase2[dir] & mask;
//	*/
//
//	phase = ((phase^-dir) + dir) & mask;	// 2s complement gives flipped phase
//
//	if(phase == 0){
//		val.i = 0x3f800000 & (-dir) | sign;
//		return val.f;
//	}
//	
//	val.f = qsin[phase];
//	val.i |= sign;	// sign bit
//	return val.f;
//}

// 0 branches
// requires n + 1 table with qsin[n] = 1
//inline float TblOp::atQ(float * qsin, ULONG bits, ULONG mask, ULONG phase){
//inline float TblOp::atQ(float * table, ULONG bits, ULONG phase){
//	union{float f; ULONG i;} val;
//	ULONG sign = phase & 0x80000000;
//	ULONG dir  = (phase & 0x40000000) >> 30;	// 0 = fwd or 1 = bwd
//
//	//phase >>= bits;	// integer part of index
//	
//	//ULONG tmp = phase >> 30;
//	//printBinaryUserInt32((phase^-tmp) + tmp, ".", "1");
//	
//	// ulong complementor(ulong v, ulong c){ return v^(-c) + c; }
//	//phase = ((phase^-dir) + dir) & ((mask << 1) | 1);
//	//phase = ((phase^(-dir)) + dir) & mask); // if mask = n<<1 - 1
//
//	phase = (((phase^-dir) + (dir<<bits)) & 0x7fffffff) >> bits;
//
//	val.f = table[phase];
//	val.i |= sign;	// sign bit
//	return val.f;
//}
///*
inline float atQ(const float * src, ULONG fbits, ULONG phase){
	union{float f; ULONG i;} val;
	ULONG sign = phase & 0x80000000;
	ULONG dir  = (phase & 0x40000000) >> 30;	// 0 = fwd or 1 = bwd
	val.f = src[(((phase^-dir) + (dir<<fbits)) & 0x7fffffff) >> fbits];
	val.i |= sign;	// sign bit
	return val.f;
}
//*/
/*
inline float TblOp::atQ(float * table, ULONG bits, ULONG phase){
	switch(phase>>30){
	case 0: return  table[( phase & 0x3fffffff) >> bits]; break;
	case 1: return  table[(-phase & 0x7fffffff) >> bits]; break;
	case 2: return -table[( phase & 0x3fffffff) >> bits]; break;
	default:return -table[(-phase & 0x7fffffff) >> bits]; break;
	}
}*/

inline float atH(const float * src, ULONG bits, ULONG phase){
	union{float f; ULONG i;} val;
	val.f = src[(phase & 0x7fffffff) >> bits];
	val.i |= phase & 0x80000000;				// sign bit
	return val.f;
}

// i: 0 1 2 3 4 5 6 7
// o: 0 1 2 3 0 1 2 3

//TEM inline T at_dq(const T * src, uint32_t len_2, uint32_t i){
//	return i < len_2 ? src[i] : -src[i - len_2];
//}


inline float fraction(ULONG bits, ULONG phase){	
	phase = phase << bits >> 9 | 0x3f800000;
	return scl::punUF32(phase) - 1.f;
}

inline float phaseIncFactor(double framesPerSec){
	//return float(4294967296. / framesPerSec);
	return float(65536. / framesPerSec) * 65536.;
}


inline void printHexArray(float * table, ULONG len, ULONG valuesPerLine){
	printf("{");
	LOOP(len,
		if((i % valuesPerLine) == 0) printf("\n\t");
		printf("0x%08x%s", *(unsigned int *)(table + i), i == len-1 ? "\n};" : ",");
	)
}


} // tbl::

} // gam::


#include "MacroU.h"

#endif

