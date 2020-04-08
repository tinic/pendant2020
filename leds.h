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
#ifndef _LEDS_H_
#define _LEDS_H_

#include "./vector.h"

#include <array>

template <typename T, std::size_t n, std::size_t... ns>
struct multi_array_ {
    using type = std::array<typename multi_array_<T, ns...>::type, n>;
};

template <typename T, std::size_t n>
struct multi_array_<T, n> {
    using type = std::array<T, n>;
};

template <typename T, std::size_t... ns>
using multi_array = typename multi_array_<T, ns...>::type;

class Leds {
public:
    static constexpr size_t face_n = 2;
    static constexpr size_t led_n = 24;
    static constexpr size_t led_outer_n = 16;
    static constexpr size_t led_inner_n = 8;

    static Leds &instance();
    
    void nothing();
    void color_walker();
    void start();

    void set_inner_flat(const vector::float4 &col);
    void set_outer_flat(const vector::float4 &col);

    const std::array<vector::float4, Leds::led_n> &pos();
    const std::array<vector::float4, Leds::led_inner_n> &pos_inner();

private:
    void commit();

    multi_array<vector::float4, face_n, led_n> leds;

    bool initialized = false;
    void init();
};

#endif  // #ifndef _LEDS_H_
