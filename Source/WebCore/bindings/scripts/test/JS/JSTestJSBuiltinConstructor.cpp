/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "JSTestJSBuiltinConstructor.h"

#include "JSDOMBinding.h"
#include "JSDOMConstructor.h"
#include "TestJSBuiltinConstructorBuiltins.h"
#include <runtime/Error.h>
#include <runtime/FunctionPrototype.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

// Functions

JSC::EncodedJSValue JSC_HOST_CALL jsTestJSBuiltinConstructorPrototypeFunctionTestCustomFunction(JSC::ExecState*);

// Attributes

JSC::EncodedJSValue jsTestJSBuiltinConstructorTestAttributeCustom(JSC::ExecState*, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsTestJSBuiltinConstructorTestAttributeRWCustom(JSC::ExecState*, JSC::EncodedJSValue, JSC::PropertyName);
bool setJSTestJSBuiltinConstructorTestAttributeRWCustom(JSC::ExecState*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsTestJSBuiltinConstructorConstructor(JSC::ExecState*, JSC::EncodedJSValue, JSC::PropertyName);
bool setJSTestJSBuiltinConstructorConstructor(JSC::ExecState*, JSC::EncodedJSValue, JSC::EncodedJSValue);

class JSTestJSBuiltinConstructorPrototype : public JSC::JSNonFinalObject {
public:
    using Base = JSC::JSNonFinalObject;
    static JSTestJSBuiltinConstructorPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSTestJSBuiltinConstructorPrototype* ptr = new (NotNull, JSC::allocateCell<JSTestJSBuiltinConstructorPrototype>(vm.heap)) JSTestJSBuiltinConstructorPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSTestJSBuiltinConstructorPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

using JSTestJSBuiltinConstructorConstructor = JSBuiltinConstructor<JSTestJSBuiltinConstructor>;

template<> JSValue JSTestJSBuiltinConstructorConstructor::prototypeForStructure(JSC::VM& vm, const JSDOMGlobalObject& globalObject)
{
    UNUSED_PARAM(vm);
    return globalObject.functionPrototype();
}

template<> void JSTestJSBuiltinConstructorConstructor::initializeProperties(VM& vm, JSDOMGlobalObject& globalObject)
{
    putDirect(vm, vm.propertyNames->prototype, JSTestJSBuiltinConstructor::prototype(vm, &globalObject), DontDelete | ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("TestJSBuiltinConstructor"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontEnum);
}

template<> FunctionExecutable* JSTestJSBuiltinConstructorConstructor::initializeExecutable(VM& vm)
{
    return testJSBuiltinConstructorInitializeTestJSBuiltinConstructorCodeGenerator(vm);
}

template<> const ClassInfo JSTestJSBuiltinConstructorConstructor::s_info = { "TestJSBuiltinConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSTestJSBuiltinConstructorConstructor) };

/* Hash table for prototype */

static const HashTableValue JSTestJSBuiltinConstructorPrototypeTableValues[] =
{
    { "constructor", DontEnum, NoIntrinsic, { (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestJSBuiltinConstructorConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSTestJSBuiltinConstructorConstructor) } },
    { "testAttribute", Accessor | Builtin, NoIntrinsic, { (intptr_t)static_cast<BuiltinGenerator>(testJSBuiltinConstructorTestAttributeCodeGenerator), (intptr_t) (setTestJSBuiltinConstructorTestAttributeCodeGenerator) } },
    { "testAttributeCustom", ReadOnly | CustomAccessor, NoIntrinsic, { (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestJSBuiltinConstructorTestAttributeCustom), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) } },
    { "testAttributeRWCustom", CustomAccessor, NoIntrinsic, { (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestJSBuiltinConstructorTestAttributeRWCustom), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSTestJSBuiltinConstructorTestAttributeRWCustom) } },
    { "testFunction", JSC::Builtin, NoIntrinsic, { (intptr_t)static_cast<BuiltinGenerator>(testJSBuiltinConstructorTestFunctionCodeGenerator), (intptr_t) (0) } },
    { "testCustomFunction", JSC::Function, NoIntrinsic, { (intptr_t)static_cast<NativeFunction>(jsTestJSBuiltinConstructorPrototypeFunctionTestCustomFunction), (intptr_t) (0) } },
};

const ClassInfo JSTestJSBuiltinConstructorPrototype::s_info = { "TestJSBuiltinConstructorPrototype", &Base::s_info, 0, CREATE_METHOD_TABLE(JSTestJSBuiltinConstructorPrototype) };

void JSTestJSBuiltinConstructorPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSTestJSBuiltinConstructorPrototypeTableValues, *this);
}

const ClassInfo JSTestJSBuiltinConstructor::s_info = { "TestJSBuiltinConstructor", &Base::s_info, 0, CREATE_METHOD_TABLE(JSTestJSBuiltinConstructor) };

JSTestJSBuiltinConstructor::JSTestJSBuiltinConstructor(Structure* structure, JSDOMGlobalObject& globalObject)
    : JSDOMObject(structure, globalObject) { }

JSObject* JSTestJSBuiltinConstructor::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSTestJSBuiltinConstructorPrototype::create(vm, globalObject, JSTestJSBuiltinConstructorPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

JSObject* JSTestJSBuiltinConstructor::prototype(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSTestJSBuiltinConstructor>(vm, globalObject);
}

void JSTestJSBuiltinConstructor::destroy(JSC::JSCell* cell)
{
    JSTestJSBuiltinConstructor* thisObject = static_cast<JSTestJSBuiltinConstructor*>(cell);
    thisObject->JSTestJSBuiltinConstructor::~JSTestJSBuiltinConstructor();
}

inline JSTestJSBuiltinConstructor* JSTestJSBuiltinConstructor::castForAttribute(JSC::ExecState*, EncodedJSValue thisValue)
{
    return jsDynamicCast<JSTestJSBuiltinConstructor*>(JSValue::decode(thisValue));
}

static inline JSValue jsTestJSBuiltinConstructorTestAttributeCustomGetter(ExecState*, JSTestJSBuiltinConstructor*, ThrowScope& throwScope);

EncodedJSValue jsTestJSBuiltinConstructorTestAttributeCustom(ExecState* state, EncodedJSValue thisValue, PropertyName)
{
    return BindingCaller<JSTestJSBuiltinConstructor>::attribute<jsTestJSBuiltinConstructorTestAttributeCustomGetter>(state, thisValue, "testAttributeCustom");
}

static inline JSValue jsTestJSBuiltinConstructorTestAttributeCustomGetter(ExecState* state, JSTestJSBuiltinConstructor* thisObject, ThrowScope& throwScope)
{
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(state);
    return thisObject->testAttributeCustom(*state);
}

static inline JSValue jsTestJSBuiltinConstructorTestAttributeRWCustomGetter(ExecState*, JSTestJSBuiltinConstructor*, ThrowScope& throwScope);

EncodedJSValue jsTestJSBuiltinConstructorTestAttributeRWCustom(ExecState* state, EncodedJSValue thisValue, PropertyName)
{
    return BindingCaller<JSTestJSBuiltinConstructor>::attribute<jsTestJSBuiltinConstructorTestAttributeRWCustomGetter>(state, thisValue, "testAttributeRWCustom");
}

static inline JSValue jsTestJSBuiltinConstructorTestAttributeRWCustomGetter(ExecState* state, JSTestJSBuiltinConstructor* thisObject, ThrowScope& throwScope)
{
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(state);
    return thisObject->testAttributeRWCustom(*state);
}

EncodedJSValue jsTestJSBuiltinConstructorConstructor(ExecState* state, EncodedJSValue thisValue, PropertyName)
{
    VM& vm = state->vm();
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    JSTestJSBuiltinConstructorPrototype* domObject = jsDynamicCast<JSTestJSBuiltinConstructorPrototype*>(JSValue::decode(thisValue));
    if (UNLIKELY(!domObject))
        return throwVMTypeError(state, throwScope);
    return JSValue::encode(JSTestJSBuiltinConstructor::getConstructor(state->vm(), domObject->globalObject()));
}

bool setJSTestJSBuiltinConstructorConstructor(ExecState* state, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    VM& vm = state->vm();
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    JSValue value = JSValue::decode(encodedValue);
    JSTestJSBuiltinConstructorPrototype* domObject = jsDynamicCast<JSTestJSBuiltinConstructorPrototype*>(JSValue::decode(thisValue));
    if (UNLIKELY(!domObject)) {
        throwVMTypeError(state, throwScope);
        return false;
    }
    // Shadowing a built-in constructor
    return domObject->putDirect(state->vm(), state->propertyNames().constructor, value);
}

bool setJSTestJSBuiltinConstructorTestAttributeRWCustom(ExecState* state, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    VM& vm = state->vm();
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    JSValue value = JSValue::decode(encodedValue);
    UNUSED_PARAM(thisValue);
    JSTestJSBuiltinConstructor* castedThis = jsDynamicCast<JSTestJSBuiltinConstructor*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis))
        return throwSetterTypeError(*state, throwScope, "TestJSBuiltinConstructor", "testAttributeRWCustom");
    castedThis->setTestAttributeRWCustom(*state, value);
    return true;
}


JSValue JSTestJSBuiltinConstructor::getConstructor(VM& vm, const JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestJSBuiltinConstructorConstructor>(vm, *jsCast<const JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsTestJSBuiltinConstructorPrototypeFunctionTestCustomFunction(ExecState* state)
{
    VM& vm = state->vm();
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    JSValue thisValue = state->thisValue();
    auto castedThis = jsDynamicCast<JSTestJSBuiltinConstructor*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*state, throwScope, "TestJSBuiltinConstructor", "testCustomFunction");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTestJSBuiltinConstructor::info());
    return JSValue::encode(castedThis->testCustomFunction(*state));
}

void JSTestJSBuiltinConstructor::visitChildren(JSCell* cell, SlotVisitor& visitor)
{
    auto* thisObject = jsCast<JSTestJSBuiltinConstructor*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    Base::visitChildren(thisObject, visitor);
}


}
