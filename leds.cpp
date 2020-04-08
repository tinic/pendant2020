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

#include "stm32f401xc.h"
#include "stm32f4xx_hal.h"

#include <memory.h>

extern "C" SPI_HandleTypeDef hspi1;
extern "C" SPI_HandleTypeDef hspi2;

static constexpr color::gradient rainbow((const vector::float4[7]){
    vector::float4(color::convert().sRGB2CIELUV(color::rgba<uint8_t>(0xff,0x00,0x00)), 0.00f),
    vector::float4(color::convert().sRGB2CIELUV(color::rgba<uint8_t>(0xff,0xff,0x00)), 0.16f),
    vector::float4(color::convert().sRGB2CIELUV(color::rgba<uint8_t>(0x00,0xff,0x00)), 0.33f),
    vector::float4(color::convert().sRGB2CIELUV(color::rgba<uint8_t>(0x00,0xff,0xff)), 0.50f),
    vector::float4(color::convert().sRGB2CIELUV(color::rgba<uint8_t>(0x00,0x00,0xff)), 0.66f),
    vector::float4(color::convert().sRGB2CIELUV(color::rgba<uint8_t>(0xff,0x00,0xff)), 0.83f),
    vector::float4(color::convert().sRGB2CIELUV(color::rgba<uint8_t>(0xff,0x00,0x00)), 1.00f)},7);

static constexpr vector::float4 duck_yellow(color::convert().sRGB2CIELUV(color::rgba<uint8_t>(0xff,0xcc,0x00)));
static constexpr vector::float4 white(color::convert().sRGB2CIELUV(color::rgba<uint8_t>(0xff,0xff,0xff)));
static constexpr vector::float4 black(color::convert().sRGB2CIELUV(color::rgba<uint8_t>(0x00,0x00,0x00)));

Leds &Leds::instance() {
    static Leds leds;
    if (!leds.initialized) {
        leds.initialized = true;
        leds.init();
    }
    return leds;
}

const std::array<vector::float4, Leds::led_n> &Leds::pos() {
    static constexpr std::array<vector::float4, Leds::led_n> leds_pos = {
        // outer
        vector::float4(-0.00000000f,+1.00000000f,+0.00000000f,+0.00000000f),
        vector::float4(+0.38268343f,+0.92387953f,+0.00000000f,+0.00000000f),
        vector::float4(+0.70710678f,+0.70710678f,+0.00000000f,+0.00000000f),
        vector::float4(+0.92387953f,+0.38268343f,+0.00000000f,+0.00000000f),
        vector::float4(+1.00000000f,+0.00000000f,+0.00000000f,+0.00000000f),
        vector::float4(+0.92387953f,-0.38268343f,+0.00000000f,+0.00000000f),
        vector::float4(+0.70710678f,-0.70710678f,+0.00000000f,+0.00000000f),
        vector::float4(+0.38268343f,-0.92387953f,+0.00000000f,+0.00000000f),
        vector::float4(+0.00000000f,-1.00000000f,+0.00000000f,+0.00000000f),
        vector::float4(-0.38268343f,-0.92387953f,+0.00000000f,+0.00000000f),
        vector::float4(-0.70710678f,-0.70710678f,+0.00000000f,+0.00000000f),
        vector::float4(-0.92387953f,-0.38268343f,+0.00000000f,+0.00000000f),
        vector::float4(-1.00000000f,-0.00000000f,+0.00000000f,+0.00000000f),
        vector::float4(-0.92387953f,+0.38268343f,+0.00000000f,+0.00000000f),
        vector::float4(-0.70710678f,+0.70710678f,+0.00000000f,+0.00000000f),
        vector::float4(-0.38268343f,+0.92387953f,+0.00000000f,+0.00000000f),
        // inner
        vector::float4(+0.00000000f,+0.50000000f,+0.00000000f,+0.00000000f),
        vector::float4(-0.45833333f,+0.20833333f,+0.00000000f,+0.00000000f),
        vector::float4(-0.29166666f,+0.00000000f,+0.00000000f,+0.00000000f),
        vector::float4(+0.00000000f,+0.00000000f,+0.00000000f,+0.00000000f),
        vector::float4(+0.00000000f,-0.66666666f,+0.00000000f,+0.00000000f),
        vector::float4(+0.00000000f,-0.33333333f,+0.00000000f,+0.00000000f),
        vector::float4(+0.45833333f,-0.20833333f,+0.00000000f,+0.00000000f),
        vector::float4(+0.29166666f,-0.00000000f,+0.00000000f,+0.00000000f)};
    return leds_pos;
}

const std::array<vector::float4, Leds::led_inner_n> &Leds::pos_inner() {
    constexpr float e = 1.5f;
    static constexpr std::array<vector::float4, Leds::led_inner_n> leds_pos = {
        vector::float4(+0.00000000f*e,+0.50000000f*e,+0.00000000f,+0.00000000f),
        vector::float4(-0.45833333f*e,+0.20833333f*e,+0.00000000f,+0.00000000f),
        vector::float4(-0.29166666f*e,+0.00000000f*e,+0.00000000f,+0.00000000f),
        vector::float4(+0.00000000f*e,+0.00000000f*e,+0.00000000f,+0.00000000f),
        vector::float4(+0.00000000f*e,-0.66666666f*e,+0.00000000f,+0.00000000f),
        vector::float4(+0.00000000f*e,-0.33333333f*e,+0.00000000f,+0.00000000f),
        vector::float4(+0.45833333f*e,-0.20833333f*e,+0.00000000f,+0.00000000f),
        vector::float4(+0.29166666f*e,-0.00000000f*e,+0.00000000f,+0.00000000f)
    };
    return leds_pos;
}

void Leds::init() {
    nothing();
}

void Leds::set_inner_flat(const vector::float4 &col) {
    std::fill(&leds[0][led_outer_n], &leds[0][led_outer_n] + led_inner_n, col);
    std::fill(&leds[1][led_outer_n], &leds[1][led_outer_n] + led_inner_n, col);
}

void Leds::set_outer_flat(const vector::float4 &col) {
    std::fill(&leds[0][0], &leds[0][0] + led_outer_n, col);
    std::fill(&leds[1][0], &leds[1][0] + led_outer_n, col);
}

void Leds::nothing() {
    std::fill(&leds[0][0], &leds[0][0] + face_n * led_n, black);
}

void Leds::color_walker() {
    double now = Model::instance().Time();

    const double speed = 0.25f;

    float val_walk = (1.0f - static_cast<float>(fmodf(static_cast<float>(now * speed), 1.0)));

    float inner_b = 0.20f * (sinf(static_cast<float>(now) * .5f)+1.0f);
    float outer_b = 0.01f * (sinf(static_cast<float>(now) * .5f)+1.0f);

    for (size_t c = 0; c < led_inner_n; c++) {
        vector::float4 p = pos_inner()[c];
        leds[0][led_outer_n + c] = ( duck_yellow + white * powf(1.0f - p.len(), 3.0f)) * inner_b;
        leds[1][led_outer_n + c] = ( duck_yellow + white * powf(1.0f - p.len(), 3.0f)) * inner_b;
    }

    for (size_t c = 0; c < led_outer_n; c++) {
        float mod_walk = val_walk + (1.0f - (c * ( 1.0f / static_cast<float>(led_outer_n) ) ) );
        
        vector::float4 out = rainbow.repeat(mod_walk) * outer_b;

        leds[0][c] = out;
        leds[1][c] = out;
    }
}

extern "C" void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);

void Leds::start() {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_SPI_TxCpltCallback(&hspi1);
    HAL_SPI_TxCpltCallback(&hspi2);

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
                nothing();
                break;
                case 1:
                color_walker();
                break;
            }
        };

        double blend_duration = 0.5;
        double now = Model::instance().Time();
        
        if ((now - switch_time) < blend_duration) {
            calc_effect(previous_effect);

            multi_array<vector::float4, face_n, led_n> leds_prev;
            std::copy(&leds_prev[0][0], &leds_prev[0][0] + face_n * led_n, &leds[0][0]);

            calc_effect(current_effect);

            float blend = static_cast<float>(now - switch_time) * (1.0f / static_cast<float>(blend_duration));

            for (size_t d = 0; d < face_n; d++) {
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

__attribute__ ((hot, optimize("O3"), flatten))
void Leds::commit() {
    constexpr size_t preamble_len = 256 * 4;
    constexpr size_t array_len = Leds::led_n * 3 * 16 * 8 + preamble_len;

    static uint8_t ws2816buf[2][array_len];

    auto convert_to_one_wire = [] (uint8_t *p, uint16_t v) {
        // TODO: optimize using DSP instructions
        for (uint32_t c = 0; c < 16; c++) {
            if ( ((1<<(15-c)) & v) != 0 ) {
                *p++ = 0b11110000;
            } else {
                *p++ = 0b10000000;
            }
        }
        return p;
    };

    uint8_t *ptr0 = &ws2816buf[0][0];
    memset(ptr0, 0, sizeof(uint8_t) * preamble_len);
    ptr0 += preamble_len;

    uint8_t *ptr1 = &ws2816buf[1][0];
    memset(ptr1, 0, sizeof(uint8_t) * preamble_len);
    ptr1 += preamble_len;

    const uint8_t map[2][led_n] = {
        {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
        {  0, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1, 16, 17, 18, 19, 23, 22, 21, 20},
    };

    static color::convert converter;

    for (size_t c = 0; c < Leds::led_n; c++) {
        color::rgba<uint16_t> pixel0(color::rgba<uint16_t>(converter.CIELUV2sRGB(leds[0][map[0][c]])).fix_for_ws2816());
        ptr0 = convert_to_one_wire(ptr0, pixel0.g);
        ptr0 = convert_to_one_wire(ptr0, pixel0.r);
        ptr0 = convert_to_one_wire(ptr0, pixel0.b);

        color::rgba<uint16_t> pixel1(color::rgba<uint16_t>(converter.CIELUV2sRGB(leds[1][map[1][c]])).fix_for_ws2816());
        ptr1 = convert_to_one_wire(ptr1, pixel1.g);
        ptr1 = convert_to_one_wire(ptr1, pixel1.r);
        ptr1 = convert_to_one_wire(ptr1, pixel1.b);
    }

    HAL_SPI_DMAStop(&hspi1);
    HAL_SPI_DMAStop(&hspi2);

    HAL_SPI_Transmit_DMA(&hspi1, &ws2816buf[0][0], array_len);
    HAL_SPI_Transmit_DMA(&hspi2, &ws2816buf[1][0], array_len);
}
