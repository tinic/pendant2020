/*
Copyright 2019 Tinic Uro

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
#include "sdd1306.h"

#include "stm32f401xc.h"
#include "stm32f4xx_hal.h"

#include "./stm32/Inc/main.h"

extern "C" I2C_HandleTypeDef hi2c1;

SDD1306::SDD1306() {
    devicePresent = false;
    deviceChecked = false;
}

SDD1306 &SDD1306::instance() {
    static SDD1306 sdd1306;
    if (!sdd1306.deviceChecked) {
        sdd1306.deviceChecked = true;
        if (sdd1306.init()) {
            // https://jure.tuta.si/?p=7 ... This is asinine
            sdd1306.devicePresent = true;
        }
    }
    return sdd1306;
}

bool SDD1306::init() const {
    HAL_GPIO_WritePin(GPIOB, OLED_CS_Pin, GPIO_PIN_SET);

    // Toggle RESET line
    HAL_GPIO_WritePin(GPIOB, OLED_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, OLED_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOB, OLED_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(20);

    for( ; HAL_I2C_IsDeviceReady(&hi2c1, i2caddr<<1, 1, HAL_MAX_DELAY) != HAL_OK ;) { }

    uint8_t status = 0x8;
    uint8_t value = 0x0;

    if (HAL_I2C_Master_Transmit(&hi2c1, i2caddr<<1, &status, 1, HAL_MAX_DELAY) == HAL_OK) {

        if(HAL_I2C_Master_Receive(&hi2c1, i2caddr<<1, &value, 1, HAL_MAX_DELAY) == HAL_OK) {
            static uint8_t startup_sequence[] = {
                0xAE,			// Display off
                0xD5, 0x80,		// Set Display Clock Divide Ratio
                0xA8, 0x1F,		// Set Multiplex Ratio
                0xD3, 0x00,		// Set Display Offset
                0x8D, 0x14,		// Enable Charge Pump
                0x40,			// Set Display RAM start
                0xA6,			// Set to normal display (0xA7 == inverse)
                0xA4,			// Force Display From RAM On
                0xA1,			// Set Segment Re-map
                0xC8,			// Set COM Output Scan Direction (flipped)
                0xDA, 0x12, 	// Set Pins configuration
                0x81, 0x00,		// Set Contrast (0x00-0xFF)
                0xD9, 0xF1,		// Set Pre-Charge period
                0xDB, 0x40,		// Adjust Vcomm regulator output
                0xAF			// Display on
            };

            for (size_t c = 0; c < sizeof(startup_sequence); c++) {
                writeCommand(startup_sequence[c]);
            }

            return true;
        }

    }

    return false;
}

void SDD1306::writeCommand(uint8_t v) const {
    uint8_t control[2];
    control[0] = 0;
    control[1] = v;
    HAL_I2C_Master_Transmit(&hi2c1, i2caddr<<1, control, 2, HAL_MAX_DELAY);
}
