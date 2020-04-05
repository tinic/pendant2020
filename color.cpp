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

__attribute__ ((hot, optimize("O3")))
void gradient::init(const vector::float4 stops[], size_t n) {
    initialized = true;
    for (size_t c = 0; c < colors_n; c++) {
        float f = static_cast<float>(c) / static_cast<float>(colors_n - 1); 
        vector::float4 a = stops[0];
        vector::float4 b = stops[1];
        if (n > 2) {
            for (int32_t d = static_cast<int32_t>(n-2); d >= 0 ; d--) {
                if ( f >= (stops[d].w) ) {
                    a = stops[d+0];
                    b = stops[d+1];
                    break;
                }
            }
        }
        f -= a.w;
        f /= b.w - a.w;
        colors[c] = a.lerp(b,f);
    }
}

__attribute__ ((hot, optimize("O3")))
vector::float4 gradient::repeat(float i) {
    i = fmodf(i, 1.0f);
    i *= colors_mul;
    return vector::float4::lerp(colors[(static_cast<size_t>(i))&colors_mask], colors[(static_cast<size_t>(i)+1)&colors_mask], fmodf(i, 1.0f));
}

__attribute__ ((hot, optimize("O3")))
vector::float4 gradient::reflect(float i) {
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

__attribute__ ((hot, optimize("O3")))
vector::float4 gradient::clamp(float i) {
    if (i <= 0.0f) {
        return colors[0];
    }
    if (i >= 1.0f) {
        return colors[colors_n-1];
    }
    i *= colors_mul;
    return vector::float4::lerp(colors[(static_cast<size_t>(i))&colors_mask], colors[(static_cast<size_t>(i)+1)&colors_mask], fmodf(i, 1.0f));
}

convert &convert::instance() {
    static convert c;
    if (!c.initialized) {
        c.initialized = true;
        c.init();
    }
    return c;
}

__attribute__ ((hot, optimize("O3")))
static float sRGBTransfer(float a) {
    if (a <= 0.0f) {
        return 0.0f;
    } else if (a < 0.0031308f) {
        return a * 12.92f;
    } else if ( a < 1.0f ) {
        return powf(a, 1.0f/2.4f) * 1.055f - 0.055f;
    } else {
        return 1.0f;
    }
}

__attribute__ ((hot, optimize("O3")))
vector::float4 convert::sRGB2CIELUV(const rgba<uint8_t> &in) {
    float r = sRGB2lRGB[in.r];
    float g = sRGB2lRGB[in.g];
    float b = sRGB2lRGB[in.b];

    float X = 0.4124564f * r + 0.3575761f * g + 0.1804375f * b;
    float Y = 0.2126729f * r + 0.7151522f * g + 0.0721750f * b;
    float Z = 0.0193339f * r + 0.1191920f * g + 0.9503041f * b;

    const float wu = 0.197839825f;
    const float wv = 0.468336303f;

    float l = ( Y <= 0.008856452f ) ? ( 9.03296296296f * Y) : ( 1.16f * powf(Y, 1.0f / 3.0f ) - 0.16f);
    float d = X + 15.f * Y + 3.0f * Z;

    return vector::float4(l,
        ( d > 0.001f ) ? ( 13.0f * l * ( ( 4.0f * X / d ) - wu ) ) : 0.0f,
        ( d > 0.001f ) ? ( 13.0f * l * ( ( 9.0f * Y / d ) - wv ) ) : 0.0f);
}

__attribute__ ((hot, optimize("O3")))
vector::float4 convert::CIELUV2sRGB(const vector::float4 &in) {
    const float wu = 0.197839825f;
    const float wv = 0.468336303f;

    float up_13l = in.y + wu * (13.0f * in.x);
    float vp_13l = in.z + wv * (13.0f * in.x);
    
    float Y = ( in.x + 0.16f ) * (1.0f / 1.16f);
    float y = ( in.x <= 0.08f ) ? ( in.x * 0.1107056f ) : ( Y * Y * Y );
    float x = ( vp_13l > 0.001f ) ? ( 2.25f * y * up_13l / vp_13l ) : 0.0f;
    float z = ( vp_13l > 0.001f ) ? ( y * ( 156.0f * in.x - 3.0f * up_13l - 20.0f * vp_13l ) / ( 4.0f * vp_13l )) : 0.0f;

    float r =  3.2404542f * x + -1.5371385f * y + -0.4985314f * z;
    float g = -0.9692660f * x +  1.8760108f * y +  0.0415560f * z;
    float b =  0.0556434f * x + -0.2040259f * y +  1.0572252f * z;

    return vector::float4(sRGBTransfer(r),sRGBTransfer(g),sRGBTransfer(b));
}

__attribute__ ((hot, optimize("O3")))
void convert::init() {
    float v = 0;
    for (size_t c = 0; c < 256; c++) {
        if (v > 0.04045f) {
            sRGB2lRGB[c] = powf( (v + 0.055f) / 1.055f, 2.4f);
        } else {
            sRGB2lRGB[c] = v * ( 1.0f * 12.92f );
        };
        v += 1.0f / 256.0f;
    }
}

}