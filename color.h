/*
Copyright 2020 Tinic Uro

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _COLOR_H_
#define _COLOR_H_

#include <cstdint>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <array>

#include "./vector.h"

namespace color {

    template<class T> struct rgba {

        T r;
        T g;
        T b;
        T a;

        rgba() :
            r(0),
            g(0),
            b(0),
            a(0){
        }
    
        rgba(const rgba &from) :
            r(from.r),
            g(from.g),
            b(from.b),
            a(from.a) {
        }

        explicit rgba(T _r, T _g, T _b) :
            r(_r),
            g(_g),
            b(_b),
            a(0) {
        }

        explicit rgba(T _r, T _g, T _b, T _a) :
            r(_r),
            g(_g),
            b(_b),
            a(_a) {
        }

        explicit rgba(const vector::float4 &from) {
            r = clamp_to_type(from.x);
            g = clamp_to_type(from.y);
            b = clamp_to_type(from.z);
            a = clamp_to_type(from.w);
        }

        rgba<T>& operator=(const rgba<T>& other) = default;

        rgba<T> fix_for_ws2816();

        uint8_t *write_grb_bytes(uint8_t *dst);

    private:
        T clamp_to_type(float v);
    };

    template<> inline rgba<uint16_t> rgba<uint16_t>::fix_for_ws2816() {
        return rgba<uint16_t>(  r < 384 ? ( ( r * 256 ) / 384 ) : r,
                                g < 384 ? ( ( g * 256 ) / 384 ) : g,
                                b < 384 ? ( ( b * 256 ) / 384 ) : b,
                                a < 384 ? ( ( a * 256 ) / 384 ) : a);
    }

    
    template<> __attribute__((always_inline)) inline float rgba<float>::clamp_to_type(float v) {
        return v;
    }

    template<> __attribute__((always_inline)) inline uint8_t rgba<uint8_t>::clamp_to_type(float v) {
        return v < 0.0f ? uint8_t(0) : ( v > 1.0f ? uint8_t(0xFF) : uint8_t( v * 255.f ) );
    }

    template<> __attribute__((always_inline)) inline uint16_t rgba<uint16_t>::clamp_to_type(float v) {
        return v < 0.0f ? uint16_t(0) : ( v > 1.0f ? uint16_t(0xFFFF) : uint16_t( v * 65535.f ) );
    }

    template<> __attribute__((always_inline)) inline uint8_t *rgba<uint8_t>::write_grb_bytes(uint8_t *dst) {
        *dst++ = g;
        *dst++ = r;
        *dst++ = b;
        return dst;
    }

    template<> inline uint8_t *rgba<uint16_t>::write_grb_bytes(uint8_t *dst) {
        *dst++ = (g >> 8) & 0xFF;
        *dst++ = (g >> 0) & 0xFF;
        *dst++ = (r >> 8) & 0xFF;
        *dst++ = (r >> 0) & 0xFF;
        *dst++ = (b >> 8) & 0xFF;
        *dst++ = (b >> 0) & 0xFF;
        return dst;
    }

    class gradient {
    public:
        void init(const vector::float4 stops[], size_t n);
        bool check_init() const { return !initialized; }

        vector::float4 repeat(float i);
        vector::float4 reflect(float i);
        vector::float4 clamp(float i);

    private:
        static constexpr size_t colors_n = 256;
        static constexpr float colors_mul = 255.0;
        static constexpr size_t colors_mask = 0xFF;
        vector::float4 colors[colors_n];
        bool initialized = false;
    };

    class convert {
    public:
        static convert &instance();

        vector::float4 sRGB2CIELUV(const rgba<uint8_t> &);
        vector::float4 CIELUV2sRGB(const vector::float4 &);

    private:
        bool initialized = false;
        void init();
        std::array<float, 256> sRGB2lRGB;
    };

}

#endif  // #ifndef _COLOR_H_
