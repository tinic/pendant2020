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
#include "./leds.h"
#include "./vector.h"
#include "./timeline.h"
#include "./model.h"

#include <memory.h>

Leds &Leds::instance() {
    static Leds leds;
    if (!leds.initialized) {
        leds.initialized = true;
        leds.init();
    }
    return leds;
}

const std::array<vector::float4, Leds::led_n> &Leds::pos() {
    static std::array<vector::float4, Leds::led_n> leds_pos;
    static bool init = false;
    if (!init) {
        init = true;
        // outer
        leds_pos[ 0] = vector::float4(-0.00000000f,+1.00000000f,+0.00000000f,+0.00000000f);
        leds_pos[ 1] = vector::float4(+0.38268343f,+0.92387953f,+0.00000000f,+0.00000000f);
        leds_pos[ 2] = vector::float4(+0.70710678f,+0.70710678f,+0.00000000f,+0.00000000f);
        leds_pos[ 3] = vector::float4(+0.92387953f,+0.38268343f,+0.00000000f,+0.00000000f);
        leds_pos[ 4] = vector::float4(+1.00000000f,+0.00000000f,+0.00000000f,+0.00000000f);
        leds_pos[ 5] = vector::float4(+0.92387953f,-0.38268343f,+0.00000000f,+0.00000000f);
        leds_pos[ 6] = vector::float4(+0.70710678f,-0.70710678f,+0.00000000f,+0.00000000f);
        leds_pos[ 7] = vector::float4(+0.38268343f,-0.92387953f,+0.00000000f,+0.00000000f);
        leds_pos[ 8] = vector::float4(+0.00000000f,-1.00000000f,+0.00000000f,+0.00000000f);
        leds_pos[ 9] = vector::float4(-0.38268343f,-0.92387953f,+0.00000000f,+0.00000000f);
        leds_pos[10] = vector::float4(-0.70710678f,-0.70710678f,+0.00000000f,+0.00000000f);
        leds_pos[11] = vector::float4(-0.92387953f,-0.38268343f,+0.00000000f,+0.00000000f);
        leds_pos[12] = vector::float4(-1.00000000f,-0.00000000f,+0.00000000f,+0.00000000f);
        leds_pos[13] = vector::float4(-0.92387953f,+0.38268343f,+0.00000000f,+0.00000000f);
        leds_pos[14] = vector::float4(-0.70710678f,+0.70710678f,+0.00000000f,+0.00000000f);
        leds_pos[15] = vector::float4(-0.38268343f,+0.92387953f,+0.00000000f,+0.00000000f);
        // inner
        leds_pos[16] = vector::float4(+0.00000000f,+0.50000000f,+0.00000000f,+0.00000000f);
        leds_pos[17] = vector::float4(-0.45833333f,+0.20833333f,+0.00000000f,+0.00000000f);
        leds_pos[18] = vector::float4(-0.29166666f,+0.00000000f,+0.00000000f,+0.00000000f);
        leds_pos[19] = vector::float4(+0.00000000f,+0.00000000f,+0.00000000f,+0.00000000f);
        leds_pos[20] = vector::float4(+0.00000000f,-0.66666666f,+0.00000000f,+0.00000000f);
        leds_pos[21] = vector::float4(+0.00000000f,-0.33333333f,+0.00000000f,+0.00000000f);
        leds_pos[22] = vector::float4(+0.45833333f,-0.20833333f,+0.00000000f,+0.00000000f);
        leds_pos[23] = vector::float4(+0.29166666f,-0.00000000f,+0.00000000f,+0.00000000f);
    }
    return leds_pos;
}

void Leds::init() {
    black();
}

void Leds::black() {
    memset(leds, 0, sizeof(leds));
}

void Leds::start() {
    static Timeline::Span span;

    static uint32_t current_effect = 0;
    static uint32_t previous_effect = 0;
    static double switch_time = 0;

    span.type = Timeline::Span::Effect;
    span.time = 0;
    span.duration = std::numeric_limits<double>::infinity();
    span.calcFunc = [=](Timeline::Span &, Timeline::Span &) {

        if ( current_effect != Model::instance().Effect() ) {
            previous_effect = current_effect;
            current_effect = Model::instance().Effect();
            switch_time = Model::instance().Time();
        }

        auto calc_effect = [=] (uint32_t effect) mutable {
            switch (effect) {
                case 0:
                black();
                break;
            }
        };

        double blend_duration = 0.5;
        double now = Model::instance().Time();
        
        if ((now - switch_time) < blend_duration) {
            calc_effect(previous_effect);

            vector::float4 leds_prev[2][Leds::led_n];
            memcpy(leds_prev, leds, sizeof(leds));

            calc_effect(current_effect);

            float blend = static_cast<float>(now - switch_time) * (1.0f / static_cast<float>(blend_duration));

            for (size_t d = 0; d < 2; d++) {
                for (size_t c = 0; c < Leds::led_n; c++) {
                    leds[d][c] = vector::float4::lerp(leds_prev[d][c], leds[d][c], blend);
                }
            }

        } else {
            calc_effect(current_effect);
        }
    };

    span.commitFunc = [=](Timeline::Span &) {
        commit();
    };

    Timeline::instance().Add(span);
    current_effect = Model::instance().Effect();
    switch_time = Model::instance().Time();
}


__attribute__ ((hot, optimize("O3")))
void Leds::commit() {
    constexpr size_t preamble_len = 256;
    constexpr size_t array_len = Leds::led_n * 3 * 16 + preamble_len;

    static uint32_t ws2816buf[2][array_len];

    auto convert_to_one_wire = [] (uint32_t *ptr, uint32_t p) {
        *ptr++ = 0x88888888UL |
                (((p >>  4) | (p <<  6) | (p << 16) | (p << 26)) & 0x04040404)|
                (((p >>  1) | (p <<  9) | (p << 19) | (p << 29)) & 0x40404040);
        return ptr;
    };

    for (size_t d = 0; d < 2; d++) {
        uint32_t *ptr = &ws2816buf[d][0];
        memset(ptr, 0, sizeof(uint32_t) * preamble_len);
        ptr += preamble_len;
        for (size_t c = 0; c < Leds::led_n; c++) {
            color::rgba<uint16_t> pixel(color::rgba<uint16_t>(color::convert::instance().CIELUV2lRGB(leds[d][c])).fix_for_ws2816());
            ptr = convert_to_one_wire(ptr, (pixel.g >> 8) & 0xFF);
            ptr = convert_to_one_wire(ptr, (pixel.g >> 0) & 0xFF);
            ptr = convert_to_one_wire(ptr, (pixel.r >> 8) & 0xFF);
            ptr = convert_to_one_wire(ptr, (pixel.r >> 8) & 0xFF);
            ptr = convert_to_one_wire(ptr, (pixel.b >> 8) & 0xFF);
            ptr = convert_to_one_wire(ptr, (pixel.b >> 8) & 0xFF);
        }
    }
}
