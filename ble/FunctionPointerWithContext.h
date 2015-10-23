/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MBED_FUNCTIONPOINTER_WITH_CONTEXT_H
#define MBED_FUNCTIONPOINTER_WITH_CONTEXT_H

#include <string.h>



/** A class for storing and calling a pointer to a static or member void function
 *  which takes a context.
 */
template <typename ContextType>
class FunctionPointerWithContext {
public:
    typedef FunctionPointerWithContext<ContextType> *pFunctionPointerWithContext_t;
    typedef void (*pvoidfcontext_t)(ContextType context);

    /** Create a FunctionPointerWithContext, attaching a static function
     *
     *  @param function The void static function to attach (default is none)
     */
    FunctionPointerWithContext(void (*function)(ContextType context) = NULL) :
        _function(NULL), _caller(NULL), _next(NULL) {
        attach(function);
    }

    /** Create a FunctionPointerWithContext, attaching a member function
     *
     *  @param object The object pointer to invoke the member function on (i.e. the this pointer)
     *  @param function The address of the void member function to attach
     */
    template<typename T>
    FunctionPointerWithContext(T *object, void (T::*member)(ContextType context)) :
        _memberFunctionAndPointer(), _caller(NULL), _next(NULL) {
        attach(object, member);
    }

    /** Attach a static function
     *
     *  @param function The void static function to attach (default is none)
     */
    void attach(void (*function)(ContextType context) = NULL) {
        _function = function;
        _caller = functioncaller;
    }

    /** Attach a member function
     *
     *  @param object The object pointer to invoke the member function on (i.e. the this pointer)
     *  @param function The address of the void member function to attach
     */
    template<typename T>
    void attach(T *object, void (T::*member)(ContextType context)) {
        _memberFunctionAndPointer._object = static_cast<void *>(object);
        memcpy(_memberFunctionAndPointer._memberFunction, (char*) &member, sizeof(member));
        _caller = &FunctionPointerWithContext::membercaller<T>;
    }

    /** Call the attached static or member function; and if there are chained
     *  FunctionPointers their callbacks are invoked as well.
     *  @Note: all chained callbacks stack up; so hopefully there won't be too
     *  many FunctionPointers in a chain. */
    void call(ContextType context) {
        _caller(this, context);

        /* Propagate the call to next in the chain. */
        if (_next) {
            _next->call(context);
        }
    }

    /**
     * Setup an external FunctionPointer as a next in the chain of related
     * callbacks. Invoking call() on the head FunctionPointer will invoke all
     * chained callbacks.
     *
     * Refer to 'CallChain' as an alternative.
     */
    void chainAsNext(pFunctionPointerWithContext_t next) {
        _next = next;
    }

    pFunctionPointerWithContext_t getNext(void) const {
        return _next;
    }

    pvoidfcontext_t get_function() const {
        return (pvoidfcontext_t)_function;
    }

private:
    template<typename T>
    static void membercaller(FunctionPointerWithContext* self, ContextType context) {
        if(self->_memberFunctionAndPointer._object) {
            T *o = static_cast<T *>(self->_memberFunctionAndPointer._object);        
            void (T::*m)(ContextType);
            memcpy((char*) &m, self->_memberFunctionAndPointer._memberFunction, sizeof(m));
            (o->*m)(context);            
        }
    }

    static void functioncaller(FunctionPointerWithContext* self, ContextType context) {
        if(self->_function) {
            self->_function(context);
        }
    }

    struct MemberFunctionAndPtr {
        /*
         * forward declaration of a class and a member function to this class. 
         * Because the compiler doesnt know anything about the member function, it 
         * will always take the biggest size that a member function can take.
         * This also guarantee that the proper alignement will be chosen 
         */
        class UndefinedClass;
        typedef void (UndefinedClass::*UndefinedMemberFunction)(ContextType);

        void* _object;
        union {
            char _memberFunction[sizeof(UndefinedMemberFunction)];
            UndefinedMemberFunction _allignement;            
        };
    };

    union {
        void (*_function)(ContextType context);             /**< static function pointer - NULL if none attached */        
        /** 
         * object this pointer and pointer to member - 
         * _memberFunctionAndPointer._object will be NULL if none attached 
         */        
        MemberFunctionAndPtr _memberFunctionAndPointer;      
    };

    void (*_caller)(FunctionPointerWithContext*, ContextType);

    pFunctionPointerWithContext_t _next;                /**< Optional link to make a chain out of functionPointers; this
                                                         *   allows chaining function pointers without requiring
                                                         *   external memory to manage the chain. Also refer to
                                                         *   'CallChain' as an alternative. */
};

#endif // ifndef MBED_FUNCTIONPOINTER_WITH_CONTEXT_H
