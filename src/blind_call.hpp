#ifndef __BLIND_CALL_HPP__
#define __BLIND_CALL_HPP__

#include <inttypes.h>
#include <stddef.h>

/**
 * @brief Auxilary class for creating default BlindCall object
 */
struct StubCall
{
    void doNothing(void*) {};
};
extern StubCall stubCall;

/**
 * @brief Class like std::function but without dynamic memory allocation for internal data
 *
 * Class goals are next:
 * - avoid creation separate out of class functions which work as callbacks
 * to call class member functions
 * - avoid dynamic memory allocation and be sure that call instance is fully allocated on stack
 * - be target function signature independent
 * - to be able store such "callback holders" in array
 * - to not know callback signature and class name at the moment of invoke callback
 * Support non void return value of callback is also possible but looks not so elegant 
 * (like bc.invoke<int>() instead of bc.()) due to ...:
 * - ... inability by C compiler (at least C++11) to automatically detect return value of template function
 * - ... inability to overload two functions with different return values
 * So that feature has been skiped.
 *
 * @example blind_call.t.cpp
 */
class BlindCall
{
protected:
    /// @brief Interface class to allow not know target function signature at call moment.
    /// Derived class will receive that info at creation and remember up to call.
    template<typename... Arg>
    class Invoker
    {
    public:
        virtual void invoke(Arg...) = 0;

    protected:
        Invoker() = default;
    };

    /// @brief Class which has full info how call target function - class member
    template<typename Class, typename... Args>
    class ClassMemberInvoker : public Invoker<Args...>
    {
    public:
        /// @param[in] cls - pointer to class instance
        /// @param[in] method - pointer to class member function which will be called via BlindCall
        ClassMemberInvoker(Class *cls, void (Class::*method)(Args... arg))
            : _this(cls)
            , method_(method)
        {}

        virtual void invoke(Args... arg) override
        {
            (_this->*method_)(arg...);
        }

        static void *operator new (size_t size, void* ptr)
        {
            (void)(size);
            return ptr;
        }

    protected:
        Class *_this;
        void (Class::*method_)(Args...arg);

        friend class BlindCall;
    };

public:
    /// @brief Could not remove ctor from "public" section to force factory method usage 
    /// because array of blind call objects will be uninitialized
    BlindCall()
    {
        new (reinterpret_cast<ClassMemberInvoker<StubCall, void*>*>(data_)) ClassMemberInvoker<StubCall, void*>(&stubCall, &StubCall::doNothing);
    }

    /// factory method
    template<typename T, typename... Args>
    static BlindCall make(T *cls, void (T::*m)(Args...))
    {
        BlindCall bc;
        // we don't allocate memory here actually but call constuctor for pre-allocated memory block
        new (reinterpret_cast<ClassMemberInvoker<T, Args...>*>(bc.data_)) ClassMemberInvoker<T, Args...>(cls, m);
        return bc;
    }

    static BlindCall stub()
    {
        return make(&stubCall, &StubCall::doNothing);
    }

    template<typename... Args>
    void operator()(Args... arg)
    {
        reinterpret_cast<Invoker<Args...>*>(data_)->invoke(arg...);
    }

    /// @return true BlindCall is not stub
    operator bool()
    {
        return reinterpret_cast<ClassMemberInvoker<StubCall, void*>*>(data_)->_this != &stubCall;
    }

    BlindCall(const BlindCall&) = default;
    BlindCall& operator=(const BlindCall&) = default;

protected:
    uint8_t data_[32] {0}; /// contains ClassMemberInvoker<T> instance. If array size is not enough you will receive compiler error with required one
};

#endif // !__BLIND_CALL_HPP__
