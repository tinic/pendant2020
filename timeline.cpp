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
#include "./timeline.h"
#include "./model.h"
#include "./system_time.h"

#include "stm32f401xc.h"
#include "stm32f4xx_hal.h"

#include <limits>
#include <array>

float Quad::easeIn (float t,float b , float c, float d) {
    t /= d;
    return c*t*t+b;
}
float Quad::easeOut(float t,float b , float c, float d) {
    t /= d;
    return -c*t*(t-2.0f)+b;
}

float Quad::easeInOut(float t,float b , float c, float d) {
    t /= d/2.0f;
    if (t < 1.0f) {
        return ((c/2.0f)*t*t)+b;
    } else {
        t -= 1.0f;
        return -c/2.0f*(((t-2)*t)-1.0f)+b;
    }
}

float Cubic::easeIn (float t, float b, float c, float d) {
    t /= d;
    return c*t*t*t+b;
}

float Cubic::easeOut(float t, float b, float c, float d) {
    t = t/d-1.0f;
    return c*(t*t*t+1.0f)+b;
}

float Cubic::easeInOut(float t, float b, float c, float d) {
    t /= d/2.0f;
    if (t < 1.0f) {
        return c/2.0f*t*t*t+b;
    } else {
        t -= 2.0f;
        return c/2.0f*(t*t*t+2.0f)+b;    
    }
}

Timeline &Timeline::instance() {
    static Timeline timeline;
    if (!timeline.initialized) {
        timeline.initialized = true;
        timeline.init();
    }
    return timeline;
}

bool Timeline::Scheduled(Timeline::Span &span) {
    for (Span *i = head; i ; i = i->next) {
        if ( i == &span ) {
            return true;
        }
    }
    return false;
}

void Timeline::Add(Timeline::Span &span) {
    for (Span *i = head; i ; i = i->next) {
        if ( i == &span ) {
            return;
        }
    }

    span.next = head;
    head = &span;
}

void Timeline::Remove(Timeline::Span &span) {
    Span *p = 0;
    for (Span *i = head; i ; i = i->next) {
        if ( i == &span ) {
            if (p) {
                p->next = i->next;
            } else {
                head = i->next;
            }
            i->next = 0;
            i->Done();
            return;
        }
        p = i;
    }
}

void Timeline::Process(Span::Type type) {
    static std::array<Span *, 64> collected;
    size_t collected_num = 0;
    double time = Model::instance().Time();
    Span *p = 0;
    for (Span *i = head; i ; i = i->next) {
        if (i->type == type) {
            if ((i->time) >= time && !i->active) {
                i->active = true;
                i->Start();
            }
            if (i->duration != std::numeric_limits<double>::infinity() && ((i->time + i->duration) < time)) {
                if (p) {
                    p->next = i->next;
                } else {
                    head = i->next;
                }
                collected[collected_num++] = i;
            }
        }
        p = i;
    }
    for (size_t c = 0; c < collected_num; c++) {
        collected[c]->next = 0;
        collected[c]->Done();
    }
}

Timeline::Span &Timeline::Top(Span::Type type) const {
    static Timeline::Span empty;
    double time = Model::instance().Time();
    for (Span *i = head; i ; i = i->next) {
        if ((i->type == type) &&
            (i->time <= time) &&
            ( (i->duration == std::numeric_limits<double>::infinity()) || ((i->time + i->duration) > time) ) ) {
            return *i;
        }
    }
    return empty;
}

Timeline::Span &Timeline::Below(Span *context, Span::Type type) const {
    static Timeline::Span empty;
    double time = Model::instance().Time();
    for (Span *i = head; i ; i = i->next) {
        if (i == context) {
            continue;
        }
        if ((i->type == type) &&
            (i->time <= time) &&
            ( (i->duration == std::numeric_limits<double>::infinity()) || ((i->time + i->duration) > time) ) ) {
            return *i;
        }
    }
    return empty;
}

bool Timeline::Span::InBeginPeriod(float &interpolation, float period_length) {
    double now = Model::instance().Time();
    if ( (now - time) < static_cast<double>(period_length)) {
        interpolation = static_cast<float>((now - time) * (1.0 / static_cast<double>(period_length)));
        return true;
    }
    return false;
}

bool Timeline::Span::InEndPeriod(float &interpolation, float period_length) {
    double now = Model::instance().Time();
    if ( ((time + duration) - now) < static_cast<double>(period_length)) {
        interpolation = 1.0f - static_cast<float>(((time + duration) - now) * (1.0 / static_cast<double>(period_length)));
        return true;
    }
    return false;
}

void Timeline::ProcessEffect()
{
    return Process(Span::Effect);
}

void Timeline::ProcessDisplay()
{
    return Process(Span::Display);
}

Timeline::Span &Timeline::TopEffect() const
{
    return Top(Span::Effect);
}

Timeline::Span &Timeline::TopDisplay() const
{
    return Top(Span::Display);
}

void Timeline::init() {
}
