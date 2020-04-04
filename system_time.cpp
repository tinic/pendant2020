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
#include "./system_time.h"
#include "./model.h"
#include "./timeline.h"

#include "stm32f401xc.h"
#include "stm32f4xx_hal.h"

extern "C" TIM_HandleTypeDef htim1;
extern "C" IWDG_HandleTypeDef hiwdg;

extern "C" void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim1) {
        HAL_IWDG_Refresh(&hiwdg);
    
        Model::instance().SetTime(system_time());
        
        Timeline::instance().ProcessEffect();
        Timeline::instance().ProcessDisplay();
    }
}

// Generate system time based on 32-bit cycle count
static uint64_t large_dwt_cyccnt() {

    volatile uint32_t *DWT_CYCCNT  = reinterpret_cast<volatile uint32_t *>(0xE0001004);
    volatile uint32_t *DWT_CONTROL = reinterpret_cast<volatile uint32_t *>(0xE0001000);
    volatile uint32_t *SCB_DEMCR   = reinterpret_cast<volatile uint32_t *>(0xE000EDFC);

    static uint32_t PREV_DWT_CYCCNT = 0;
    static uint64_t LARGE_DWT_CYCCNT = 0;

    static bool init = false;
    if (!init) {
        init = true;
        *SCB_DEMCR   = *SCB_DEMCR | 0x01000000;
        *DWT_CYCCNT  = 0; // reset the counter
        *DWT_CONTROL = 0; 
        *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter

    }

    uint32_t CURRENT_DWT_CYCCNT = *DWT_CYCCNT;

    // wrap around
    if (PREV_DWT_CYCCNT > CURRENT_DWT_CYCCNT) {
        LARGE_DWT_CYCCNT += 0x100000000UL;
    }

    PREV_DWT_CYCCNT = CURRENT_DWT_CYCCNT;

    return LARGE_DWT_CYCCNT + CURRENT_DWT_CYCCNT;
}

double system_time() {
    return double(static_cast<double>(large_dwt_cyccnt()) / 65536.0) * (1.0 / ( double(RCC_MAX_FREQUENCY) / 65536.0 ) );
}
