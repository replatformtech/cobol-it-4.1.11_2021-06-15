/* #include <stdint.h>*/
#include "digitslut.h"

// Branching for different cases (forward)
// Use lookup table of two digits

int u32toa_branchlut(unsigned int value, char* buffer) {
    char *start = buffer;
    if (value < 10000) {
        const unsigned int d1 = (value / 100) << 1;
        const unsigned int d2 = (value % 100) << 1;
        
        if (value >= 1000)
            *buffer++ = gDigitsLut[d1];
        if (value >= 100)
            *buffer++ = gDigitsLut[d1 + 1];
        if (value >= 10)
            *buffer++ = gDigitsLut[d2];
        *buffer++ = gDigitsLut[d2 + 1];
    }
    else if (value < 100000000) {
        // value = bbbbcccc
        const unsigned int b = value / 10000;
        const unsigned int c = value % 10000;
        
        const unsigned int d1 = (b / 100) << 1;
        const unsigned int d2 = (b % 100) << 1;
        
        const unsigned int d3 = (c / 100) << 1;
        const unsigned int d4 = (c % 100) << 1;
        
        if (value >= 10000000)
            *buffer++ = gDigitsLut[d1];
        if (value >= 1000000)
            *buffer++ = gDigitsLut[d1 + 1];
        if (value >= 100000)
            *buffer++ = gDigitsLut[d2];
        *buffer++ = gDigitsLut[d2 + 1];
        
        *buffer++ = gDigitsLut[d3];
        *buffer++ = gDigitsLut[d3 + 1];
        *buffer++ = gDigitsLut[d4];
        *buffer++ = gDigitsLut[d4 + 1];
    }
    else {
        // value = aabbbbcccc in decimal
        
        const unsigned int a = value / 100000000; // 1 to 42
        value %= 100000000;
        
        if (a >= 10) {
            const unsigned i = a << 1;
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
        }
        else
            *buffer++ = '0' + (int)(a);
		{
			const unsigned int b = value / 10000; // 0 to 9999
			const unsigned int c = value % 10000; // 0 to 9999

			const unsigned int d1 = (b / 100) << 1;
			const unsigned int d2 = (b % 100) << 1;

			const unsigned int d3 = (c / 100) << 1;
			const unsigned int d4 = (c % 100) << 1;

			*buffer++ = gDigitsLut[d1];
			*buffer++ = gDigitsLut[d1 + 1];
			*buffer++ = gDigitsLut[d2];
			*buffer++ = gDigitsLut[d2 + 1];
			*buffer++ = gDigitsLut[d3];
			*buffer++ = gDigitsLut[d3 + 1];
			*buffer++ = gDigitsLut[d4];
			*buffer++ = gDigitsLut[d4 + 1];
		}
    }
    *buffer++ = '\0';
    return buffer -start;
}


int u64toa_branchlut(unsigned long long value, char* buffer) {
    char* start = buffer;
    if (value < 100000000) {
        unsigned int v = (unsigned int)(value);
        if (v < 10000) {
            const unsigned int d1 = (v / 100) << 1;
            const unsigned int d2 = (v % 100) << 1;
            
            if (v >= 1000)
                *buffer++ = gDigitsLut[d1];
            if (v >= 100)
                *buffer++ = gDigitsLut[d1 + 1];
            if (v >= 10)
                *buffer++ = gDigitsLut[d2];
            *buffer++ = gDigitsLut[d2 + 1];
        }
        else {
            // value = bbbbcccc
            const unsigned int b = v / 10000;
            const unsigned int c = v % 10000;
            
            const unsigned int d1 = (b / 100) << 1;
            const unsigned int d2 = (b % 100) << 1;
            
            const unsigned int d3 = (c / 100) << 1;
            const unsigned int d4 = (c % 100) << 1;
            
            if (value >= 10000000)
                *buffer++ = gDigitsLut[d1];
            if (value >= 1000000)
                *buffer++ = gDigitsLut[d1 + 1];
            if (value >= 100000)
                *buffer++ = gDigitsLut[d2];
            *buffer++ = gDigitsLut[d2 + 1];
            
            *buffer++ = gDigitsLut[d3];
            *buffer++ = gDigitsLut[d3 + 1];
            *buffer++ = gDigitsLut[d4];
            *buffer++ = gDigitsLut[d4 + 1];
        }
    }
    else if (value < 10000000000000000LL) {
        const unsigned int v0 = (unsigned int)(value / 100000000);
        const unsigned int v1 = (unsigned int)(value % 100000000);
        
        const unsigned int b0 = v0 / 10000;
        const unsigned int c0 = v0 % 10000;
        
        const unsigned int d1 = (b0 / 100) << 1;
        const unsigned int d2 = (b0 % 100) << 1;
        
        const unsigned int d3 = (c0 / 100) << 1;
        const unsigned int d4 = (c0 % 100) << 1;

        const unsigned int b1 = v1 / 10000;
        const unsigned int c1 = v1 % 10000;
        
        const unsigned int d5 = (b1 / 100) << 1;
        const unsigned int d6 = (b1 % 100) << 1;
        
        const unsigned int d7 = (c1 / 100) << 1;
        const unsigned int d8 = (c1 % 100) << 1;

        if (value >= 1000000000000000LL)
            *buffer++ = gDigitsLut[d1];
        if (value >= 100000000000000LL)
            *buffer++ = gDigitsLut[d1 + 1];
        if (value >= 10000000000000LL)
            *buffer++ = gDigitsLut[d2];
        if (value >= 1000000000000LL)
            *buffer++ = gDigitsLut[d2 + 1];
        if (value >= 100000000000LL)
            *buffer++ = gDigitsLut[d3];
        if (value >= 10000000000LL)
            *buffer++ = gDigitsLut[d3 + 1];
        if (value >= 1000000000LL)
            *buffer++ = gDigitsLut[d4];
        if (value >= 100000000LL)
            *buffer++ = gDigitsLut[d4 + 1];
        
        *buffer++ = gDigitsLut[d5];
        *buffer++ = gDigitsLut[d5 + 1];
        *buffer++ = gDigitsLut[d6];
        *buffer++ = gDigitsLut[d6 + 1];
        *buffer++ = gDigitsLut[d7];
        *buffer++ = gDigitsLut[d7 + 1];
        *buffer++ = gDigitsLut[d8];
        *buffer++ = gDigitsLut[d8 + 1];
    }
    else {
        const unsigned int a = (unsigned int)(value / 10000000000000000LL); // 1 to 1844
        value %= 10000000000000000LL;
        
        if (a < 10)
            *buffer++ = '0' + (char)(a);
        else if (a < 100) {
            const unsigned int i = a << 1;
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
        }
        else if (a < 1000) {
			const unsigned int i = (a % 100) << 1;
			*buffer++ = '0' + (char)(a / 100);
            
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
        }
        else {
            const unsigned int i = (a / 100) << 1;
            const unsigned int j = (a % 100) << 1;
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
            *buffer++ = gDigitsLut[j];
            *buffer++ = gDigitsLut[j + 1];
        }
        
		{
			const unsigned int v0 = (unsigned int)(value / 100000000);
			const unsigned int v1 = (unsigned int)(value % 100000000);

			const unsigned int b0 = v0 / 10000;
			const unsigned int c0 = v0 % 10000;

			const unsigned int d1 = (b0 / 100) << 1;
			const unsigned int d2 = (b0 % 100) << 1;

			const unsigned int d3 = (c0 / 100) << 1;
			const unsigned int d4 = (c0 % 100) << 1;

			const unsigned int b1 = v1 / 10000;
			const unsigned int c1 = v1 % 10000;

			const unsigned int d5 = (b1 / 100) << 1;
			const unsigned int d6 = (b1 % 100) << 1;

			const unsigned int d7 = (c1 / 100) << 1;
			const unsigned int d8 = (c1 % 100) << 1;

			*buffer++ = gDigitsLut[d1];
			*buffer++ = gDigitsLut[d1 + 1];
			*buffer++ = gDigitsLut[d2];
			*buffer++ = gDigitsLut[d2 + 1];
			*buffer++ = gDigitsLut[d3];
			*buffer++ = gDigitsLut[d3 + 1];
			*buffer++ = gDigitsLut[d4];
			*buffer++ = gDigitsLut[d4 + 1];
			*buffer++ = gDigitsLut[d5];
			*buffer++ = gDigitsLut[d5 + 1];
			*buffer++ = gDigitsLut[d6];
			*buffer++ = gDigitsLut[d6 + 1];
			*buffer++ = gDigitsLut[d7];
			*buffer++ = gDigitsLut[d7 + 1];
			*buffer++ = gDigitsLut[d8];
			*buffer++ = gDigitsLut[d8 + 1];
		}
    }
    
    *buffer = '\0';
    return buffer -start;
}



