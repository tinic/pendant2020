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
#ifndef MODEL_H_
#define MODEL_H_

#include "./color.h"

class Model {
public:
    static Model &instance();

    uint32_t Effect() { return effect; }
    void SetEffect(uint32_t neweffect) { effect = neweffect; }

    color::rgba<uint8_t> BirdColor() const { return bird_color; }
    void SetBirdColor(color::rgba<uint8_t> color) { bird_color = color; }

    color::rgba<uint8_t> RingColor() const { return ring_color; }
    void SetRingColor(color::rgba<uint8_t> color) { ring_color = color; }

    float Brightness() const { return brightness; }
    void SetBrightness(float newbrightness) { brightness = newbrightness; }

    double Time() const { return time; }
    void SetTime(double _time)  { time = _time; }

    bool IsSwitch1Down(double &timestamp) const { 
        if (switch_1_down_time != 0.0) {
            timestamp = switch_1_down_time;
            return true;
        } 
        else { 
            return false; 
        }
    }
    
    bool IsSwitch2Down(double &timestamp) const {
        if (switch_2_down_time != 0.0) {
            timestamp = switch_2_down_time;
            return true;
        } 
        else { 
            return false; 
        }
    }

    bool IsSwitch3Down(double &timestamp) const {
        if (switch_3_down_time != 0.0) {
            timestamp = switch_3_down_time;
            return true;
        } 
        else { 
            return false; 
        }
    }

    void SetSwitch1Down(double timestamp) { switch_1_down_time = timestamp; }
    void SetSwitch2Down(double timestamp) { switch_2_down_time = timestamp; }
    void SetSwitch3Down(double timestamp) { switch_3_down_time = timestamp; }

    static constexpr float MinBatteryVoltage() { return 3.5f; }
    static constexpr float MaxBatteryVoltage() { return 4.2f; }

    float BatteryVoltage() const  { return battery_voltage; }
    void SetBatteryVoltage(float voltage) { battery_voltage = voltage; }
    std::string BatteryVoltageString();

    float SystemVoltage() const  { return system_voltage; }
    void SetSystemVoltage(float voltage) { system_voltage = voltage; }
    std::string SystemVoltageString();

    float VbusVoltage() const  { return vbus_voltage; }
    void SetVbusVoltage(float voltage) { vbus_voltage = voltage; }
    std::string VbusVoltageString();

    float ChargeCurrent() const  { return charge_current; }
    void SetChargeCurrent(float current) { charge_current = current; }
    std::string ChargeCurrentString();

private:

    uint32_t effect = 0;

    float brightness = 1.0f;

    color::rgba<uint8_t> bird_color = color::rgba<uint8_t>(0xFF, 0xFF, 0x00);
    color::rgba<uint8_t> ring_color = color::rgba<uint8_t>(0x00, 0x2F, 0xFF);

    double time = 0.0;

    double switch_1_down_time = 0.0;
    double switch_2_down_time = 0.0;
    double switch_3_down_time = 0.0;

    float battery_voltage = 0;
    float system_voltage = 0;
    float vbus_voltage = 0;
    float charge_current = 0;

    void init();
    bool initialized = false;
};

#endif /* MODEL_H_ */
