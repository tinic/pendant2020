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

#include "./pendant.h"
#include "./model.h"
#include "./timeline.h"
#include "./bq25895.h"
#include "./sdd1306.h"
#include "./system_time.h"
#include "./leds.h"

#include "main.h"

extern "C" TIM_HandleTypeDef htim2;
extern "C" IWDG_HandleTypeDef hiwdg;

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch(GPIO_Pin) {
        case SWITCH1_Pin: {
            if (HAL_GPIO_ReadPin(GPIOB, SWITCH1_Pin) == GPIO_PIN_SET) {
                Timeline::instance().ProcessDisplay();
                if (Timeline::instance().TopDisplay().Valid()) {
                    Timeline::instance().TopDisplay().ProcessSwitch1();
                }
                Model::instance().SetSwitch1Down(0.0);
            } else {
                Model::instance().SetSwitch1Down(system_time());
            }
        } break;
        case SWITCH2_Pin: {
            if (HAL_GPIO_ReadPin(GPIOB, SWITCH2_Pin) == GPIO_PIN_SET) {
                Timeline::instance().ProcessDisplay();
                if (Timeline::instance().TopDisplay().Valid()) {
                    Timeline::instance().TopDisplay().ProcessSwitch2();
                }
                Model::instance().SetSwitch2Down(0.0);
            } else {
                Model::instance().SetSwitch2Down(system_time());
            }
        } break;
        case SWITCH3_Pin: {
            if (HAL_GPIO_ReadPin(GPIOB, SWITCH3_Pin) == GPIO_PIN_SET) {
                Timeline::instance().ProcessDisplay();
                if (Timeline::instance().TopDisplay().Valid()) {
                    Timeline::instance().TopDisplay().ProcessSwitch3();
                }
                Model::instance().SetSwitch3Down(0.0);
            } else {
                Model::instance().SetSwitch3Down(system_time());
            }
        } break;
        case DSEL_Pin: {
        } break;
        case BQ_INT_Pin: {
        } break;
    }
}

Pendant &Pendant::instance() {
    static Pendant pendant;
    if (!pendant.initialized) {
        pendant.initialized = true;
        pendant.init();
    }
    return pendant;
}

void Pendant::init() {

    if (BQ25895::instance().DevicePresent()) {
        BQ25895::instance().SetBoostVoltage(4550);
        BQ25895::instance().DisableWatchdog();
        BQ25895::instance().DisableOTG();
        BQ25895::instance().OneShotADC();
        BQ25895::instance().SetInputCurrent(500);
    }

    if (SDD1306::instance().DevicePresent()) {
    }

    Leds::instance().start();
}

extern "C" I2C_HandleTypeDef hi2c1;

void Pendant::Run() {
    HAL_TIM_Base_Start_IT(&htim2);
    while (1) {
        __WFI();

        HAL_IWDG_Refresh(&hiwdg);

        static double effecttime = 0.0f;
        if ( ( Model::instance().Time() - effecttime ) >= (1.0 / 60.0)) {
            effecttime = Model::instance().Time();
            Timeline::instance().ProcessEffect();
            if (Timeline::instance().TopEffect().Valid()) {
                Timeline::instance().TopEffect().Calc();
                Timeline::instance().TopEffect().Commit();
            }
        }

        static double displaytime = 0.0f;
        if ( ( Model::instance().Time() - displaytime ) >= (1.0 / 30.0)) {
            displaytime = Model::instance().Time();
            Timeline::instance().ProcessDisplay();
            if (Timeline::instance().TopDisplay().Valid()) {
                Timeline::instance().TopDisplay().Calc();
                Timeline::instance().TopDisplay().Commit();
            }
        }
    }
}

void pendant_entry(void) {
    Pendant::instance().Run();
}
