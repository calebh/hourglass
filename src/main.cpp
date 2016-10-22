#include <inttypes.h>
#include <stdbool.h>

#ifndef JUNIPER_H
#define JUNIPER_H

#include <stdlib.h>

namespace juniper
{
    template<typename Result, typename ...Args>
    struct abstract_function
    {
        virtual Result operator()(Args... args) = 0;
        virtual abstract_function *clone() const = 0;
        virtual ~abstract_function() = default;
    };

    template<typename Func, typename Result, typename ...Args>
    class concrete_function : public abstract_function<Result, Args...>
    {
        Func f;
    public:
        concrete_function(const Func &x)
            : f(x)
        {}
        Result operator()(Args... args) override
        {
            return f(args...);
        }
        concrete_function *clone() const override
        {
            return new concrete_function{ f };
        }
    };

    template<typename Func>
    struct func_filter
    {
        typedef Func type;
    };
    template<typename Result, typename ...Args>
    struct func_filter<Result(Args...)>
    {
        typedef Result(*type)(Args...);
    };

    template<typename signature>
    class function;

    template<typename Result, typename ...Args>
    class function<Result(Args...)>
    {
        abstract_function<Result, Args...> *f;
    public:
        function()
            : f(nullptr)
        {}
        template<typename Func> function(const Func &x)
            : f(new concrete_function<typename func_filter<Func>::type, Result, Args...>(x))
        {}
        function(const function &rhs)
            : f(rhs.f ? rhs.f->clone() : nullptr)
        {}
        function &operator=(const function &rhs)
        {
            if ((&rhs != this) && (rhs.f))
            {
                auto *temp = rhs.f->clone();
                delete f;
                f = temp;
            }
            return *this;
        }
        template<typename Func> function &operator=(const Func &x)
        {
            auto *temp = new concrete_function<typename func_filter<Func>::type, Result, Args...>(x);
            delete f;
            f = temp;
            return *this;
        }
        Result operator()(Args... args)
        {
            if (f)
                return (*f)(args...);
            else
                return Result{};
        }
        ~function()
        {
            delete f;
        }
    };

    template <class T>
    void swap(T& a, T& b) {
        T c(a);
        a = b;
        b = c;
    }

    template <typename contained>
    class shared_ptr {
    public:
        shared_ptr() : ptr_(NULL), ref_count_(NULL) { }

        shared_ptr(contained * p)
            : ptr_(p), ref_count_(new int)
        {
            *ref_count_ = 0;
            inc_ref();
        }

        shared_ptr(const shared_ptr& rhs)
            : ptr_(rhs.ptr_), ref_count_(rhs.ref_count_)
        {
            inc_ref();
        }

        ~shared_ptr() {
            if (ref_count_ && 0 == dec_ref()) {
                if (ptr_) {
                    delete ptr_;
                }
                delete ref_count_;
            }
        }

        void set(contained* p) {
          ptr_ = p;
        }

        contained* get() { return ptr_; }
        const contained* get() const { return ptr_; }

        void swap(shared_ptr& rhs) {
            juniper::swap(ptr_, rhs.ptr_);
            juniper::swap(ref_count_, rhs.ref_count_);
        }

        shared_ptr& operator=(const shared_ptr& rhs) {
            shared_ptr tmp(rhs);
            this->swap(tmp);
            return *this;
        }

        //contained& operator*() {
        //    return *ptr_;
        //}

        contained* operator->() {
            return ptr_;
        }

        bool operator==(shared_ptr& rhs) {
            return ptr_ == rhs.ptr_;
        }

        bool operator!=(shared_ptr& rhs) { return !(rhs == *this); }
    private:
        void inc_ref() {
            if (ref_count_) {
                ++(*ref_count_);
            }
        }

        int dec_ref() {
            return --(*ref_count_);
        }

        contained * ptr_;
        int * ref_count_;
    };

    template<typename T, size_t N>
    class array {
    public:
        array<T, N>& fill(T fillWith) {
            for (size_t i = 0; i < N; i++) {
                data[i] = fillWith;
            }

            return *this;
        }

        T& operator[](int i) {
            return data[i];
        }

        bool operator==(array<T, N>& rhs) {
            for (auto i = 0; i < N; i++) {
                if (data[i] != rhs[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(array<T, N>& rhs) { return !(rhs == *this); }

        T data[N];
    };

    template<typename T>
    T quit() {
        exit(1);
    }
}

#endif

#include <Arduino.h>
#include <FastLED.h>

namespace Prelude {}
namespace List {}
namespace Signal {}
namespace Io {}
namespace Maybe {}
namespace Time {}
namespace Math {}
namespace Button {}
namespace Vector {}
namespace CharList {}
namespace StringM {}
namespace FastLed {}
namespace Accelerometer {}
namespace IoExt {}
namespace SignalExt {}
namespace Constants {}
namespace Timing {}
namespace Setting {}
namespace Paused {}
namespace Finale {}
namespace Program {}
namespace List {
    using namespace Prelude;

}

namespace Signal {
    using namespace Prelude;

}

namespace Io {
    using namespace Prelude;

}

namespace Maybe {
    using namespace Prelude;

}

namespace Time {
    using namespace Prelude;

}

namespace Math {
    using namespace Prelude;

}

namespace Button {
    using namespace Prelude;
    using namespace Io;

}

namespace Vector {
    using namespace Prelude;
    using namespace List;
    using namespace Math;

}

namespace CharList {
    using namespace Prelude;

}

namespace StringM {
    using namespace Prelude;

}

namespace FastLed {
    using namespace Prelude;

}

namespace Accelerometer {
    using namespace Prelude;

}

namespace IoExt {
    using namespace Prelude;

}

namespace SignalExt {
    using namespace Prelude;

}

namespace Constants {
    using namespace Prelude;

}

namespace Timing {
    using namespace Prelude;
    using namespace Constants;
    using namespace FastLed;

}

namespace Setting {
    using namespace Prelude;
    using namespace Constants;

}

namespace Paused {
    using namespace Prelude;
    using namespace Constants;
    using namespace FastLed;

}

namespace Finale {
    using namespace Prelude;
    using namespace Constants;

}

namespace Program {
    using namespace Prelude;
    using namespace Constants;

}

namespace Prelude {
    struct unit {
        bool operator==(unit rhs) {
            return true;
        }

        bool operator!=(unit rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b>
    struct tuple2 {
        a e1;
        b e2;
        bool operator==(tuple2 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2;
        }

        bool operator!=(tuple2 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c>
    struct tuple3 {
        a e1;
        b e2;
        c e3;
        bool operator==(tuple3 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3;
        }

        bool operator!=(tuple3 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d>
    struct tuple4 {
        a e1;
        b e2;
        c e3;
        d e4;
        bool operator==(tuple4 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4;
        }

        bool operator!=(tuple4 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e>
    struct tuple5 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        bool operator==(tuple5 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5;
        }

        bool operator!=(tuple5 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e, typename f>
    struct tuple6 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        bool operator==(tuple6 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6;
        }

        bool operator!=(tuple6 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e, typename f, typename g>
    struct tuple7 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        g e7;
        bool operator==(tuple7 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7;
        }

        bool operator!=(tuple7 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e, typename f, typename g, typename h>
    struct tuple8 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        g e7;
        h e8;
        bool operator==(tuple8 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7 && e8 == rhs.e8;
        }

        bool operator!=(tuple8 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e, typename f, typename g, typename h, typename i>
    struct tuple9 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        g e7;
        h e8;
        i e9;
        bool operator==(tuple9 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7 && e8 == rhs.e8 && e9 == rhs.e9;
        }

        bool operator!=(tuple9 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e, typename f, typename g, typename h, typename i, typename j>
    struct tuple10 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        g e7;
        h e8;
        i e9;
        j e10;
        bool operator==(tuple10 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7 && e8 == rhs.e8 && e9 == rhs.e9 && e10 == rhs.e10;
        }

        bool operator!=(tuple10 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a>
    struct maybe {
        uint8_t tag;
        bool operator==(maybe rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->just == rhs.just;
                case 1:
                    return this->nothing == rhs.nothing;
            }
            return false;
        }

        bool operator!=(maybe rhs) { return !(rhs == *this); }
        union {
            a just;
            uint8_t nothing;
        };
    };

    template<typename a>
    Prelude::maybe<a> just(a data) {
        return (([&]() -> Prelude::maybe<a> { Prelude::maybe<a> ret; ret.tag = 0; ret.just = data; return ret; })());
    }

    template<typename a>
    Prelude::maybe<a> nothing() {
        return (([&]() -> Prelude::maybe<a> { Prelude::maybe<a> ret; ret.tag = 1; ret.nothing = 0; return ret; })());
    }


}

namespace Prelude {
    template<typename a, typename b>
    struct either {
        uint8_t tag;
        bool operator==(either rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->left == rhs.left;
                case 1:
                    return this->right == rhs.right;
            }
            return false;
        }

        bool operator!=(either rhs) { return !(rhs == *this); }
        union {
            a left;
            b right;
        };
    };

    template<typename a, typename b>
    Prelude::either<a, b> left(a data) {
        return (([&]() -> Prelude::either<a, b> { Prelude::either<a, b> ret; ret.tag = 0; ret.left = data; return ret; })());
    }

    template<typename a, typename b>
    Prelude::either<a, b> right(b data) {
        return (([&]() -> Prelude::either<a, b> { Prelude::either<a, b> ret; ret.tag = 1; ret.right = data; return ret; })());
    }


}

namespace Prelude {
    template<typename a, int n>
    struct list {
        juniper::array<a, n> data;
        uint32_t length;
        bool operator==(list rhs) {
            return true && data == rhs.data && length == rhs.length;
        }

        bool operator!=(list rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a>
    struct sig {
        uint8_t tag;
        bool operator==(sig rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->signal == rhs.signal;
            }
            return false;
        }

        bool operator!=(sig rhs) { return !(rhs == *this); }
        union {
            Prelude::maybe<a> signal;
        };
    };

    template<typename a>
    Prelude::sig<a> signal(Prelude::maybe<a> data) {
        return (([&]() -> Prelude::sig<a> { Prelude::sig<a> ret; ret.tag = 0; ret.signal = data; return ret; })());
    }


}

namespace Io {
    struct pinState {
        uint8_t tag;
        bool operator==(pinState rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->high == rhs.high;
                case 1:
                    return this->low == rhs.low;
            }
            return false;
        }

        bool operator!=(pinState rhs) { return !(rhs == *this); }
        union {
            uint8_t high;
            uint8_t low;
        };
    };

    Io::pinState high() {
        return (([&]() -> Io::pinState { Io::pinState ret; ret.tag = 0; ret.high = 0; return ret; })());
    }

    Io::pinState low() {
        return (([&]() -> Io::pinState { Io::pinState ret; ret.tag = 1; ret.low = 0; return ret; })());
    }


}

namespace Io {
    struct mode {
        uint8_t tag;
        bool operator==(mode rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->input == rhs.input;
                case 1:
                    return this->output == rhs.output;
                case 2:
                    return this->inputPullup == rhs.inputPullup;
            }
            return false;
        }

        bool operator!=(mode rhs) { return !(rhs == *this); }
        union {
            uint8_t input;
            uint8_t output;
            uint8_t inputPullup;
        };
    };

    Io::mode input() {
        return (([&]() -> Io::mode { Io::mode ret; ret.tag = 0; ret.input = 0; return ret; })());
    }

    Io::mode output() {
        return (([&]() -> Io::mode { Io::mode ret; ret.tag = 1; ret.output = 0; return ret; })());
    }

    Io::mode inputPullup() {
        return (([&]() -> Io::mode { Io::mode ret; ret.tag = 2; ret.inputPullup = 0; return ret; })());
    }


}

namespace Io {
    struct base {
        uint8_t tag;
        bool operator==(base rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->binary == rhs.binary;
                case 1:
                    return this->octal == rhs.octal;
                case 2:
                    return this->decimal == rhs.decimal;
                case 3:
                    return this->hexadecimal == rhs.hexadecimal;
            }
            return false;
        }

        bool operator!=(base rhs) { return !(rhs == *this); }
        union {
            uint8_t binary;
            uint8_t octal;
            uint8_t decimal;
            uint8_t hexadecimal;
        };
    };

    Io::base binary() {
        return (([&]() -> Io::base { Io::base ret; ret.tag = 0; ret.binary = 0; return ret; })());
    }

    Io::base octal() {
        return (([&]() -> Io::base { Io::base ret; ret.tag = 1; ret.octal = 0; return ret; })());
    }

    Io::base decimal() {
        return (([&]() -> Io::base { Io::base ret; ret.tag = 2; ret.decimal = 0; return ret; })());
    }

    Io::base hexadecimal() {
        return (([&]() -> Io::base { Io::base ret; ret.tag = 3; ret.hexadecimal = 0; return ret; })());
    }


}

namespace Time {
    struct timerState {
        uint32_t lastPulse;
        bool operator==(timerState rhs) {
            return true && lastPulse == rhs.lastPulse;
        }

        bool operator!=(timerState rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Button {
    struct buttonState {
        Io::pinState actualState;
        Io::pinState lastState;
        uint32_t lastDebounceTime;
        bool operator==(buttonState rhs) {
            return true && actualState == rhs.actualState && lastState == rhs.lastState && lastDebounceTime == rhs.lastDebounceTime;
        }

        bool operator!=(buttonState rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Vector {
    template<typename a, int n>
    struct vector {
        juniper::array<a, n> data;
        bool operator==(vector rhs) {
            return true && data == rhs.data;
        }

        bool operator!=(vector rhs) {
            return !(rhs == *this);
        }
    };
}

namespace FastLed {
    struct fastLedStrip {
        juniper::shared_ptr<void> ptr;
        bool operator==(fastLedStrip rhs) {
            return true && ptr == rhs.ptr;
        }

        bool operator!=(fastLedStrip rhs) {
            return !(rhs == *this);
        }
    };
}

namespace FastLed {
    struct color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        bool operator==(color rhs) {
            return true && r == rhs.r && g == rhs.g && b == rhs.b;
        }

        bool operator!=(color rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Accelerometer {
    struct axis {
        uint8_t tag;
        bool operator==(axis rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->xAxis == rhs.xAxis;
                case 1:
                    return this->yAxis == rhs.yAxis;
                case 2:
                    return this->zAxis == rhs.zAxis;
            }
            return false;
        }

        bool operator!=(axis rhs) { return !(rhs == *this); }
        union {
            uint8_t xAxis;
            uint8_t yAxis;
            uint8_t zAxis;
        };
    };

    Accelerometer::axis xAxis() {
        return (([&]() -> Accelerometer::axis { Accelerometer::axis ret; ret.tag = 0; ret.xAxis = 0; return ret; })());
    }

    Accelerometer::axis yAxis() {
        return (([&]() -> Accelerometer::axis { Accelerometer::axis ret; ret.tag = 1; ret.yAxis = 0; return ret; })());
    }

    Accelerometer::axis zAxis() {
        return (([&]() -> Accelerometer::axis { Accelerometer::axis ret; ret.tag = 2; ret.zAxis = 0; return ret; })());
    }


}

namespace Accelerometer {
    struct orientation {
        uint8_t tag;
        bool operator==(orientation rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->xUp == rhs.xUp;
                case 1:
                    return this->xDown == rhs.xDown;
                case 2:
                    return this->yUp == rhs.yUp;
                case 3:
                    return this->yDown == rhs.yDown;
                case 4:
                    return this->zUp == rhs.zUp;
                case 5:
                    return this->zDown == rhs.zDown;
            }
            return false;
        }

        bool operator!=(orientation rhs) { return !(rhs == *this); }
        union {
            uint8_t xUp;
            uint8_t xDown;
            uint8_t yUp;
            uint8_t yDown;
            uint8_t zUp;
            uint8_t zDown;
        };
    };

    Accelerometer::orientation xUp() {
        return (([&]() -> Accelerometer::orientation { Accelerometer::orientation ret; ret.tag = 0; ret.xUp = 0; return ret; })());
    }

    Accelerometer::orientation xDown() {
        return (([&]() -> Accelerometer::orientation { Accelerometer::orientation ret; ret.tag = 1; ret.xDown = 0; return ret; })());
    }

    Accelerometer::orientation yUp() {
        return (([&]() -> Accelerometer::orientation { Accelerometer::orientation ret; ret.tag = 2; ret.yUp = 0; return ret; })());
    }

    Accelerometer::orientation yDown() {
        return (([&]() -> Accelerometer::orientation { Accelerometer::orientation ret; ret.tag = 3; ret.yDown = 0; return ret; })());
    }

    Accelerometer::orientation zUp() {
        return (([&]() -> Accelerometer::orientation { Accelerometer::orientation ret; ret.tag = 4; ret.zUp = 0; return ret; })());
    }

    Accelerometer::orientation zDown() {
        return (([&]() -> Accelerometer::orientation { Accelerometer::orientation ret; ret.tag = 5; ret.zDown = 0; return ret; })());
    }


}

namespace Setting {
    struct timeSetting {
        int32_t minutes;
        int32_t fifteenSeconds;
        bool operator==(timeSetting rhs) {
            return true && minutes == rhs.minutes && fifteenSeconds == rhs.fifteenSeconds;
        }

        bool operator!=(timeSetting rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Program {
    struct mode {
        uint8_t tag;
        bool operator==(mode rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->setting == rhs.setting;
                case 1:
                    return this->timing == rhs.timing;
                case 2:
                    return this->paused == rhs.paused;
                case 3:
                    return this->finale == rhs.finale;
            }
            return false;
        }

        bool operator!=(mode rhs) { return !(rhs == *this); }
        union {
            uint8_t setting;
            uint8_t timing;
            uint8_t paused;
            uint8_t finale;
        };
    };

    Program::mode setting() {
        return (([&]() -> Program::mode { Program::mode ret; ret.tag = 0; ret.setting = 0; return ret; })());
    }

    Program::mode timing() {
        return (([&]() -> Program::mode { Program::mode ret; ret.tag = 1; ret.timing = 0; return ret; })());
    }

    Program::mode paused() {
        return (([&]() -> Program::mode { Program::mode ret; ret.tag = 2; ret.paused = 0; return ret; })());
    }

    Program::mode finale() {
        return (([&]() -> Program::mode { Program::mode ret; ret.tag = 3; ret.finale = 0; return ret; })());
    }


}

namespace Program {
    struct flip {
        uint8_t tag;
        bool operator==(flip rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->flipUp == rhs.flipUp;
                case 1:
                    return this->flipDown == rhs.flipDown;
                case 2:
                    return this->flipFlat == rhs.flipFlat;
            }
            return false;
        }

        bool operator!=(flip rhs) { return !(rhs == *this); }
        union {
            uint8_t flipUp;
            uint8_t flipDown;
            uint8_t flipFlat;
        };
    };

    Program::flip flipUp() {
        return (([&]() -> Program::flip { Program::flip ret; ret.tag = 0; ret.flipUp = 0; return ret; })());
    }

    Program::flip flipDown() {
        return (([&]() -> Program::flip { Program::flip ret; ret.tag = 1; ret.flipDown = 0; return ret; })());
    }

    Program::flip flipFlat() {
        return (([&]() -> Program::flip { Program::flip ret; ret.tag = 2; ret.flipFlat = 0; return ret; })());
    }


}

namespace Prelude {
    template<typename t5, typename t3, typename t4>
    juniper::function<t4(t5)> compose(juniper::function<t4(t3)> f, juniper::function<t3(t5)> g);
}

namespace Prelude {
    template<typename t14, typename t15, typename t13>
    juniper::function<juniper::function<t13(t15)>(t14)> curry(juniper::function<t13(t14,t15)> f);
}

namespace Prelude {
    template<typename t23, typename t24, typename t22>
    juniper::function<t22(t23,t24)> uncurry(juniper::function<juniper::function<t22(t24)>(t23)> f);
}

namespace Prelude {
    template<typename t34, typename t35, typename t36, typename t33>
    juniper::function<juniper::function<juniper::function<t33(t36)>(t35)>(t34)> curry3(juniper::function<t33(t34,t35,t36)> f);
}

namespace Prelude {
    template<typename t46, typename t47, typename t48, typename t45>
    juniper::function<t45(t46,t47,t48)> uncurry3(juniper::function<juniper::function<juniper::function<t45(t48)>(t47)>(t46)> f);
}

namespace Prelude {
    template<typename t56>
    bool eq(t56 x, t56 y);
}

namespace Prelude {
    template<typename t58, typename t59>
    bool neq(t58 x, t59 y);
}

namespace Prelude {
    template<typename t62>
    bool gt(t62 x, t62 y);
}

namespace Prelude {
    template<typename t65>
    bool geq(t65 x, t65 y);
}

namespace Prelude {
    template<typename t68>
    bool lt(t68 x, t68 y);
}

namespace Prelude {
    template<typename t71>
    bool leq(t71 x, t71 y);
}

namespace Prelude {
    bool notf(bool x);
}

namespace Prelude {
    bool andf(bool x, bool y);
}

namespace Prelude {
    bool orf(bool x, bool y);
}

namespace Prelude {
    template<typename t81, typename t82>
    t82 apply(juniper::function<t82(t81)> f, t81 x);
}

namespace Prelude {
    template<typename t86, typename t87, typename t88>
    t88 apply2(juniper::function<t88(t86,t87)> f, Prelude::tuple2<t86,t87> tup);
}

namespace Prelude {
    template<typename t97, typename t98, typename t99, typename t100>
    t100 apply3(juniper::function<t100(t97,t98,t99)> f, Prelude::tuple3<t97,t98,t99> tup);
}

namespace Prelude {
    template<typename t112, typename t113, typename t114, typename t115, typename t116>
    t116 apply4(juniper::function<t116(t112,t113,t114,t115)> f, Prelude::tuple4<t112,t113,t114,t115> tup);
}

namespace Prelude {
    template<typename t131, typename t132>
    t131 fst(Prelude::tuple2<t131,t132> tup);
}

namespace Prelude {
    template<typename t136, typename t137>
    t137 snd(Prelude::tuple2<t136,t137> tup);
}

namespace Prelude {
    template<typename t141>
    t141 add(t141 numA, t141 numB);
}

namespace Prelude {
    template<typename t143>
    t143 sub(t143 numA, t143 numB);
}

namespace Prelude {
    template<typename t145>
    t145 mul(t145 numA, t145 numB);
}

namespace Prelude {
    template<typename t147>
    t147 div(t147 numA, t147 numB);
}

namespace Prelude {
    template<typename t150, typename t151>
    Prelude::tuple2<t151,t150> swap(Prelude::tuple2<t150,t151> tup);
}

namespace Prelude {
    template<typename t155>
    t155 until(juniper::function<bool(t155)> p, juniper::function<t155(t155)> f, t155 a0);
}

namespace List {
    template<typename t161, typename t162, int c1>
    Prelude::list<t162, c1> map(juniper::function<t162(t161)> f, Prelude::list<t161, c1> lst);
}

namespace List {
    template<typename t171, typename t172, int c4>
    t172 foldl(juniper::function<t172(t171,t172)> f, t172 initState, Prelude::list<t171, c4> lst);
}

namespace List {
    template<typename t180, typename t181, int c6>
    t181 foldr(juniper::function<t181(t180,t181)> f, t181 initState, Prelude::list<t180, c6> lst);
}

namespace List {
    template<typename t189, int c8, int c9, int c10>
    Prelude::list<t189, c10> append(Prelude::list<t189, c8> lstA, Prelude::list<t189, c9> lstB);
}

namespace List {
    template<typename t205, int c16>
    t205 nth(uint32_t i, Prelude::list<t205, c16> lst);
}

namespace List {
    template<typename t207, int c17, int c18>
    Prelude::list<t207, (c17)*(c18)> flattenSafe(Prelude::list<Prelude::list<t207, c17>, c18> listOfLists);
}

namespace List {
    template<typename t218, int c23, int c24>
    Prelude::list<t218, c24> resize(Prelude::list<t218, c23> lst);
}

namespace List {
    template<typename t225, int c27>
    bool all(juniper::function<bool(t225)> pred, Prelude::list<t225, c27> lst);
}

namespace List {
    template<typename t232, int c29>
    bool any(juniper::function<bool(t232)> pred, Prelude::list<t232, c29> lst);
}

namespace List {
    template<typename t239, int c31>
    Prelude::list<t239, c31> pushBack(t239 elem, Prelude::list<t239, c31> lst);
}

namespace List {
    template<typename t247, int c33>
    Prelude::list<t247, c33> pushOffFront(t247 elem, Prelude::list<t247, c33> lst);
}

namespace List {
    template<typename t258, int c37>
    Prelude::list<t258, c37> setNth(uint32_t index, t258 elem, Prelude::list<t258, c37> lst);
}

namespace List {
    template<typename t263, int c39>
    Prelude::list<t263, c39> replicate(uint32_t numOfElements, t263 elem);
}

namespace List {
    template<typename t265, int c40>
    Prelude::list<t265, c40> remove(t265 elem, Prelude::list<t265, c40> lst);
}

namespace List {
    template<typename t277, int c44>
    Prelude::list<t277, c44> dropLast(Prelude::list<t277, c44> lst);
}

namespace List {
    template<typename t282, int c45>
    Prelude::unit foreach(juniper::function<Prelude::unit(t282)> f, Prelude::list<t282, c45> lst);
}

namespace List {
    template<typename t290, int c48>
    t290 last(Prelude::list<t290, c48> lst);
}

namespace List {
    template<typename t295, int c49>
    t295 max_(Prelude::list<t295, c49> lst);
}

namespace List {
    template<typename t305, int c53>
    t305 min_(Prelude::list<t305, c53> lst);
}

namespace List {
    template<typename t312, int c57>
    bool member(t312 elem, Prelude::list<t312, c57> lst);
}

namespace List {
    template<typename t317, typename t318, int c59>
    Prelude::list<Prelude::tuple2<t317,t318>, c59> zip(Prelude::list<t317, c59> lstA, Prelude::list<t318, c59> lstB);
}

namespace List {
    template<typename t330, typename t331, int c63>
    Prelude::tuple2<Prelude::list<t330, c63>,Prelude::list<t331, c63>> unzip(Prelude::list<Prelude::tuple2<t330,t331>, c63> lst);
}

namespace List {
    template<typename t336, int c64>
    t336 sum(Prelude::list<t336, c64> lst);
}

namespace List {
    template<typename t345, int c65>
    t345 average(Prelude::list<t345, c65> lst);
}

namespace Signal {
    template<typename t347, typename t348>
    Prelude::sig<t348> map(juniper::function<t348(t347)> f, Prelude::sig<t347> s);
}

namespace Signal {
    template<typename t359>
    Prelude::unit sink(juniper::function<Prelude::unit(t359)> f, Prelude::sig<t359> s);
}

namespace Signal {
    template<typename t363>
    Prelude::sig<t363> filter(juniper::function<bool(t363)> f, Prelude::sig<t363> s);
}

namespace Signal {
    template<typename t373>
    Prelude::sig<t373> merge(Prelude::sig<t373> sigA, Prelude::sig<t373> sigB);
}

namespace Signal {
    template<typename t375, int c66>
    Prelude::sig<t375> mergeMany(Prelude::list<Prelude::sig<t375>, c66> sigs);
}

namespace Signal {
    template<typename t383, typename t384>
    Prelude::sig<Prelude::either<t383, t384>> join(Prelude::sig<t383> sigA, Prelude::sig<t384> sigB);
}

namespace Signal {
    template<typename t406>
    Prelude::sig<Prelude::unit> toUnit(Prelude::sig<t406> s);
}

namespace Signal {
    template<typename t411, typename t417>
    Prelude::sig<t417> foldP(juniper::function<t417(t411,t417)> f, juniper::shared_ptr<t417> state0, Prelude::sig<t411> incoming);
}

namespace Signal {
    template<typename t427>
    Prelude::sig<t427> dropRepeats(Prelude::sig<t427> incoming, juniper::shared_ptr<Prelude::maybe<t427>> maybePrevValue);
}

namespace Signal {
    template<typename t437>
    Prelude::sig<t437> latch(Prelude::sig<t437> incoming, juniper::shared_ptr<t437> prevValue);
}

namespace Signal {
    template<typename t448, typename t451, typename t446>
    Prelude::sig<t446> map2(juniper::function<t446(t448,t451)> f, Prelude::sig<t448> incomingA, Prelude::sig<t451> incomingB, juniper::shared_ptr<Prelude::tuple2<t448,t451>> state);
}

namespace Signal {
    template<typename t467, int c67>
    Prelude::sig<Prelude::list<t467, c67>> record(Prelude::sig<t467> incoming, juniper::shared_ptr<Prelude::list<t467, c67>> pastValues);
}

namespace Io {
    Io::pinState toggle(Io::pinState p);
}

namespace Io {
    Prelude::unit printStr(const char * str);
}

namespace Io {
    template<int c68>
    Prelude::unit printCharList(Prelude::list<uint8_t, c68> cl);
}

namespace Io {
    Prelude::unit printFloat(float f);
}

namespace Io {
    Prelude::unit printInt(int32_t n);
}

namespace Io {
    int32_t baseToInt(Io::base b);
}

namespace Io {
    Prelude::unit printIntBase(int32_t n, Io::base b);
}

namespace Io {
    Prelude::unit printFloatPlaces(float f, int32_t numPlaces);
}

namespace Io {
    Prelude::unit beginSerial(uint32_t speed);
}

namespace Io {
    int32_t pinStateToInt(Io::pinState value);
}

namespace Io {
    Io::pinState intToPinState(uint8_t value);
}

namespace Io {
    Prelude::unit digWrite(uint16_t pin, Io::pinState value);
}

namespace Io {
    Io::pinState digRead(uint16_t pin);
}

namespace Io {
    Prelude::sig<Io::pinState> digIn(uint16_t pin);
}

namespace Io {
    Prelude::unit digOut(uint16_t pin, Prelude::sig<Io::pinState> sig);
}

namespace Io {
    int32_t anaRead(uint16_t pin);
}

namespace Io {
    Prelude::unit anaWrite(uint16_t pin, uint8_t value);
}

namespace Io {
    Prelude::sig<uint16_t> anaIn(uint16_t pin);
}

namespace Io {
    Prelude::unit anaOut(uint16_t pin, Prelude::sig<uint16_t> sig);
}

namespace Io {
    int32_t pinModeToInt(Io::mode m);
}

namespace Io {
    Io::mode intToPinMode(uint8_t m);
}

namespace Io {
    Prelude::unit setPinMode(uint16_t pin, Io::mode m);
}

namespace Io {
    Prelude::sig<Prelude::unit> risingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState);
}

namespace Io {
    Prelude::sig<Prelude::unit> fallingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState);
}

namespace Io {
    Prelude::sig<Io::pinState> edge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState);
}

namespace Maybe {
    template<typename t570, typename t571>
    Prelude::maybe<t571> map(juniper::function<t571(t570)> f, Prelude::maybe<t570> maybeVal);
}

namespace Maybe {
    template<typename t578>
    t578 get(Prelude::maybe<t578> maybeVal);
}

namespace Maybe {
    template<typename t580>
    bool isJust(Prelude::maybe<t580> maybeVal);
}

namespace Maybe {
    template<typename t582>
    bool isNothing(Prelude::maybe<t582> maybeVal);
}

namespace Maybe {
    template<typename t586>
    int32_t count(Prelude::maybe<t586> maybeVal);
}

namespace Maybe {
    template<typename t588, typename t589>
    t589 foldl(juniper::function<t589(t588,t589)> f, t589 initState, Prelude::maybe<t588> maybeVal);
}

namespace Maybe {
    template<typename t594, typename t595>
    t595 fodlr(juniper::function<t595(t594,t595)> f, t595 initState, Prelude::maybe<t594> maybeVal);
}

namespace Maybe {
    template<typename t601>
    Prelude::unit iter(juniper::function<Prelude::unit(t601)> f, Prelude::maybe<t601> maybeVal);
}

namespace Time {
    Prelude::unit wait(uint32_t time);
}

namespace Time {
    int32_t now();
}

namespace Time {
    juniper::shared_ptr<Time::timerState> state();
}

namespace Time {
    Prelude::sig<uint32_t> every(uint32_t interval, juniper::shared_ptr<Time::timerState> state);
}

namespace Math {
    double degToRad(double degrees);
}

namespace Math {
    double radToDeg(double radians);
}

namespace Math {
    double acos_(double x);
}

namespace Math {
    double asin_(double x);
}

namespace Math {
    double atan_(double x);
}

namespace Math {
    double atan2_(double y, double x);
}

namespace Math {
    double cos_(double x);
}

namespace Math {
    double cosh_(double x);
}

namespace Math {
    double sin_(double x);
}

namespace Math {
    double sinh_(double x);
}

namespace Math {
    double tan_(double x);
}

namespace Math {
    double tanh_(double x);
}

namespace Math {
    double exp_(double x);
}

namespace Math {
    Prelude::tuple2<double,int16_t> frexp_(double x);
}

namespace Math {
    double ldexp_(double x, int16_t exponent);
}

namespace Math {
    double log_(double x);
}

namespace Math {
    double log10_(double x);
}

namespace Math {
    Prelude::tuple2<double,double> modf_(double x);
}

namespace Math {
    double pow_(double x, double y);
}

namespace Math {
    double sqrt_(double x);
}

namespace Math {
    double ceil_(double x);
}

namespace Math {
    double fabs_(double x);
}

namespace Math {
    double floor_(double x);
}

namespace Math {
    double fmod_(double x, double y);
}

namespace Math {
    double round_(double x);
}

namespace Math {
    double min_(double x, double y);
}

namespace Math {
    double max_(double x, double y);
}

namespace Math {
    double mapRange(double x, double a1, double a2, double b1, double b2);
}

namespace Math {
    template<typename t654>
    t654 clamp(t654 x, t654 min, t654 max);
}

namespace Math {
    template<typename t656>
    int32_t sign(t656 n);
}

namespace Button {
    juniper::shared_ptr<Button::buttonState> state();
}

namespace Button {
    Prelude::sig<Io::pinState> debounceDelay(Prelude::sig<Io::pinState> incoming, uint16_t delay, juniper::shared_ptr<Button::buttonState> buttonState);
}

namespace Button {
    Prelude::sig<Io::pinState> debounce(Prelude::sig<Io::pinState> incoming, juniper::shared_ptr<Button::buttonState> buttonState);
}

namespace Vector {
    template<typename t679, int c69>
    Vector::vector<t679, c69> make(juniper::array<t679, c69> d);
}

namespace Vector {
    template<typename t682, int c71>
    t682 get(uint32_t i, Vector::vector<t682, c71> v);
}

namespace Vector {
    template<typename t684, int c72>
    Vector::vector<t684, c72> add(Vector::vector<t684, c72> v1, Vector::vector<t684, c72> v2);
}

namespace Vector {
    template<typename t693, int c76>
    Vector::vector<t693, c76> zero();
}

namespace Vector {
    template<typename t695, int c77>
    Vector::vector<t695, c77> subtract(Vector::vector<t695, c77> v1, Vector::vector<t695, c77> v2);
}

namespace Vector {
    template<typename t703, int c81>
    Vector::vector<t703, c81> scale(t703 scalar, Vector::vector<t703, c81> v);
}

namespace Vector {
    template<typename t709, int c84>
    t709 dot(Vector::vector<t709, c84> v1, Vector::vector<t709, c84> v2);
}

namespace Vector {
    template<typename t715, int c87>
    t715 magnitude2(Vector::vector<t715, c87> v);
}

namespace Vector {
    template<typename t721, int c90>
    double magnitude(Vector::vector<t721, c90> v);
}

namespace Vector {
    template<typename t727, int c91>
    Vector::vector<t727, c91> multiply(Vector::vector<t727, c91> u, Vector::vector<t727, c91> v);
}

namespace Vector {
    template<typename t735, int c95>
    Vector::vector<t735, c95> normalize(Vector::vector<t735, c95> v);
}

namespace Vector {
    template<typename t743, int c98>
    double angle(Vector::vector<t743, c98> v1, Vector::vector<t743, c98> v2);
}

namespace Vector {
    template<typename t781>
    Vector::vector<t781, 3> cross(Vector::vector<t781, 3> u, Vector::vector<t781, 3> v);
}

namespace Vector {
    template<typename t783, int c111>
    Vector::vector<t783, c111> project(Vector::vector<t783, c111> a, Vector::vector<t783, c111> b);
}

namespace Vector {
    template<typename t793, int c112>
    Vector::vector<t793, c112> projectPlane(Vector::vector<t793, c112> a, Vector::vector<t793, c112> m);
}

namespace CharList {
    template<int c113>
    Prelude::list<uint8_t, c113> toUpper(Prelude::list<uint8_t, c113> str);
}

namespace CharList {
    template<int c114>
    Prelude::list<uint8_t, c114> toLower(Prelude::list<uint8_t, c114> str);
}

namespace FastLed {
    FastLed::fastLedStrip make(uint16_t numLeds);
}

namespace FastLed {
    template<typename t822, typename t823>
    Prelude::unit setLedColor(uint16_t n, t822 c, t823 strip);
}

namespace FastLed {
    template<typename t829>
    FastLed::color getLedColor(uint16_t n, t829 strip);
}

namespace FastLed {
    Prelude::unit show();
}

namespace Accelerometer {
    int32_t axisToPin(Accelerometer::axis a);
}

namespace Accelerometer {
    Prelude::tuple2<int32_t,int32_t> axisToRange(Accelerometer::axis a);
}

namespace Accelerometer {
    int32_t readRaw(Accelerometer::axis a);
}

namespace Accelerometer {
    double read(Accelerometer::axis a);
}

namespace Accelerometer {
    Prelude::maybe<Accelerometer::orientation> getOrientation();
}

namespace Accelerometer {
    Prelude::sig<Accelerometer::orientation> getSignal();
}

namespace IoExt {
    Prelude::sig<Io::pinState> every(uint32_t interval, juniper::shared_ptr<Time::timerState> tState, juniper::shared_ptr<Io::pinState> outState);
}

namespace SignalExt {
    template<typename t920>
    Prelude::sig<t920> constant(t920 val);
}

namespace SignalExt {
    template<typename t929>
    Prelude::sig<Prelude::maybe<t929>> meta(Prelude::sig<t929> sigA);
}

namespace SignalExt {
    template<typename t935>
    Prelude::sig<t944> unmeta(Prelude::sig<Prelude::maybe<t944>> sigA);
}

namespace SignalExt {
    template<typename t955, typename t956>
    Prelude::sig<Prelude::tuple2<t955,t956>> zip(Prelude::sig<t955> sigA, Prelude::sig<t956> sigB, juniper::shared_ptr<Prelude::tuple2<t955,t956>> state);
}

namespace SignalExt {
    template<typename t963, typename t969>
    Prelude::sig<t963> toggle(t963 val1, t963 val2, juniper::shared_ptr<t963> state, Prelude::sig<t969> incoming);
}

namespace Timing {
    Prelude::unit reset();
}

namespace Timing {
    template<typename t981, typename t982>
    FastLed::color interpolate(t981 c1, t982 c2, float t);
}

namespace Timing {
    Prelude::unit execute(juniper::shared_ptr<int32_t> timeRemaining, float totalTime);
}

namespace Setting {
    Prelude::unit reset(juniper::shared_ptr<int32_t> timeRemaining);
}

namespace Setting {
    Prelude::unit execute(juniper::shared_ptr<int32_t> timeRemaining);
}

namespace Paused {
    Prelude::unit execute(juniper::shared_ptr<int32_t> timeRemaining, float totalTime);
}

namespace Finale {
    Prelude::unit execute();
}

namespace Program {
    Prelude::unit setup();
}

namespace Program {
    Prelude::unit clearDisplay();
}

namespace Program {
    Prelude::unit main();
}

namespace Prelude {
    template<typename t5, typename t3, typename t4>
    juniper::function<t4(t5)> compose(juniper::function<t4(t3)> f, juniper::function<t3(t5)> g) {
        return juniper::function<t4(t5)>([=](t5 x) mutable -> t4 { 
            return f(g(x));
         });
    }
}

namespace Prelude {
    template<typename t14, typename t15, typename t13>
    juniper::function<juniper::function<t13(t15)>(t14)> curry(juniper::function<t13(t14,t15)> f) {
        return juniper::function<juniper::function<t13(t15)>(t14)>([=](t14 valueA) mutable -> juniper::function<t13(t15)> { 
            return juniper::function<t13(t15)>([=](t15 valueB) mutable -> t13 { 
                return f(valueA, valueB);
             });
         });
    }
}

namespace Prelude {
    template<typename t23, typename t24, typename t22>
    juniper::function<t22(t23,t24)> uncurry(juniper::function<juniper::function<t22(t24)>(t23)> f) {
        return juniper::function<t22(t23,t24)>([=](t23 valueA, t24 valueB) mutable -> t22 { 
            return f(valueA)(valueB);
         });
    }
}

namespace Prelude {
    template<typename t34, typename t35, typename t36, typename t33>
    juniper::function<juniper::function<juniper::function<t33(t36)>(t35)>(t34)> curry3(juniper::function<t33(t34,t35,t36)> f) {
        return juniper::function<juniper::function<juniper::function<t33(t36)>(t35)>(t34)>([=](t34 valueA) mutable -> juniper::function<juniper::function<t33(t36)>(t35)> { 
            return juniper::function<juniper::function<t33(t36)>(t35)>([=](t35 valueB) mutable -> juniper::function<t33(t36)> { 
                return juniper::function<t33(t36)>([=](t36 valueC) mutable -> t33 { 
                    return f(valueA, valueB, valueC);
                 });
             });
         });
    }
}

namespace Prelude {
    template<typename t46, typename t47, typename t48, typename t45>
    juniper::function<t45(t46,t47,t48)> uncurry3(juniper::function<juniper::function<juniper::function<t45(t48)>(t47)>(t46)> f) {
        return juniper::function<t45(t46,t47,t48)>([=](t46 valueA, t47 valueB, t48 valueC) mutable -> t45 { 
            return f(valueA)(valueB)(valueC);
         });
    }
}

namespace Prelude {
    template<typename t56>
    bool eq(t56 x, t56 y) {
        return (x == y);
    }
}

namespace Prelude {
    template<typename t58, typename t59>
    bool neq(t58 x, t59 y) {
        return (x != y);
    }
}

namespace Prelude {
    template<typename t62>
    bool gt(t62 x, t62 y) {
        return (x > y);
    }
}

namespace Prelude {
    template<typename t65>
    bool geq(t65 x, t65 y) {
        return (x >= y);
    }
}

namespace Prelude {
    template<typename t68>
    bool lt(t68 x, t68 y) {
        return (x < y);
    }
}

namespace Prelude {
    template<typename t71>
    bool leq(t71 x, t71 y) {
        return (x <= y);
    }
}

namespace Prelude {
    bool notf(bool x) {
        return !(x);
    }
}

namespace Prelude {
    bool andf(bool x, bool y) {
        return (x && y);
    }
}

namespace Prelude {
    bool orf(bool x, bool y) {
        return (x || y);
    }
}

namespace Prelude {
    template<typename t81, typename t82>
    t82 apply(juniper::function<t82(t81)> f, t81 x) {
        return f(x);
    }
}

namespace Prelude {
    template<typename t86, typename t87, typename t88>
    t88 apply2(juniper::function<t88(t86,t87)> f, Prelude::tuple2<t86,t87> tup) {
        return (([&]() -> t88 {
            auto guid0 = tup;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto b = (guid0).e2;
            auto a = (guid0).e1;
            
            return f(a, b);
        })());
    }
}

namespace Prelude {
    template<typename t97, typename t98, typename t99, typename t100>
    t100 apply3(juniper::function<t100(t97,t98,t99)> f, Prelude::tuple3<t97,t98,t99> tup) {
        return (([&]() -> t100 {
            auto guid1 = tup;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto c = (guid1).e3;
            auto b = (guid1).e2;
            auto a = (guid1).e1;
            
            return f(a, b, c);
        })());
    }
}

namespace Prelude {
    template<typename t112, typename t113, typename t114, typename t115, typename t116>
    t116 apply4(juniper::function<t116(t112,t113,t114,t115)> f, Prelude::tuple4<t112,t113,t114,t115> tup) {
        return (([&]() -> t116 {
            auto guid2 = tup;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto d = (guid2).e4;
            auto c = (guid2).e3;
            auto b = (guid2).e2;
            auto a = (guid2).e1;
            
            return f(a, b, c, d);
        })());
    }
}

namespace Prelude {
    template<typename t131, typename t132>
    t131 fst(Prelude::tuple2<t131,t132> tup) {
        return (([&]() -> t131 {
            auto guid3 = tup;
            return (true ? 
                (([&]() -> t131 {
                    auto x = (guid3).e1;
                    return x;
                })())
            :
                juniper::quit<t131>());
        })());
    }
}

namespace Prelude {
    template<typename t136, typename t137>
    t137 snd(Prelude::tuple2<t136,t137> tup) {
        return (([&]() -> t137 {
            auto guid4 = tup;
            return (true ? 
                (([&]() -> t137 {
                    auto x = (guid4).e2;
                    return x;
                })())
            :
                juniper::quit<t137>());
        })());
    }
}

namespace Prelude {
    template<typename t141>
    t141 add(t141 numA, t141 numB) {
        return (numA + numB);
    }
}

namespace Prelude {
    template<typename t143>
    t143 sub(t143 numA, t143 numB) {
        return (numA - numB);
    }
}

namespace Prelude {
    template<typename t145>
    t145 mul(t145 numA, t145 numB) {
        return (numA * numB);
    }
}

namespace Prelude {
    template<typename t147>
    t147 div(t147 numA, t147 numB) {
        return (numA / numB);
    }
}

namespace Prelude {
    template<typename t150, typename t151>
    Prelude::tuple2<t151,t150> swap(Prelude::tuple2<t150,t151> tup) {
        return (([&]() -> Prelude::tuple2<t151,t150> {
            auto guid5 = tup;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto beta = (guid5).e2;
            auto alpha = (guid5).e1;
            
            return (Prelude::tuple2<t151,t150>{beta, alpha});
        })());
    }
}

namespace Prelude {
    template<typename t155>
    t155 until(juniper::function<bool(t155)> p, juniper::function<t155(t155)> f, t155 a0) {
        return (([&]() -> t155 {
            auto guid6 = a0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto a = guid6;
            
            (([&]() -> Prelude::unit {
                while (!(p(a))) {
                    (([&]() -> Prelude::unit {
                        (a = f(a));
                        return Prelude::unit();
                    })());
                }
                return {};
            })());
            return a;
        })());
    }
}

namespace List {
    template<typename t161, typename t162, int c1>
    Prelude::list<t162, c1> map(juniper::function<t162(t161)> f, Prelude::list<t161, c1> lst) {
        return (([&]() -> Prelude::list<t162, c1> {
            auto n = c1;
            return (([&]() -> Prelude::list<t162, c1> {
                auto guid7 = (juniper::array<t162, c1>());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid7;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid8 = 0;
                    uint32_t guid9 = ((lst).length - 1);
                    for (uint32_t i = guid8; i <= guid9; i++) {
                        ((ret)[i] = f(((lst).data)[i]));
                    }
                    return {};
                })());
                return (([&]() -> Prelude::list<t162, c1>{
                    Prelude::list<t162, c1> guid10;
                    guid10.data = ret;
                    guid10.length = (lst).length;
                    return guid10;
                })());
            })());
        })());
    }
}

namespace List {
    template<typename t171, typename t172, int c4>
    t172 foldl(juniper::function<t172(t171,t172)> f, t172 initState, Prelude::list<t171, c4> lst) {
        return (([&]() -> t172 {
            auto n = c4;
            return (([&]() -> t172 {
                auto guid11 = initState;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto s = guid11;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid12 = 0;
                    uint32_t guid13 = ((lst).length - 1);
                    for (uint32_t i = guid12; i <= guid13; i++) {
                        (s = f(((lst).data)[i], s));
                    }
                    return {};
                })());
                return s;
            })());
        })());
    }
}

namespace List {
    template<typename t180, typename t181, int c6>
    t181 foldr(juniper::function<t181(t180,t181)> f, t181 initState, Prelude::list<t180, c6> lst) {
        return (([&]() -> t181 {
            auto n = c6;
            return (([&]() -> t181 {
                auto guid14 = initState;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto s = guid14;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid15 = ((lst).length - 1);
                    uint32_t guid16 = 0;
                    for (uint32_t i = guid15; i >= guid16; i--) {
                        (s = f(((lst).data)[i], s));
                    }
                    return {};
                })());
                return s;
            })());
        })());
    }
}

namespace List {
    template<typename t189, int c8, int c9, int c10>
    Prelude::list<t189, c10> append(Prelude::list<t189, c8> lstA, Prelude::list<t189, c9> lstB) {
        return (([&]() -> Prelude::list<t189, c10> {
            auto aCap = c8;
            auto bCap = c9;
            auto retCap = c10;
            return (([&]() -> Prelude::list<t189, c10> {
                auto guid17 = 0;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto j = guid17;
                
                auto guid18 = (([&]() -> Prelude::list<t189, c10>{
                    Prelude::list<t189, c10> guid19;
                    guid19.data = (juniper::array<t189, c10>());
                    guid19.length = ((lstA).length + (lstB).length);
                    return guid19;
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto out = guid18;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid20 = 0;
                    uint32_t guid21 = ((lstA).length - 1);
                    for (uint32_t i = guid20; i <= guid21; i++) {
                        (([&]() -> Prelude::unit {
                            (((out).data)[j] = ((lstA).data)[i]);
                            (j = (j + 1));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                (([&]() -> Prelude::unit {
                    uint32_t guid22 = 0;
                    uint32_t guid23 = ((lstB).length - 1);
                    for (uint32_t i = guid22; i <= guid23; i++) {
                        (([&]() -> Prelude::unit {
                            (((out).data)[j] = ((lstB).data)[i]);
                            (j = (j + 1));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return out;
            })());
        })());
    }
}

namespace List {
    template<typename t205, int c16>
    t205 nth(uint32_t i, Prelude::list<t205, c16> lst) {
        return (([&]() -> t205 {
            auto n = c16;
            return ((i < (lst).length) ? 
                (([&]() -> t205 {
                    auto guid24 = lst;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto data = (guid24).data;
                    
                    return (data)[i];
                })())
            :
                juniper::quit<t205>());
        })());
    }
}

namespace List {
    template<typename t207, int c17, int c18>
    Prelude::list<t207, (c17)*(c18)> flattenSafe(Prelude::list<Prelude::list<t207, c17>, c18> listOfLists) {
        return (([&]() -> Prelude::list<t207, (c17)*(c18)> {
            auto m = c17;
            auto n = c18;
            return (([&]() -> Prelude::list<t207, (c17)*(c18)> {
                auto guid25 = (juniper::array<t207, (c17)*(c18)>());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid25;
                
                auto guid26 = 0;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto index = guid26;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid27 = 0;
                    uint32_t guid28 = ((listOfLists).length - 1);
                    for (uint32_t i = guid27; i <= guid28; i++) {
                        (([&]() -> Prelude::unit {
                            uint32_t guid29 = 0;
                            uint32_t guid30 = ((((listOfLists).data)[i]).length - 1);
                            for (uint32_t j = guid29; j <= guid30; j++) {
                                (([&]() -> int32_t {
                                    ((ret)[index] = ((((listOfLists).data)[i]).data)[j]);
                                    return (index = (index + 1));
                                })());
                            }
                            return {};
                        })());
                    }
                    return {};
                })());
                return (([&]() -> Prelude::list<t207, (c17)*(c18)>{
                    Prelude::list<t207, (c17)*(c18)> guid31;
                    guid31.data = ret;
                    guid31.length = index;
                    return guid31;
                })());
            })());
        })());
    }
}

namespace List {
    template<typename t218, int c23, int c24>
    Prelude::list<t218, c24> resize(Prelude::list<t218, c23> lst) {
        return (([&]() -> Prelude::list<t218, c24> {
            auto n = c23;
            auto m = c24;
            return (([&]() -> Prelude::list<t218, c24> {
                auto guid32 = (juniper::array<t218, c24>());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid32;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid33 = 0;
                    uint32_t guid34 = ((lst).length - 1);
                    for (uint32_t i = guid33; i <= guid34; i++) {
                        ((ret)[i] = ((lst).data)[i]);
                    }
                    return {};
                })());
                return (([&]() -> Prelude::list<t218, c24>{
                    Prelude::list<t218, c24> guid35;
                    guid35.data = ret;
                    guid35.length = (lst).length;
                    return guid35;
                })());
            })());
        })());
    }
}

namespace List {
    template<typename t225, int c27>
    bool all(juniper::function<bool(t225)> pred, Prelude::list<t225, c27> lst) {
        return (([&]() -> bool {
            auto n = c27;
            return (([&]() -> bool {
                auto guid36 = true;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto satisfied = guid36;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid37 = 0;
                    uint32_t guid38 = ((lst).length - 1);
                    for (uint32_t i = guid37; i <= guid38; i++) {
                        (satisfied ? 
                            (([&]() -> Prelude::unit {
                                (satisfied = pred(((lst).data)[i]));
                                return Prelude::unit();
                            })())
                        :
                            Prelude::unit());
                    }
                    return {};
                })());
                return satisfied;
            })());
        })());
    }
}

namespace List {
    template<typename t232, int c29>
    bool any(juniper::function<bool(t232)> pred, Prelude::list<t232, c29> lst) {
        return (([&]() -> bool {
            auto n = c29;
            return (([&]() -> bool {
                auto guid39 = false;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto satisfied = guid39;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid40 = 0;
                    uint32_t guid41 = ((lst).length - 1);
                    for (uint32_t i = guid40; i <= guid41; i++) {
                        (!(satisfied) ? 
                            (([&]() -> Prelude::unit {
                                (satisfied = pred(((lst).data)[i]));
                                return Prelude::unit();
                            })())
                        :
                            Prelude::unit());
                    }
                    return {};
                })());
                return satisfied;
            })());
        })());
    }
}

namespace List {
    template<typename t239, int c31>
    Prelude::list<t239, c31> pushBack(t239 elem, Prelude::list<t239, c31> lst) {
        return (([&]() -> Prelude::list<t239, c31> {
            auto n = c31;
            return (((lst).length >= n) ? 
                juniper::quit<Prelude::list<t239, c31>>()
            :
                (([&]() -> Prelude::list<t239, c31> {
                    auto guid42 = lst;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto ret = guid42;
                    
                    (((ret).data)[(lst).length] = elem);
                    ((ret).length = ((lst).length + 1));
                    return ret;
                })()));
        })());
    }
}

namespace List {
    template<typename t247, int c33>
    Prelude::list<t247, c33> pushOffFront(t247 elem, Prelude::list<t247, c33> lst) {
        return (([&]() -> Prelude::list<t247, c33> {
            auto n = c33;
            return (([&]() -> Prelude::list<t247, c33> {
                auto guid43 = lst;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid43;
                
                (([&]() -> Prelude::unit {
                    int32_t guid44 = (n - 2);
                    int32_t guid45 = 0;
                    for (int32_t i = guid44; i >= guid45; i--) {
                        (((ret).data)[(i + 1)] = ((ret).data)[i]);
                    }
                    return {};
                })());
                (((ret).data)[0] = elem);
                return (((ret).length == n) ? 
                    ret
                :
                    (([&]() -> Prelude::list<t247, c33> {
                        ((ret).length = ((lst).length + 1));
                        return ret;
                    })()));
            })());
        })());
    }
}

namespace List {
    template<typename t258, int c37>
    Prelude::list<t258, c37> setNth(uint32_t index, t258 elem, Prelude::list<t258, c37> lst) {
        return (([&]() -> Prelude::list<t258, c37> {
            auto n = c37;
            return (((lst).length <= index) ? 
                juniper::quit<Prelude::list<t258, c37>>()
            :
                (([&]() -> Prelude::list<t258, c37> {
                    auto guid46 = lst;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto ret = guid46;
                    
                    (((ret).data)[index] = elem);
                    return ret;
                })()));
        })());
    }
}

namespace List {
    template<typename t263, int c39>
    Prelude::list<t263, c39> replicate(uint32_t numOfElements, t263 elem) {
        return (([&]() -> Prelude::list<t263, c39> {
            auto n = c39;
            return (([&]() -> Prelude::list<t263, c39>{
                Prelude::list<t263, c39> guid47;
                guid47.data = (juniper::array<t263, c39>().fill(elem));
                guid47.length = numOfElements;
                return guid47;
            })());
        })());
    }
}

namespace List {
    template<typename t265, int c40>
    Prelude::list<t265, c40> remove(t265 elem, Prelude::list<t265, c40> lst) {
        return (([&]() -> Prelude::list<t265, c40> {
            auto n = c40;
            return (([&]() -> Prelude::list<t265, c40> {
                auto guid48 = 0;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto index = guid48;
                
                auto guid49 = false;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto found = guid49;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid50 = 0;
                    uint32_t guid51 = ((lst).length - 1);
                    for (uint32_t i = guid50; i <= guid51; i++) {
                        ((!(found) && (((lst).data)[i] == elem)) ? 
                            (([&]() -> Prelude::unit {
                                (index = i);
                                (found = true);
                                return Prelude::unit();
                            })())
                        :
                            Prelude::unit());
                    }
                    return {};
                })());
                return (found ? 
                    (([&]() -> Prelude::list<t265, c40> {
                        auto guid52 = lst;
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto ret = guid52;
                        
                        ((ret).length = ((lst).length - 1));
                        (([&]() -> Prelude::unit {
                            uint32_t guid53 = index;
                            uint32_t guid54 = ((lst).length - 2);
                            for (uint32_t i = guid53; i <= guid54; i++) {
                                (((ret).data)[i] = ((lst).data)[(i + 1)]);
                            }
                            return {};
                        })());
                        return ret;
                    })())
                :
                    lst);
            })());
        })());
    }
}

namespace List {
    template<typename t277, int c44>
    Prelude::list<t277, c44> dropLast(Prelude::list<t277, c44> lst) {
        return (([&]() -> Prelude::list<t277, c44> {
            auto n = c44;
            return (((lst).length == 0) ? 
                juniper::quit<Prelude::list<t277, c44>>()
            :
                (([&]() -> Prelude::list<t277, c44>{
                    Prelude::list<t277, c44> guid55;
                    guid55.data = (lst).data;
                    guid55.length = ((lst).length - 1);
                    return guid55;
                })()));
        })());
    }
}

namespace List {
    template<typename t282, int c45>
    Prelude::unit foreach(juniper::function<Prelude::unit(t282)> f, Prelude::list<t282, c45> lst) {
        return (([&]() -> Prelude::unit {
            auto n = c45;
            return (([&]() -> Prelude::unit {
                uint32_t guid56 = 0;
                uint32_t guid57 = ((lst).length - 1);
                for (uint32_t i = guid56; i <= guid57; i++) {
                    f(((lst).data)[i]);
                }
                return {};
            })());
        })());
    }
}

namespace List {
    template<typename t290, int c48>
    t290 last(Prelude::list<t290, c48> lst) {
        return (([&]() -> t290 {
            auto n = c48;
            return (([&]() -> t290 {
                auto guid58 = lst;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto length = (guid58).length;
                auto data = (guid58).data;
                
                return (data)[(length - 1)];
            })());
        })());
    }
}

namespace List {
    template<typename t295, int c49>
    t295 max_(Prelude::list<t295, c49> lst) {
        return (([&]() -> t295 {
            auto n = c49;
            return ((((lst).length == 0) || (n == 0)) ? 
                juniper::quit<t295>()
            :
                (([&]() -> t295 {
                    auto guid59 = ((lst).data)[0];
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto maxVal = guid59;
                    
                    (([&]() -> Prelude::unit {
                        uint32_t guid60 = 1;
                        uint32_t guid61 = ((lst).length - 1);
                        for (uint32_t i = guid60; i <= guid61; i++) {
                            ((((lst).data)[i] > maxVal) ? 
                                (([&]() -> Prelude::unit {
                                    (maxVal = ((lst).data)[i]);
                                    return Prelude::unit();
                                })())
                            :
                                Prelude::unit());
                        }
                        return {};
                    })());
                    return maxVal;
                })()));
        })());
    }
}

namespace List {
    template<typename t305, int c53>
    t305 min_(Prelude::list<t305, c53> lst) {
        return (([&]() -> t305 {
            auto n = c53;
            return ((((lst).length == 0) || (n == 0)) ? 
                juniper::quit<t305>()
            :
                (([&]() -> t305 {
                    auto guid62 = ((lst).data)[0];
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto minVal = guid62;
                    
                    (([&]() -> Prelude::unit {
                        uint32_t guid63 = 1;
                        uint32_t guid64 = ((lst).length - 1);
                        for (uint32_t i = guid63; i <= guid64; i++) {
                            ((((lst).data)[i] < minVal) ? 
                                (([&]() -> Prelude::unit {
                                    (minVal = ((lst).data)[i]);
                                    return Prelude::unit();
                                })())
                            :
                                Prelude::unit());
                        }
                        return {};
                    })());
                    return minVal;
                })()));
        })());
    }
}

namespace List {
    template<typename t312, int c57>
    bool member(t312 elem, Prelude::list<t312, c57> lst) {
        return (([&]() -> bool {
            auto n = c57;
            return (([&]() -> bool {
                auto guid65 = false;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto found = guid65;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid66 = 0;
                    uint32_t guid67 = ((lst).length - 1);
                    for (uint32_t i = guid66; i <= guid67; i++) {
                        ((!(found) && (((lst).data)[i] == elem)) ? 
                            (([&]() -> Prelude::unit {
                                (found = true);
                                return Prelude::unit();
                            })())
                        :
                            Prelude::unit());
                    }
                    return {};
                })());
                return found;
            })());
        })());
    }
}

namespace List {
    template<typename t317, typename t318, int c59>
    Prelude::list<Prelude::tuple2<t317,t318>, c59> zip(Prelude::list<t317, c59> lstA, Prelude::list<t318, c59> lstB) {
        return (([&]() -> Prelude::list<Prelude::tuple2<t317,t318>, c59> {
            auto n = c59;
            return (((lstA).length == (lstB).length) ? 
                (([&]() -> Prelude::list<Prelude::tuple2<t317,t318>, c59> {
                    auto guid68 = (([&]() -> Prelude::list<Prelude::tuple2<t317,t318>, c59>{
                        Prelude::list<Prelude::tuple2<t317,t318>, c59> guid69;
                        guid69.data = (juniper::array<Prelude::tuple2<t317,t318>, c59>());
                        guid69.length = (lstA).length;
                        return guid69;
                    })());
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto ret = guid68;
                    
                    (([&]() -> Prelude::unit {
                        uint32_t guid70 = 0;
                        uint32_t guid71 = (lstA).length;
                        for (uint32_t i = guid70; i <= guid71; i++) {
                            (([&]() -> Prelude::unit {
                                (((ret).data)[i] = (Prelude::tuple2<t317,t318>{((lstA).data)[i], ((lstB).data)[i]}));
                                return Prelude::unit();
                            })());
                        }
                        return {};
                    })());
                    return ret;
                })())
            :
                juniper::quit<Prelude::list<Prelude::tuple2<t317,t318>, c59>>());
        })());
    }
}

namespace List {
    template<typename t330, typename t331, int c63>
    Prelude::tuple2<Prelude::list<t330, c63>,Prelude::list<t331, c63>> unzip(Prelude::list<Prelude::tuple2<t330,t331>, c63> lst) {
        return (([&]() -> Prelude::tuple2<Prelude::list<t330, c63>,Prelude::list<t331, c63>> {
            auto n = c63;
            return (([&]() -> Prelude::tuple2<Prelude::list<t330, c63>,Prelude::list<t331, c63>> {
                auto guid72 = (([&]() -> Prelude::list<t330, c63>{
                    Prelude::list<t330, c63> guid73;
                    guid73.data = (juniper::array<t330, c63>());
                    guid73.length = (lst).length;
                    return guid73;
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto retA = guid72;
                
                auto guid74 = (([&]() -> Prelude::list<t331, c63>{
                    Prelude::list<t331, c63> guid75;
                    guid75.data = (juniper::array<t331, c63>());
                    guid75.length = (lst).length;
                    return guid75;
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto retB = guid74;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid76 = 0;
                    uint32_t guid77 = ((lst).length - 1);
                    for (uint32_t i = guid76; i <= guid77; i++) {
                        (([&]() -> Prelude::unit {
                            retA.data[i] = lst.data[i].e1;
         retB.data[i] = lst.data[i].e2;
                            return {};
                        })());
                    }
                    return {};
                })());
                return (Prelude::tuple2<Prelude::list<t330, c63>,Prelude::list<t331, c63>>{retA, retB});
            })());
        })());
    }
}

namespace List {
    template<typename t336, int c64>
    t336 sum(Prelude::list<t336, c64> lst) {
        return (([&]() -> t336 {
            auto n = c64;
            return List::foldl<t336, t336, c64>(add<t336>, 0, lst);
        })());
    }
}

namespace List {
    template<typename t345, int c65>
    t345 average(Prelude::list<t345, c65> lst) {
        return (([&]() -> t345 {
            auto n = c65;
            return (sum<t345, c65>(lst) / (lst).length);
        })());
    }
}

namespace Signal {
    template<typename t347, typename t348>
    Prelude::sig<t348> map(juniper::function<t348(t347)> f, Prelude::sig<t347> s) {
        return (([&]() -> Prelude::sig<t348> {
            auto guid78 = s;
            return ((((guid78).tag == 0) && ((((guid78).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<t348> {
                    auto val = ((guid78).signal).just;
                    return signal<t348>(just<t348>(f(val)));
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<t348> {
                        return signal<t348>(nothing<t348>());
                    })())
                :
                    juniper::quit<Prelude::sig<t348>>()));
        })());
    }
}

namespace Signal {
    template<typename t359>
    Prelude::unit sink(juniper::function<Prelude::unit(t359)> f, Prelude::sig<t359> s) {
        return (([&]() -> Prelude::unit {
            auto guid79 = s;
            return ((((guid79).tag == 0) && ((((guid79).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::unit {
                    auto val = ((guid79).signal).just;
                    return f(val);
                })())
            :
                (true ? 
                    (([&]() -> Prelude::unit {
                        return Prelude::unit();
                    })())
                :
                    juniper::quit<Prelude::unit>()));
        })());
    }
}

namespace Signal {
    template<typename t363>
    Prelude::sig<t363> filter(juniper::function<bool(t363)> f, Prelude::sig<t363> s) {
        return (([&]() -> Prelude::sig<t363> {
            auto guid80 = s;
            return ((((guid80).tag == 0) && ((((guid80).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<t363> {
                    auto val = ((guid80).signal).just;
                    return (f(val) ? 
                        signal<t363>(nothing<t363>())
                    :
                        s);
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<t363> {
                        return signal<t363>(nothing<t363>());
                    })())
                :
                    juniper::quit<Prelude::sig<t363>>()));
        })());
    }
}

namespace Signal {
    template<typename t373>
    Prelude::sig<t373> merge(Prelude::sig<t373> sigA, Prelude::sig<t373> sigB) {
        return (([&]() -> Prelude::sig<t373> {
            auto guid81 = sigA;
            return ((((guid81).tag == 0) && ((((guid81).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<t373> {
                    return sigA;
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<t373> {
                        return sigB;
                    })())
                :
                    juniper::quit<Prelude::sig<t373>>()));
        })());
    }
}

namespace Signal {
    template<typename t375, int c66>
    Prelude::sig<t375> mergeMany(Prelude::list<Prelude::sig<t375>, c66> sigs) {
        return (([&]() -> Prelude::sig<t375> {
            auto n = c66;
            return (([&]() -> Prelude::sig<t375> {
                auto guid82 = nothing<t375>();
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid82;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid83 = 0;
                    uint32_t guid84 = (n - 1);
                    for (uint32_t i = guid83; i <= guid84; i++) {
                        (([&]() -> Prelude::unit {
                            auto guid85 = ret;
                            return ((((guid85).tag == 1) && true) ? 
                                (([&]() -> Prelude::unit {
                                    return (([&]() -> Prelude::unit {
                                        auto guid86 = List::nth<Prelude::sig<t375>, c66>(i, sigs);
                                        if (!((((guid86).tag == 0) && true))) {
                                            juniper::quit<Prelude::unit>();
                                        }
                                        auto heldValue = (guid86).signal;
                                        
                                        (ret = heldValue);
                                        return Prelude::unit();
                                    })());
                                })())
                            :
                                (true ? 
                                    (([&]() -> Prelude::unit {
                                        return Prelude::unit();
                                    })())
                                :
                                    juniper::quit<Prelude::unit>()));
                        })());
                    }
                    return {};
                })());
                return signal<t375>(ret);
            })());
        })());
    }
}

namespace Signal {
    template<typename t383, typename t384>
    Prelude::sig<Prelude::either<t383, t384>> join(Prelude::sig<t383> sigA, Prelude::sig<t384> sigB) {
        return (([&]() -> Prelude::sig<Prelude::either<t383, t384>> {
            auto guid87 = (Prelude::tuple2<Prelude::sig<t383>,Prelude::sig<t384>>{sigA, sigB});
            return (((((guid87).e1).tag == 0) && (((((guid87).e1).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<Prelude::either<t383, t384>> {
                    auto value = (((guid87).e1).signal).just;
                    return signal<Prelude::either<t383, t384>>(just<Prelude::either<t383, t384>>(left<t383, t384>(value)));
                })())
            :
                (((((guid87).e2).tag == 0) && (((((guid87).e2).signal).tag == 0) && true)) ? 
                    (([&]() -> Prelude::sig<Prelude::either<t383, t384>> {
                        auto value = (((guid87).e2).signal).just;
                        return signal<Prelude::either<t383, t384>>(just<Prelude::either<t383, t384>>(right<t383, t384>(value)));
                    })())
                :
                    (true ? 
                        (([&]() -> Prelude::sig<Prelude::either<t383, t384>> {
                            return signal<Prelude::either<t383, t384>>(nothing<Prelude::either<t383, t384>>());
                        })())
                    :
                        juniper::quit<Prelude::sig<Prelude::either<t383, t384>>>())));
        })());
    }
}

namespace Signal {
    template<typename t406>
    Prelude::sig<Prelude::unit> toUnit(Prelude::sig<t406> s) {
        return map<t406, Prelude::unit>(juniper::function<Prelude::unit(t406)>([=](t406 x) mutable -> Prelude::unit { 
            return Prelude::unit();
         }), s);
    }
}

namespace Signal {
    template<typename t411, typename t417>
    Prelude::sig<t417> foldP(juniper::function<t417(t411,t417)> f, juniper::shared_ptr<t417> state0, Prelude::sig<t411> incoming) {
        return (([&]() -> Prelude::sig<t417> {
            auto guid88 = incoming;
            return ((((guid88).tag == 0) && ((((guid88).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<t417> {
                    auto val = ((guid88).signal).just;
                    return (([&]() -> Prelude::sig<t417> {
                        auto guid89 = f(val, (*((state0).get())));
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto state1 = guid89;
                        
                        (*((t417*) (state0.get())) = state1);
                        return signal<t417>(just<t417>(state1));
                    })());
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<t417> {
                        return signal<t417>(nothing<t417>());
                    })())
                :
                    juniper::quit<Prelude::sig<t417>>()));
        })());
    }
}

namespace Signal {
    template<typename t427>
    Prelude::sig<t427> dropRepeats(Prelude::sig<t427> incoming, juniper::shared_ptr<Prelude::maybe<t427>> maybePrevValue) {
        return filter<t427>(juniper::function<bool(t427)>([=](t427 value) mutable -> bool { 
            return (([&]() -> bool {
                auto guid90 = (([&]() -> bool {
                    auto guid91 = (*((maybePrevValue).get()));
                    return ((((guid91).tag == 1) && true) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        ((((guid91).tag == 0) && true) ? 
                            (([&]() -> bool {
                                auto prevValue = (guid91).just;
                                return (value == prevValue);
                            })())
                        :
                            juniper::quit<bool>()));
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto filtered = guid90;
                
                (!(filtered) ? 
                    (([&]() -> Prelude::unit {
                        (*((Prelude::maybe<t427>*) (maybePrevValue.get())) = just<t427>(value));
                        return Prelude::unit();
                    })())
                :
                    Prelude::unit());
                return filtered;
            })());
         }), incoming);
    }
}

namespace Signal {
    template<typename t437>
    Prelude::sig<t437> latch(Prelude::sig<t437> incoming, juniper::shared_ptr<t437> prevValue) {
        return (([&]() -> Prelude::sig<t437> {
            auto guid92 = incoming;
            return ((((guid92).tag == 0) && ((((guid92).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<t437> {
                    auto val = ((guid92).signal).just;
                    return (([&]() -> Prelude::sig<t437> {
                        (*((t437*) (prevValue.get())) = val);
                        return incoming;
                    })());
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<t437> {
                        return signal<t437>(just<t437>((*((prevValue).get()))));
                    })())
                :
                    juniper::quit<Prelude::sig<t437>>()));
        })());
    }
}

namespace Signal {
    template<typename t448, typename t451, typename t446>
    Prelude::sig<t446> map2(juniper::function<t446(t448,t451)> f, Prelude::sig<t448> incomingA, Prelude::sig<t451> incomingB, juniper::shared_ptr<Prelude::tuple2<t448,t451>> state) {
        return (([&]() -> Prelude::sig<t446> {
            auto guid93 = (([&]() -> t448 {
                auto guid94 = incomingA;
                return ((((guid94).tag == 0) && ((((guid94).signal).tag == 0) && true)) ? 
                    (([&]() -> t448 {
                        auto val1 = ((guid94).signal).just;
                        return val1;
                    })())
                :
                    (true ? 
                        (([&]() -> t448 {
                            return fst<t448, t451>((*((state).get())));
                        })())
                    :
                        juniper::quit<t448>()));
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto valA = guid93;
            
            auto guid95 = (([&]() -> t451 {
                auto guid96 = incomingB;
                return ((((guid96).tag == 0) && ((((guid96).signal).tag == 0) && true)) ? 
                    (([&]() -> t451 {
                        auto val2 = ((guid96).signal).just;
                        return val2;
                    })())
                :
                    (true ? 
                        (([&]() -> t451 {
                            return snd<t448, t451>((*((state).get())));
                        })())
                    :
                        juniper::quit<t451>()));
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto valB = guid95;
            
            (*((Prelude::tuple2<t448,t451>*) (state.get())) = (Prelude::tuple2<t448,t451>{valA, valB}));
            return (([&]() -> Prelude::sig<t446> {
                auto guid97 = (Prelude::tuple2<Prelude::sig<t448>,Prelude::sig<t451>>{incomingA, incomingB});
                return (((((guid97).e2).tag == 0) && (((((guid97).e2).signal).tag == 1) && ((((guid97).e1).tag == 0) && (((((guid97).e1).signal).tag == 1) && true)))) ? 
                    (([&]() -> Prelude::sig<t446> {
                        return signal<t446>(nothing<t446>());
                    })())
                :
                    (true ? 
                        (([&]() -> Prelude::sig<t446> {
                            return signal<t446>(just<t446>(f(valA, valB)));
                        })())
                    :
                        juniper::quit<Prelude::sig<t446>>()));
            })());
        })());
    }
}

namespace Signal {
    template<typename t467, int c67>
    Prelude::sig<Prelude::list<t467, c67>> record(Prelude::sig<t467> incoming, juniper::shared_ptr<Prelude::list<t467, c67>> pastValues) {
        return (([&]() -> Prelude::sig<Prelude::list<t467, c67>> {
            auto n = c67;
            return foldP<t467, Prelude::list<t467, c67>>(List::pushOffFront<t467, c67>, pastValues, incoming);
        })());
    }
}

namespace Io {
    Io::pinState toggle(Io::pinState p) {
        return (([&]() -> Io::pinState {
            auto guid98 = p;
            return ((((guid98).tag == 0) && true) ? 
                (([&]() -> Io::pinState {
                    return low();
                })())
            :
                ((((guid98).tag == 1) && true) ? 
                    (([&]() -> Io::pinState {
                        return high();
                    })())
                :
                    juniper::quit<Io::pinState>()));
        })());
    }
}

namespace Io {
    Prelude::unit printStr(const char * str) {
        return (([&]() -> Prelude::unit {
            Serial.print(str);
            return {};
        })());
    }
}

namespace Io {
    template<int c68>
    Prelude::unit printCharList(Prelude::list<uint8_t, c68> cl) {
        return (([&]() -> Prelude::unit {
            auto n = c68;
            return (([&]() -> Prelude::unit {
                Serial.print((char *) &cl.data[0]);
                return {};
            })());
        })());
    }
}

namespace Io {
    Prelude::unit printFloat(float f) {
        return (([&]() -> Prelude::unit {
            Serial.print(f);
            return {};
        })());
    }
}

namespace Io {
    Prelude::unit printInt(int32_t n) {
        return (([&]() -> Prelude::unit {
            Serial.print(n);
            return {};
        })());
    }
}

namespace Io {
    int32_t baseToInt(Io::base b) {
        return (([&]() -> int32_t {
            auto guid99 = b;
            return ((((guid99).tag == 0) && true) ? 
                (([&]() -> int32_t {
                    return 2;
                })())
            :
                ((((guid99).tag == 1) && true) ? 
                    (([&]() -> int32_t {
                        return 8;
                    })())
                :
                    ((((guid99).tag == 2) && true) ? 
                        (([&]() -> int32_t {
                            return 10;
                        })())
                    :
                        ((((guid99).tag == 3) && true) ? 
                            (([&]() -> int32_t {
                                return 16;
                            })())
                        :
                            juniper::quit<int32_t>()))));
        })());
    }
}

namespace Io {
    Prelude::unit printIntBase(int32_t n, Io::base b) {
        return (([&]() -> Prelude::unit {
            auto guid100 = baseToInt(b);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto bint = guid100;
            
            return (([&]() -> Prelude::unit {
                Serial.print(n, bint);
                return {};
            })());
        })());
    }
}

namespace Io {
    Prelude::unit printFloatPlaces(float f, int32_t numPlaces) {
        return (([&]() -> Prelude::unit {
            Serial.print(f, numPlaces);
            return {};
        })());
    }
}

namespace Io {
    Prelude::unit beginSerial(uint32_t speed) {
        return (([&]() -> Prelude::unit {
            Serial.begin(speed);
            return {};
        })());
    }
}

namespace Io {
    int32_t pinStateToInt(Io::pinState value) {
        return (([&]() -> int32_t {
            auto guid101 = value;
            return ((((guid101).tag == 1) && true) ? 
                (([&]() -> int32_t {
                    return 0;
                })())
            :
                ((((guid101).tag == 0) && true) ? 
                    (([&]() -> int32_t {
                        return 1;
                    })())
                :
                    juniper::quit<int32_t>()));
        })());
    }
}

namespace Io {
    Io::pinState intToPinState(uint8_t value) {
        return ((value == 0) ? 
            low()
        :
            high());
    }
}

namespace Io {
    Prelude::unit digWrite(uint16_t pin, Io::pinState value) {
        return (([&]() -> Prelude::unit {
            auto guid102 = pinStateToInt(value);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto intVal = guid102;
            
            return (([&]() -> Prelude::unit {
                digitalWrite(pin, intVal);
                return {};
            })());
        })());
    }
}

namespace Io {
    Io::pinState digRead(uint16_t pin) {
        return (([&]() -> Io::pinState {
            auto guid103 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto intVal = guid103;
            
            (([&]() -> Prelude::unit {
                intVal = digitalRead(pin);
                return {};
            })());
            return intToPinState(intVal);
        })());
    }
}

namespace Io {
    Prelude::sig<Io::pinState> digIn(uint16_t pin) {
        return signal<Io::pinState>(just<Io::pinState>(digRead(pin)));
    }
}

namespace Io {
    Prelude::unit digOut(uint16_t pin, Prelude::sig<Io::pinState> sig) {
        return Signal::sink<Io::pinState>(juniper::function<Prelude::unit(Io::pinState)>([=](Io::pinState value) mutable -> Prelude::unit { 
            return digWrite(pin, value);
         }), sig);
    }
}

namespace Io {
    int32_t anaRead(uint16_t pin) {
        return (([&]() -> int32_t {
            auto guid104 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto value = guid104;
            
            (([&]() -> Prelude::unit {
                value = analogRead(pin);
                return {};
            })());
            return value;
        })());
    }
}

namespace Io {
    Prelude::unit anaWrite(uint16_t pin, uint8_t value) {
        return (([&]() -> Prelude::unit {
            analogWrite(pin, value);
            return {};
        })());
    }
}

namespace Io {
    Prelude::sig<uint16_t> anaIn(uint16_t pin) {
        return signal<uint16_t>(just<uint16_t>(anaRead(pin)));
    }
}

namespace Io {
    Prelude::unit anaOut(uint16_t pin, Prelude::sig<uint16_t> sig) {
        return Signal::sink<uint16_t>(juniper::function<Prelude::unit(uint16_t)>([=](uint16_t value) mutable -> Prelude::unit { 
            return anaWrite(pin, value);
         }), sig);
    }
}

namespace Io {
    int32_t pinModeToInt(Io::mode m) {
        return (([&]() -> int32_t {
            auto guid105 = m;
            return ((((guid105).tag == 0) && true) ? 
                (([&]() -> int32_t {
                    return 0;
                })())
            :
                ((((guid105).tag == 1) && true) ? 
                    (([&]() -> int32_t {
                        return 1;
                    })())
                :
                    ((((guid105).tag == 2) && true) ? 
                        (([&]() -> int32_t {
                            return 2;
                        })())
                    :
                        juniper::quit<int32_t>())));
        })());
    }
}

namespace Io {
    Io::mode intToPinMode(uint8_t m) {
        return (([&]() -> Io::mode {
            auto guid106 = m;
            return (((guid106 == 0) && true) ? 
                (([&]() -> Io::mode {
                    return input();
                })())
            :
                (((guid106 == 1) && true) ? 
                    (([&]() -> Io::mode {
                        return output();
                    })())
                :
                    (((guid106 == 2) && true) ? 
                        (([&]() -> Io::mode {
                            return inputPullup();
                        })())
                    :
                        juniper::quit<Io::mode>())));
        })());
    }
}

namespace Io {
    Prelude::unit setPinMode(uint16_t pin, Io::mode m) {
        return (([&]() -> Prelude::unit {
            auto guid107 = pinModeToInt(m);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto m2 = guid107;
            
            return (([&]() -> Prelude::unit {
                pinMode(pin, m2);
                return {};
            })());
        })());
    }
}

namespace Io {
    Prelude::sig<Prelude::unit> risingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::toUnit<Io::pinState>(Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid108 = (([&]() -> bool {
                    auto guid109 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid109).e2).tag == 1) && ((((guid109).e1).tag == 0) && true)) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        (true ? 
                            (([&]() -> bool {
                                return true;
                            })())
                        :
                            juniper::quit<bool>()));
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid108;
                
                (*((Io::pinState*) (prevState.get())) = currState);
                return ret;
            })());
         }), sig));
    }
}

namespace Io {
    Prelude::sig<Prelude::unit> fallingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::toUnit<Io::pinState>(Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid110 = (([&]() -> bool {
                    auto guid111 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid111).e2).tag == 0) && ((((guid111).e1).tag == 1) && true)) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        (true ? 
                            (([&]() -> bool {
                                return true;
                            })())
                        :
                            juniper::quit<bool>()));
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid110;
                
                (*((Io::pinState*) (prevState.get())) = currState);
                return ret;
            })());
         }), sig));
    }
}

namespace Io {
    Prelude::sig<Io::pinState> edge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid112 = (([&]() -> bool {
                    auto guid113 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid113).e2).tag == 1) && ((((guid113).e1).tag == 0) && true)) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        (((((guid113).e2).tag == 0) && ((((guid113).e1).tag == 1) && true)) ? 
                            (([&]() -> bool {
                                return false;
                            })())
                        :
                            (true ? 
                                (([&]() -> bool {
                                    return true;
                                })())
                            :
                                juniper::quit<bool>())));
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid112;
                
                (*((Io::pinState*) (prevState.get())) = currState);
                return ret;
            })());
         }), sig);
    }
}

namespace Maybe {
    template<typename t570, typename t571>
    Prelude::maybe<t571> map(juniper::function<t571(t570)> f, Prelude::maybe<t570> maybeVal) {
        return (([&]() -> Prelude::maybe<t571> {
            auto guid114 = maybeVal;
            return ((((guid114).tag == 0) && true) ? 
                (([&]() -> Prelude::maybe<t571> {
                    auto val = (guid114).just;
                    return just<t571>(f(val));
                })())
            :
                (true ? 
                    (([&]() -> Prelude::maybe<t571> {
                        return nothing<t571>();
                    })())
                :
                    juniper::quit<Prelude::maybe<t571>>()));
        })());
    }
}

namespace Maybe {
    template<typename t578>
    t578 get(Prelude::maybe<t578> maybeVal) {
        return (([&]() -> t578 {
            auto guid115 = maybeVal;
            return ((((guid115).tag == 0) && true) ? 
                (([&]() -> t578 {
                    auto val = (guid115).just;
                    return val;
                })())
            :
                juniper::quit<t578>());
        })());
    }
}

namespace Maybe {
    template<typename t580>
    bool isJust(Prelude::maybe<t580> maybeVal) {
        return (([&]() -> bool {
            auto guid116 = maybeVal;
            return ((((guid116).tag == 0) && true) ? 
                (([&]() -> bool {
                    return true;
                })())
            :
                (true ? 
                    (([&]() -> bool {
                        return false;
                    })())
                :
                    juniper::quit<bool>()));
        })());
    }
}

namespace Maybe {
    template<typename t582>
    bool isNothing(Prelude::maybe<t582> maybeVal) {
        return !(isJust<t582>(maybeVal));
    }
}

namespace Maybe {
    template<typename t586>
    int32_t count(Prelude::maybe<t586> maybeVal) {
        return (([&]() -> int32_t {
            auto guid117 = maybeVal;
            return ((((guid117).tag == 0) && true) ? 
                (([&]() -> int32_t {
                    return 1;
                })())
            :
                (true ? 
                    (([&]() -> int32_t {
                        return 0;
                    })())
                :
                    juniper::quit<int32_t>()));
        })());
    }
}

namespace Maybe {
    template<typename t588, typename t589>
    t589 foldl(juniper::function<t589(t588,t589)> f, t589 initState, Prelude::maybe<t588> maybeVal) {
        return (([&]() -> t589 {
            auto guid118 = maybeVal;
            return ((((guid118).tag == 0) && true) ? 
                (([&]() -> t589 {
                    auto val = (guid118).just;
                    return f(val, initState);
                })())
            :
                (true ? 
                    (([&]() -> t589 {
                        return initState;
                    })())
                :
                    juniper::quit<t589>()));
        })());
    }
}

namespace Maybe {
    template<typename t594, typename t595>
    t595 fodlr(juniper::function<t595(t594,t595)> f, t595 initState, Prelude::maybe<t594> maybeVal) {
        return foldl<t594, t595>(f, initState, maybeVal);
    }
}

namespace Maybe {
    template<typename t601>
    Prelude::unit iter(juniper::function<Prelude::unit(t601)> f, Prelude::maybe<t601> maybeVal) {
        return (([&]() -> Prelude::unit {
            auto guid119 = maybeVal;
            return ((((guid119).tag == 0) && true) ? 
                (([&]() -> Prelude::unit {
                    auto val = (guid119).just;
                    return f(val);
                })())
            :
                (true ? 
                    (([&]() -> Prelude::unit {
                        auto nothing = guid119;
                        return Prelude::unit();
                    })())
                :
                    juniper::quit<Prelude::unit>()));
        })());
    }
}

namespace Time {
    Prelude::unit wait(uint32_t time) {
        return (([&]() -> Prelude::unit {
            delay(time);
            return {};
        })());
    }
}

namespace Time {
    int32_t now() {
        return (([&]() -> int32_t {
            auto guid120 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid120;
            
            (([&]() -> Prelude::unit {
                ret = millis();
                return {};
            })());
            return ret;
        })());
    }
}

namespace Time {
    juniper::shared_ptr<Time::timerState> state() {
        return (juniper::shared_ptr<Time::timerState>(new Time::timerState((([&]() -> Time::timerState{
            Time::timerState guid121;
            guid121.lastPulse = 0;
            return guid121;
        })()))));
    }
}

namespace Time {
    Prelude::sig<uint32_t> every(uint32_t interval, juniper::shared_ptr<Time::timerState> state) {
        return (([&]() -> Prelude::sig<uint32_t> {
            auto guid122 = now();
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto t = guid122;
            
            auto guid123 = ((interval == 0) ? 
                t
            :
                ((t / interval) * interval));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto lastWindow = guid123;
            
            return ((((*((state).get()))).lastPulse >= lastWindow) ? 
                signal<uint32_t>(nothing<uint32_t>())
            :
                (([&]() -> Prelude::sig<uint32_t> {
                    (*((Time::timerState*) (state.get())) = (([&]() -> Time::timerState{
                        Time::timerState guid124;
                        guid124.lastPulse = t;
                        return guid124;
                    })()));
                    return signal<uint32_t>(just<uint32_t>(t));
                })()));
        })());
    }
}

namespace Math {
    double pi = 3.141593;
}

namespace Math {
    double e = 2.718282;
}

namespace Math {
    double degToRad(double degrees) {
        return (degrees * 0.017453);
    }
}

namespace Math {
    double radToDeg(double radians) {
        return (radians * 57.295780);
    }
}

namespace Math {
    double acos_(double x) {
        return (([&]() -> double {
            auto guid125 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid125;
            
            (([&]() -> Prelude::unit {
                ret = acos(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double asin_(double x) {
        return (([&]() -> double {
            auto guid126 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid126;
            
            (([&]() -> Prelude::unit {
                ret = asin(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double atan_(double x) {
        return (([&]() -> double {
            auto guid127 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid127;
            
            (([&]() -> Prelude::unit {
                ret = atan(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double atan2_(double y, double x) {
        return (([&]() -> double {
            auto guid128 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid128;
            
            (([&]() -> Prelude::unit {
                ret = atan2(y, x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double cos_(double x) {
        return (([&]() -> double {
            auto guid129 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid129;
            
            (([&]() -> Prelude::unit {
                ret = cos(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double cosh_(double x) {
        return (([&]() -> double {
            auto guid130 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid130;
            
            (([&]() -> Prelude::unit {
                ret = cosh(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double sin_(double x) {
        return (([&]() -> double {
            auto guid131 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid131;
            
            (([&]() -> Prelude::unit {
                ret = sin(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double sinh_(double x) {
        return (([&]() -> double {
            auto guid132 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid132;
            
            (([&]() -> Prelude::unit {
                ret = sinh(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double tan_(double x) {
        return (sin_(x) / cos_(x));
    }
}

namespace Math {
    double tanh_(double x) {
        return (([&]() -> double {
            auto guid133 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid133;
            
            (([&]() -> Prelude::unit {
                ret = tanh(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double exp_(double x) {
        return (([&]() -> double {
            auto guid134 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid134;
            
            (([&]() -> Prelude::unit {
                ret = exp(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    Prelude::tuple2<double,int16_t> frexp_(double x) {
        return (([&]() -> Prelude::tuple2<double,int16_t> {
            auto guid135 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid135;
            
            auto guid136 = ((int16_t) 0);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto exponent = guid136;
            
            (([&]() -> Prelude::unit {
                int exponent2 = (int) exponent;
    ret = frexp(x, &exponent2);
                return {};
            })());
            return (Prelude::tuple2<double,int16_t>{ret, exponent});
        })());
    }
}

namespace Math {
    double ldexp_(double x, int16_t exponent) {
        return (([&]() -> double {
            auto guid137 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid137;
            
            (([&]() -> Prelude::unit {
                ret = ldexp(x, exponent);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double log_(double x) {
        return (([&]() -> double {
            auto guid138 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid138;
            
            (([&]() -> Prelude::unit {
                ret = log(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double log10_(double x) {
        return (([&]() -> double {
            auto guid139 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid139;
            
            (([&]() -> Prelude::unit {
                ret = log10(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    Prelude::tuple2<double,double> modf_(double x) {
        return (([&]() -> Prelude::tuple2<double,double> {
            auto guid140 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid140;
            
            auto guid141 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto integer = guid141;
            
            (([&]() -> Prelude::unit {
                ret = modf(x, &integer);
                return {};
            })());
            return (Prelude::tuple2<double,double>{ret, integer});
        })());
    }
}

namespace Math {
    double pow_(double x, double y) {
        return (([&]() -> double {
            auto guid142 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid142;
            
            (([&]() -> Prelude::unit {
                ret = pow(x, y);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double sqrt_(double x) {
        return (([&]() -> double {
            auto guid143 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid143;
            
            (([&]() -> Prelude::unit {
                ret = sqrt(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double ceil_(double x) {
        return (([&]() -> double {
            auto guid144 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid144;
            
            (([&]() -> Prelude::unit {
                ret = ceil(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double fabs_(double x) {
        return (([&]() -> double {
            auto guid145 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid145;
            
            (([&]() -> Prelude::unit {
                ret = fabs(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double floor_(double x) {
        return (([&]() -> double {
            auto guid146 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid146;
            
            (([&]() -> Prelude::unit {
                ret = floor(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double fmod_(double x, double y) {
        return (([&]() -> double {
            auto guid147 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid147;
            
            (([&]() -> Prelude::unit {
                ret = fmod(x, y);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double round_(double x) {
        return floor_((x + 0.500000));
    }
}

namespace Math {
    double min_(double x, double y) {
        return ((x > y) ? 
            y
        :
            x);
    }
}

namespace Math {
    double max_(double x, double y) {
        return ((x > y) ? 
            x
        :
            y);
    }
}

namespace Math {
    double mapRange(double x, double a1, double a2, double b1, double b2) {
        return (b1 + (((x - a1) * (b2 - b1)) / (a2 - a1)));
    }
}

namespace Math {
    template<typename t654>
    t654 clamp(t654 x, t654 min, t654 max) {
        return ((min > x) ? 
            min
        :
            ((x > max) ? 
                max
            :
                x));
    }
}

namespace Math {
    template<typename t656>
    int32_t sign(t656 n) {
        return ((n == 0) ? 
            0
        :
            ((n > 0) ? 
                1
            :
                -(1)));
    }
}

namespace Button {
    juniper::shared_ptr<Button::buttonState> state() {
        return (juniper::shared_ptr<Button::buttonState>(new Button::buttonState((([&]() -> Button::buttonState{
            Button::buttonState guid148;
            guid148.actualState = Io::low();
            guid148.lastState = Io::low();
            guid148.lastDebounceTime = 0;
            return guid148;
        })()))));
    }
}

namespace Button {
    Prelude::sig<Io::pinState> debounceDelay(Prelude::sig<Io::pinState> incoming, uint16_t delay, juniper::shared_ptr<Button::buttonState> buttonState) {
        return Signal::map<Io::pinState, Io::pinState>(juniper::function<Io::pinState(Io::pinState)>([=](Io::pinState currentState) mutable -> Io::pinState { 
            return (([&]() -> Io::pinState {
                auto guid149 = (*((buttonState).get()));
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto lastDebounceTime = (guid149).lastDebounceTime;
                auto lastState = (guid149).lastState;
                auto actualState = (guid149).actualState;
                
                return ((currentState != lastState) ? 
                    (([&]() -> Io::pinState {
                        (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                            Button::buttonState guid150;
                            guid150.actualState = actualState;
                            guid150.lastState = currentState;
                            guid150.lastDebounceTime = Time::now();
                            return guid150;
                        })()));
                        return actualState;
                    })())
                :
                    (((currentState != actualState) && ((Time::now() - ((*((buttonState).get()))).lastDebounceTime) > delay)) ? 
                        (([&]() -> Io::pinState {
                            (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                                Button::buttonState guid151;
                                guid151.actualState = currentState;
                                guid151.lastState = currentState;
                                guid151.lastDebounceTime = lastDebounceTime;
                                return guid151;
                            })()));
                            return currentState;
                        })())
                    :
                        (([&]() -> Io::pinState {
                            (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                                Button::buttonState guid152;
                                guid152.actualState = actualState;
                                guid152.lastState = currentState;
                                guid152.lastDebounceTime = lastDebounceTime;
                                return guid152;
                            })()));
                            return actualState;
                        })())));
            })());
         }), incoming);
    }
}

namespace Button {
    Prelude::sig<Io::pinState> debounce(Prelude::sig<Io::pinState> incoming, juniper::shared_ptr<Button::buttonState> buttonState) {
        return debounceDelay(incoming, 50, buttonState);
    }
}

namespace Vector {
    int32_t x = 0;
}

namespace Vector {
    int32_t y = 1;
}

namespace Vector {
    int32_t z = 2;
}

namespace Vector {
    template<typename t679, int c69>
    Vector::vector<t679, c69> make(juniper::array<t679, c69> d) {
        return (([&]() -> Vector::vector<t679, c69> {
            auto n = c69;
            return (([&]() -> Vector::vector<t679, c69>{
                Vector::vector<t679, c69> guid153;
                guid153.data = d;
                return guid153;
            })());
        })());
    }
}

namespace Vector {
    template<typename t682, int c71>
    t682 get(uint32_t i, Vector::vector<t682, c71> v) {
        return (([&]() -> t682 {
            auto n = c71;
            return (([&]() -> t682 {
                auto guid154 = v;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto data = (guid154).data;
                
                return (data)[i];
            })());
        })());
    }
}

namespace Vector {
    template<typename t684, int c72>
    Vector::vector<t684, c72> add(Vector::vector<t684, c72> v1, Vector::vector<t684, c72> v2) {
        return (([&]() -> Vector::vector<t684, c72> {
            auto n = c72;
            return (([&]() -> Vector::vector<t684, c72> {
                auto guid155 = v1;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto result = guid155;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid156 = 0;
                    uint32_t guid157 = (n - 1);
                    for (uint32_t i = guid156; i <= guid157; i++) {
                        (((result).data)[i] = (((result).data)[i] + ((v2).data)[i]()));
                    }
                    return {};
                })());
                return result;
            })());
        })());
    }
}

namespace Vector {
    template<typename t693, int c76>
    Vector::vector<t693, c76> zero() {
        return (([&]() -> Vector::vector<t693, c76> {
            auto n = c76;
            return (([&]() -> Vector::vector<t693, c76>{
                Vector::vector<t693, c76> guid158;
                guid158.data = (juniper::array<t693, c76>().fill(0));
                return guid158;
            })());
        })());
    }
}

namespace Vector {
    template<typename t695, int c77>
    Vector::vector<t695, c77> subtract(Vector::vector<t695, c77> v1, Vector::vector<t695, c77> v2) {
        return (([&]() -> Vector::vector<t695, c77> {
            auto n = c77;
            return (([&]() -> Vector::vector<t695, c77> {
                auto guid159 = v1;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto result = guid159;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid160 = 0;
                    uint32_t guid161 = (n - 1);
                    for (uint32_t i = guid160; i <= guid161; i++) {
                        (([&]() -> Prelude::unit {
                            (((result).data)[i] = (((result).data)[i] - ((v2).data)[i]));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return result;
            })());
        })());
    }
}

namespace Vector {
    template<typename t703, int c81>
    Vector::vector<t703, c81> scale(t703 scalar, Vector::vector<t703, c81> v) {
        return (([&]() -> Vector::vector<t703, c81> {
            auto n = c81;
            return (([&]() -> Vector::vector<t703, c81> {
                auto guid162 = v;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto result = guid162;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid163 = 0;
                    uint32_t guid164 = (n - 1);
                    for (uint32_t i = guid163; i <= guid164; i++) {
                        (([&]() -> Prelude::unit {
                            (((result).data)[i] = (((result).data)[i] * scalar));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return result;
            })());
        })());
    }
}

namespace Vector {
    template<typename t709, int c84>
    t709 dot(Vector::vector<t709, c84> v1, Vector::vector<t709, c84> v2) {
        return (([&]() -> t709 {
            auto n = c84;
            return (([&]() -> t709 {
                auto guid165 = 0;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto sum = guid165;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid166 = 0;
                    uint32_t guid167 = (n - 1);
                    for (uint32_t i = guid166; i <= guid167; i++) {
                        (([&]() -> Prelude::unit {
                            (sum = (sum + (((v1).data)[i] * ((v2).data)[i])));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return sum;
            })());
        })());
    }
}

namespace Vector {
    template<typename t715, int c87>
    t715 magnitude2(Vector::vector<t715, c87> v) {
        return (([&]() -> t715 {
            auto n = c87;
            return (([&]() -> t715 {
                auto guid168 = 0;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto sum = guid168;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid169 = 0;
                    uint32_t guid170 = (n - 1);
                    for (uint32_t i = guid169; i <= guid170; i++) {
                        (([&]() -> Prelude::unit {
                            (sum = (sum + (((v).data)[i] * ((v).data)[i])));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return sum;
            })());
        })());
    }
}

namespace Vector {
    template<typename t721, int c90>
    double magnitude(Vector::vector<t721, c90> v) {
        return (([&]() -> double {
            auto n = c90;
            return sqrt_(magnitude2<t721, c90>(v));
        })());
    }
}

namespace Vector {
    template<typename t727, int c91>
    Vector::vector<t727, c91> multiply(Vector::vector<t727, c91> u, Vector::vector<t727, c91> v) {
        return (([&]() -> Vector::vector<t727, c91> {
            auto n = c91;
            return (([&]() -> Vector::vector<t727, c91> {
                auto guid171 = u;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto result = guid171;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid172 = 0;
                    uint32_t guid173 = (n - 1);
                    for (uint32_t i = guid172; i <= guid173; i++) {
                        (([&]() -> Prelude::unit {
                            (((result).data)[i] = (((result).data)[i] * ((v).data)[i]));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return result;
            })());
        })());
    }
}

namespace Vector {
    template<typename t735, int c95>
    Vector::vector<t735, c95> normalize(Vector::vector<t735, c95> v) {
        return (([&]() -> Vector::vector<t735, c95> {
            auto n = c95;
            return (([&]() -> Vector::vector<t735, c95> {
                auto guid174 = magnitude<t735, c95>(v);
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto mag = guid174;
                
                return ((mag > 0) ? 
                    (([&]() -> Vector::vector<t735, c95> {
                        auto guid175 = v;
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto result = guid175;
                        
                        (([&]() -> Prelude::unit {
                            uint32_t guid176 = 0;
                            uint32_t guid177 = (n - 1);
                            for (uint32_t i = guid176; i <= guid177; i++) {
                                (([&]() -> Prelude::unit {
                                    (((result).data)[i] = (((result).data)[i] / mag));
                                    return Prelude::unit();
                                })());
                            }
                            return {};
                        })());
                        return result;
                    })())
                :
                    v);
            })());
        })());
    }
}

namespace Vector {
    template<typename t743, int c98>
    double angle(Vector::vector<t743, c98> v1, Vector::vector<t743, c98> v2) {
        return (([&]() -> double {
            auto n = c98;
            return acos_((dot<t743, c98>(v1, v2) / sqrt_((magnitude2<t743, c98>(v1) * magnitude2<t743, c98>(v2)))));
        })());
    }
}

namespace Vector {
    template<typename t781>
    Vector::vector<t781, 3> cross(Vector::vector<t781, 3> u, Vector::vector<t781, 3> v) {
        return (([&]() -> Vector::vector<t781, 3>{
            Vector::vector<t781, 3> guid178;
            guid178.data = (juniper::array<t781, 3> { {((((u).data)[1] * ((v).data)[2]) - (((u).data)[2] * ((v).data)[1])), ((((u).data)[2] * ((v).data)[0]) - (((u).data)[0] * ((v).data)[2])), ((((u).data)[0] * ((v).data)[1]) - (((u).data)[1] * ((v).data)[0]))} });
            return guid178;
        })());
    }
}

namespace Vector {
    template<typename t783, int c111>
    Vector::vector<t783, c111> project(Vector::vector<t783, c111> a, Vector::vector<t783, c111> b) {
        return (([&]() -> Vector::vector<t783, c111> {
            auto n = c111;
            return (([&]() -> Vector::vector<t783, c111> {
                auto guid179 = normalize<t783, c111>(b);
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto bn = guid179;
                
                return scale<t783, c111>(dot<t783, c111>(a, bn), bn);
            })());
        })());
    }
}

namespace Vector {
    template<typename t793, int c112>
    Vector::vector<t793, c112> projectPlane(Vector::vector<t793, c112> a, Vector::vector<t793, c112> m) {
        return (([&]() -> Vector::vector<t793, c112> {
            auto n = c112;
            return subtract<t793, c112>(a, project<t793, c112>(a, m));
        })());
    }
}

namespace CharList {
    template<int c113>
    Prelude::list<uint8_t, c113> toUpper(Prelude::list<uint8_t, c113> str) {
        return List::map<uint8_t, uint8_t, c113>(juniper::function<uint8_t(uint8_t)>([=](uint8_t c) mutable -> uint8_t { 
            return (((c >= ((uint8_t) 97)) && (c <= ((uint8_t) 122))) ? 
                (c - ((uint8_t) 32))
            :
                c);
         }), str);
    }
}

namespace CharList {
    template<int c114>
    Prelude::list<uint8_t, c114> toLower(Prelude::list<uint8_t, c114> str) {
        return List::map<uint8_t, uint8_t, c114>(juniper::function<uint8_t(uint8_t)>([=](uint8_t c) mutable -> uint8_t { 
            return (((c >= ((uint8_t) 65)) && (c <= ((uint8_t) 90))) ? 
                (c + ((uint8_t) 32))
            :
                c);
         }), str);
    }
}

namespace FastLed {
    FastLed::fastLedStrip make(uint16_t numLeds) {
        return (([&]() -> FastLed::fastLedStrip {
            auto guid180 = juniper::shared_ptr<void>(NULL);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto p = guid180;
            
            (([&]() -> Prelude::unit {
                
    CRGB *leds = new CRGB[numLeds];
    p.set((void *) leds);
    FastLED.addLeds<WS2812, 6, RGB>(leds, numLeds);
    
                return {};
            })());
            Io::setPinMode(6, Io::output());
            return (([&]() -> FastLed::fastLedStrip{
                FastLed::fastLedStrip guid181;
                guid181.ptr = p;
                return guid181;
            })());
        })());
    }
}

namespace FastLed {
    template<typename t822, typename t823>
    Prelude::unit setLedColor(uint16_t n, t822 c, t823 strip) {
        return (([&]() -> Prelude::unit {
            auto guid182 = strip;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto p = (guid182).ptr;
            
            auto guid183 = (strip).ptr;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto p = guid183;
            
            auto guid184 = (c).r;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto r = guid184;
            
            auto guid185 = (c).g;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto g = guid185;
            
            auto guid186 = (c).b;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto b = guid186;
            
            return (([&]() -> Prelude::unit {
                ((CRGB *) p.get())[n] = CRGB(g, r, b);
                return {};
            })());
        })());
    }
}

namespace FastLed {
    template<typename t829>
    FastLed::color getLedColor(uint16_t n, t829 strip) {
        return (([&]() -> FastLed::color {
            auto guid187 = strip;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto p = (guid187).ptr;
            
            auto guid188 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto r = guid188;
            
            auto guid189 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto g = guid189;
            
            auto guid190 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto b = guid190;
            
            (([&]() -> Prelude::unit {
                
    CRGB c = ((CRGB *) p.get())[n];
    r = c.r;
    g = c.g;
    b = c.b;
    
                return {};
            })());
            return (([&]() -> FastLed::color{
                FastLed::color guid191;
                guid191.r = g;
                guid191.g = r;
                guid191.b = b;
                return guid191;
            })());
        })());
    }
}

namespace FastLed {
    Prelude::unit show() {
        return (([&]() -> Prelude::unit {
            FastLED.show();
            return {};
        })());
    }
}

namespace Accelerometer {
    int32_t xPin = (([&]() -> int32_t {
        auto guid192 = 0;
        if (!(true)) {
            juniper::quit<Prelude::unit>();
        }
        auto p = guid192;
        
        (([&]() -> Prelude::unit {
            p = A0;
            return {};
        })());
        return p;
    })());
}

namespace Accelerometer {
    int32_t yPin = (([&]() -> int32_t {
        auto guid193 = 0;
        if (!(true)) {
            juniper::quit<Prelude::unit>();
        }
        auto p = guid193;
        
        (([&]() -> Prelude::unit {
            p = A1;
            return {};
        })());
        return p;
    })());
}

namespace Accelerometer {
    int32_t zPin = (([&]() -> int32_t {
        auto guid194 = 0;
        if (!(true)) {
            juniper::quit<Prelude::unit>();
        }
        auto p = guid194;
        
        (([&]() -> Prelude::unit {
            p = A2;
            return {};
        })());
        return p;
    })());
}

namespace Accelerometer {
    int32_t axisToPin(Accelerometer::axis a) {
        return (([&]() -> int32_t {
            auto guid195 = a;
            return ((((guid195).tag == 0) && true) ? 
                (([&]() -> int32_t {
                    return xPin;
                })())
            :
                ((((guid195).tag == 1) && true) ? 
                    (([&]() -> int32_t {
                        return yPin;
                    })())
                :
                    ((((guid195).tag == 2) && true) ? 
                        (([&]() -> int32_t {
                            return zPin;
                        })())
                    :
                        juniper::quit<int32_t>())));
        })());
    }
}

namespace Accelerometer {
    Prelude::tuple2<int32_t,int32_t> axisToRange(Accelerometer::axis a) {
        return (([&]() -> Prelude::tuple2<int32_t,int32_t> {
            auto guid196 = a;
            return ((((guid196).tag == 0) && true) ? 
                (([&]() -> Prelude::tuple2<int32_t,int32_t> {
                    return (Prelude::tuple2<int32_t,int32_t>{404, 612});
                })())
            :
                ((((guid196).tag == 1) && true) ? 
                    (([&]() -> Prelude::tuple2<int32_t,int32_t> {
                        return (Prelude::tuple2<int32_t,int32_t>{409, 622});
                    })())
                :
                    ((((guid196).tag == 2) && true) ? 
                        (([&]() -> Prelude::tuple2<int32_t,int32_t> {
                            return (Prelude::tuple2<int32_t,int32_t>{418, 622});
                        })())
                    :
                        juniper::quit<Prelude::tuple2<int32_t,int32_t>>())));
        })());
    }
}

namespace Accelerometer {
    int32_t readRaw(Accelerometer::axis a) {
        return (([&]() -> int32_t {
            auto guid197 = axisToPin(a);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto pin = guid197;
            
            Io::anaRead(pin);
            Time::wait(1);
            auto guid198 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto total = guid198;
            
            (([&]() -> Prelude::unit {
                uint8_t guid199 = 0;
                uint8_t guid200 = 3;
                for (uint8_t i = guid199; i <= guid200; i++) {
                    (([&]() -> Prelude::unit {
                        (total = (total + Io::anaRead(pin)));
                        return Prelude::unit();
                    })());
                }
                return {};
            })());
            return (total / 4);
        })());
    }
}

namespace Accelerometer {
    double read(Accelerometer::axis a) {
        return (([&]() -> double {
            auto guid201 = axisToRange(a);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto max = (guid201).e2;
            auto min = (guid201).e1;
            
            return Math::mapRange(readRaw(a), min, max, -(1000), 1000);
        })());
    }
}

namespace Accelerometer {
    Prelude::maybe<Accelerometer::orientation> getOrientation() {
        return (([&]() -> Prelude::maybe<Accelerometer::orientation> {
            auto guid202 = read(xAxis());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto xScaled = guid202;
            
            auto guid203 = read(yAxis());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto yScaled = guid203;
            
            auto guid204 = read(zAxis());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto zScaled = guid204;
            
            return ((-(800) > zScaled) ? 
just<Accelerometer::orientation>(zDown())
            :
                ((zScaled > 800) ? 
just<Accelerometer::orientation>(zUp())
                :
                    ((-(800) > yScaled) ? 
just<Accelerometer::orientation>(yDown())
                    :
                        ((yScaled > 800) ? 
just<Accelerometer::orientation>(yUp())
                        :
                            ((-(800) > xScaled) ? 
just<Accelerometer::orientation>(xDown())
                            :
                                ((xScaled > 800) ? 
just<Accelerometer::orientation>(xUp())
                                :
nothing<Accelerometer::orientation>()))))));
        })());
    }
}

namespace Accelerometer {
    Prelude::sig<Accelerometer::orientation> getSignal() {
        return signal<Accelerometer::orientation>(getOrientation());
    }
}

namespace IoExt {
    Prelude::sig<Io::pinState> every(uint32_t interval, juniper::shared_ptr<Time::timerState> tState, juniper::shared_ptr<Io::pinState> outState) {
        return Signal::foldP<uint32_t, Io::pinState>(juniper::function<Io::pinState(uint32_t,Io::pinState)>([=](uint32_t currentTime, Io::pinState lastState) mutable -> Io::pinState { 
            return Io::toggle(lastState);
         }), outState, Time::every(interval, tState));
    }
}

namespace SignalExt {
    template<typename t920>
    Prelude::sig<t920> constant(t920 val) {
        return signal<t920>(just<t920>(val));
    }
}

namespace SignalExt {
    template<typename t929>
    Prelude::sig<Prelude::maybe<t929>> meta(Prelude::sig<t929> sigA) {
        return (([&]() -> Prelude::sig<Prelude::maybe<t929>> {
            auto guid205 = sigA;
            if (!((((guid205).tag == 0) && true))) {
                juniper::quit<Prelude::unit>();
            }
            auto val = (guid205).signal;
            
            return constant<Prelude::maybe<t929>>(val);
        })());
    }
}

namespace SignalExt {
    template<typename t935>
    Prelude::sig<t944> unmeta(Prelude::sig<Prelude::maybe<t944>> sigA) {
        return (([&]() -> Prelude::sig<t944> {
            auto guid206 = sigA;
            return ((((guid206).tag == 0) && ((((guid206).signal).tag == 0) && (((((guid206).signal).just).tag == 0) && true))) ? 
                (([&]() -> Prelude::sig<t944> {
                    auto val = (((guid206).signal).just).just;
                    return constant<t944>(val);
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<t944> {
                        return signal<t944>(nothing<t944>());
                    })())
                :
                    juniper::quit<Prelude::sig<t944>>()));
        })());
    }
}

namespace SignalExt {
    template<typename t955, typename t956>
    Prelude::sig<Prelude::tuple2<t955,t956>> zip(Prelude::sig<t955> sigA, Prelude::sig<t956> sigB, juniper::shared_ptr<Prelude::tuple2<t955,t956>> state) {
        return Signal::map2<t955, t956, Prelude::tuple2<t955,t956>>(juniper::function<Prelude::tuple2<t955,t956>(t955,t956)>([=](t955 valA, t956 valB) mutable -> Prelude::tuple2<t955,t956> { 
            return (Prelude::tuple2<t955,t956>{valA, valB});
         }), sigA, sigB, state);
    }
}

namespace SignalExt {
    template<typename t963, typename t969>
    Prelude::sig<t963> toggle(t963 val1, t963 val2, juniper::shared_ptr<t963> state, Prelude::sig<t969> incoming) {
        return Signal::foldP<t969, t963>(juniper::function<t963(t969,t963)>([=](t969 event, t963 prevVal) mutable -> t963 { 
            return ((prevVal == val1) ? 
                val2
            :
                val1);
         }), state, incoming);
    }
}

namespace Constants {
    FastLed::color blank = (([&]() -> FastLed::color{
        FastLed::color guid207;
        guid207.r = 0;
        guid207.g = 0;
        guid207.b = 0;
        return guid207;
    })());
}

namespace Constants {
    FastLed::color red = (([&]() -> FastLed::color{
        FastLed::color guid208;
        guid208.r = 255;
        guid208.g = 0;
        guid208.b = 0;
        return guid208;
    })());
}

namespace Constants {
    FastLed::color green = (([&]() -> FastLed::color{
        FastLed::color guid209;
        guid209.r = 0;
        guid209.g = 255;
        guid209.b = 0;
        return guid209;
    })());
}

namespace Constants {
    FastLed::color blue = (([&]() -> FastLed::color{
        FastLed::color guid210;
        guid210.r = 0;
        guid210.g = 0;
        guid210.b = 255;
        return guid210;
    })());
}

namespace Constants {
    FastLed::color white = (([&]() -> FastLed::color{
        FastLed::color guid211;
        guid211.r = 255;
        guid211.g = 255;
        guid211.b = 255;
        return guid211;
    })());
}

namespace Constants {
    FastLed::color pink = (([&]() -> FastLed::color{
        FastLed::color guid212;
        guid212.r = 255;
        guid212.g = 50;
        guid212.b = 100;
        return guid212;
    })());
}

namespace Constants {
    int32_t buttonPin = 4;
}

namespace Constants {
    int32_t numLeds = 33;
}

namespace Constants {
    FastLed::fastLedStrip leds = FastLed::make(numLeds);
}

namespace Timing {
    juniper::shared_ptr<int32_t> lastTime = (juniper::shared_ptr<int32_t>(new int32_t(0)));
}

namespace Timing {
    Prelude::unit reset() {
        return (([&]() -> Prelude::unit {
            (*((int32_t*) (lastTime.get())) = Time::now());
            return Prelude::unit();
        })());
    }
}

namespace Timing {
    template<typename t981, typename t982>
    FastLed::color interpolate(t981 c1, t982 c2, float t) {
        return (([&]() -> FastLed::color {
            auto guid213 = c1;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto b1 = (guid213).b;
            auto g1 = (guid213).g;
            auto r1 = (guid213).r;
            
            auto guid214 = c2;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto b2 = (guid214).b;
            auto g2 = (guid214).g;
            auto r2 = (guid214).r;
            
            return (([&]() -> FastLed::color{
                FastLed::color guid215;
                guid215.r = ((t * r1) + ((1.000000 - t) * r2));
                guid215.g = ((t * g1) + ((1.000000 - t) * g2));
                guid215.b = ((t * b1) + ((1.000000 - t) * b2));
                return guid215;
            })());
        })());
    }
}

namespace Timing {
    Prelude::unit execute(juniper::shared_ptr<int32_t> timeRemaining, float totalTime) {
        return (([&]() -> Prelude::unit {
            auto guid216 = Time::now();
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto currentTime = guid216;
            
            auto guid217 = (currentTime - (*((lastTime).get())));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto deltaT = guid217;
            
            (*((int32_t*) (timeRemaining.get())) = ((*((timeRemaining).get())) - deltaT));
            (*((int32_t*) (lastTime.get())) = currentTime);
            auto guid218 = (*((timeRemaining).get()));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto timeRemainingF = guid218;
            
            auto guid219 = totalTime;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto totalTimeF = guid219;
            
            auto guid220 = (timeRemainingF / totalTimeF);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto tPrime = guid220;
            
            auto guid221 = numLeds;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto numLedsF = guid221;
            
            (([&]() -> Prelude::unit {
                int32_t guid222 = 0;
                int32_t guid223 = (numLeds - 1);
                for (int32_t i = guid222; i <= guid223; i++) {
                    (([&]() -> Prelude::unit {
                        auto guid224 = Math::min_((((i + 1) * numLeds) - (((1 - tPrime) * numLeds) * numLeds)), i);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto pos = guid224;
                        
                        return (((0.000000 <= pos) && (numLeds > pos)) ? 
setLedColor<t995, t996>(pos, interpolate<t997, t998>(red, green, (i / numLedsF)), leds)
                        :
                            Prelude::unit());
                    })());
                }
                return {};
            })());
            return Prelude::unit();
        })());
    }
}

namespace Setting {
    juniper::shared_ptr<Button::buttonState> bState = Button::state();
}

namespace Setting {
    juniper::shared_ptr<Io::pinState> bEdgeState = (juniper::shared_ptr<Io::pinState>(new Io::pinState(Io::low())));
}

namespace Setting {
    juniper::shared_ptr<Setting::timeSetting> numLedsLit = (juniper::shared_ptr<Setting::timeSetting>(new Setting::timeSetting((([&]() -> Setting::timeSetting{
        Setting::timeSetting guid225;
        guid225.minutes = 0;
        guid225.fifteenSeconds = 0;
        return guid225;
    })()))));
}

namespace Setting {
    juniper::shared_ptr<Time::timerState> tState = Time::state();
}

namespace Setting {
    juniper::shared_ptr<Io::pinState> cursorState = (juniper::shared_ptr<Io::pinState>(new Io::pinState(Io::low())));
}

namespace Setting {
    juniper::shared_ptr<Prelude::tuple2<Io::pinState,Setting::timeSetting>> outputUpdateState = (juniper::shared_ptr<Prelude::tuple2<Io::pinState,Setting::timeSetting>>(new Prelude::tuple2<Io::pinState,Setting::timeSetting>((Prelude::tuple2<Io::pinState,Setting::timeSetting>{(*((cursorState).get())), (*((numLedsLit).get()))}))));
}

namespace Setting {
    juniper::shared_ptr<Prelude::tuple2<Io::pinState,Setting::timeSetting>> outputState = (juniper::shared_ptr<Prelude::tuple2<Io::pinState,Setting::timeSetting>>(new Prelude::tuple2<Io::pinState,Setting::timeSetting>((*((outputUpdateState).get())))));
}

namespace Setting {
    Prelude::unit reset(juniper::shared_ptr<int32_t> timeRemaining) {
        return (([&]() -> Prelude::unit {
            (*((Setting::timeSetting*) (numLedsLit.get())) = (([&]() -> Setting::timeSetting{
                Setting::timeSetting guid226;
                guid226.minutes = 0;
                guid226.fifteenSeconds = 0;
                return guid226;
            })()));
            (*((Io::pinState*) (cursorState.get())) = Io::low());
            (*((Prelude::tuple2<Io::pinState,Setting::timeSetting>*) (outputUpdateState.get())) = (Prelude::tuple2<Io::pinState,Setting::timeSetting>{(*((cursorState).get())), (*((numLedsLit).get()))}));
            (*((Prelude::tuple2<Io::pinState,Setting::timeSetting>*) (outputState.get())) = (*((outputUpdateState).get())));
            (*((int32_t*) (timeRemaining.get())) = 0);
            return Prelude::unit();
        })());
    }
}

namespace Setting {
    Prelude::unit execute(juniper::shared_ptr<int32_t> timeRemaining) {
        return (([&]() -> Prelude::unit {
            auto guid227 = IoExt::every(500, tState, cursorState);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto cursorSig = guid227;
            
            auto guid228 = Io::risingEdge(Button::debounce(Io::digIn(buttonPin), bState), bEdgeState);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto buttonSig = guid228;
            
            auto guid229 = Signal::foldP<Prelude::unit, Setting::timeSetting>(juniper::function<Setting::timeSetting(Prelude::unit,Setting::timeSetting)>([=](Prelude::unit u, Setting::timeSetting prevSetting) mutable -> Setting::timeSetting { 
                return (([&]() -> Setting::timeSetting {
                    auto guid230 = prevSetting;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto prevFifteenSeconds = (guid230).fifteenSeconds;
                    auto prevMinutes = (guid230).minutes;
                    
                    return ((((prevMinutes + prevFifteenSeconds) + 1) >= numLeds) ? 
                        prevSetting
                    :
                        ((((prevFifteenSeconds + 1) % 4) == 0) ? 
                            (([&]() -> Setting::timeSetting{
                                Setting::timeSetting guid231;
                                guid231.minutes = (prevMinutes + 1);
                                guid231.fifteenSeconds = 0;
                                return guid231;
                            })())
                        :
                            (([&]() -> Setting::timeSetting{
                                Setting::timeSetting guid232;
                                guid232.minutes = prevMinutes;
                                guid232.fifteenSeconds = (prevFifteenSeconds + 1);
                                return guid232;
                            })())));
                })());
             }), numLedsLit, buttonSig);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto numLedsLitUpdateSig = guid229;
            
            auto guid233 = SignalExt::zip<Io::pinState, Setting::timeSetting>(cursorSig, numLedsLitUpdateSig, outputUpdateState);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto outputUpdateSig = guid233;
            
            auto guid234 = Signal::latch<Prelude::tuple2<Io::pinState,Setting::timeSetting>>(outputUpdateSig, outputState);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto outputSig = guid234;
            
            return Signal::sink<Prelude::tuple2<Io::pinState,Setting::timeSetting>>(juniper::function<Prelude::unit(Prelude::tuple2<Io::pinState,Setting::timeSetting>)>([=](Prelude::tuple2<Io::pinState,Setting::timeSetting> out) mutable -> Prelude::unit { 
                return (([&]() -> Prelude::unit {
                    auto guid235 = out;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto numFifteenSeconds = ((guid235).e2).fifteenSeconds;
                    auto numMinutes = ((guid235).e2).minutes;
                    auto cursor = (guid235).e1;
                    
                    (*((int32_t*) (timeRemaining.get())) = ((numMinutes * 60000) + (numFifteenSeconds * 15000)));
                    (([&]() -> Prelude::unit {
                        int32_t guid236 = 0;
                        int32_t guid237 = (numMinutes - 1);
                        for (int32_t i = guid236; i <= guid237; i++) {
                            FastLed::setLedColor<t1063, t1064>(((numLeds - i) - 1), blue, leds);
                        }
                        return {};
                    })());
                    (([&]() -> Prelude::unit {
                        int32_t guid238 = 0;
                        int32_t guid239 = (numFifteenSeconds - 1);
                        for (int32_t i = guid238; i <= guid239; i++) {
                            FastLed::setLedColor<t1069, t1070>(((numLeds - (numMinutes + i)) - 1), pink, leds);
                        }
                        return {};
                    })());
                    return (([&]() -> Prelude::unit {
                        auto guid240 = cursor;
                        return ((((guid240).tag == 0) && true) ? 
                            (([&]() -> Prelude::unit {
                                return FastLed::setLedColor<FastLed::color, FastLed::fastLedStrip>(((numLeds - (numMinutes + numFifteenSeconds)) - 1), white, leds);
                            })())
                        :
                            (true ? 
                                (([&]() -> Prelude::unit {
                                    return Prelude::unit();
                                })())
                            :
                                juniper::quit<Prelude::unit>()));
                    })());
                })());
             }), outputSig);
        })());
    }
}

namespace Paused {
    Prelude::unit execute(juniper::shared_ptr<int32_t> timeRemaining, float totalTime) {
        return (([&]() -> Prelude::unit {
            auto guid241 = (*((timeRemaining).get()));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto t = guid241;
            
            Timing::execute(timeRemaining, totalTime);
            (*((int32_t*) (timeRemaining.get())) = t);
            auto guid242 = (0.500000 * (Math::sin_((((2.000000 * Math::pi) * Time::now()) / 1000.000000)) + 1.000000));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto multiplier = guid242;
            
            return (([&]() -> Prelude::unit {
                int32_t guid243 = 0;
                int32_t guid244 = (numLeds - 1);
                for (int32_t i = guid243; i <= guid244; i++) {
                    (([&]() -> Prelude::unit {
                        auto guid245 = FastLed::getLedColor<t1095>(i, leds);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto b = (guid245).b;
                        auto g = (guid245).g;
                        auto r = (guid245).r;
                        
                        auto guid246 = (([&]() -> FastLed::color{
                            FastLed::color guid247;
                            guid247.r = (r * multiplier);
                            guid247.g = (g * multiplier);
                            guid247.b = (b * multiplier);
                            return guid247;
                        })());
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto newColor = guid246;
                        
                        return setLedColor<t1099, t1100>(i, newColor, leds);
                    })());
                }
                return {};
            })());
        })());
    }
}

namespace Finale {
    int32_t numLedsF = numLeds;
}

namespace Finale {
    Prelude::unit execute() {
        return (([&]() -> Prelude::unit {
            int32_t guid248 = 0;
            int32_t guid249 = (numLeds - 1);
            for (int32_t i = guid248; i <= guid249; i++) {
                (([&]() -> Prelude::unit {
                    auto guid250 = Time::now();
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto t = guid250;
                    
                    auto guid251 = (((i / numLedsF) * 1000) + t);
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto x = guid251;
                    
                    auto guid252 = (([&]() -> FastLed::color{
                        FastLed::color guid253;
                        guid253.r = (50 * Math::sin_((((2.000000 * Math::pi) * x) / 1000.000000)));
                        guid253.g = (50 * Math::cos_((((2.000000 * Math::pi) * x) / 1000.000000)));
                        guid253.b = (50 * Math::sin_((((2.000000 * Math::pi) * (x + (Math::pi / 2.000000))) / 1000.000000)));
                        return guid253;
                    })());
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto color = guid252;
                    
                    return FastLed::setLedColor<t1113, t1114>(i, color, leds);
                })());
            }
            return {};
        })());
    }
}

namespace Program {
    juniper::shared_ptr<Prelude::maybe<Accelerometer::orientation>> accState = (juniper::shared_ptr<Prelude::maybe<Accelerometer::orientation>>(new Prelude::maybe<Accelerometer::orientation>(nothing<Accelerometer::orientation>())));
}

namespace Program {
    juniper::shared_ptr<Program::mode> modeState = (juniper::shared_ptr<Program::mode>(new Program::mode(setting())));
}

namespace Program {
    juniper::shared_ptr<int32_t> timeRemaining = (juniper::shared_ptr<int32_t>(new int32_t(0)));
}

namespace Program {
    juniper::shared_ptr<int32_t> totalTime = (juniper::shared_ptr<int32_t>(new int32_t(0)));
}

namespace Program {
    Prelude::unit setup() {
        return Time::wait(500);
    }
}

namespace Program {
    Prelude::unit clearDisplay() {
        return (([&]() -> Prelude::unit {
            uint16_t guid254 = 0;
            uint16_t guid255 = (numLeds - 1);
            for (uint16_t i = guid254; i <= guid255; i++) {
                FastLed::setLedColor<t1125, t1126>(i, blank, leds);
            }
            return {};
        })());
    }
}

namespace Program {
    Prelude::unit main() {
        return (([&]() -> Prelude::unit {
            setup();
            return (([&]() -> Prelude::unit {
                while (true) {
                    (([&]() -> Prelude::unit {
                        clearDisplay();
                        auto guid256 = Signal::dropRepeats<Accelerometer::orientation>(Accelerometer::getSignal(), accState);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto accSig = guid256;
                        
                        auto guid257 = Signal::map<Accelerometer::orientation, Program::flip>(juniper::function<Program::flip(Accelerometer::orientation)>([=](Accelerometer::orientation o) mutable -> Program::flip { 
                            return (([&]() -> Program::flip {
                                auto guid258 = o;
                                return ((((guid258).tag == 0) && true) ? 
                                    (([&]() -> Program::flip {
                                        return flipUp();
                                    })())
                                :
                                    ((((guid258).tag == 1) && true) ? 
                                        (([&]() -> Program::flip {
                                            return flipDown();
                                        })())
                                    :
                                        (true ? 
                                            (([&]() -> Program::flip {
                                                return flipFlat();
                                            })())
                                        :
                                            juniper::quit<Program::flip>())));
                            })());
                         }), accSig);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto flipSig = guid257;
                        
                        auto guid259 = SignalExt::meta<Program::flip>(flipSig);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto metaFlipSig = guid259;
                        
                        auto guid260 = Signal::foldP<Prelude::maybe<Program::flip>, Program::mode>(juniper::function<Program::mode(Prelude::maybe<Program::flip>,Program::mode)>([=](Prelude::maybe<Program::flip> maybeFlipEvent, Program::mode prevMode) mutable -> Program::mode { 
                            return (((prevMode == timing()) && ((*((timeRemaining).get())) <= 0)) ? 
                                finale()
                            :
                                (([&]() -> Program::mode {
                                    auto guid261 = maybeFlipEvent;
                                    return ((((guid261).tag == 0) && true) ? 
                                        (([&]() -> Program::mode {
                                            auto flipEvent = (guid261).just;
                                            return (([&]() -> Program::mode {
                                                auto guid262 = (Prelude::tuple2<Program::flip,Program::mode>{flipEvent, prevMode});
                                                return (((((guid262).e2).tag == 0) && ((((guid262).e1).tag == 0) && true)) ? 
                                                    (([&]() -> Program::mode {
                                                        return (([&]() -> Program::mode {
                                                            (*((int32_t*) (totalTime.get())) = (*((timeRemaining).get())));
                                                            Timing::reset();
                                                            return timing();
                                                        })());
                                                    })())
                                                :
                                                    (((((guid262).e2).tag == 2) && ((((guid262).e1).tag == 0) && true)) ? 
                                                        (([&]() -> Program::mode {
                                                            return timing();
                                                        })())
                                                    :
                                                        (((((guid262).e2).tag == 1) && ((((guid262).e1).tag == 1) && true)) ? 
                                                            (([&]() -> Program::mode {
                                                                return (([&]() -> Program::mode {
                                                                    Setting::reset(timeRemaining);
                                                                    return setting();
                                                                })());
                                                            })())
                                                        :
                                                            (((((guid262).e2).tag == 2) && ((((guid262).e1).tag == 1) && true)) ? 
                                                                (([&]() -> Program::mode {
                                                                    return (([&]() -> Program::mode {
                                                                        Setting::reset(timeRemaining);
                                                                        return setting();
                                                                    })());
                                                                })())
                                                            :
                                                                (((((guid262).e2).tag == 3) && ((((guid262).e1).tag == 1) && true)) ? 
                                                                    (([&]() -> Program::mode {
                                                                        return (([&]() -> Program::mode {
                                                                            Setting::reset(timeRemaining);
                                                                            return setting();
                                                                        })());
                                                                    })())
                                                                :
                                                                    (((((guid262).e2).tag == 1) && ((((guid262).e1).tag == 2) && true)) ? 
                                                                        (([&]() -> Program::mode {
                                                                            return paused();
                                                                        })())
                                                                    :
                                                                        (true ? 
                                                                            (([&]() -> Program::mode {
                                                                                return prevMode;
                                                                            })())
                                                                        :
                                                                            juniper::quit<Program::mode>())))))));
                                            })());
                                        })())
                                    :
                                        (true ? 
                                            (([&]() -> Program::mode {
                                                return prevMode;
                                            })())
                                        :
                                            juniper::quit<Program::mode>()));
                                })()));
                         }), modeState, metaFlipSig);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto modeSig = guid260;
                        
                        Signal::sink<Program::mode>(juniper::function<Prelude::unit(Program::mode)>([=](Program::mode m) mutable -> Prelude::unit { 
                            return (([&]() -> Prelude::unit {
                                auto guid263 = m;
                                return ((((guid263).tag == 0) && true) ? 
                                    (([&]() -> Prelude::unit {
                                        return Setting::execute(timeRemaining);
                                    })())
                                :
                                    ((((guid263).tag == 1) && true) ? 
                                        (([&]() -> Prelude::unit {
                                            return Timing::execute(timeRemaining, (*((totalTime).get())));
                                        })())
                                    :
                                        ((((guid263).tag == 2) && true) ? 
                                            (([&]() -> Prelude::unit {
                                                return Paused::execute(timeRemaining, (*((totalTime).get())));
                                            })())
                                        :
                                            ((((guid263).tag == 3) && true) ? 
                                                (([&]() -> Prelude::unit {
                                                    return Finale::execute();
                                                })())
                                            :
                                                juniper::quit<Prelude::unit>()))));
                            })());
                         }), modeSig);
                        return FastLed::show();
                    })());
                }
                return {};
            })());
        })());
    }
}

int main() {
    init();
    Program::main();
    return 0;
}