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
#include "./model.h"

Model &Model::instance() {
    static Model model;
    if (!model.initialized) {
        model.initialized = true;
        model.init();
    }
    return model;
}

void Model::init() {
}

std::string Model::BatteryVoltageString() {
    char str[8];
    int32_t i = static_cast<int32_t>(BatteryVoltage());
    int32_t f = static_cast<int32_t>(fmodf(BatteryVoltage(),1.0f)*100);
    sprintf(str,"%1d.%02d", int(i), int(f));
    return std::string(str);
}

std::string Model::SystemVoltageString() {
    char str[8];
    int32_t i = static_cast<int32_t>(SystemVoltage());
    int32_t f = static_cast<int32_t>(fmodf(SystemVoltage(),1.0f)*100);
    sprintf(str,"%1d.%02d", int(i), int(f));
    return std::string(str);
}

std::string Model::VbusVoltageString() {
    char str[8];
    int32_t i = static_cast<int32_t>(VbusVoltage());
    int32_t f = static_cast<int32_t>(fmodf(VbusVoltage(),1.0f)*100);
    sprintf(str,"%1d.%02d", int(i), int(f));
    return std::string(str);
}

std::string Model::ChargeCurrentString() {
    char str[8];
    int32_t i = static_cast<int32_t>(ChargeCurrent());
    sprintf(str,"%d", int(i));
    return std::string(str);
}
