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
#include "./color.h"

namespace color {

__attribute__ ((hot, optimize("O3"), flatten))
vector::float4 gradient::repeat(float i) const {
    i = fmodf(i, 1.0f);
    i *= colors_mul;
    return vector::float4::lerp(colors[(static_cast<size_t>(i))&colors_mask], colors[(static_cast<size_t>(i)+1)&colors_mask], fmodf(i, 1.0f));
}

__attribute__ ((hot, optimize("O3"), flatten))
vector::float4 gradient::reflect(float i) const {
    i = fabsf(i);
    if ((static_cast<int32_t>(i) & 1) == 0) {
        i = fmodf(i, 1.0f);
    } else {
        i = fmodf(i, 1.0f);
        i = 1.0f - i;
    }
    i *= colors_mul;
    return vector::float4::lerp(colors[(static_cast<size_t>(i))&colors_mask], colors[(static_cast<size_t>(i)+1)&colors_mask], fmodf(i, 1.0f));
}

__attribute__ ((hot, optimize("O3"), flatten))
vector::float4 gradient::clamp(float i) const {
    if (i <= 0.0f) {
        return colors[0];
    }
    if (i >= 1.0f) {
        return colors[colors_n-1];
    }
    i *= colors_mul;
    return vector::float4::lerp(colors[(static_cast<size_t>(i))&colors_mask], colors[(static_cast<size_t>(i)+1)&colors_mask], fmodf(i, 1.0f));
}

static float fast_exp2(const float p) {
    const float offset = (p < 0) ? 1.0f : 0.0f;
    const float clipp = (p < -126) ? -126.0f : p;
    const int w = static_cast<int>(clipp);
    const float z = clipp - w + offset;
    const union { uint32_t i; float f; } v = {
        static_cast<uint32_t>((1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z))
    };
    return v.f;
}

static float fast_log2(const float x) {
    const union { float f; uint32_t i; } vx = { x };
    const union { uint32_t i; float f; } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
    const float y = static_cast<float>(vx.i) * 1.1920928955078125e-7f;
    return y - 124.22551499f
             - 1.498030302f * mx.f
             - 1.72587999f / (0.3520887068f + mx.f);
}

static float fast_pow(const float x, const float p) {
    return fast_exp2(p * fast_log2(x));
}

__attribute__ ((hot, optimize("O3"), flatten))
vector::float4 convert::CIELUV2sRGB(const vector::float4 &in) const {
    const float wu = 0.197839825f;
    const float wv = 0.468336303f;

    float up_13l = in.y + wu * (13.0f * in.x);
    float vp_13l = in.z + wv * (13.0f * in.x);
    float vp_13li = 1.0f / vp_13l;
    
    float Y = ( in.x + 0.16f ) * (1.0f / 1.16f);
    float y = ( in.x <= 0.08f ) ? ( in.x * 0.1107056f ) : ( Y * Y * Y );
    float x = (vp_13l > (1.0f / 65536.0f)) ? ( 2.25f * y * up_13l * vp_13li ) : 0.0f;
    float z = (vp_13l > (1.0f / 65536.0f)) ? ( y * ( 156.0f * in.x - 3.0f * up_13l - 20.0f * vp_13l ) * (1.0f / 4.0f) * vp_13li ) : 0.0f;

    float r =  3.2404542f * x + -1.5371385f * y + -0.4985314f * z;
    float g = -0.9692660f * x +  1.8760108f * y +  0.0415560f * z;
    float b =  0.0556434f * x + -0.2040259f * y +  1.0572252f * z;

    auto sRGBTransfer = [] (float a) {
        if (a <= 0.0f) {
            return 0.0f;
        } else if (a < 0.0031308f) {
            return a * 12.92f;
        } else if ( a < 0.999982f ) {
            return fast_pow(a, 1.0f / 2.4f) * 1.055f - 0.055f;
        } else {
            return 1.0f;
        }
    };

    return vector::float4(sRGBTransfer(r),sRGBTransfer(g),sRGBTransfer(b));
}

}
