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
#ifndef _SDD1306_H_
#define _SDD1306_H_

#include <stdint.h>

class SDD1306 {
	public:
        static SDD1306 &instance();

        SDD1306();
        bool init() const;

        void DisplayOn() const {
            writeCommand(0xAF);
        }

        void DisplayOff() const {
            writeCommand(0xAE);
        }

        bool DevicePresent() const { return devicePresent; }

    private:
        static const uint32_t i2caddr = 0x3C;

        void writeCommand(uint8_t v) const;

        bool devicePresent;
        bool deviceChecked;
};

#endif  // #ifndef _SDD1306_H_

