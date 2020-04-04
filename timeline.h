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
#ifndef TIMELINE_H_
#define TIMELINE_H_

#include <cstdint>
#include <functional>

class Quad {
public:
    static float easeIn(float t, float b, float c, float d);
    static float easeOut(float t, float b, float c, float d);
    static float easeInOut(float t, float b, float c, float d);
};

class Cubic {
public:
    static float easeIn(float t, float b, float c, float d);
    static float easeOut(float t, float b, float c, float d);
    static float easeInOut(float t, float b, float c, float d);
};

class Timeline {
public:

    class Span {
    public:

        enum Type {
            None,
            Effect,
            Display
        };

        Type type = None;
        double time = 0.0;
        double duration = 0.0;

        std::function<void (Span &span)> startFunc;
        std::function<void (Span &span, Span &below)> calcFunc;
        std::function<void (Span &span)> commitFunc;
        std::function<void (Span &span)> doneFunc;

        std::function<void (Span &span)> switch1Func;
        std::function<void (Span &span)> switch2Func;
        std::function<void (Span &span)> switch3Func;

        void Start() { if (startFunc) startFunc(*this); }
        void Calc() { if (calcFunc) calcFunc(*this, Timeline::instance().Below(this, type)); }
        void Commit() { if (commitFunc) commitFunc(*this); }
        void Done() { if (doneFunc) doneFunc(*this); }
        
        void ProcessSwitch1() { if (switch1Func) switch1Func(*this); }
        void ProcessSwitch2() { if (switch2Func) switch2Func(*this); }
        void ProcessSwitch3() { if (switch3Func) switch3Func(*this); }

        bool Valid() const { return type != None; }

        bool InBeginPeriod(float &interpolation, float period_length = 0.25f);
        bool InEndPeriod(float &interpolation, float period_length = 0.25f);

    private:

        friend class Timeline;
        bool active = false;
        Span *next = 0;
    };

    static Timeline &instance();

    void Add(Timeline::Span &span);
    void Remove(Timeline::Span &span);
    bool Scheduled(Timeline::Span &span);

    void ProcessEffect();
    Span &TopEffect() const;

    void ProcessDisplay();
    Span &TopDisplay() const;

private:
    void Process(Span::Type type);
    Span &Top(Span::Type type) const;
    Span &Below(Span *context, Span::Type type) const;

    Span *head = 0;

    void init();
    bool initialized = false;
};

#endif /* TIMELINE_H_ */