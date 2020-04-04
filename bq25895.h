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
#ifndef BQ25895_H_
#define BQ25895_H_

#include <cstdint>

extern "C" {
    struct io_descriptor;
};

class BQ25895 {
     
public:
     BQ25895() {
         devicePresent = false;
         deviceChecked = false;
         fault_state = 0;
         I2C_0_io = 0;
     }
     
     static BQ25895 &instance();

     void SetBoostVoltage(uint32_t voltageMV);
     uint32_t GetBoostVoltage();

     void DisableWatchdog();
     void DisableOTG();

     void SetInputCurrent(uint32_t currentMA);
     uint32_t GetInputCurrent();

     void StartContinousADC();

     bool ADCActive();
     void OneShotADC();

     float BatteryVoltage();
     float SystemVoltage();
     float VBUSVoltage();
     float ChargeCurrent();

     uint8_t GetStatus();
     uint8_t FaultState();
     bool IsInFaultState();
     bool DevicePresent() const { return devicePresent; }

     uint8_t getRegister(uint8_t address);

 private:

     void PinInterrupt();
     static void PinInterrupt_C();

     static constexpr uint32_t i2caddr = 0x6A;

     void setRegister(uint8_t address, uint8_t value);
     void setRegisterBits(uint8_t address, uint8_t mask);
     void clearRegisterBits(uint8_t address, uint8_t mask);

     bool devicePresent;
     bool deviceChecked;
     uint8_t fault_state;
     
     struct io_descriptor *I2C_0_io;
};

#endif /* BQ25895_H_ */