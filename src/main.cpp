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
        bool operator==(unit rhs) {
            return true;
        }

        bool operator!=(unit rhs) {
            return !(rhs == *this);
        }
    };

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
        bool operator==(list rhs) {
            return true && data == rhs.data && length == rhs.length;
        }

        bool operator!=(list rhs) {
            return !(rhs == *this);
        }
    };

    template<int n>
    struct string {
        Prelude::list<uint8_t, n> characters;
        bool operator==(string rhs) {
            return true && characters == rhs.characters;
        }

        bool operator!=(string rhs) {
            return !(rhs == *this);
        }
    };

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
        bool operator==(tuple2 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2;
        }

        bool operator!=(tuple2 rhs) {
            return !(rhs == *this);
        }
    };

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

    template<typename a, typename b>
    a fst(Prelude::tuple2<a,b> tup) {
        return (([&]() -> a {
            Prelude::tuple2<a,b> guid859 = tup;
            return (true ? 
                (([&]() -> a {
                    a x = (guid859).e1;
                    return x;
                })())
            :
                juniper::quit<a>());
        })());
    }

    template<typename a, typename b>
    b snd(Prelude::tuple2<a,b> tup) {
        return (([&]() -> b {
            Prelude::tuple2<a,b> guid860 = tup;
            return (true ? 
                (([&]() -> b {
                    b x = (guid860).e2;
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
            auto guid861 = (juniper::array<b, n>());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            juniper::array<b,n> ret = guid861;
            
            (([&]() -> Prelude::unit {
                uint32_t guid862 = 0;
                uint32_t guid863 = ((lst).length - 1);
                for (uint32_t i = guid862; i <= guid863; i++) {
                    ((ret)[i] = f(((lst).data)[i]));
                }
                return {};
            })());
            return (([&]() -> Prelude::list<b, n>{
                Prelude::list<b, n> guid864;
                guid864.data = ret;
                guid864.length = (lst).length;
                return guid864;
            })());
        })());
    }

    template<typename t, typename state, int n>
    state foldl(juniper::function<state(t,state)> f, state initState, Prelude::list<t, n> lst) {
        return (([&]() -> state {
            auto guid865 = initState;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            state s = guid865;
            
            (([&]() -> Prelude::unit {
                uint32_t guid866 = 0;
                uint32_t guid867 = ((lst).length - 1);
                for (uint32_t i = guid866; i <= guid867; i++) {
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
            auto guid868 = initState;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            state s = guid868;
            
            (([&]() -> Prelude::unit {
                uint32_t guid869 = ((lst).length - 1);
                uint32_t guid870 = 0;
                for (uint32_t i = guid869; i >= guid870; i--) {
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
            auto guid871 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t j = guid871;
            
            auto guid872 = (([&]() -> Prelude::list<t, retCap>{
                Prelude::list<t, retCap> guid873;
                guid873.data = (juniper::array<t, retCap>());
                guid873.length = ((lstA).length + (lstB).length);
                return guid873;
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::list<t, retCap> out = guid872;
            
            (([&]() -> Prelude::unit {
                uint32_t guid874 = 0;
                uint32_t guid875 = ((lstA).length - 1);
                for (uint32_t i = guid874; i <= guid875; i++) {
                    (([&]() -> int32_t {
                        (((out).data)[j] = ((lstA).data)[i]);
                        return (j = (j + 1));
                    })());
                }
                return {};
            })());
            (([&]() -> Prelude::unit {
                uint32_t guid876 = 0;
                uint32_t guid877 = ((lstB).length - 1);
                for (uint32_t i = guid876; i <= guid877; i++) {
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
            auto guid878 = (juniper::array<t, (m)*(n)>());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            juniper::array<t,(m)*(n)> ret = guid878;
            
            auto guid879 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t index = guid879;
            
            (([&]() -> Prelude::unit {
                uint32_t guid880 = 0;
                uint32_t guid881 = ((listOfLists).length - 1);
                for (uint32_t i = guid880; i <= guid881; i++) {
                    (([&]() -> Prelude::unit {
                        uint32_t guid882 = 0;
                        uint32_t guid883 = ((((listOfLists).data)[i]).length - 1);
                        for (uint32_t j = guid882; j <= guid883; j++) {
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
                Prelude::list<t, (m)*(n)> guid884;
                guid884.data = ret;
                guid884.length = index;
                return guid884;
            })());
        })());
    }

    template<typename t, int n, int m>
    Prelude::list<t, m> resize(Prelude::list<t, n> lst) {
        return (([&]() -> Prelude::list<t, m> {
            auto guid885 = (juniper::array<t, m>());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            juniper::array<t,m> ret = guid885;
            
            (([&]() -> Prelude::unit {
                uint32_t guid886 = 0;
                uint32_t guid887 = ((lst).length - 1);
                for (uint32_t i = guid886; i <= guid887; i++) {
                    ((ret)[i] = ((lst).data)[i]);
                }
                return {};
            })());
            return (([&]() -> Prelude::list<t, m>{
                Prelude::list<t, m> guid888;
                guid888.data = ret;
                guid888.length = (lst).length;
                return guid888;
            })());
        })());
    }

    template<typename t, int n>
    bool all(juniper::function<bool(t)> pred, Prelude::list<t, n> lst) {
        return (([&]() -> bool {
            auto guid889 = true;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            bool satisfied = guid889;
            
            (([&]() -> Prelude::unit {
                uint32_t guid890 = 0;
                uint32_t guid891 = ((lst).length - 1);
                for (uint32_t i = guid890; i <= guid891; i++) {
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
            auto guid892 = false;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            bool satisfied = guid892;
            
            (([&]() -> Prelude::unit {
                uint32_t guid893 = 0;
                uint32_t guid894 = ((lst).length - 1);
                for (uint32_t i = guid893; i <= guid894; i++) {
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
                auto guid895 = lst;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                Prelude::list<t, n> ret = guid895;
                
                (((ret).data)[(lst).length] = elem);
                ((ret).length = ((lst).length + 1));
                return ret;
            })()));
    }

    template<typename t, int n>
    Prelude::list<t, n> pushOffFront(t elem, Prelude::list<t, n> lst) {
        return (([&]() -> Prelude::list<t, n> {
            auto guid896 = lst;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::list<t, n> ret = guid896;
            
            (([&]() -> Prelude::unit {
                uint32_t guid897 = (n - 2);
                uint32_t guid898 = 0;
                for (uint32_t i = guid897; i >= guid898; i--) {
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
                auto guid899 = lst;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                Prelude::list<t, n> ret = guid899;
                
                (((ret).data)[index] = elem);
                return ret;
            })()));
    }

    template<typename t, int n>
    Prelude::list<t, n> replicate(uint32_t numOfElements, t elem) {
        return (([&]() -> Prelude::list<t, n>{
            Prelude::list<t, n> guid900;
            guid900.data = (juniper::array<t, n>().fill(elem));
            guid900.length = numOfElements;
            return guid900;
        })());
    }

    template<typename t, int n>
    Prelude::list<t, n> remove(t elem, Prelude::list<t, n> lst) {
        return (([&]() -> Prelude::list<t, n> {
            auto guid901 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t index = guid901;
            
            auto guid902 = false;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            bool found = guid902;
            
            (([&]() -> Prelude::unit {
                uint32_t guid903 = 0;
                uint32_t guid904 = ((lst).length - 1);
                for (uint32_t i = guid903; i <= guid904; i++) {
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
                    auto guid905 = lst;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    Prelude::list<t, n> ret = guid905;
                    
                    ((ret).length = ((lst).length - 1));
                    (([&]() -> Prelude::unit {
                        uint32_t guid906 = index;
                        uint32_t guid907 = ((lst).length - 2);
                        for (uint32_t i = guid906; i <= guid907; i++) {
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
                Prelude::list<t, n> guid908;
                guid908.data = (lst).data;
                guid908.length = ((lst).length - 1);
                return guid908;
            })()));
    }

    template<typename t, int n>
    Prelude::unit foreach(juniper::function<Prelude::unit(t)> f, Prelude::list<t, n> lst) {
        return (([&]() -> Prelude::unit {
            uint32_t guid909 = 0;
            uint32_t guid910 = ((lst).length - 1);
            for (uint32_t i = guid909; i <= guid910; i++) {
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
                auto guid911 = ((lst).data)[0];
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                t maxVal = guid911;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid912 = 1;
                    uint32_t guid913 = ((lst).length - 1);
                    for (uint32_t i = guid912; i <= guid913; i++) {
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
                auto guid914 = ((lst).data)[0];
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                t minVal = guid914;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid915 = 1;
                    uint32_t guid916 = ((lst).length - 1);
                    for (uint32_t i = guid915; i <= guid916; i++) {
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
            auto guid917 = false;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            bool found = guid917;
            
            (([&]() -> Prelude::unit {
                uint32_t guid918 = 0;
                uint32_t guid919 = ((lst).length - 1);
                for (uint32_t i = guid918; i <= guid919; i++) {
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
                auto guid920 = (([&]() -> Prelude::list<Prelude::tuple2<a,b>, n>{
                    Prelude::list<Prelude::tuple2<a,b>, n> guid921;
                    guid921.data = (juniper::array<Prelude::tuple2<a,b>, n>());
                    guid921.length = (lstA).length;
                    return guid921;
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                Prelude::list<Prelude::tuple2<a,b>, n> ret = guid920;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid922 = 0;
                    uint32_t guid923 = (lstA).length;
                    for (uint32_t i = guid922; i <= guid923; i++) {
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
            auto guid924 = (([&]() -> Prelude::list<a, n>{
                Prelude::list<a, n> guid925;
                guid925.data = (juniper::array<a, n>());
                guid925.length = (lst).length;
                return guid925;
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::list<a, n> retA = guid924;
            
            auto guid926 = (([&]() -> Prelude::list<a, n>{
                Prelude::list<a, n> guid927;
                guid927.data = (juniper::array<b, n>());
                guid927.length = (lst).length;
                return guid927;
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::list<a, n> retB = guid926;
            
            (([&]() -> Prelude::unit {
                uint32_t guid928 = 0;
                uint32_t guid929 = ((lst).length - 1);
                for (uint32_t i = guid928; i <= guid929; i++) {
                    (([&]() -> b {
                        auto guid930 = ((lst).data)[i];
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        b elemB = (guid930).e2;
                        a elemA = (guid930).e1;
                        
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
            Prelude::sig<a> guid931 = s;
            return ((((guid931).tag == 0) && ((((guid931).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<b> {
                    auto val = ((guid931).signal).just;
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
            Prelude::sig<a> guid932 = s;
            return ((((guid932).tag == 0) && ((((guid932).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::unit {
                    auto val = ((guid932).signal).just;
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
            Prelude::sig<a> guid933 = s;
            return ((((guid933).tag == 0) && ((((guid933).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<a> {
                    auto val = ((guid933).signal).just;
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
            Prelude::sig<a> guid934 = sigA;
            return ((((guid934).tag == 0) && ((((guid934).signal).tag == 0) && true)) ? 
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
            auto guid935 = nothing<a>();
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::maybe<a> ret = guid935;
            
            (([&]() -> Prelude::unit {
                uint32_t guid936 = 0;
                uint32_t guid937 = (n - 1);
                for (uint32_t i = guid936; i <= guid937; i++) {
                    (([&]() -> Prelude::unit {
                        Prelude::maybe<a> guid938 = ret;
                        return ((((guid938).tag == 1) && true) ? 
                            (([&]() -> Prelude::unit {
                                return (([&]() -> Prelude::unit {
                                    auto guid939 = List::nth<a, n>(i, sigs);
                                    if (!((((guid939).tag == 0) && true))) {
                                        juniper::quit<Prelude::unit>();
                                    }
                                    auto heldValue = (guid939).signal;
                                    
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
            Prelude::tuple2<Prelude::sig<a>,Prelude::sig<b>> guid940 = (Prelude::tuple2<Prelude::sig<a>,Prelude::sig<b>>{sigA, sigB});
            return (((((guid940).e1).tag == 0) && (((((guid940).e1).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<Prelude::either<a, b>> {
                    auto value = (((guid940).e1).signal).just;
                    return signal<Prelude::either<a, b>>(just<Prelude::either<a, b>>(left<a, b>(value)));
                })())
            :
                (((((guid940).e2).tag == 0) && (((((guid940).e2).signal).tag == 0) && true)) ? 
                    (([&]() -> Prelude::sig<Prelude::either<a, b>> {
                        auto value = (((guid940).e2).signal).just;
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
            Prelude::sig<a> guid941 = incoming;
            return ((((guid941).tag == 0) && ((((guid941).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<state> {
                    auto val = ((guid941).signal).just;
                    return (([&]() -> Prelude::sig<state> {
                        auto guid942 = f(val, (*((state0).get())));
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        state state1 = guid942;
                        
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
                auto guid943 = (([&]() -> bool {
                    Prelude::maybe<a> guid944 = (*((maybePrevValue).get()));
                    return ((((guid944).tag == 1) && true) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        ((((guid944).tag == 0) && true) ? 
                            (([&]() -> bool {
                                auto prevValue = (guid944).just;
                                return (value == prevValue);
                            })())
                        :
                            juniper::quit<bool>()));
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                bool filtered = guid943;
                
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
            Prelude::sig<a> guid945 = incoming;
            return ((((guid945).tag == 0) && ((((guid945).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<a> {
                    auto val = ((guid945).signal).just;
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
            auto guid946 = (([&]() -> a {
                Prelude::sig<a> guid947 = incomingA;
                return ((((guid947).tag == 0) && ((((guid947).signal).tag == 0) && true)) ? 
                    (([&]() -> a {
                        auto val1 = ((guid947).signal).just;
                        return val1;
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
            a valA = guid946;
            
            auto guid948 = (([&]() -> b {
                Prelude::sig<b> guid949 = incomingB;
                return ((((guid949).tag == 0) && ((((guid949).signal).tag == 0) && true)) ? 
                    (([&]() -> b {
                        auto val2 = ((guid949).signal).just;
                        return val2;
                    })())
                :
                    (true ? 
                        (([&]() -> b {
                            return snd<a, b>((*((state).get())));
                        })())
                    :
                        juniper::quit<b>()));
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            b valB = guid948;
            
            (*((Prelude::tuple2<a,b>*) (state.get())) = (Prelude::tuple2<a,b>{valA, valB}));
            return (([&]() -> Prelude::sig<c> {
                Prelude::tuple2<Prelude::sig<a>,Prelude::sig<b>> guid950 = (Prelude::tuple2<Prelude::sig<a>,Prelude::sig<b>>{incomingA, incomingB});
                return (((((guid950).e2).tag == 0) && (((((guid950).e2).signal).tag == 1) && ((((guid950).e1).tag == 0) && (((((guid950).e1).signal).tag == 1) && true)))) ? 
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

    pinState high() {
        return (([&]() -> pinState { pinState ret; ret.tag = 0; ret.high = 0; return ret; })());
    }

    pinState low() {
        return (([&]() -> pinState { pinState ret; ret.tag = 1; ret.low = 0; return ret; })());
    }



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
            Io::pinState guid951 = p;
            return ((((guid951).tag == 0) && true) ? 
                (([&]() -> Io::pinState {
                    return low();
                })())
            :
                ((((guid951).tag == 1) && true) ? 
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
            Io::pinState guid952 = value;
            return ((((guid952).tag == 1) && true) ? 
                (([&]() -> int32_t {
                    return 0;
                })())
            :
                ((((guid952).tag == 0) && true) ? 
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
            auto guid953 = pinStateToInt(value);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t intVal = guid953;
            
            return (([&]() -> Prelude::unit {
                digitalWrite(pin, intVal);
                return {};
            })());
        })());
    }

    Io::pinState digRead(uint16_t pin) {
        return (([&]() -> Io::pinState {
            auto guid954 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t intVal = guid954;
            
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
            auto guid955 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint16_t value = guid955;
            
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
            Io::mode guid956 = m;
            return ((((guid956).tag == 0) && true) ? 
                (([&]() -> int32_t {
                    return 0;
                })())
            :
                ((((guid956).tag == 1) && true) ? 
                    (([&]() -> int32_t {
                        return 1;
                    })())
                :
                    ((((guid956).tag == 2) && true) ? 
                        (([&]() -> int32_t {
                            return 2;
                        })())
                    :
                        juniper::quit<int32_t>())));
        })());
    }

    Io::mode intToPinMode(uint8_t m) {
        return (([&]() -> Io::mode {
            uint8_t guid957 = m;
            return (((guid957 == 0) && true) ? 
                (([&]() -> Io::mode {
                    return input();
                })())
            :
                (((guid957 == 1) && true) ? 
                    (([&]() -> Io::mode {
                        return output();
                    })())
                :
                    (((guid957 == 2) && true) ? 
                        (([&]() -> Io::mode {
                            return inputPullup();
                        })())
                    :
                        juniper::quit<Io::mode>())));
        })());
    }

    Prelude::unit setPinMode(uint16_t pin, Io::mode m) {
        return (([&]() -> Prelude::unit {
            auto guid958 = pinModeToInt(m);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t m2 = guid958;
            
            return (([&]() -> Prelude::unit {
                pinMode(pin, m2);
                return {};
            })());
        })());
    }

    Prelude::sig<Prelude::unit> risingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::toUnit<Io::pinState>(Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid959 = (([&]() -> bool {
                    Prelude::tuple2<Io::pinState,Io::pinState> guid960 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid960).e2).tag == 1) && ((((guid960).e1).tag == 0) && true)) ? 
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
                bool ret = guid959;
                
                (*((Io::pinState*) (prevState.get())) = currState);
                return ret;
            })());
         }), sig));
    }

    Prelude::sig<Prelude::unit> fallingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::toUnit<Io::pinState>(Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid961 = (([&]() -> bool {
                    Prelude::tuple2<Io::pinState,Io::pinState> guid962 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid962).e2).tag == 0) && ((((guid962).e1).tag == 1) && true)) ? 
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
                bool ret = guid961;
                
                (*((Io::pinState*) (prevState.get())) = currState);
                return ret;
            })());
         }), sig));
    }

    Prelude::sig<Io::pinState> edge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid963 = (([&]() -> bool {
                    Prelude::tuple2<Io::pinState,Io::pinState> guid964 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid964).e2).tag == 1) && ((((guid964).e1).tag == 0) && true)) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        (((((guid964).e2).tag == 0) && ((((guid964).e1).tag == 1) && true)) ? 
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
                bool ret = guid963;
                
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
            Prelude::maybe<a> guid965 = maybeVal;
            return ((((guid965).tag == 0) && true) ? 
                (([&]() -> Prelude::maybe<b> {
                    auto val = (guid965).just;
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
            Prelude::maybe<a> guid966 = maybeVal;
            return ((((guid966).tag == 0) && true) ? 
                (([&]() -> a {
                    auto val = (guid966).just;
                    return val;
                })())
            :
                juniper::quit<a>());
        })());
    }

    template<typename a>
    bool isJust(Prelude::maybe<a> maybeVal) {
        return (([&]() -> bool {
            Prelude::maybe<a> guid967 = maybeVal;
            return ((((guid967).tag == 0) && true) ? 
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
            Prelude::maybe<a> guid968 = maybeVal;
            return ((((guid968).tag == 0) && true) ? 
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
            Prelude::maybe<t> guid969 = maybeVal;
            return ((((guid969).tag == 0) && true) ? 
                (([&]() -> state {
                    auto val = (guid969).just;
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
            Prelude::maybe<a> guid970 = maybeVal;
            return ((((guid970).tag == 0) && true) ? 
                (([&]() -> Prelude::unit {
                    auto val = (guid970).just;
                    return f(val);
                })())
            :
                (true ? 
                    (([&]() -> Prelude::unit {
                        Prelude::maybe<a> nothing = guid970;
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
            auto guid971 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t ret = guid971;
            
            (([&]() -> Prelude::unit {
                ret = millis();
                return {};
            })());
            return ret;
        })());
    }

    struct timerState {
        uint32_t lastPulse;
        bool operator==(timerState rhs) {
            return true && lastPulse == rhs.lastPulse;
        }

        bool operator!=(timerState rhs) {
            return !(rhs == *this);
        }
    };

    juniper::shared_ptr<Time::timerState> state() {
        return (juniper::shared_ptr<Time::timerState>(new Time::timerState((([&]() -> Time::timerState{
            Time::timerState guid972;
            guid972.lastPulse = 0;
            return guid972;
        })()))));
    }

    Prelude::sig<uint32_t> every(uint32_t interval, juniper::shared_ptr<Time::timerState> state) {
        return (([&]() -> Prelude::sig<uint32_t> {
            auto guid973 = now();
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t t = guid973;
            
            auto guid974 = ((interval == 0) ? 
                t
            :
                ((t / interval) * interval));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t lastWindow = guid974;
            
            return ((((*((state).get()))).lastPulse >= lastWindow) ? 
                signal<uint32_t>(nothing<uint32_t>())
            :
                (([&]() -> Prelude::sig<uint32_t> {
                    (*((Time::timerState*) (state.get())) = (([&]() -> Time::timerState{
                        Time::timerState guid975;
                        guid975.lastPulse = t;
                        return guid975;
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
            auto guid976 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid976;
            
            (([&]() -> Prelude::unit {
                ret = acos(x);
                return {};
            })());
            return ret;
        })());
    }

    double asin_(double x) {
        return (([&]() -> double {
            auto guid977 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid977;
            
            (([&]() -> Prelude::unit {
                ret = asin(x);
                return {};
            })());
            return ret;
        })());
    }

    double atan_(double x) {
        return (([&]() -> double {
            auto guid978 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid978;
            
            (([&]() -> Prelude::unit {
                ret = atan(x);
                return {};
            })());
            return ret;
        })());
    }

    double atan2_(double y, double x) {
        return (([&]() -> double {
            auto guid979 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid979;
            
            (([&]() -> Prelude::unit {
                ret = atan2(y, x);
                return {};
            })());
            return ret;
        })());
    }

    double cos_(double x) {
        return (([&]() -> double {
            auto guid980 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid980;
            
            (([&]() -> Prelude::unit {
                ret = cos(x);
                return {};
            })());
            return ret;
        })());
    }

    double cosh_(double x) {
        return (([&]() -> double {
            auto guid981 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid981;
            
            (([&]() -> Prelude::unit {
                ret = cosh(x);
                return {};
            })());
            return ret;
        })());
    }

    double sin_(double x) {
        return (([&]() -> double {
            auto guid982 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid982;
            
            (([&]() -> Prelude::unit {
                ret = sin(x);
                return {};
            })());
            return ret;
        })());
    }

    double sinh_(double x) {
        return (([&]() -> double {
            auto guid983 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid983;
            
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
            auto guid984 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid984;
            
            (([&]() -> Prelude::unit {
                ret = tanh(x);
                return {};
            })());
            return ret;
        })());
    }

    double exp_(double x) {
        return (([&]() -> double {
            auto guid985 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid985;
            
            (([&]() -> Prelude::unit {
                ret = exp(x);
                return {};
            })());
            return ret;
        })());
    }

    Prelude::tuple2<double,int16_t> frexp_(double x) {
        return (([&]() -> Prelude::tuple2<double,int16_t> {
            auto guid986 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid986;
            
            auto guid987 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            int16_t exponent = guid987;
            
            (([&]() -> Prelude::unit {
                ret = frexp(x, &exponent);
                return {};
            })());
            return (Prelude::tuple2<double,int16_t>{ret, exponent});
        })());
    }

    double ldexp_(double x, int16_t exponent) {
        return (([&]() -> double {
            auto guid988 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid988;
            
            (([&]() -> Prelude::unit {
                ret = ldexp(x, exponent);
                return {};
            })());
            return ret;
        })());
    }

    double log_(double x) {
        return (([&]() -> double {
            auto guid989 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid989;
            
            (([&]() -> Prelude::unit {
                ret = log(x);
                return {};
            })());
            return ret;
        })());
    }

    double log10_(double x) {
        return (([&]() -> double {
            auto guid990 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid990;
            
            (([&]() -> Prelude::unit {
                ret = log10(x);
                return {};
            })());
            return ret;
        })());
    }

    Prelude::tuple2<double,double> modf_(double x) {
        return (([&]() -> Prelude::tuple2<double,double> {
            auto guid991 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid991;
            
            auto guid992 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double integer = guid992;
            
            (([&]() -> Prelude::unit {
                ret = modf(x, &integer);
                return {};
            })());
            return (Prelude::tuple2<double,double>{ret, integer});
        })());
    }

    double pow_(double x, double y) {
        return (([&]() -> double {
            auto guid993 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid993;
            
            (([&]() -> Prelude::unit {
                ret = pow(x, y);
                return {};
            })());
            return ret;
        })());
    }

    double sqrt_(double x) {
        return (([&]() -> double {
            auto guid994 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid994;
            
            (([&]() -> Prelude::unit {
                ret = sqrt(x);
                return {};
            })());
            return ret;
        })());
    }

    double ceil_(double x) {
        return (([&]() -> double {
            auto guid995 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid995;
            
            (([&]() -> Prelude::unit {
                ret = ceil(x);
                return {};
            })());
            return ret;
        })());
    }

    double fabs_(double x) {
        return (([&]() -> double {
            auto guid996 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid996;
            
            (([&]() -> Prelude::unit {
                ret = fabs(x);
                return {};
            })());
            return ret;
        })());
    }

    double floor_(double x) {
        return (([&]() -> double {
            auto guid997 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid997;
            
            (([&]() -> Prelude::unit {
                ret = floor(x);
                return {};
            })());
            return ret;
        })());
    }

    double fmod_(double x, double y) {
        return (([&]() -> double {
            auto guid998 = 0.0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double ret = guid998;
            
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
        bool operator==(buttonState rhs) {
            return true && actualState == rhs.actualState && lastState == rhs.lastState && lastDebounceTime == rhs.lastDebounceTime;
        }

        bool operator!=(buttonState rhs) {
            return !(rhs == *this);
        }
    };

    juniper::shared_ptr<Button::buttonState> state() {
        return (juniper::shared_ptr<Button::buttonState>(new Button::buttonState((([&]() -> Button::buttonState{
            Button::buttonState guid999;
            guid999.actualState = Io::low();
            guid999.lastState = Io::low();
            guid999.lastDebounceTime = 0;
            return guid999;
        })()))));
    }

    Prelude::sig<Io::pinState> debounceDelay(Prelude::sig<Io::pinState> incoming, uint16_t delay, juniper::shared_ptr<Button::buttonState> buttonState) {
        return Signal::map<Io::pinState, Io::pinState>(juniper::function<Io::pinState(Io::pinState)>([=](Io::pinState currentState) mutable -> Io::pinState { 
            return (([&]() -> Io::pinState {
                auto guid1000 = (*((buttonState).get()));
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                uint32_t lastDebounceTime = (guid1000).lastDebounceTime;
                Io::pinState lastState = (guid1000).lastState;
                Io::pinState actualState = (guid1000).actualState;
                
                return ((currentState != lastState) ? 
                    (([&]() -> Io::pinState {
                        (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                            Button::buttonState guid1001;
                            guid1001.actualState = actualState;
                            guid1001.lastState = currentState;
                            guid1001.lastDebounceTime = Time::now();
                            return guid1001;
                        })()));
                        return actualState;
                    })())
                :
                    (((currentState != actualState) && ((Time::now() - ((*((buttonState).get()))).lastDebounceTime) > delay)) ? 
                        (([&]() -> Io::pinState {
                            (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                                Button::buttonState guid1002;
                                guid1002.actualState = currentState;
                                guid1002.lastState = currentState;
                                guid1002.lastDebounceTime = lastDebounceTime;
                                return guid1002;
                            })()));
                            return currentState;
                        })())
                    :
                        (([&]() -> Io::pinState {
                            (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                                Button::buttonState guid1003;
                                guid1003.actualState = actualState;
                                guid1003.lastState = currentState;
                                guid1003.lastDebounceTime = lastDebounceTime;
                                return guid1003;
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
        bool operator==(vector rhs) {
            return true && data == rhs.data;
        }

        bool operator!=(vector rhs) {
            return !(rhs == *this);
        }
    };

    uint8_t x = 0;

    uint8_t y = 1;

    uint8_t z = 2;

    template<typename a, int n>
    Vector::vector<a, n> make(juniper::array<a,n> d) {
        return (([&]() -> Vector::vector<a, n>{
            Vector::vector<a, n> guid1004;
            guid1004.data = d;
            return guid1004;
        })());
    }

    template<typename a, int n>
    a get(uint32_t i, Vector::vector<a, n> v) {
        return ((v).data)[i];
    }

    template<typename a, int n>
    Vector::vector<a, n> add(Vector::vector<a, n> v1, Vector::vector<a, n> v2) {
        return (([&]() -> Vector::vector<a, n> {
            auto guid1005 = v1;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Vector::vector<a, n> result = guid1005;
            
            (([&]() -> Prelude::unit {
                uint32_t guid1006 = 0;
                uint32_t guid1007 = (n - 1);
                for (uint32_t i = guid1006; i <= guid1007; i++) {
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
            Vector::vector<a, n> guid1008;
            guid1008.data = (juniper::array<a, n>().fill(0));
            return guid1008;
        })());
    }

    template<typename a, int n>
    Vector::vector<a, n> subtract(Vector::vector<a, n> v1, Vector::vector<a, n> v2) {
        return (([&]() -> Vector::vector<a, n> {
            auto guid1009 = v1;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Vector::vector<a, n> result = guid1009;
            
            (([&]() -> Prelude::unit {
                uint32_t guid1010 = 0;
                uint32_t guid1011 = (n - 1);
                for (uint32_t i = guid1010; i <= guid1011; i++) {
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
            auto guid1012 = v;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Vector::vector<a, n> result = guid1012;
            
            (([&]() -> Prelude::unit {
                uint32_t guid1013 = 0;
                uint32_t guid1014 = (n - 1);
                for (uint32_t i = guid1013; i <= guid1014; i++) {
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
            auto guid1015 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            a sum = guid1015;
            
            (([&]() -> Prelude::unit {
                uint32_t guid1016 = 0;
                uint32_t guid1017 = (n - 1);
                for (uint32_t i = guid1016; i <= guid1017; i++) {
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
            auto guid1018 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            a sum = guid1018;
            
            (([&]() -> Prelude::unit {
                uint32_t guid1019 = 0;
                uint32_t guid1020 = (n - 1);
                for (uint32_t i = guid1019; i <= guid1020; i++) {
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
            auto guid1021 = u;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Vector::vector<a, n> result = guid1021;
            
            (([&]() -> Prelude::unit {
                uint32_t guid1022 = 0;
                uint32_t guid1023 = (n - 1);
                for (uint32_t i = guid1022; i <= guid1023; i++) {
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
            auto guid1024 = magnitude<a, n>(v);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            double mag = guid1024;
            
            return ((mag > 0) ? 
                (([&]() -> Vector::vector<a, n> {
                    auto guid1025 = v;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    Vector::vector<a, n> result = guid1025;
                    
                    (([&]() -> Prelude::unit {
                        uint32_t guid1026 = 0;
                        uint32_t guid1027 = (n - 1);
                        for (uint32_t i = guid1026; i <= guid1027; i++) {
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
            Vector::vector<a, 3> guid1028;
            guid1028.data = (juniper::array<a, 3> { {((((u).data)[1] * ((v).data)[2]) - (((u).data)[2] * ((v).data)[1])), ((((u).data)[2] * ((v).data)[0]) - (((u).data)[0] * ((v).data)[2])), ((((u).data)[0] * ((v).data)[1]) - (((u).data)[1] * ((v).data)[0]))} });
            return guid1028;
        })());
    }

    template<typename z, int n>
    Vector::vector<z, n> project(Vector::vector<z, n> a, Vector::vector<z, n> b) {
        return (([&]() -> Vector::vector<z, n> {
            auto guid1029 = normalize<z, n>(b);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Vector::vector<z, n> bn = guid1029;
            
            return scale<z, n>(dot<z, n>(a, bn), bn);
        })());
    }

    template<typename z, int n>
    Vector::vector<z, n> projectPlane(Vector::vector<z, n> a, Vector::vector<z, n> m) {
        return subtract<z, n>(a, project<z, n>(a, m));
    }

}

#include <FastLED.h>
namespace FastLed {


    using namespace Prelude;






    struct fastLedStrip {
        juniper::shared_ptr<void> ptr;
        bool operator==(fastLedStrip rhs) {
            return true && ptr == rhs.ptr;
        }

        bool operator!=(fastLedStrip rhs) {
            return !(rhs == *this);
        }
    };

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

    FastLed::fastLedStrip make(uint16_t numLeds) {
        return (([&]() -> FastLed::fastLedStrip {
            auto guid1030 = juniper::shared_ptr<void>(NULL);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            juniper::shared_ptr<void> p = guid1030;
            
            (([&]() -> Prelude::unit {
                
    CRGB *leds = new CRGB[numLeds];
    p.set((void *) leds);
    FastLED.addLeds<WS2812, 6, RGB>(leds, numLeds);
    //FastLED.setBrightness(MAX_BRIGHTNESS);
    
                return {};
            })());
            Io::setPinMode(6, Io::output());
            return (([&]() -> FastLed::fastLedStrip{
                FastLed::fastLedStrip guid1031;
                guid1031.ptr = p;
                return guid1031;
            })());
        })());
    }

    Prelude::unit setLedColor(uint16_t n, FastLed::color c, FastLed::fastLedStrip strip) {
        return (([&]() -> Prelude::unit {
            auto guid1032 = (strip).ptr;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            juniper::shared_ptr<void> p = guid1032;
            
            auto guid1033 = (c).r;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t r = guid1033;
            
            auto guid1034 = (c).g;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t g = guid1034;
            
            auto guid1035 = (c).b;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t b = guid1035;
            
            return (([&]() -> Prelude::unit {
                ((CRGB *) p.get())[n] = CRGB(g, r, b);
                return {};
            })());
        })());
    }

    FastLed::color getLedColor(uint16_t n, FastLed::fastLedStrip strip) {
        return (([&]() -> FastLed::color {
            auto guid1036 = (strip).ptr;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            juniper::shared_ptr<void> p = guid1036;
            
            auto guid1037 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t r = guid1037;
            
            auto guid1038 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t g = guid1038;
            
            auto guid1039 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t b = guid1039;
            
            (([&]() -> Prelude::unit {
                
    CRGB c = ((CRGB *) p.get())[n];
    r = c.r;
    g = c.g;
    b = c.b;
    
                return {};
            })());
            return (([&]() -> FastLed::color{
                FastLed::color guid1040;
                guid1040.r = g;
                guid1040.g = r;
                guid1040.b = b;
                return guid1040;
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

namespace Accelerometer {


    using namespace Prelude;




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
        auto guid1041 = 0;
        if (!(true)) {
            juniper::quit<Prelude::unit>();
        }
        uint16_t p = guid1041;
        
        (([&]() -> Prelude::unit {
            p = A0;
            return {};
        })());
        return p;
    })());

    uint16_t yPin = (([&]() -> uint16_t {
        auto guid1042 = 0;
        if (!(true)) {
            juniper::quit<Prelude::unit>();
        }
        uint16_t p = guid1042;
        
        (([&]() -> Prelude::unit {
            p = A1;
            return {};
        })());
        return p;
    })());

    uint16_t zPin = (([&]() -> uint16_t {
        auto guid1043 = 0;
        if (!(true)) {
            juniper::quit<Prelude::unit>();
        }
        uint16_t p = guid1043;
        
        (([&]() -> Prelude::unit {
            p = A2;
            return {};
        })());
        return p;
    })());

    uint16_t axisToPin(Accelerometer::axis a) {
        return (([&]() -> uint16_t {
            Accelerometer::axis guid1044 = a;
            return ((((guid1044).tag == 0) && true) ? 
                (([&]() -> uint16_t {
                    return xPin;
                })())
            :
                ((((guid1044).tag == 1) && true) ? 
                    (([&]() -> uint16_t {
                        return yPin;
                    })())
                :
                    ((((guid1044).tag == 2) && true) ? 
                        (([&]() -> uint16_t {
                            return zPin;
                        })())
                    :
                        juniper::quit<uint16_t>())));
        })());
    }

    Prelude::tuple2<int32_t,int32_t> axisToRange(Accelerometer::axis a) {
        return (([&]() -> Prelude::tuple2<int32_t,int32_t> {
            Accelerometer::axis guid1045 = a;
            return ((((guid1045).tag == 0) && true) ? 
                (([&]() -> Prelude::tuple2<int32_t,int32_t> {
                    return (Prelude::tuple2<int32_t,int32_t>{404, 612});
                })())
            :
                ((((guid1045).tag == 1) && true) ? 
                    (([&]() -> Prelude::tuple2<int32_t,int32_t> {
                        return (Prelude::tuple2<int32_t,int32_t>{409, 622});
                    })())
                :
                    ((((guid1045).tag == 2) && true) ? 
                        (([&]() -> Prelude::tuple2<int32_t,int32_t> {
                            return (Prelude::tuple2<int32_t,int32_t>{418, 622});
                        })())
                    :
                        juniper::quit<Prelude::tuple2<int32_t,int32_t>>())));
        })());
    }

    uint16_t readRaw(Accelerometer::axis a) {
        return (([&]() -> int32_t {
            auto guid1046 = axisToPin(a);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint16_t pin = guid1046;
            
            Io::anaRead(pin);
            Time::wait(1);
            auto guid1047 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t total = guid1047;
            
            (([&]() -> Prelude::unit {
                uint8_t guid1048 = 0;
                uint8_t guid1049 = 3;
                for (uint8_t i = guid1048; i <= guid1049; i++) {
                    (total = (total + Io::anaRead(pin)));
                }
                return {};
            })());
            return (total / 4);
        })());
    }

    int16_t read(Accelerometer::axis a) {
        return (([&]() -> double {
            auto guid1050 = axisToRange(a);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            int32_t max = (guid1050).e2;
            int32_t min = (guid1050).e1;
            
            return Math::mapRange(readRaw(a), min, max, -1000, 1000);
        })());
    }

    Prelude::maybe<Accelerometer::orientation> getOrientation() {
        return (([&]() -> Prelude::maybe<Accelerometer::orientation> {
            auto guid1051 = read(xAxis());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            int16_t xScaled = guid1051;
            
            auto guid1052 = read(yAxis());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            int16_t yScaled = guid1052;
            
            auto guid1053 = read(zAxis());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            int16_t zScaled = guid1053;
            
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

namespace IoExt {


    using namespace Prelude;




    Prelude::sig<Io::pinState> every(uint32_t interval, juniper::shared_ptr<Time::timerState> tState, juniper::shared_ptr<Io::pinState> outState) {
        return (([&]() -> Prelude::sig<Io::pinState> {
            auto guid1054 = Time::every(interval, tState);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::sig<uint32_t> timerSig = guid1054;
            
            return Signal::foldP<uint32_t, Io::pinState>(juniper::function<Io::pinState(uint32_t,Io::pinState)>([=](uint32_t currentTime, Io::pinState lastState) mutable -> Io::pinState { 
                return Io::toggle(lastState);
             }), outState, timerSig);
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
            auto guid1055 = sigA;
            if (!((((guid1055).tag == 0) && true))) {
                juniper::quit<Prelude::unit>();
            }
            auto val = (guid1055).signal;
            
            return constant<Prelude::maybe<a>>(val);
        })());
    }

    template<typename a>
    Prelude::sig<a> unmeta(Prelude::sig<Prelude::maybe<a>> sigA) {
        return (([&]() -> Prelude::sig<a> {
            Prelude::sig<Prelude::maybe<a>> guid1056 = sigA;
            return ((((guid1056).tag == 0) && ((((guid1056).signal).tag == 0) && (((((guid1056).signal).just).tag == 0) && true))) ? 
                (([&]() -> Prelude::sig<a> {
                    auto val = (((guid1056).signal).just).just;
                    return constant<a>(val);
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

    template<typename a, typename b>
    Prelude::sig<Prelude::tuple2<a,b>> zip(Prelude::sig<a> sigA, Prelude::sig<b> sigB, juniper::shared_ptr<Prelude::tuple2<a,b>> state) {
        return Signal::map2<a, b, Prelude::tuple2<a,b>>(juniper::function<Prelude::tuple2<a,b>(a,b)>([=](a valA, b valB) mutable -> Prelude::tuple2<a,b> { 
            return (Prelude::tuple2<a,b>{valA, valB});
         }), sigA, sigB, state);
    }

}

namespace Constants {


    using namespace Prelude;




    FastLed::color blank = (([&]() -> FastLed::color{
        FastLed::color guid1057;
        guid1057.r = 0;
        guid1057.g = 0;
        guid1057.b = 0;
        return guid1057;
    })());

    FastLed::color red = (([&]() -> FastLed::color{
        FastLed::color guid1058;
        guid1058.r = 255;
        guid1058.g = 0;
        guid1058.b = 0;
        return guid1058;
    })());

    FastLed::color green = (([&]() -> FastLed::color{
        FastLed::color guid1059;
        guid1059.r = 0;
        guid1059.g = 255;
        guid1059.b = 0;
        return guid1059;
    })());

    FastLed::color blue = (([&]() -> FastLed::color{
        FastLed::color guid1060;
        guid1060.r = 0;
        guid1060.g = 0;
        guid1060.b = 255;
        return guid1060;
    })());

    FastLed::color white = (([&]() -> FastLed::color{
        FastLed::color guid1061;
        guid1061.r = 255;
        guid1061.g = 255;
        guid1061.b = 255;
        return guid1061;
    })());

    FastLed::color pink = (([&]() -> FastLed::color{
        FastLed::color guid1062;
        guid1062.r = 255;
        guid1062.g = 50;
        guid1062.b = 100;
        return guid1062;
    })());

    uint16_t buttonPin = 4;

    uint16_t numLeds = 33;

    FastLed::fastLedStrip leds = FastLed::make(numLeds);

}

namespace Timing {


    using namespace Prelude;
    using namespace Constants;




    juniper::shared_ptr<int32_t> lastTime = (juniper::shared_ptr<int32_t>(new int32_t(0)));

    Prelude::unit reset() {
        return (([&]() -> Prelude::unit {
            (*((uint32_t*) (lastTime.get())) = Time::now());
            return Prelude::unit();
        })());
    }

    FastLed::color interpolate(FastLed::color c1, FastLed::color c2, float t) {
        return (([&]() -> FastLed::color {
            auto guid1063 = c1;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t b1 = (guid1063).b;
            uint8_t g1 = (guid1063).g;
            uint8_t r1 = (guid1063).r;
            
            auto guid1064 = c2;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint8_t b2 = (guid1064).b;
            uint8_t g2 = (guid1064).g;
            uint8_t r2 = (guid1064).r;
            
            return (([&]() -> FastLed::color{
                FastLed::color guid1065;
                guid1065.r = ((t * r1) + ((1.0 - t) * r2));
                guid1065.g = ((t * g1) + ((1.0 - t) * g2));
                guid1065.b = ((t * b1) + ((1.0 - t) * b2));
                return guid1065;
            })());
        })());
    }

    Prelude::unit execute(juniper::shared_ptr<int32_t> timeRemaining, int32_t totalTime) {
        return (([&]() -> Prelude::unit {
            auto guid1066 = Time::now();
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            uint32_t currentTime = guid1066;
            
            auto guid1067 = (currentTime - (*((lastTime).get())));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            int32_t deltaT = guid1067;
            
            (*((int32_t*) (timeRemaining.get())) = ((*((timeRemaining).get())) - deltaT));
            (*((uint32_t*) (lastTime.get())) = currentTime);
            auto guid1068 = (*((timeRemaining).get()));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            float timeRemainingF = guid1068;
            
            auto guid1069 = totalTime;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            float totalTimeF = guid1069;
            
            auto guid1070 = (timeRemainingF / totalTimeF);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            float tPrime = guid1070;
            
            auto guid1071 = numLeds;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            float numLedsF = guid1071;
            
            (([&]() -> Prelude::unit {
                int32_t guid1072 = 0;
                int32_t guid1073 = (numLeds - 1);
                for (int32_t i = guid1072; i <= guid1073; i++) {
                    (([&]() -> Prelude::unit {
                        auto guid1074 = Math::min_((((i + 1) * numLeds) - (((1 - tPrime) * numLeds) * numLeds)), i);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        double pos = guid1074;
                        
                        return (((0.0 <= pos) && (numLeds > pos)) ? 
                            FastLed::setLedColor(pos, interpolate(red, green, (i / numLedsF)), leds)
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


    using namespace Prelude;
    using namespace Constants;




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

    juniper::shared_ptr<Button::buttonState> bState = Button::state();

    juniper::shared_ptr<Io::pinState> bEdgeState = (juniper::shared_ptr<Io::pinState>(new Io::pinState(Io::low())));

    juniper::shared_ptr<Setting::timeSetting> numLedsLit = (juniper::shared_ptr<Setting::timeSetting>(new Setting::timeSetting((([&]() -> Setting::timeSetting{
        Setting::timeSetting guid1075;
        guid1075.minutes = 0;
        guid1075.fifteenSeconds = 0;
        return guid1075;
    })()))));

    juniper::shared_ptr<Time::timerState> tState = Time::state();

    juniper::shared_ptr<Io::pinState> cursorState = (juniper::shared_ptr<Io::pinState>(new Io::pinState(Io::low())));

    juniper::shared_ptr<Prelude::tuple2<Io::pinState,Setting::timeSetting>> outputUpdateState = (juniper::shared_ptr<Prelude::tuple2<Io::pinState,Setting::timeSetting>>(new Prelude::tuple2<Io::pinState,Setting::timeSetting>((Prelude::tuple2<Io::pinState,Setting::timeSetting>{(*((cursorState).get())), (*((numLedsLit).get()))}))));

    juniper::shared_ptr<Prelude::tuple2<Io::pinState,Setting::timeSetting>> outputState = (juniper::shared_ptr<Prelude::tuple2<Io::pinState,Setting::timeSetting>>(new Prelude::tuple2<Io::pinState,Setting::timeSetting>((*((outputUpdateState).get())))));

    Prelude::unit reset(juniper::shared_ptr<int32_t> timeRemaining) {
        return (([&]() -> Prelude::unit {
            (*((Setting::timeSetting*) (numLedsLit.get())) = (([&]() -> Setting::timeSetting{
                Setting::timeSetting guid1076;
                guid1076.minutes = 0;
                guid1076.fifteenSeconds = 0;
                return guid1076;
            })()));
            (*((Io::pinState*) (cursorState.get())) = Io::low());
            (*((Prelude::tuple2<Io::pinState,Setting::timeSetting>*) (outputUpdateState.get())) = (Prelude::tuple2<Io::pinState,Setting::timeSetting>{(*((cursorState).get())), (*((numLedsLit).get()))}));
            (*((Prelude::tuple2<Io::pinState,Setting::timeSetting>*) (outputState.get())) = (*((outputUpdateState).get())));
            (*((int32_t*) (timeRemaining.get())) = 0);
            return Prelude::unit();
        })());
    }

    Prelude::unit execute(juniper::shared_ptr<int32_t> timeRemaining) {
        return (([&]() -> Prelude::unit {
            auto guid1077 = IoExt::every(500, tState, cursorState);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::sig<Io::pinState> cursorSig = guid1077;
            
            auto guid1078 = Io::risingEdge(Button::debounce(Io::digIn(buttonPin), bState), bEdgeState);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::sig<Prelude::unit> buttonSig = guid1078;
            
            auto guid1079 = Signal::foldP<Prelude::unit, Setting::timeSetting>(juniper::function<Setting::timeSetting(Prelude::unit,Setting::timeSetting)>([=](Prelude::unit u, Setting::timeSetting prevSetting) mutable -> Setting::timeSetting { 
                return (([&]() -> Setting::timeSetting {
                    auto guid1080 = prevSetting;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    int32_t prevFifteenSeconds = (guid1080).fifteenSeconds;
                    int32_t prevMinutes = (guid1080).minutes;
                    
                    return ((((prevMinutes + prevFifteenSeconds) + 1) >= numLeds) ? 
                        prevSetting
                    :
                        ((((prevFifteenSeconds + 1) % 4) == 0) ? 
                            (([&]() -> Setting::timeSetting{
                                Setting::timeSetting guid1081;
                                guid1081.minutes = (prevMinutes + 1);
                                guid1081.fifteenSeconds = 0;
                                return guid1081;
                            })())
                        :
                            (([&]() -> Setting::timeSetting{
                                Setting::timeSetting guid1082;
                                guid1082.minutes = prevMinutes;
                                guid1082.fifteenSeconds = (prevFifteenSeconds + 1);
                                return guid1082;
                            })())));
                })());
             }), numLedsLit, buttonSig);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::sig<Setting::timeSetting> numLedsLitUpdateSig = guid1079;
            
            auto guid1083 = SignalExt::zip<Io::pinState, Setting::timeSetting>(cursorSig, numLedsLitUpdateSig, outputUpdateState);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::sig<Prelude::tuple2<Io::pinState,Setting::timeSetting>> outputUpdateSig = guid1083;
            
            auto guid1084 = Signal::latch<Prelude::tuple2<Io::pinState,Setting::timeSetting>>(outputUpdateSig, outputState);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            Prelude::sig<Prelude::tuple2<Io::pinState,Setting::timeSetting>> outputSig = guid1084;
            
            return Signal::sink<Prelude::tuple2<Io::pinState,Setting::timeSetting>>(juniper::function<Prelude::unit(Prelude::tuple2<Io::pinState,Setting::timeSetting>)>([=](Prelude::tuple2<Io::pinState,Setting::timeSetting> out) mutable -> Prelude::unit { 
                return (([&]() -> Prelude::unit {
                    auto guid1085 = out;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    int32_t numFifteenSeconds = ((guid1085).e2).fifteenSeconds;
                    int32_t numMinutes = ((guid1085).e2).minutes;
                    Io::pinState cursor = (guid1085).e1;
                    
                    (*((int32_t*) (timeRemaining.get())) = ((numMinutes * 60000) + (numFifteenSeconds * 15000)));
                    (([&]() -> Prelude::unit {
                        int32_t guid1086 = 0;
                        int32_t guid1087 = (numMinutes - 1);
                        for (int32_t i = guid1086; i <= guid1087; i++) {
                            FastLed::setLedColor(((numLeds - i) - 1), blue, leds);
                        }
                        return {};
                    })());
                    (([&]() -> Prelude::unit {
                        int32_t guid1088 = 0;
                        int32_t guid1089 = (numFifteenSeconds - 1);
                        for (int32_t i = guid1088; i <= guid1089; i++) {
                            FastLed::setLedColor(((numLeds - (numMinutes + i)) - 1), pink, leds);
                        }
                        return {};
                    })());
                    return (([&]() -> Prelude::unit {
                        Io::pinState guid1090 = cursor;
                        return ((((guid1090).tag == 0) && true) ? 
                            (([&]() -> Prelude::unit {
                                return FastLed::setLedColor(((numLeds - (numMinutes + numFifteenSeconds)) - 1), white, leds);
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


    using namespace Prelude;
    using namespace Constants;




    Prelude::unit execute(juniper::shared_ptr<int32_t> timeRemaining, int32_t totalTime) {
        return (([&]() -> Prelude::unit {
            auto guid1091 = (*((timeRemaining).get()));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            int32_t t = guid1091;
            
            Timing::execute(timeRemaining, totalTime);
            (*((int32_t*) (timeRemaining.get())) = t);
            auto guid1092 = (0.5 * (Math::sin_((((2.0 * Math::pi) * Time::now()) / 1000.0)) + 1.0));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            float multiplier = guid1092;
            
            return (([&]() -> Prelude::unit {
                int32_t guid1093 = 0;
                int32_t guid1094 = (numLeds - 1);
                for (int32_t i = guid1093; i <= guid1094; i++) {
                    (([&]() -> Prelude::unit {
                        auto guid1095 = FastLed::getLedColor(i, leds);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        uint8_t b = (guid1095).b;
                        uint8_t g = (guid1095).g;
                        uint8_t r = (guid1095).r;
                        
                        auto guid1096 = (([&]() -> FastLed::color{
                            FastLed::color guid1097;
                            guid1097.r = (r * multiplier);
                            guid1097.g = (g * multiplier);
                            guid1097.b = (b * multiplier);
                            return guid1097;
                        })());
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        FastLed::color newColor = guid1096;
                        
                        return FastLed::setLedColor(i, newColor, leds);
                    })());
                }
                return {};
            })());
        })());
    }

}

namespace Finale {


    using namespace Prelude;
    using namespace Constants;




    float numLedsF = numLeds;

    Prelude::unit execute() {
        return (([&]() -> Prelude::unit {
            int32_t guid1098 = 0;
            int32_t guid1099 = (numLeds - 1);
            for (int32_t i = guid1098; i <= guid1099; i++) {
                (([&]() -> Prelude::unit {
                    auto guid1100 = Time::now();
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    uint32_t t = guid1100;
                    
                    auto guid1101 = (((i / numLedsF) * 1000) + t);
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    float x = guid1101;
                    
                    auto guid1102 = (([&]() -> FastLed::color{
                        FastLed::color guid1103;
                        guid1103.r = (50 * Math::sin_((((2.0 * Math::pi) * x) / 1000.0)));
                        guid1103.g = (50 * Math::cos_((((2.0 * Math::pi) * x) / 1000.0)));
                        guid1103.b = (50 * Math::sin_((((2.0 * Math::pi) * (x + (Math::pi / 2.0))) / 1000.0)));
                        return guid1103;
                    })());
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    FastLed::color color = guid1102;
                    
                    return FastLed::setLedColor(i, color, leds);
                })());
            }
            return {};
        })());
    }

}

namespace Program {


    using namespace Prelude;
    using namespace Constants;


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

    flip flipUp() {
        return (([&]() -> flip { flip ret; ret.tag = 0; ret.flipUp = 0; return ret; })());
    }

    flip flipDown() {
        return (([&]() -> flip { flip ret; ret.tag = 1; ret.flipDown = 0; return ret; })());
    }

    flip flipFlat() {
        return (([&]() -> flip { flip ret; ret.tag = 2; ret.flipFlat = 0; return ret; })());
    }



    juniper::shared_ptr<Prelude::maybe<Accelerometer::orientation>> accState = (juniper::shared_ptr<Prelude::maybe<Accelerometer::orientation>>(new Prelude::maybe<Accelerometer::orientation>(nothing<Accelerometer::orientation>())));

    juniper::shared_ptr<Program::mode> modeState = (juniper::shared_ptr<Program::mode>(new Program::mode(setting())));

    juniper::shared_ptr<int32_t> timeRemaining = (juniper::shared_ptr<int32_t>(new int32_t(0)));

    juniper::shared_ptr<int32_t> totalTime = (juniper::shared_ptr<int32_t>(new int32_t(0)));

    Prelude::unit setup() {
        return Time::wait(500);
    }

    Prelude::unit clearDisplay() {
        return (([&]() -> Prelude::unit {
            uint16_t guid1104 = 0;
            uint16_t guid1105 = (numLeds - 1);
            for (uint16_t i = guid1104; i <= guid1105; i++) {
                FastLed::setLedColor(i, blank, leds);
            }
            return {};
        })());
    }

    Prelude::unit main() {
        return (([&]() -> Prelude::unit {
            setup();
            return (([&]() -> Prelude::unit {
                while (true) {
                    (([&]() -> Prelude::unit {
                        clearDisplay();
                        auto guid1106 = Signal::dropRepeats<Accelerometer::orientation>(Accelerometer::getSignal(), accState);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        Prelude::sig<Accelerometer::orientation> accSig = guid1106;
                        
                        auto guid1107 = Signal::map<Accelerometer::orientation, Program::flip>(juniper::function<Program::flip(Accelerometer::orientation)>([=](Accelerometer::orientation o) mutable -> Program::flip { 
                            return (([&]() -> Program::flip {
                                Accelerometer::orientation guid1108 = o;
                                return ((((guid1108).tag == 0) && true) ? 
                                    (([&]() -> Program::flip {
                                        return flipUp();
                                    })())
                                :
                                    ((((guid1108).tag == 1) && true) ? 
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
                        Prelude::sig<Program::flip> flipSig = guid1107;
                        
                        auto guid1109 = SignalExt::meta<Program::flip>(flipSig);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        Prelude::sig<Prelude::maybe<Program::flip>> metaFlipSig = guid1109;
                        
                        auto guid1110 = Signal::foldP<Prelude::maybe<Program::flip>, Program::mode>(juniper::function<Program::mode(Prelude::maybe<Program::flip>,Program::mode)>([=](Prelude::maybe<Program::flip> f1, Program::mode prevMode) mutable -> Program::mode { 
                            return (((prevMode == timing()) && ((*((timeRemaining).get())) <= 0)) ? 
                                finale()
                            :
                                (([&]() -> Program::mode {
                                    Prelude::maybe<Program::flip> guid1111 = f1;
                                    return ((((guid1111).tag == 0) && true) ? 
                                        (([&]() -> Program::mode {
                                            auto f2 = (guid1111).just;
                                            return (([&]() -> Program::mode {
                                                Prelude::tuple2<Program::flip,Program::mode> guid1112 = (Prelude::tuple2<Program::flip,Program::mode>{f2, prevMode});
                                                return (((((guid1112).e2).tag == 0) && ((((guid1112).e1).tag == 0) && true)) ? 
                                                    (([&]() -> Program::mode {
                                                        return (([&]() -> Program::mode {
                                                            (*((int32_t*) (totalTime.get())) = (*((timeRemaining).get())));
                                                            Timing::reset();
                                                            return timing();
                                                        })());
                                                    })())
                                                :
                                                    (((((guid1112).e2).tag == 2) && ((((guid1112).e1).tag == 0) && true)) ? 
                                                        (([&]() -> Program::mode {
                                                            return timing();
                                                        })())
                                                    :
                                                        (((((guid1112).e2).tag == 1) && ((((guid1112).e1).tag == 1) && true)) ? 
                                                            (([&]() -> Program::mode {
                                                                return (([&]() -> Program::mode {
                                                                    Setting::reset(timeRemaining);
                                                                    return setting();
                                                                })());
                                                            })())
                                                        :
                                                            (((((guid1112).e2).tag == 2) && ((((guid1112).e1).tag == 1) && true)) ? 
                                                                (([&]() -> Program::mode {
                                                                    return (([&]() -> Program::mode {
                                                                        Setting::reset(timeRemaining);
                                                                        return setting();
                                                                    })());
                                                                })())
                                                            :
                                                                (((((guid1112).e2).tag == 1) && ((((guid1112).e1).tag == 2) && true)) ? 
                                                                    (([&]() -> Program::mode {
                                                                        return paused();
                                                                    })())
                                                                :
                                                                    (((((guid1112).e2).tag == 3) && ((((guid1112).e1).tag == 1) && true)) ? 
                                                                        (([&]() -> Program::mode {
                                                                            return (([&]() -> Program::mode {
                                                                                Setting::reset(timeRemaining);
                                                                                return setting();
                                                                            })());
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
                        Prelude::sig<Program::mode> modeSig = guid1110;
                        
                        Signal::sink<Program::mode>(juniper::function<Prelude::unit(Program::mode)>([=](Program::mode m) mutable -> Prelude::unit { 
                            return (([&]() -> Prelude::unit {
                                Program::mode guid1113 = m;
                                return ((((guid1113).tag == 0) && true) ? 
                                    (([&]() -> Prelude::unit {
                                        return Setting::execute(timeRemaining);
                                    })())
                                :
                                    ((((guid1113).tag == 1) && true) ? 
                                        (([&]() -> Prelude::unit {
                                            return Timing::execute(timeRemaining, (*((totalTime).get())));
                                        })())
                                    :
                                        ((((guid1113).tag == 2) && true) ? 
                                            (([&]() -> Prelude::unit {
                                                return Paused::execute(timeRemaining, (*((totalTime).get())));
                                            })())
                                        :
                                            ((((guid1113).tag == 3) && true) ? 
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