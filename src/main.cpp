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


namespace Prelude {




    struct unit {
        bool operator==(unit& rhs) {
            return true;
        }

        bool operator!=(unit& rhs) {
            return !(rhs == *this);
        }
    };

    template<typename a>
    struct maybe {
        uint8_t tag;
        bool operator==(maybe& rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->just == rhs.just;
                case 1:
                    return this->nothing == rhs.nothing;
            }
            return false;
        }

        bool operator!=(maybe&rhs) { return !(rhs == *this); }
        union {
            a just;
            uint8_t nothing;
        };
    };

    template<typename a>
    maybe<a> just(a data) {
        return (([&]() -> maybe<a> { maybe<a> ret; ret.tag = 0; ret.just = data; return ret; })());
    }

    template<typename a>
    maybe<a> nothing() {
        return (([&]() -> maybe<a> { maybe<a> ret; ret.tag = 1; ret.nothing = 0; return ret; })());
    }



    template<typename a, typename b>
    struct either {
        uint8_t tag;
        bool operator==(either& rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->left == rhs.left;
                case 1:
                    return this->right == rhs.right;
            }
            return false;
        }

        bool operator!=(either&rhs) { return !(rhs == *this); }
        union {
            a left;
            b right;
        };
    };

    template<typename a, typename b>
    either<a, b> left(a data) {
        return (([&]() -> either<a, b> { either<a, b> ret; ret.tag = 0; ret.left = data; return ret; })());
    }

    template<typename a, typename b>
    either<a, b> right(b data) {
        return (([&]() -> either<a, b> { either<a, b> ret; ret.tag = 1; ret.right = data; return ret; })());
    }



    template<typename a, int n>
    struct list {
        juniper::array<a,n> data;
        uint32_t length;
        bool operator==(list& rhs) {
            return true && data == rhs.data && length == rhs.length;
        }

        bool operator!=(list& rhs) {
            return !(rhs == *this);
        }
    };

    template<int n>
    struct string {
        Prelude::list<uint8_t, n> characters;
        bool operator==(string& rhs) {
            return true && characters == rhs.characters;
        }

        bool operator!=(string& rhs) {
            return !(rhs == *this);
        }
    };

    template<typename a>
    struct sig {
        uint8_t tag;
        bool operator==(sig& rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->signal == rhs.signal;
            }
            return false;
        }

        bool operator!=(sig&rhs) { return !(rhs == *this); }
        union {
            Prelude::maybe<a> signal;
        };
    };

    template<typename a>
    sig<a> signal(Prelude::maybe<a> data) {
        return (([&]() -> sig<a> { sig<a> ret; ret.tag = 0; ret.signal = data; return ret; })());
    }



    template<typename a, typename b, typename c>
    juniper::function<c(a)> compose(juniper::function<c(b)> f, juniper::function<b(a)> g) {
        return juniper::function<c(a)>([=](a x) mutable -> c { 
            return f(g(x));
         });
    }

    template<typename a, typename b, typename c>
    juniper::function<juniper::function<c(b)>(a)> curry(juniper::function<c(a,b)> f) {
        return juniper::function<juniper::function<c(b)>(a)>([=](a valueA) mutable -> juniper::function<c(b)> { 
            return juniper::function<c(b)>([=](b valueB) mutable -> c { 
                return f(valueA, valueB);
             });
         });
    }

    template<typename a, typename b, typename c, typename d>
    juniper::function<juniper::function<juniper::function<d(c)>(b)>(a)> curry3(juniper::function<d(a,b,c)> f) {
        return juniper::function<juniper::function<juniper::function<d(c)>(b)>(a)>([=](a valueA) mutable -> juniper::function<juniper::function<d(c)>(b)> { 
            return juniper::function<juniper::function<d(c)>(b)>([=](b valueB) mutable -> juniper::function<d(c)> { 
                return juniper::function<d(c)>([=](c valueC) mutable -> d { 
                    return f(valueA, valueB, valueC);
                 });
             });
         });
    }

    template<typename a>
    bool equal(a x, a y) {
        return (x == y);
    }

    template<typename a, typename b>
    struct tuple2 {
        a e1;
        b e2;
        bool operator==(tuple2& rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2;
        }

        bool operator!=(tuple2& rhs) {
            return !(rhs == *this);
        }
    };

    template<typename a, typename b, typename c>
    struct tuple3 {
        a e1;
        b e2;
        c e3;
        bool operator==(tuple3& rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3;
        }

        bool operator!=(tuple3& rhs) {
            return !(rhs == *this);
        }
    };

    template<typename a, typename b, typename c, typename d>
    struct tuple4 {
        a e1;
        b e2;
        c e3;
        d e4;
        bool operator==(tuple4& rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4;
        }

        bool operator!=(tuple4& rhs) {
            return !(rhs == *this);
        }
    };

    template<typename a, typename b, typename c, typename d, typename e>
    struct tuple5 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        bool operator==(tuple5& rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5;
        }

        bool operator!=(tuple5& rhs) {
            return !(rhs == *this);
        }
    };

    template<typename a, typename b, typename c, typename d, typename e, typename f>
    struct tuple6 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        bool operator==(tuple6& rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6;
        }

        bool operator!=(tuple6& rhs) {
            return !(rhs == *this);
        }
    };

    template<typename a, typename b, typename c, typename d, typename e, typename f, typename g>
    struct tuple7 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        g e7;
        bool operator==(tuple7& rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7;
        }

        bool operator!=(tuple7& rhs) {
            return !(rhs == *this);
        }
    };

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
        bool operator==(tuple8& rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7 && e8 == rhs.e8;
        }

        bool operator!=(tuple8& rhs) {
            return !(rhs == *this);
        }
    };

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
        bool operator==(tuple9& rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7 && e8 == rhs.e8 && e9 == rhs.e9;
        }

        bool operator!=(tuple9& rhs) {
            return !(rhs == *this);
        }
    };

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
        bool operator==(tuple10& rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7 && e8 == rhs.e8 && e9 == rhs.e9 && e10 == rhs.e10;
        }

        bool operator!=(tuple10& rhs) {
            return !(rhs == *this);
        }
    };

    template<typename a, typename b>
    a fst(Prelude::tuple2<a,b> tup) {
        return (([&]() -> a {
            Prelude::tuple2<a,b> guid735 = tup;
            return (true ? 
                (([&]() -> a {
                    a x = (guid735).e1;
                    return x;
                })())
            :
                juniper::quit<a>());
        })());
    }

    template<typename a, typename b>
    b snd(Prelude::tuple2<a,b> tup) {
        return (([&]() -> b {
            Prelude::tuple2<a,b> guid736 = tup;
            return (true ? 
                (([&]() -> b {
                    b x = (guid736).e2;
                    return x;
                })())
            :
                juniper::quit<b>());
        })());
    }

}

namespace List {


    using namespace Prelude;




    template<typename a, typename b, int n>
    Prelude::list<b, n> map(juniper::function<b(a)> f, Prelude::list<a, n> lst) {
        return (([&]() -> Prelude::list<b, n> {
            auto guid737 = (juniper::array<b, n>());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            juniper::array<b,n> ret = guid737;
            
            (([&]() -> Prelude::unit {
                uint32_t guid738 = 0;
                uint32_t guid739 = ((lst).length - 1);
                for (uint32_t i = guid738; i <= guid739; i++) {
                    ((ret)[i] = f(((lst).data)[i]));
                }
                return {};
            })());
            return (([&]() -> Prelude::list<b, n>{
                Prelude::list<b, n> guid740;
                guid740.data = ret;
                guid740.length = (lst).length;
                return guid740;
            })());
        })());
    }

    template<typename t, typename state, int n>
    state foldl(juniper::function<state(t,state)> f, state initState, Prelude::list<t, n> lst) {
        return (([&]() -> state {
            auto guid741 = initState;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            state s = guid741;
            
            (([&]() -> Prelude::unit {
                uint32_t guid742 = 0;
                uint32_t guid743 = ((lst).length - 1);
                for (uint32_t i = guid742; i <= guid743; i++) {
                    (s = f(((lst).data)[i], s));
                }
                return {};
            })());
            return s;
        })());
    }

    template<typename t, typename state, int n>
    state foldr(juniper::function<state(t,state)> f, state initState, Prelude::list<t, n> lst) {
        return (([&]() -> state {
            auto guid744 = initState;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            state s = guid744;
            
            (([&]() -> Prelude::unit {
                uint32_t guid745 = ((lst).length - 1);
                uint32_t guid746 = 0;
                for (uint32_t i = guid745; i >= guid746; i--) {
                    (s = f(((lst).data)[i], s));
                }
                return {};
            })());
            return s;
        })());
    }

    template<typename t, int aCap, int bCap, int retCap>
    Prelude::list<t, retCap> append(Prelude::list<t, aCap> lstA, Prelude::list<t, bCap> lstB) {
        return (([&]() -> Prelude::list<t, retCap> {
            auto guid747 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t j = guid747;
            
            auto guid748 = (([&]() -> Prelude::list<t, retCap>{
                Prelude::list<t, retCap> guid749;
                guid749.data = (juniper::array<t, retCap>());
                guid749.length = ((lstA).length + (lstB).length);
                return guid749;
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::list<t, retCap> out = guid748;
            
            (([&]() -> Prelude::unit {
                uint32_t guid750 = 0;
                uint32_t guid751 = ((lstA).length - 1);
                for (uint32_t i = guid750; i <= guid751; i++) {
                    (([&]() -> int32_t {
                        (((out).data)[j] = ((lstA).data)[i]);
                        return (j = (j + 1));
                    })());
                }
                return {};
            })());
            (([&]() -> Prelude::unit {
                uint32_t guid752 = 0;
                uint32_t guid753 = ((lstB).length - 1);
                for (uint32_t i = guid752; i <= guid753; i++) {
                    (([&]() -> int32_t {
                        (((out).data)[j] = ((lstB).data)[i]);
                        return (j = (j + 1));
                    })());
                }
                return {};
            })());
            return out;
        })());
    }

    template<typename t, int n>
    t nth(uint32_t i, Prelude::list<t, n> lst) {
        return ((i >= (lst).length) ? 
            juniper::quit<t>()
        :
            ((lst).data)[i]);
    }

    template<typename t, int m, int n>
    Prelude::list<t, (m)*(n)> flattenSafe(Prelude::list<Prelude::list<t, m>, n> listOfLists) {
        return (([&]() -> Prelude::list<t, (m)*(n)> {
            auto guid754 = (juniper::array<t, (m)*(n)>());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            juniper::array<t,(m)*(n)> ret = guid754;
            
            auto guid755 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t index = guid755;
            
            (([&]() -> Prelude::unit {
                uint32_t guid756 = 0;
                uint32_t guid757 = ((listOfLists).length - 1);
                for (uint32_t i = guid756; i <= guid757; i++) {
                    (([&]() -> Prelude::unit {
                        uint32_t guid758 = 0;
                        uint32_t guid759 = ((((listOfLists).data)[i]).length - 1);
                        for (uint32_t j = guid758; j <= guid759; j++) {
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
            return (([&]() -> Prelude::list<t, (m)*(n)>{
                Prelude::list<t, (m)*(n)> guid760;
                guid760.data = ret;
                guid760.length = index;
                return guid760;
            })());
        })());
    }

    template<typename t, int n, int m>
    Prelude::list<t, m> resize(Prelude::list<t, n> lst) {
        return (([&]() -> Prelude::list<t, m> {
            auto guid761 = (juniper::array<t, m>());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            juniper::array<t,m> ret = guid761;
            
            (([&]() -> Prelude::unit {
                uint32_t guid762 = 0;
                uint32_t guid763 = ((lst).length - 1);
                for (uint32_t i = guid762; i <= guid763; i++) {
                    ((ret)[i] = ((lst).data)[i]);
                }
                return {};
            })());
            return (([&]() -> Prelude::list<t, m>{
                Prelude::list<t, m> guid764;
                guid764.data = ret;
                guid764.length = (lst).length;
                return guid764;
            })());
        })());
    }

    template<typename t, int n>
    bool all(juniper::function<bool(t)> pred, Prelude::list<t, n> lst) {
        return (([&]() -> bool {
            auto guid765 = true;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            bool satisfied = guid765;
            
            (([&]() -> Prelude::unit {
                uint32_t guid766 = 0;
                uint32_t guid767 = ((lst).length - 1);
                for (uint32_t i = guid766; i <= guid767; i++) {
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
    }

    template<typename t, int n>
    bool any(juniper::function<bool(t)> pred, Prelude::list<t, n> lst) {
        return (([&]() -> bool {
            auto guid768 = false;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            bool satisfied = guid768;
            
            (([&]() -> Prelude::unit {
                uint32_t guid769 = 0;
                uint32_t guid770 = ((lst).length - 1);
                for (uint32_t i = guid769; i <= guid770; i++) {
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
    }

    template<typename t, int n>
    Prelude::list<t, n> pushBack(t elem, Prelude::list<t, n> lst) {
        return (((lst).length >= n) ? 
            juniper::quit<Prelude::list<t, n>>()
        :
            (([&]() -> Prelude::list<t, n> {
                auto guid771 = lst;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                Prelude::list<t, n> ret = guid771;
                
                (((ret).data)[(lst).length] = elem);
                ((ret).length = ((lst).length + 1));
                return ret;
            })()));
    }

    template<typename t, int n>
    Prelude::list<t, n> pushOffFront(t elem, Prelude::list<t, n> lst) {
        return (([&]() -> Prelude::list<t, n> {
            auto guid772 = lst;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::list<t, n> ret = guid772;
            
            (([&]() -> Prelude::unit {
                uint32_t guid773 = (n - 2);
                uint32_t guid774 = 0;
                for (uint32_t i = guid773; i >= guid774; i--) {
                    (((ret).data)[(i + 1)] = ((ret).data)[i]);
                }
                return {};
            })());
            (((ret).data)[0] = elem);
            return (((ret).length == n) ? 
                ret
            :
                (([&]() -> Prelude::list<t, n> {
                    ((ret).length = ((lst).length + 1));
                    return ret;
                })()));
        })());
    }

    template<typename t, int n>
    Prelude::list<t, n> setNth(uint32_t index, t elem, Prelude::list<t, n> lst) {
        return (((lst).length <= index) ? 
            juniper::quit<Prelude::list<t, n>>()
        :
            (([&]() -> Prelude::list<t, n> {
                auto guid775 = lst;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                Prelude::list<t, n> ret = guid775;
                
                (((ret).data)[index] = elem);
                return ret;
            })()));
    }

    template<typename t, int n>
    Prelude::list<t, n> replicate(uint32_t numOfElements, t elem) {
        return (([&]() -> Prelude::list<t, n>{
            Prelude::list<t, n> guid776;
            guid776.data = (juniper::array<t, n>().fill(elem));
            guid776.length = numOfElements;
            return guid776;
        })());
    }

    template<typename t, int n>
    Prelude::list<t, n> remove(t elem, Prelude::list<t, n> lst) {
        return (([&]() -> Prelude::list<t, n> {
            auto guid777 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t index = guid777;
            
            auto guid778 = false;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            bool found = guid778;
            
            (([&]() -> Prelude::unit {
                uint32_t guid779 = 0;
                uint32_t guid780 = ((lst).length - 1);
                for (uint32_t i = guid779; i <= guid780; i++) {
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
                (([&]() -> Prelude::list<t, n> {
                    auto guid781 = lst;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    Prelude::list<t, n> ret = guid781;
                    
                    ((ret).length = ((lst).length - 1));
                    (([&]() -> Prelude::unit {
                        uint32_t guid782 = index;
                        uint32_t guid783 = ((lst).length - 2);
                        for (uint32_t i = guid782; i <= guid783; i++) {
                            (((ret).data)[i] = ((lst).data)[(i + 1)]);
                        }
                        return {};
                    })());
                    return ret;
                })())
            :
                lst);
        })());
    }

    template<typename t, int n>
    Prelude::list<t, n> dropLast(Prelude::list<t, n> lst) {
        return (((lst).length == 0) ? 
            juniper::quit<Prelude::list<t, n>>()
        :
            (([&]() -> Prelude::list<t, n>{
                Prelude::list<t, n> guid784;
                guid784.data = (lst).data;
                guid784.length = ((lst).length - 1);
                return guid784;
            })()));
    }

    template<typename t, int n>
    Prelude::unit foreach(juniper::function<Prelude::unit(t)> f, Prelude::list<t, n> lst) {
        return (([&]() -> Prelude::unit {
            uint32_t guid785 = 0;
            uint32_t guid786 = ((lst).length - 1);
            for (uint32_t i = guid785; i <= guid786; i++) {
                f(((lst).data)[i]);
            }
            return {};
        })());
    }

    template<typename t, int n>
    t last(Prelude::list<t, n> lst) {
        return ((lst).data)[((lst).length - 1)];
    }

    template<typename t, int n>
    t max(Prelude::list<t, n> lst) {
        return ((((lst).length == 0) || (n == 0)) ? 
            juniper::quit<t>()
        :
            (([&]() -> t {
                auto guid787 = ((lst).data)[0];
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                t maxVal = guid787;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid788 = 1;
                    uint32_t guid789 = ((lst).length - 1);
                    for (uint32_t i = guid788; i <= guid789; i++) {
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
    }

    template<typename t, int n>
    t min(Prelude::list<t, n> lst) {
        return ((((lst).length == 0) || (n == 0)) ? 
            juniper::quit<t>()
        :
            (([&]() -> t {
                auto guid790 = ((lst).data)[0];
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                t minVal = guid790;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid791 = 1;
                    uint32_t guid792 = ((lst).length - 1);
                    for (uint32_t i = guid791; i <= guid792; i++) {
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
    }

    template<typename t, int n>
    bool member(t elem, Prelude::list<t, n> lst) {
        return (([&]() -> bool {
            auto guid793 = false;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            bool found = guid793;
            
            (([&]() -> Prelude::unit {
                uint32_t guid794 = 0;
                uint32_t guid795 = ((lst).length - 1);
                for (uint32_t i = guid794; i <= guid795; i++) {
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
    }

    template<typename a, typename b, int n>
    Prelude::list<Prelude::tuple2<a,b>, n> zip(Prelude::list<a, n> lstA, Prelude::list<b, n> lstB) {
        return (((lstA).length == (lstB).length) ? 
            (([&]() -> Prelude::list<Prelude::tuple2<a,b>, n> {
                auto guid796 = (([&]() -> Prelude::list<Prelude::tuple2<a,b>, n>{
                    Prelude::list<Prelude::tuple2<a,b>, n> guid797;
                    guid797.data = (juniper::array<Prelude::tuple2<a,b>, n>());
                    guid797.length = (lstA).length;
                    return guid797;
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                Prelude::list<Prelude::tuple2<a,b>, n> ret = guid796;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid798 = 0;
                    uint32_t guid799 = (lstA).length;
                    for (uint32_t i = guid798; i <= guid799; i++) {
                        (((ret).data)[i] = (Prelude::tuple2<a,b>{((lstA).data)[i], ((lstB).data)[i]}));
                    }
                    return {};
                })());
                return ret;
            })())
        :
            juniper::quit<Prelude::list<Prelude::tuple2<a,b>, n>>());
    }

    template<typename a, typename b, int n>
    Prelude::tuple2<Prelude::list<a, n>,Prelude::list<b, n>> unzip(Prelude::list<Prelude::tuple2<a,b>, n> lst) {
        return (([&]() -> Prelude::tuple2<Prelude::list<a, n>,Prelude::list<a, n>> {
            auto guid800 = (([&]() -> Prelude::list<a, n>{
                Prelude::list<a, n> guid801;
                guid801.data = (juniper::array<a, n>());
                guid801.length = (lst).length;
                return guid801;
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::list<a, n> retA = guid800;
            
            auto guid802 = (([&]() -> Prelude::list<a, n>{
                Prelude::list<a, n> guid803;
                guid803.data = (juniper::array<b, n>());
                guid803.length = (lst).length;
                return guid803;
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::list<a, n> retB = guid802;
            
            (([&]() -> Prelude::unit {
                uint32_t guid804 = 0;
                uint32_t guid805 = ((lst).length - 1);
                for (uint32_t i = guid804; i <= guid805; i++) {
                    (([&]() -> b {
                        auto guid806 = ((lst).data)[i];
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        b elemB = (guid806).e2;
                        a elemA = (guid806).e1;
                        
                        (((retA).data)[i] = elemA);
                        return (((retB).data)[i] = elemB);
                    })());
                }
                return {};
            })());
            return (Prelude::tuple2<Prelude::list<a, n>,Prelude::list<a, n>>{retA, retB});
        })());
    }

}

namespace Signal {


    using namespace Prelude;




    template<typename a, typename b>
    Prelude::sig<b> map(juniper::function<b(a)> f, Prelude::sig<a> s) {
        return (([&]() -> Prelude::sig<b> {
            Prelude::sig<a> guid807 = s;
            return ((((guid807).tag == 0) && ((((guid807).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<b> {
                    auto val = ((guid807).signal).just;
                    return signal<b>(just<b>(f(val)));
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<b> {
                        return signal<b>(nothing<b>());
                    })())
                :
                    juniper::quit<Prelude::sig<b>>()));
        })());
    }

    template<typename a>
    Prelude::unit sink(juniper::function<Prelude::unit(a)> f, Prelude::sig<a> s) {
        return (([&]() -> Prelude::unit {
            Prelude::sig<a> guid808 = s;
            return ((((guid808).tag == 0) && ((((guid808).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::unit {
                    auto val = ((guid808).signal).just;
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

    template<typename a>
    Prelude::sig<a> filter(juniper::function<bool(a)> f, Prelude::sig<a> s) {
        return (([&]() -> Prelude::sig<a> {
            Prelude::sig<a> guid809 = s;
            return ((((guid809).tag == 0) && ((((guid809).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<a> {
                    auto val = ((guid809).signal).just;
                    return (f(val) ? 
                        signal<a>(nothing<a>())
                    :
                        s);
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<a> {
                        return signal<a>(nothing<a>());
                    })())
                :
                    juniper::quit<Prelude::sig<a>>()));
        })());
    }

    template<typename a>
    Prelude::sig<a> merge(Prelude::sig<a> sigA, Prelude::sig<a> sigB) {
        return (([&]() -> Prelude::sig<a> {
            Prelude::sig<a> guid810 = sigA;
            return ((((guid810).tag == 0) && ((((guid810).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<a> {
                    return sigA;
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<a> {
                        return sigB;
                    })())
                :
                    juniper::quit<Prelude::sig<a>>()));
        })());
    }

    template<typename a, int n>
    Prelude::sig<a> mergeMany(Prelude::list<Prelude::sig<a>, n> sigs) {
        return (([&]() -> Prelude::sig<a> {
            auto guid811 = nothing<a>();
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::maybe<a> ret = guid811;
            
            (([&]() -> Prelude::unit {
                uint32_t guid812 = 0;
                uint32_t guid813 = (n - 1);
                for (uint32_t i = guid812; i <= guid813; i++) {
                    (([&]() -> Prelude::unit {
                        Prelude::maybe<a> guid814 = ret;
                        return ((((guid814).tag == 1) && true) ? 
                            (([&]() -> Prelude::unit {
                                return (([&]() -> Prelude::unit {
                                    auto guid815 = List::nth<a, n>(i, sigs);
                                    if (!((((guid815).tag == 0) && true))) {
                                        juniper::quit<Prelude::unit>();
                                    }
                                    auto heldValue = (guid815).signal;
                                    
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
            return signal<a>(ret);
        })());
    }

    template<typename a, typename b>
    Prelude::sig<Prelude::either<a, b>> join(Prelude::sig<a> sigA, Prelude::sig<b> sigB) {
        return (([&]() -> Prelude::sig<Prelude::either<a, b>> {
            Prelude::tuple2<Prelude::sig<a>,Prelude::sig<b>> guid816 = (Prelude::tuple2<Prelude::sig<a>,Prelude::sig<b>>{sigA, sigB});
            return (((((guid816).e1).tag == 0) && (((((guid816).e1).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<Prelude::either<a, b>> {
                    auto value = (((guid816).e1).signal).just;
                    return signal<Prelude::either<a, b>>(just<Prelude::either<a, b>>(left<a, b>(value)));
                })())
            :
                (((((guid816).e2).tag == 0) && (((((guid816).e2).signal).tag == 0) && true)) ? 
                    (([&]() -> Prelude::sig<Prelude::either<a, b>> {
                        auto value = (((guid816).e2).signal).just;
                        return signal<Prelude::either<a, b>>(just<Prelude::either<a, b>>(right<a, b>(value)));
                    })())
                :
                    (true ? 
                        (([&]() -> Prelude::sig<Prelude::either<a, b>> {
                            return signal<Prelude::either<a, b>>(nothing<Prelude::either<a, b>>());
                        })())
                    :
                        juniper::quit<Prelude::sig<Prelude::either<a, b>>>())));
        })());
    }

    template<typename a>
    Prelude::sig<Prelude::unit> toUnit(Prelude::sig<a> s) {
        return map<a, Prelude::unit>(juniper::function<Prelude::unit(a)>([=](a x) mutable -> Prelude::unit { 
            return Prelude::unit();
         }), s);
    }

    template<typename a, typename state>
    Prelude::sig<state> foldP(juniper::function<state(a,state)> f, juniper::shared_ptr<state> state0, Prelude::sig<a> incoming) {
        return (([&]() -> Prelude::sig<state> {
            Prelude::sig<a> guid817 = incoming;
            return ((((guid817).tag == 0) && ((((guid817).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<state> {
                    auto val = ((guid817).signal).just;
                    return (([&]() -> Prelude::sig<state> {
                        auto guid818 = f(val, (*((state0).get())));
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        state state1 = guid818;
                        
                        (*((state*) (state0.get())) = state1);
                        return signal<state>(just<state>(state1));
                    })());
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<state> {
                        return signal<state>(nothing<state>());
                    })())
                :
                    juniper::quit<Prelude::sig<state>>()));
        })());
    }

    template<typename a>
    Prelude::sig<a> dropRepeats(Prelude::sig<a> incoming, juniper::shared_ptr<Prelude::maybe<a>> maybePrevValue) {
        return filter<a>(juniper::function<bool(a)>([=](a value) mutable -> bool { 
            return (([&]() -> bool {
                auto guid819 = (([&]() -> bool {
                    Prelude::maybe<a> guid820 = (*((maybePrevValue).get()));
                    return ((((guid820).tag == 1) && true) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        ((((guid820).tag == 0) && true) ? 
                            (([&]() -> bool {
                                auto prevValue = (guid820).just;
                                return (value == prevValue);
                            })())
                        :
                            juniper::quit<bool>()));
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                bool filtered = guid819;
                
                (!(filtered) ? 
                    (([&]() -> Prelude::unit {
                        (*((Prelude::maybe<a>*) (maybePrevValue.get())) = just<a>(value));
                        return Prelude::unit();
                    })())
                :
                    Prelude::unit());
                return filtered;
            })());
         }), incoming);
    }

    template<typename a>
    Prelude::sig<a> latch(Prelude::sig<a> incoming, juniper::shared_ptr<a> prevValue) {
        return (([&]() -> Prelude::sig<a> {
            Prelude::sig<a> guid821 = incoming;
            return ((((guid821).tag == 0) && ((((guid821).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<a> {
                    auto val = ((guid821).signal).just;
                    return (([&]() -> Prelude::sig<a> {
                        (*((a*) (prevValue.get())) = val);
                        return incoming;
                    })());
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<a> {
                        return signal<a>(just<a>((*((prevValue).get()))));
                    })())
                :
                    juniper::quit<Prelude::sig<a>>()));
        })());
    }

    template<typename a, typename b, typename c>
    Prelude::sig<c> map2(juniper::function<c(a,b)> f, Prelude::sig<a> incomingA, Prelude::sig<b> incomingB, juniper::shared_ptr<Prelude::tuple2<a,b>> state) {
        return (([&]() -> Prelude::sig<c> {
            auto guid822 = (([&]() -> a {
                Prelude::sig<a> guid823 = incomingA;
                return ((((guid823).tag == 0) && ((((guid823).signal).tag == 0) && true)) ? 
                    (([&]() -> a {
                        auto val = ((guid823).signal).just;
                        return val;
                    })())
                :
                    (true ? 
                        (([&]() -> a {
                            return fst<a, b>((*((state).get())));
                        })())
                    :
                        juniper::quit<a>()));
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            a valA = guid822;
            
            auto guid824 = (([&]() -> a {
                Prelude::sig<b> guid825 = incomingB;
                return ((((guid825).tag == 0) && ((((guid825).signal).tag == 0) && true)) ? 
                    (([&]() -> a {
                        auto val = ((guid825).signal).just;
                        return val;
                    })())
                :
                    (true ? 
                        (([&]() -> a {
                            return snd<a, b>((*((state).get())));
                        })())
                    :
                        juniper::quit<a>()));
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            a valB = guid824;
            
            (*((Prelude::tuple2<a,a>*) (state.get())) = (Prelude::tuple2<a,a>{valA, valB}));
            return (([&]() -> Prelude::sig<c> {
                Prelude::tuple2<Prelude::sig<a>,Prelude::sig<b>> guid826 = (Prelude::tuple2<Prelude::sig<a>,Prelude::sig<b>>{incomingA, incomingB});
                return (((((guid826).e2).tag == 0) && (((((guid826).e2).signal).tag == 1) && ((((guid826).e1).tag == 0) && (((((guid826).e1).signal).tag == 1) && true)))) ? 
                    (([&]() -> Prelude::sig<c> {
                        return signal<c>(nothing<c>());
                    })())
                :
                    (true ? 
                        (([&]() -> Prelude::sig<c> {
                            return signal<c>(just<c>(f(valA, valB)));
                        })())
                    :
                        juniper::quit<Prelude::sig<c>>()));
            })());
        })());
    }

    template<typename a, int n>
    Prelude::sig<Prelude::list<a, n>> record(Prelude::sig<a> incoming, juniper::shared_ptr<Prelude::list<a, n>> pastValues) {
        return foldP<a, Prelude::list<a, n>>(List::pushOffFront<a, n>, pastValues, incoming);
    }

}

#include <Arduino.h>
namespace Io {


    using namespace Prelude;






    struct pinState {
        uint8_t tag;
        bool operator==(pinState& rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->high == rhs.high;
                case 1:
                    return this->low == rhs.low;
            }
            return false;
        }

        bool operator!=(pinState&rhs) { return !(rhs == *this); }
        union {
            uint8_t high;
            uint8_t low;
        };
    };

    pinState high() {
        return (([&]() -> pinState { pinState ret; ret.tag = 0; ret.high = 0; return ret; })());
    }

    pinState low() {
        return (([&]() -> pinState { pinState ret; ret.tag = 1; ret.low = 0; return ret; })());
    }



    struct mode {
        uint8_t tag;
        bool operator==(mode& rhs) {
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

        bool operator!=(mode&rhs) { return !(rhs == *this); }
        union {
            uint8_t input;
            uint8_t output;
            uint8_t inputPullup;
        };
    };

    mode input() {
        return (([&]() -> mode { mode ret; ret.tag = 0; ret.input = 0; return ret; })());
    }

    mode output() {
        return (([&]() -> mode { mode ret; ret.tag = 1; ret.output = 0; return ret; })());
    }

    mode inputPullup() {
        return (([&]() -> mode { mode ret; ret.tag = 2; ret.inputPullup = 0; return ret; })());
    }



    Io::pinState toggle(Io::pinState p) {
        return (([&]() -> Io::pinState {
            Io::pinState guid827 = p;
            return ((((guid827).tag == 0) && true) ? 
                (([&]() -> Io::pinState {
                    return low();
                })())
            :
                ((((guid827).tag == 1) && true) ? 
                    (([&]() -> Io::pinState {
                        return high();
                    })())
                :
                    juniper::quit<Io::pinState>()));
        })());
    }

    template<int n>
    Prelude::unit printStr(Prelude::string<n> str) {
        return (([&]() -> Prelude::unit {
            Serial.print(&str.characters.data);
            return {};
        })());
    }

    Prelude::unit printFloat(float f) {
        return (([&]() -> Prelude::unit {
            Serial.print(f);
            return {};
        })());
    }

    Prelude::unit beginSerial(uint32_t speed) {
        return (([&]() -> Prelude::unit {
            Serial.begin(speed);
            return {};
        })());
    }

    uint8_t pinStateToInt(Io::pinState value) {
        return (([&]() -> int32_t {
            Io::pinState guid828 = value;
            return ((((guid828).tag == 1) && true) ? 
                (([&]() -> int32_t {
                    return 0;
                })())
            :
                ((((guid828).tag == 0) && true) ? 
                    (([&]() -> int32_t {
                        return 1;
                    })())
                :
                    juniper::quit<int32_t>()));
        })());
    }

    Io::pinState intToPinState(uint8_t value) {
        return ((value == 0) ? 
            low()
        :
            high());
    }

    Prelude::unit digWrite(uint16_t pin, Io::pinState value) {
        return (([&]() -> Prelude::unit {
            auto guid829 = pinStateToInt(value);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t intVal = guid829;
            
            return (([&]() -> Prelude::unit {
                digitalWrite(pin, intVal);
                return {};
            })());
        })());
    }

    Io::pinState digRead(uint16_t pin) {
        return (([&]() -> Io::pinState {
            auto guid830 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t intVal = guid830;
            
            (([&]() -> Prelude::unit {
                intVal = digitalRead(pin);
                return {};
            })());
            return intToPinState(intVal);
        })());
    }

    Prelude::sig<Io::pinState> digIn(uint16_t pin) {
        return signal<Io::pinState>(just<Io::pinState>(digRead(pin)));
    }

    Prelude::unit digOut(uint16_t pin, Prelude::sig<Io::pinState> sig) {
        return Signal::sink<Io::pinState>(juniper::function<Prelude::unit(Io::pinState)>([=](Io::pinState value) mutable -> Prelude::unit { 
            return digWrite(pin, value);
         }), sig);
    }

    uint16_t anaRead(uint16_t pin) {
        return (([&]() -> uint16_t {
            auto guid831 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint16_t value = guid831;
            
            (([&]() -> Prelude::unit {
                value = analogRead(pin);
                return {};
            })());
            return value;
        })());
    }

    Prelude::unit anaWrite(uint16_t pin, uint8_t value) {
        return (([&]() -> Prelude::unit {
            analogWrite(pin, value);
            return {};
        })());
    }

    Prelude::sig<uint16_t> anaIn(uint16_t pin) {
        return signal<uint16_t>(just<uint16_t>(anaRead(pin)));
    }

    Prelude::unit anaOut(uint16_t pin, Prelude::sig<uint16_t> sig) {
        return Signal::sink<uint16_t>(juniper::function<Prelude::unit(uint16_t)>([=](uint16_t value) mutable -> Prelude::unit { 
            return anaWrite(pin, value);
         }), sig);
    }

    uint8_t pinModeToInt(Io::mode m) {
        return (([&]() -> int32_t {
            Io::mode guid832 = m;
            return ((((guid832).tag == 0) && true) ? 
                (([&]() -> int32_t {
                    return 0;
                })())
            :
                ((((guid832).tag == 1) && true) ? 
                    (([&]() -> int32_t {
                        return 1;
                    })())
                :
                    ((((guid832).tag == 2) && true) ? 
                        (([&]() -> int32_t {
                            return 2;
                        })())
                    :
                        juniper::quit<int32_t>())));
        })());
    }

    Io::mode intToPinMode(uint8_t m) {
        return (([&]() -> Io::mode {
            uint8_t guid833 = m;
            return (((guid833 == 0) && true) ? 
                (([&]() -> Io::mode {
                    return input();
                })())
            :
                (((guid833 == 1) && true) ? 
                    (([&]() -> Io::mode {
                        return output();
                    })())
                :
                    (((guid833 == 2) && true) ? 
                        (([&]() -> Io::mode {
                            return inputPullup();
                        })())
                    :
                        juniper::quit<Io::mode>())));
        })());
    }

    Prelude::unit setPinMode(uint16_t pin, Io::mode m) {
        return (([&]() -> Prelude::unit {
            auto guid834 = pinModeToInt(m);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t m2 = guid834;
            
            return (([&]() -> Prelude::unit {
                pinMode(pin, m2);
                return {};
            })());
        })());
    }

    Prelude::sig<Prelude::unit> risingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::toUnit<Io::pinState>(Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid835 = (([&]() -> bool {
                    Prelude::tuple2<Io::pinState,Io::pinState> guid836 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid836).e2).tag == 1) && ((((guid836).e1).tag == 0) && true)) ? 
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
                bool ret = guid835;
                
                (*((Io::pinState*) (prevState.get())) = currState);
                return ret;
            })());
         }), sig));
    }

    Prelude::sig<Prelude::unit> fallingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::toUnit<Io::pinState>(Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid837 = (([&]() -> bool {
                    Prelude::tuple2<Io::pinState,Io::pinState> guid838 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid838).e2).tag == 0) && ((((guid838).e1).tag == 1) && true)) ? 
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
                bool ret = guid837;
                
                (*((Io::pinState*) (prevState.get())) = currState);
                return ret;
            })());
         }), sig));
    }

    Prelude::sig<Io::pinState> edge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid839 = (([&]() -> bool {
                    Prelude::tuple2<Io::pinState,Io::pinState> guid840 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid840).e2).tag == 1) && ((((guid840).e1).tag == 0) && true)) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        (((((guid840).e2).tag == 0) && ((((guid840).e1).tag == 1) && true)) ? 
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
                bool ret = guid839;
                
                (*((Io::pinState*) (prevState.get())) = currState);
                return ret;
            })());
         }), sig);
    }

}

namespace Maybe {


    using namespace Prelude;




    template<typename a, typename b>
    Prelude::maybe<b> map(juniper::function<b(a)> f, Prelude::maybe<a> maybeVal) {
        return (([&]() -> Prelude::maybe<b> {
            Prelude::maybe<a> guid841 = maybeVal;
            return ((((guid841).tag == 0) && true) ? 
                (([&]() -> Prelude::maybe<b> {
                    auto val = (guid841).just;
                    return just<b>(f(val));
                })())
            :
                (true ? 
                    (([&]() -> Prelude::maybe<b> {
                        return nothing<b>();
                    })())
                :
                    juniper::quit<Prelude::maybe<b>>()));
        })());
    }

    template<typename a>
    a get(Prelude::maybe<a> maybeVal) {
        return (([&]() -> a {
            Prelude::maybe<a> guid842 = maybeVal;
            return ((((guid842).tag == 0) && true) ? 
                (([&]() -> a {
                    auto val = (guid842).just;
                    return val;
                })())
            :
                juniper::quit<a>());
        })());
    }

    template<typename a>
    bool isJust(Prelude::maybe<a> maybeVal) {
        return (([&]() -> bool {
            Prelude::maybe<a> guid843 = maybeVal;
            return ((((guid843).tag == 0) && true) ? 
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

    template<typename a>
    bool isNothing(Prelude::maybe<a> maybeVal) {
        return !(isJust<a>(maybeVal));
    }

    template<typename a>
    uint8_t count(Prelude::maybe<a> maybeVal) {
        return (([&]() -> int32_t {
            Prelude::maybe<a> guid844 = maybeVal;
            return ((((guid844).tag == 0) && true) ? 
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

    template<typename t, typename state>
    state foldl(juniper::function<state(t,state)> f, state initState, Prelude::maybe<t> maybeVal) {
        return (([&]() -> state {
            Prelude::maybe<t> guid845 = maybeVal;
            return ((((guid845).tag == 0) && true) ? 
                (([&]() -> state {
                    auto val = (guid845).just;
                    return f(val, initState);
                })())
            :
                (true ? 
                    (([&]() -> state {
                        return initState;
                    })())
                :
                    juniper::quit<state>()));
        })());
    }

    template<typename t, typename state>
    state fodlr(juniper::function<state(t,state)> f, state initState, Prelude::maybe<t> maybeVal) {
        return foldl<t, state>(f, initState, maybeVal);
    }

    template<typename a>
    Prelude::unit iter(juniper::function<Prelude::unit(a)> f, Prelude::maybe<a> maybeVal) {
        return (([&]() -> Prelude::unit {
            Prelude::maybe<a> guid846 = maybeVal;
            return ((((guid846).tag == 0) && true) ? 
                (([&]() -> Prelude::unit {
                    auto val = (guid846).just;
                    return f(val);
                })())
            :
                (true ? 
                    (([&]() -> Prelude::unit {
                        Prelude::maybe<a> nothing = guid846;
                        return Prelude::unit();
                    })())
                :
                    juniper::quit<Prelude::unit>()));
        })());
    }

}

namespace Time {


    using namespace Prelude;




    Prelude::unit wait(uint32_t time) {
        return (([&]() -> Prelude::unit {
            delay(time);
            return {};
        })());
    }

    uint32_t now() {
        return (([&]() -> uint32_t {
            auto guid847 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t ret = guid847;
            
            (([&]() -> Prelude::unit {
                ret = millis();
                return {};
            })());
            return ret;
        })());
    }

    struct timerState {
        uint32_t lastPulse;
        bool operator==(timerState& rhs) {
            return true && lastPulse == rhs.lastPulse;
        }

        bool operator!=(timerState& rhs) {
            return !(rhs == *this);
        }
    };

    juniper::shared_ptr<Time::timerState> state() {
        return (juniper::shared_ptr<Time::timerState>(new Time::timerState((([&]() -> Time::timerState{
            Time::timerState guid848;
            guid848.lastPulse = 0;
            return guid848;
        })()))));
    }

    Prelude::sig<uint32_t> every(uint32_t interval, juniper::shared_ptr<Time::timerState> state) {
        return (([&]() -> Prelude::sig<uint32_t> {
            auto guid849 = now();
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t t = guid849;
            
            auto guid850 = ((interval == 0) ? 
                t
            :
                ((t / interval) * interval));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t lastWindow = guid850;
            
            return ((((*((state).get()))).lastPulse >= lastWindow) ? 
                signal<uint32_t>(nothing<uint32_t>())
            :
                (([&]() -> Prelude::sig<uint32_t> {
                    (*((Time::timerState*) (state.get())) = (([&]() -> Time::timerState{
                        Time::timerState guid851;
                        guid851.lastPulse = t;
                        return guid851;
                    })()));
                    return signal<uint32_t>(just<uint32_t>(t));
                })()));
        })());
    }

}

namespace Math {


    using namespace Prelude;




    double pi = 3.141592653589793238462643383279502884197169399375105820974;

    double e = 2.718281828459045235360287471352662497757247093699959574966;

    double degToRad(double degrees) {
        return (degrees * 0.017453292519943295769236907684886);
    }

    double radToDeg(double radians) {
        return (radians * 57.295779513082320876798154814105);
    }

    double acos_(double x) {
        return (([&]() -> double {
            auto guid852 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid852;
            
            (([&]() -> Prelude::unit {
                ret = acos(x);
                return {};
            })());
            return ret;
        })());
    }

    double asin_(double x) {
        return (([&]() -> double {
            auto guid853 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid853;
            
            (([&]() -> Prelude::unit {
                ret = asin(x);
                return {};
            })());
            return ret;
        })());
    }

    double atan_(double x) {
        return (([&]() -> double {
            auto guid854 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid854;
            
            (([&]() -> Prelude::unit {
                ret = atan(x);
                return {};
            })());
            return ret;
        })());
    }

    double atan2_(double y, double x) {
        return (([&]() -> double {
            auto guid855 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid855;
            
            (([&]() -> Prelude::unit {
                ret = atan2(y, x);
                return {};
            })());
            return ret;
        })());
    }

    double cos_(double x) {
        return (([&]() -> double {
            auto guid856 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid856;
            
            (([&]() -> Prelude::unit {
                ret = cos(x);
                return {};
            })());
            return ret;
        })());
    }

    double cosh_(double x) {
        return (([&]() -> double {
            auto guid857 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid857;
            
            (([&]() -> Prelude::unit {
                ret = cosh(x);
                return {};
            })());
            return ret;
        })());
    }

    double sin_(double x) {
        return (([&]() -> double {
            auto guid858 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid858;
            
            (([&]() -> Prelude::unit {
                ret = sin(x);
                return {};
            })());
            return ret;
        })());
    }

    double sinh_(double x) {
        return (([&]() -> double {
            auto guid859 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid859;
            
            (([&]() -> Prelude::unit {
                ret = sinh(x);
                return {};
            })());
            return ret;
        })());
    }

    double tan_(double x) {
        return (sin_(x) / cos_(x));
    }

    double tanh_(double x) {
        return (([&]() -> double {
            auto guid860 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid860;
            
            (([&]() -> Prelude::unit {
                ret = tanh(x);
                return {};
            })());
            return ret;
        })());
    }

    double exp_(double x) {
        return (([&]() -> double {
            auto guid861 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid861;
            
            (([&]() -> Prelude::unit {
                ret = exp(x);
                return {};
            })());
            return ret;
        })());
    }

    Prelude::tuple2<double,int16_t> frexp_(double x) {
        return (([&]() -> Prelude::tuple2<double,int16_t> {
            auto guid862 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid862;
            
            auto guid863 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            int16_t exponent = guid863;
            
            (([&]() -> Prelude::unit {
                ret = frexp(x, &exponent);
                return {};
            })());
            return (Prelude::tuple2<double,int16_t>{ret, exponent});
        })());
    }

    double ldexp_(double x, int16_t exponent) {
        return (([&]() -> double {
            auto guid864 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid864;
            
            (([&]() -> Prelude::unit {
                ret = ldexp(x, exponent);
                return {};
            })());
            return ret;
        })());
    }

    double log_(double x) {
        return (([&]() -> double {
            auto guid865 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid865;
            
            (([&]() -> Prelude::unit {
                ret = log(x);
                return {};
            })());
            return ret;
        })());
    }

    double log10_(double x) {
        return (([&]() -> double {
            auto guid866 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid866;
            
            (([&]() -> Prelude::unit {
                ret = log10(x);
                return {};
            })());
            return ret;
        })());
    }

    Prelude::tuple2<double,double> modf_(double x) {
        return (([&]() -> Prelude::tuple2<double,double> {
            auto guid867 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid867;
            
            auto guid868 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double integer = guid868;
            
            (([&]() -> Prelude::unit {
                ret = modf(x, &integer);
                return {};
            })());
            return (Prelude::tuple2<double,double>{ret, integer});
        })());
    }

    double pow_(double x, double y) {
        return (([&]() -> double {
            auto guid869 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid869;
            
            (([&]() -> Prelude::unit {
                ret = pow(x, y);
                return {};
            })());
            return ret;
        })());
    }

    double sqrt_(double x) {
        return (([&]() -> double {
            auto guid870 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid870;
            
            (([&]() -> Prelude::unit {
                ret = sqrt(x);
                return {};
            })());
            return ret;
        })());
    }

    double ceil_(double x) {
        return (([&]() -> double {
            auto guid871 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid871;
            
            (([&]() -> Prelude::unit {
                ret = ceil(x);
                return {};
            })());
            return ret;
        })());
    }

    double fabs_(double x) {
        return (([&]() -> double {
            auto guid872 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid872;
            
            (([&]() -> Prelude::unit {
                ret = fabs(x);
                return {};
            })());
            return ret;
        })());
    }

    double floor_(double x) {
        return (([&]() -> double {
            auto guid873 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid873;
            
            (([&]() -> Prelude::unit {
                ret = floor(x);
                return {};
            })());
            return ret;
        })());
    }

    double fmod_(double x, double y) {
        return (([&]() -> double {
            auto guid874 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid874;
            
            (([&]() -> Prelude::unit {
                ret = fmod(x, y);
                return {};
            })());
            return ret;
        })());
    }

    double round_(double x) {
        return floor_((x + 0.5));
    }

    double min_(double x, double y) {
        return ((x > y) ? 
            y
        :
            x);
    }

    double max_(double x, double y) {
        return ((x > y) ? 
            x
        :
            y);
    }

    double mapRange(double x, double a1, double a2, double b1, double b2) {
        return (b1 + (((x - a1) * (b2 - b1)) / (a2 - a1)));
    }

    template<typename a>
    a clamp(a x, a min, a max) {
        return ((min > x) ? 
            min
        :
            ((x > max) ? 
                max
            :
                x));
    }

    template<typename a>
    int8_t sign(a n) {
        return ((n == 0) ? 
            0
        :
            ((n > 0) ? 
                1
            :
                -1));
    }

}

namespace Button {


    using namespace Prelude;
    using namespace Io;




    struct buttonState {
        Io::pinState actualState;
        Io::pinState lastState;
        uint32_t lastDebounceTime;
        bool operator==(buttonState& rhs) {
            return true && actualState == rhs.actualState && lastState == rhs.lastState && lastDebounceTime == rhs.lastDebounceTime;
        }

        bool operator!=(buttonState& rhs) {
            return !(rhs == *this);
        }
    };

    juniper::shared_ptr<Button::buttonState> state() {
        return (juniper::shared_ptr<Button::buttonState>(new Button::buttonState((([&]() -> Button::buttonState{
            Button::buttonState guid875;
            guid875.actualState = Io::low();
            guid875.lastState = Io::low();
            guid875.lastDebounceTime = 0;
            return guid875;
        })()))));
    }

    Prelude::sig<Io::pinState> debounceDelay(Prelude::sig<Io::pinState> incoming, uint16_t delay, juniper::shared_ptr<Button::buttonState> buttonState) {
        return Signal::map<Io::pinState, Io::pinState>(juniper::function<Io::pinState(Io::pinState)>([=](Io::pinState currentState) mutable -> Io::pinState { 
            return (([&]() -> Io::pinState {
                auto guid876 = (*((buttonState).get()));
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                uint32_t lastDebounceTime = (guid876).lastDebounceTime;
                Io::pinState lastState = (guid876).lastState;
                Io::pinState actualState = (guid876).actualState;
                
                return ((currentState != lastState) ? 
                    (([&]() -> Io::pinState {
                        (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                            Button::buttonState guid877;
                            guid877.actualState = actualState;
                            guid877.lastState = currentState;
                            guid877.lastDebounceTime = Time::now();
                            return guid877;
                        })()));
                        return actualState;
                    })())
                :
                    (((currentState != actualState) && ((Time::now() - ((*((buttonState).get()))).lastDebounceTime) > delay)) ? 
                        (([&]() -> Io::pinState {
                            (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                                Button::buttonState guid878;
                                guid878.actualState = currentState;
                                guid878.lastState = currentState;
                                guid878.lastDebounceTime = lastDebounceTime;
                                return guid878;
                            })()));
                            return currentState;
                        })())
                    :
                        (([&]() -> Io::pinState {
                            (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                                Button::buttonState guid879;
                                guid879.actualState = actualState;
                                guid879.lastState = currentState;
                                guid879.lastDebounceTime = lastDebounceTime;
                                return guid879;
                            })()));
                            return actualState;
                        })())));
            })());
         }), incoming);
    }

    Prelude::sig<Io::pinState> debounce(Prelude::sig<Io::pinState> incoming, juniper::shared_ptr<Button::buttonState> buttonState) {
        return debounceDelay(incoming, 50, buttonState);
    }

}

namespace Vector {


    using namespace Prelude;
    using namespace List;
    using namespace Math;




    template<typename a, int n>
    struct vector {
        juniper::array<a,n> data;
        bool operator==(vector& rhs) {
            return true && data == rhs.data;
        }

        bool operator!=(vector& rhs) {
            return !(rhs == *this);
        }
    };

    uint8_t x = 0;

    uint8_t y = 1;

    uint8_t z = 2;

    template<typename a, int n>
    Vector::vector<a, n> make(juniper::array<a,n> d) {
        return (([&]() -> Vector::vector<a, n>{
            Vector::vector<a, n> guid880;
            guid880.data = d;
            return guid880;
        })());
    }

    template<typename a, int n>
    a get(uint32_t i, Vector::vector<a, n> v) {
        return ((v).data)[i];
    }

    template<typename a, int n>
    Vector::vector<a, n> add(Vector::vector<a, n> v1, Vector::vector<a, n> v2) {
        return (([&]() -> Vector::vector<a, n> {
            auto guid881 = v1;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Vector::vector<a, n> result = guid881;
            
            (([&]() -> Prelude::unit {
                uint32_t guid882 = 0;
                uint32_t guid883 = (n - 1);
                for (uint32_t i = guid882; i <= guid883; i++) {
                    (((result).data)[i] = (((result).data)[i] + ((v2).data)[i]));
                }
                return {};
            })());
            return result;
        })());
    }

    template<typename a, int n>
    Vector::vector<a, n> zero() {
        return (([&]() -> Vector::vector<a, n>{
            Vector::vector<a, n> guid884;
            guid884.data = (juniper::array<a, n>().fill(0));
            return guid884;
        })());
    }

    template<typename a, int n>
    Vector::vector<a, n> subtract(Vector::vector<a, n> v1, Vector::vector<a, n> v2) {
        return (([&]() -> Vector::vector<a, n> {
            auto guid885 = v1;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Vector::vector<a, n> result = guid885;
            
            (([&]() -> Prelude::unit {
                uint32_t guid886 = 0;
                uint32_t guid887 = (n - 1);
                for (uint32_t i = guid886; i <= guid887; i++) {
                    (((result).data)[i] = (((result).data)[i] - ((v2).data)[i]));
                }
                return {};
            })());
            return result;
        })());
    }

    template<typename a, int n>
    Vector::vector<a, n> scale(a scalar, Vector::vector<a, n> v) {
        return (([&]() -> Vector::vector<a, n> {
            auto guid888 = v;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Vector::vector<a, n> result = guid888;
            
            (([&]() -> Prelude::unit {
                uint32_t guid889 = 0;
                uint32_t guid890 = (n - 1);
                for (uint32_t i = guid889; i <= guid890; i++) {
                    (((result).data)[i] = (((result).data)[i] * scalar));
                }
                return {};
            })());
            return result;
        })());
    }

    template<typename a, int n>
    a dot(Vector::vector<a, n> v1, Vector::vector<a, n> v2) {
        return (([&]() -> a {
            auto guid891 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            a sum = guid891;
            
            (([&]() -> Prelude::unit {
                uint32_t guid892 = 0;
                uint32_t guid893 = (n - 1);
                for (uint32_t i = guid892; i <= guid893; i++) {
                    (sum = (sum + (((v1).data)[i] * ((v2).data)[i])));
                }
                return {};
            })());
            return sum;
        })());
    }

    template<typename a, int n>
    a magnitude2(Vector::vector<a, n> v) {
        return (([&]() -> a {
            auto guid894 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            a sum = guid894;
            
            (([&]() -> Prelude::unit {
                uint32_t guid895 = 0;
                uint32_t guid896 = (n - 1);
                for (uint32_t i = guid895; i <= guid896; i++) {
                    (sum = (sum + (((v).data)[i] * ((v).data)[i])));
                }
                return {};
            })());
            return sum;
        })());
    }

    template<typename a, int n>
    double magnitude(Vector::vector<a, n> v) {
        return sqrt_(magnitude2<a, n>(v));
    }

    template<typename a, int n>
    Vector::vector<a, n> multiply(Vector::vector<a, n> u, Vector::vector<a, n> v) {
        return (([&]() -> Vector::vector<a, n> {
            auto guid897 = u;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Vector::vector<a, n> result = guid897;
            
            (([&]() -> Prelude::unit {
                uint32_t guid898 = 0;
                uint32_t guid899 = (n - 1);
                for (uint32_t i = guid898; i <= guid899; i++) {
                    (((result).data)[i] = (((result).data)[i] * ((v).data)[i]));
                }
                return {};
            })());
            return result;
        })());
    }

    template<typename a, int n>
    Vector::vector<a, n> normalize(Vector::vector<a, n> v) {
        return (([&]() -> Vector::vector<a, n> {
            auto guid900 = magnitude<a, n>(v);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double mag = guid900;
            
            return ((mag > 0) ? 
                (([&]() -> Vector::vector<a, n> {
                    auto guid901 = v;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    Vector::vector<a, n> result = guid901;
                    
                    (([&]() -> Prelude::unit {
                        uint32_t guid902 = 0;
                        uint32_t guid903 = (n - 1);
                        for (uint32_t i = guid902; i <= guid903; i++) {
                            (((result).data)[i] = (((result).data)[i] / mag));
                        }
                        return {};
                    })());
                    return result;
                })())
            :
                v);
        })());
    }

    template<typename a, int n>
    double angle(Vector::vector<a, n> v1, Vector::vector<a, n> v2) {
        return acos_((dot<a, n>(v1, v2) / sqrt_((magnitude2<a, n>(v1) * magnitude2<a, n>(v2)))));
    }

    template<typename a>
    Vector::vector<a, 3> cross(Vector::vector<a, 3> u, Vector::vector<a, 3> v) {
        return (([&]() -> Vector::vector<a, 3>{
            Vector::vector<a, 3> guid904;
            guid904.data = (juniper::array<a, 3> { {((((u).data)[1] * ((v).data)[2]) - (((u).data)[2] * ((v).data)[1])), ((((u).data)[2] * ((v).data)[0]) - (((u).data)[0] * ((v).data)[2])), ((((u).data)[0] * ((v).data)[1]) - (((u).data)[1] * ((v).data)[0]))} });
            return guid904;
        })());
    }

    template<typename z, int n>
    Vector::vector<z, n> project(Vector::vector<z, n> a, Vector::vector<z, n> b) {
        return (([&]() -> Vector::vector<z, n> {
            auto guid905 = normalize<z, n>(b);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Vector::vector<z, n> bn = guid905;
            
            return scale<z, n>(dot<z, n>(a, bn), bn);
        })());
    }

    template<typename z, int n>
    Vector::vector<z, n> projectPlane(Vector::vector<z, n> a, Vector::vector<z, n> m) {
        return subtract<z, n>(a, project<z, n>(a, m));
    }

}

namespace Accelerometer {


    using namespace Prelude;




    struct axis {
        uint8_t tag;
        bool operator==(axis& rhs) {
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

        bool operator!=(axis&rhs) { return !(rhs == *this); }
        union {
            uint8_t xAxis;
            uint8_t yAxis;
            uint8_t zAxis;
        };
    };

    axis xAxis() {
        return (([&]() -> axis { axis ret; ret.tag = 0; ret.xAxis = 0; return ret; })());
    }

    axis yAxis() {
        return (([&]() -> axis { axis ret; ret.tag = 1; ret.yAxis = 0; return ret; })());
    }

    axis zAxis() {
        return (([&]() -> axis { axis ret; ret.tag = 2; ret.zAxis = 0; return ret; })());
    }



    struct orientation {
        uint8_t tag;
        bool operator==(orientation& rhs) {
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

        bool operator!=(orientation&rhs) { return !(rhs == *this); }
        union {
            uint8_t xUp;
            uint8_t xDown;
            uint8_t yUp;
            uint8_t yDown;
            uint8_t zUp;
            uint8_t zDown;
        };
    };

    orientation xUp() {
        return (([&]() -> orientation { orientation ret; ret.tag = 0; ret.xUp = 0; return ret; })());
    }

    orientation xDown() {
        return (([&]() -> orientation { orientation ret; ret.tag = 1; ret.xDown = 0; return ret; })());
    }

    orientation yUp() {
        return (([&]() -> orientation { orientation ret; ret.tag = 2; ret.yUp = 0; return ret; })());
    }

    orientation yDown() {
        return (([&]() -> orientation { orientation ret; ret.tag = 3; ret.yDown = 0; return ret; })());
    }

    orientation zUp() {
        return (([&]() -> orientation { orientation ret; ret.tag = 4; ret.zUp = 0; return ret; })());
    }

    orientation zDown() {
        return (([&]() -> orientation { orientation ret; ret.tag = 5; ret.zDown = 0; return ret; })());
    }



    uint16_t xPin = (([&]() -> uint16_t {
        auto guid906 = 0;
        if (!(true)) {
            juniper::quit<Prelude::unit>();
        }
        uint16_t p = guid906;
        
        (([&]() -> Prelude::unit {
            p = A0;
            return {};
        })());
        return p;
    })());

    uint16_t yPin = (([&]() -> uint16_t {
        auto guid907 = 0;
        if (!(true)) {
            juniper::quit<Prelude::unit>();
        }
        uint16_t p = guid907;
        
        (([&]() -> Prelude::unit {
            p = A1;
            return {};
        })());
        return p;
    })());

    uint16_t zPin = (([&]() -> uint16_t {
        auto guid908 = 0;
        if (!(true)) {
            juniper::quit<Prelude::unit>();
        }
        uint16_t p = guid908;
        
        (([&]() -> Prelude::unit {
            p = A2;
            return {};
        })());
        return p;
    })());

    uint16_t axisToPin(Accelerometer::axis a) {
        return (([&]() -> uint16_t {
            Accelerometer::axis guid909 = a;
            return ((((guid909).tag == 0) && true) ? 
                (([&]() -> uint16_t {
                    return xPin;
                })())
            :
                ((((guid909).tag == 1) && true) ? 
                    (([&]() -> uint16_t {
                        return yPin;
                    })())
                :
                    ((((guid909).tag == 2) && true) ? 
                        (([&]() -> uint16_t {
                            return zPin;
                        })())
                    :
                        juniper::quit<uint16_t>())));
        })());
    }

    Prelude::tuple2<uint16_t,uint16_t> axisToRange(Accelerometer::axis a) {
        return (([&]() -> Prelude::tuple2<int32_t,int32_t> {
            Accelerometer::axis guid910 = a;
            return ((((guid910).tag == 0) && true) ? 
                (([&]() -> Prelude::tuple2<int32_t,int32_t> {
                    return (Prelude::tuple2<int32_t,int32_t>{404, 612});
                })())
            :
                ((((guid910).tag == 1) && true) ? 
                    (([&]() -> Prelude::tuple2<int32_t,int32_t> {
                        return (Prelude::tuple2<int32_t,int32_t>{409, 622});
                    })())
                :
                    ((((guid910).tag == 2) && true) ? 
                        (([&]() -> Prelude::tuple2<int32_t,int32_t> {
                            return (Prelude::tuple2<int32_t,int32_t>{418, 622});
                        })())
                    :
                        juniper::quit<Prelude::tuple2<int32_t,int32_t>>())));
        })());
    }

    uint16_t readRaw(Accelerometer::axis a) {
        return (([&]() -> int32_t {
            auto guid911 = axisToPin(a);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint16_t pin = guid911;
            
            Io::anaRead(pin);
            Time::wait(1);
            auto guid912 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t total = guid912;
            
            (([&]() -> Prelude::unit {
                uint8_t guid913 = 0;
                uint8_t guid914 = 3;
                for (uint8_t i = guid913; i <= guid914; i++) {
                    (total = (total + Io::anaRead(pin)));
                }
                return {};
            })());
            return (total / 4);
        })());
    }

    int16_t read(Accelerometer::axis a) {
        return (([&]() -> double {
            auto guid915 = axisToRange(a);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint16_t max = (guid915).e2;
            uint16_t min = (guid915).e1;
            
            return Math::mapRange(readRaw(a), min, max, -1000, 1000);
        })());
    }

    Prelude::maybe<Accelerometer::orientation> getOrientation() {
        return (([&]() -> Prelude::maybe<Accelerometer::orientation> {
            auto guid916 = read(xAxis());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            int16_t xScaled = guid916;
            
            auto guid917 = read(yAxis());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            int16_t yScaled = guid917;
            
            auto guid918 = read(zAxis());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            int16_t zScaled = guid918;
            
            return ((-800 > zScaled) ? 
                just<Accelerometer::orientation>(zDown())
            :
                ((zScaled > 800) ? 
                    just<Accelerometer::orientation>(zUp())
                :
                    ((-800 > yScaled) ? 
                        just<Accelerometer::orientation>(yDown())
                    :
                        ((yScaled > 800) ? 
                            just<Accelerometer::orientation>(yUp())
                        :
                            ((-800 > xScaled) ? 
                                just<Accelerometer::orientation>(xDown())
                            :
                                ((xScaled > 800) ? 
                                    just<Accelerometer::orientation>(xUp())
                                :
                                    nothing<Accelerometer::orientation>()))))));
        })());
    }

    Prelude::sig<Accelerometer::orientation> getSignal() {
        return signal<Accelerometer::orientation>(getOrientation());
    }

}

#include <FastLED.h>
namespace FastLed {


    using namespace Prelude;






    struct fastLedStrip {
        juniper::shared_ptr<void> ptr;
        bool operator==(fastLedStrip& rhs) {
            return true && ptr == rhs.ptr;
        }

        bool operator!=(fastLedStrip& rhs) {
            return !(rhs == *this);
        }
    };

    struct color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        bool operator==(color& rhs) {
            return true && r == rhs.r && g == rhs.g && b == rhs.b;
        }

        bool operator!=(color& rhs) {
            return !(rhs == *this);
        }
    };

    FastLed::fastLedStrip make(uint16_t numLeds) {
        return (([&]() -> FastLed::fastLedStrip {
            auto guid919 = juniper::shared_ptr<void>(NULL);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            juniper::shared_ptr<void> p = guid919;
            
            (([&]() -> Prelude::unit {
                
    CRGB *leds = new CRGB[numLeds];
    p.set((void *) leds);
    FastLED.addLeds<WS2812, 6, RGB>(leds, numLeds);
    FastLED.setBrightness(MAX_BRIGHTNESS);
    
                return {};
            })());
            Io::setPinMode(6, Io::output());
            return (([&]() -> FastLed::fastLedStrip{
                FastLed::fastLedStrip guid920;
                guid920.ptr = p;
                return guid920;
            })());
        })());
    }

    Prelude::unit setLedColor(uint16_t n, FastLed::color c, FastLed::fastLedStrip strip) {
        return (([&]() -> Prelude::unit {
            auto guid921 = (strip).ptr;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            juniper::shared_ptr<void> p = guid921;
            
            auto guid922 = (c).r;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t r = guid922;
            
            auto guid923 = (c).g;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t g = guid923;
            
            auto guid924 = (c).b;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t b = guid924;
            
            return (([&]() -> Prelude::unit {
                ((CRGB *) p.get())[n] = CRGB(r, g, b);
                return {};
            })());
        })());
    }

    FastLed::color getLedColor(uint16_t n, FastLed::fastLedStrip strip) {
        return (([&]() -> FastLed::color {
            auto guid925 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t r = guid925;
            
            auto guid926 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t g = guid926;
            
            auto guid927 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t b = guid927;
            
            (([&]() -> Prelude::unit {
                
    CRGB c = ((CRGB *) p.get())[n];
    r = c.r;
    g = c.g;
    b = c.b;
    
                return {};
            })());
            return (([&]() -> FastLed::color{
                FastLed::color guid928;
                guid928.r = r;
                guid928.g = g;
                guid928.b = b;
                return guid928;
            })());
        })());
    }

    Prelude::unit show() {
        return (([&]() -> Prelude::unit {
            FastLED.show();
            return {};
        })());
    }

}

namespace Finale {


    using namespace Prelude;




    Prelude::unit execute() {
        return Prelude::unit();
    }

}

namespace Paused {


    using namespace Prelude;




    Prelude::unit execute() {
        return Prelude::unit();
    }

}

namespace Setting {


    using namespace Prelude;




    juniper::shared_ptr<Button::buttonState> bState = Button::state();

    juniper::shared_ptr<Io::pinState> bEdgeState = (juniper::shared_ptr<Io::pinState>(new Io::pinState(Io::low())));

    uint16_t buttonPin = 4;

    uint16_t numLeds = 33;

    juniper::shared_ptr<uint16_t> numLedsLit = (juniper::shared_ptr<int32_t>(new int32_t(1)));

    FastLed::color blue = (([&]() -> FastLed::color{
        FastLed::color guid929;
        guid929.r = 0;
        guid929.g = 0;
        guid929.b = 255;
        return guid929;
    })());

    Prelude::unit reset(juniper::shared_ptr<uint32_t> timeRemaining) {
        return (([&]() -> Prelude::unit {
            (*((int32_t*) (numLedsLit.get())) = 1);
            (*((int32_t*) (timeRemaining.get())) = 60000);
            return Prelude::unit();
        })());
    }

    Prelude::unit execute(juniper::shared_ptr<uint32_t> timeRemaining, FastLed::fastLedStrip leds) {
        return (([&]() -> Prelude::unit {
            auto guid930 = Io::risingEdge(Button::debounce(Io::digIn(buttonPin), bState), bEdgeState);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::sig<Prelude::unit> buttonSig = guid930;
            
            auto guid931 = Signal::foldP<Prelude::unit, uint16_t>(juniper::function<uint16_t(Prelude::unit,uint16_t)>([=](Prelude::unit u, uint16_t prevNumLedsLit) mutable -> uint16_t { 
                return (prevNumLedsLit + 1);
             }), numLedsLit, buttonSig);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::sig<uint16_t> numLedsLitSig = guid931;
            
            return Signal::sink<uint16_t>(juniper::function<Prelude::unit(uint16_t)>([=](uint16_t n) mutable -> Prelude::unit { 
                return (([&]() -> Prelude::unit {
                    (*((int32_t*) (timeRemaining.get())) = (n * 60000));
                    return (([&]() -> Prelude::unit {
                        uint16_t guid932 = 0;
                        uint16_t guid933 = (n - 1);
                        for (uint16_t i = guid932; i <= guid933; i++) {
                            FastLed::setLedColor(i, blue, leds);
                        }
                        return {};
                    })());
                })());
             }), numLedsLitSig);
        })());
    }

}

namespace SignalExt {


    using namespace Prelude;




    template<typename a>
    Prelude::sig<a> constant(a val) {
        return signal<a>(just<a>(val));
    }

    template<typename a>
    Prelude::sig<Prelude::maybe<a>> meta(Prelude::sig<a> sigA) {
        return (([&]() -> Prelude::sig<Prelude::maybe<a>> {
            auto guid934 = sigA;
            if (!((((guid934).tag == 0) && true))) {
                juniper::quit<Prelude::unit>();
            }
            auto val = (guid934).signal;
            
            return constant<Prelude::maybe<a>>(val);
        })());
    }

}

namespace Timing {


    using namespace Prelude;




    Prelude::unit execute() {
        return Prelude::unit();
    }

}

namespace Program {


    using namespace Prelude;


    struct mode {
        uint8_t tag;
        bool operator==(mode& rhs) {
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

        bool operator!=(mode&rhs) { return !(rhs == *this); }
        union {
            uint8_t setting;
            uint8_t timing;
            uint8_t paused;
            uint8_t finale;
        };
    };

    mode setting() {
        return (([&]() -> mode { mode ret; ret.tag = 0; ret.setting = 0; return ret; })());
    }

    mode timing() {
        return (([&]() -> mode { mode ret; ret.tag = 1; ret.timing = 0; return ret; })());
    }

    mode paused() {
        return (([&]() -> mode { mode ret; ret.tag = 2; ret.paused = 0; return ret; })());
    }

    mode finale() {
        return (([&]() -> mode { mode ret; ret.tag = 3; ret.finale = 0; return ret; })());
    }



    struct flip {
        uint8_t tag;
        bool operator==(flip& rhs) {
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

        bool operator!=(flip&rhs) { return !(rhs == *this); }
        union {
            uint8_t flipUp;
            uint8_t flipDown;
            uint8_t flipFlat;
        };
    };

    flip flipUp() {
        return (([&]() -> flip { flip ret; ret.tag = 0; ret.flipUp = 0; return ret; })());
    }

    flip flipDown() {
        return (([&]() -> flip { flip ret; ret.tag = 1; ret.flipDown = 0; return ret; })());
    }

    flip flipFlat() {
        return (([&]() -> flip { flip ret; ret.tag = 2; ret.flipFlat = 0; return ret; })());
    }



    uint16_t numLeds = 33;

    FastLed::fastLedStrip leds = FastLed::make(numLeds);

    juniper::shared_ptr<Prelude::maybe<Accelerometer::orientation>> accState = (juniper::shared_ptr<Prelude::maybe<Accelerometer::orientation>>(new Prelude::maybe<Accelerometer::orientation>(nothing<Accelerometer::orientation>())));

    juniper::shared_ptr<Program::mode> modeState = (juniper::shared_ptr<Program::mode>(new Program::mode(setting())));

    juniper::shared_ptr<uint32_t> timeRemaining = (juniper::shared_ptr<int32_t>(new int32_t(0)));

    Prelude::unit setup() {
        return Time::wait(500);
    }

    Prelude::unit main() {
        return (([&]() -> Prelude::unit {
            setup();
            return (([&]() -> Prelude::unit {
                while (true) {
                    (([&]() -> Prelude::unit {
                        auto guid935 = Signal::dropRepeats<Accelerometer::orientation>(Accelerometer::getSignal(), accState);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        Prelude::sig<Accelerometer::orientation> accSig = guid935;
                        
                        auto guid936 = Signal::map<Accelerometer::orientation, Program::flip>(juniper::function<Program::flip(Accelerometer::orientation)>([=](Accelerometer::orientation o) mutable -> Program::flip { 
                            return (([&]() -> Program::flip {
                                Accelerometer::orientation guid937 = o;
                                return ((((guid937).tag == 0) && true) ? 
                                    (([&]() -> Program::flip {
                                        return flipUp();
                                    })())
                                :
                                    ((((guid937).tag == 1) && true) ? 
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
                        Prelude::sig<Program::flip> flipSig = guid936;
                        
                        auto guid938 = SignalExt::meta<Program::flip>(flipSig);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        Prelude::sig<Prelude::maybe<Program::flip>> metaFlipSig = guid938;
                        
                        auto guid939 = Signal::foldP<Prelude::maybe<Program::flip>, Program::mode>(juniper::function<Program::mode(Prelude::maybe<Program::flip>,Program::mode)>([=](Prelude::maybe<Program::flip> f1, Program::mode prevMode) mutable -> Program::mode { 
                            return (((prevMode == timing()) && ((*((timeRemaining).get())) == 0)) ? 
                                finale()
                            :
                                (([&]() -> Program::mode {
                                    Prelude::maybe<Program::flip> guid940 = f1;
                                    return ((((guid940).tag == 0) && true) ? 
                                        (([&]() -> Program::mode {
                                            auto f2 = (guid940).just;
                                            return (([&]() -> Program::mode {
                                                Prelude::tuple2<a,Program::mode> guid941 = (Prelude::tuple2<a,Program::mode>{f2, prevMode});
                                                return (((((guid941).e2).tag == 0) && ((((guid941).e1).tag == 1) && true)) ? 
                                                    (([&]() -> Program::mode {
                                                        return timing();
                                                    })())
                                                :
                                                    (((((guid941).e2).tag == 1) && ((((guid941).e1).tag == 0) && true)) ? 
                                                        (([&]() -> Program::mode {
                                                            return (([&]() -> Program::mode {
                                                                Setting::reset(timeRemaining);
                                                                return setting();
                                                            })());
                                                        })())
                                                    :
                                                        (((((guid941).e2).tag == 2) && ((((guid941).e1).tag == 0) && true)) ? 
                                                            (([&]() -> Program::mode {
                                                                return (([&]() -> Program::mode {
                                                                    Setting::reset(timeRemaining);
                                                                    return setting();
                                                                })());
                                                            })())
                                                        :
                                                            (((((guid941).e2).tag == 1) && ((((guid941).e1).tag == 2) && true)) ? 
                                                                (([&]() -> Program::mode {
                                                                    return paused();
                                                                })())
                                                            :
                                                                (true ? 
                                                                    (([&]() -> Program::mode {
                                                                        return prevMode;
                                                                    })())
                                                                :
                                                                    juniper::quit<Program::mode>())))));
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
                        Prelude::sig<Program::mode> modeSig = guid939;
                        
                        Signal::sink<Program::mode>(juniper::function<Prelude::unit(Program::mode)>([=](Program::mode m) mutable -> Prelude::unit { 
                            return (([&]() -> Prelude::unit {
                                Program::mode guid942 = m;
                                return ((((guid942).tag == 0) && true) ? 
                                    (([&]() -> Prelude::unit {
                                        return Setting::execute(timeRemaining, leds);
                                    })())
                                :
                                    ((((guid942).tag == 1) && true) ? 
                                        (([&]() -> Prelude::unit {
                                            return Timing::execute();
                                        })())
                                    :
                                        ((((guid942).tag == 2) && true) ? 
                                            (([&]() -> Prelude::unit {
                                                return Paused::execute();
                                            })())
                                        :
                                            ((((guid942).tag == 3) && true) ? 
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