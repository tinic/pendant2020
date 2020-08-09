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

#include "stm32f401xc.h"

#include "./bootloader.h"
#include "./bq25895.h"

#include "main.h"

extern "C" IWDG_HandleTypeDef hiwdg;

extern "C" {
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
        (void)GPIO_Pin;
    }
}

Bootloader &Bootloader::instance() {
    static Bootloader bootloader;
    if (!bootloader.initialized) {
        bootloader.initialized = true;
        bootloader.init();
    }
    return bootloader;
}

void Bootloader::init() {
    if (BQ25895::instance().DevicePresent()) {
        BQ25895::instance().SetBoostVoltage(4550);
        BQ25895::instance().DisableWatchdog();
        BQ25895::instance().DisableOTG();
        BQ25895::instance().OneShotADC();
        BQ25895::instance().SetInputCurrent(500);
    }
}

void Bootloader::Run() {
    while (1) {
        __WFI();
        HAL_IWDG_Refresh(&hiwdg);
    }
}

void bootloader_entry(void) {
    Bootloader::instance().Run();
}
