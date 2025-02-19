// Copyright (C) 2016 the V8 project authors. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

// This test requires ENABLE_ES2017_ASYNCFUNCTION_SYNTAX to be enabled at build time.
//@ skip

function testSyntax(script) {
    try {
        eval(script);
    } catch (error) {
        if (error instanceof SyntaxError)
            throw new Error("Bad error: " + String(error) + "\n       evaluating `" + script + "`");
    }
}

function testSyntaxError(script, message) {
    var error = null;
    try {
        eval(script);
    } catch (e) {
        error = e;
    }
    if (!error)
        throw new Error("Expected syntax error not thrown\n       evaluating `" + script + "`");

    if (typeof message === "string" && String(error) !== message)
        throw new Error("Bad error: " + String(error) + "\n       evaluating `" + script + "`");
}

(function testTopLevelAsyncAwaitSyntaxSloppyMode() {
    testSyntax(`var asyncFn = async function() { await 1; };`);
    testSyntax(`var asyncFn = async function withName() { await 1; };`);
    testSyntax(`var asyncFn = async () => await 'test';`);
    testSyntax(`var asyncFn = async x => await x + 'test';`);
    testSyntax(`async function asyncFn() { await 1; }`);
    testSyntax(`var O = { async method() { await 1; } };`);
    testSyntax(`var O = { async ['meth' + 'od']() { await 1; } };`);
    testSyntax(`var O = { async 'method'() { await 1; } };`);
    testSyntax(`var O = { async 0() { await 1; } };`);
    testSyntax(`class C { async method() { await 1; } };`);
    testSyntax(`class C { async ['meth' + 'od']() { await 1; } };`);
    testSyntax(`class C { async 'method'() { await 1; } };`);
    testSyntax(`class C { async 0() { await 1; } };`);
    testSyntax(`var asyncFn = async({ foo = 1 }) => foo;`);
    testSyntax(`var asyncFn = async({ foo = 1 } = {}) => foo;`);
    testSyntax(`function* g() { var f = async(yield); }`);
    testSyntax(`function* g() { var f = async(x = yield); }`);
})();

(function testTopLevelAsyncAwaitSyntaxStrictMode() {
    testSyntax(`"use strict"; var asyncFn = async function() { await 1; };`);
    testSyntax(`"use strict"; var asyncFn = async function withName() { await 1; };`);
    testSyntax(`"use strict"; var asyncFn = async () => await 'test';`);
    testSyntax(`"use strict"; var asyncFn = async x => await x + 'test';`);
    testSyntax(`"use strict"; async function asyncFn() { await 1; }`);
    testSyntax(`"use strict"; var O = { async method() { await 1; } };`);
    testSyntax(`"use strict"; var O = { async ['meth' + 'od']() { await 1; } };`);
    testSyntax(`"use strict"; var O = { async 'method'() { await 1; } };`);
    testSyntax(`"use strict"; var O = { async 0() { await 1; } };`);
    testSyntax(`"use strict"; class C { async method() { await 1; } };`);
    testSyntax(`"use strict"; class C { async ['meth' + 'od']() { await 1; } };`);
    testSyntax(`"use strict"; class C { async 'method'() { await 1; } };`);
    testSyntax(`"use strict"; class C { async 0() { await 1; } };`);
    testSyntax(`"use strict"; var asyncFn = async({ foo = 1 }) => foo;`);
    testSyntax(`"use strict"; var asyncFn = async({ foo = 1 } = {}) => foo;`);
    testSyntax(`"use strict"; function* g() { var f = async(yield); }`);
    testSyntax(`"use strict"; function* g() { var f = async(x = yield); }`);
})();

(function testNestedAsyncAwaitSyntax() {
    var contextData = [
        { prefix: "function outerFunction() { ", suffix: " }" },
        { prefix: "function* outerGenerator() { ", suffix: " }" },
        { prefix: "var outerFuncExpr = function() { ", suffix: " };" },
        { prefix: "var outerGenExpr = function*() { ", suffix: " };" },
        { prefix: "var outerObject = { outerMethod() { ", suffix: " } };" },
        { prefix: "var outerObject = { *outerGenMethod() { ", suffix: " } };" },
        { prefix: "var outerClassExpr = class C { outerMethod() { ", suffix: " } };" },
        { prefix: "var outerClassExpr = class C { *outerGenMethod() { ", suffix: " } };" },
        { prefix: "var outerClassExpr = class C { static outerStaticMethod() { ", suffix: " } };" },
        { prefix: "var outerClassExpr = class C { static *outerStaticGenMethod() { ", suffix: " } };" },
        { prefix: "class outerClass { outerMethod() { ", suffix: " } };" },
        { prefix: "class outerClass { *outerGenMethod() { ", suffix: " } };" },
        { prefix: "class outerClass { static outerStaticMethod() { ", suffix: " } };" },
        { prefix: "class outerClass { static *outerStaticGenMethod() { ", suffix: " } };" },
        { prefix: "var outerArrow = () => { ", suffix: " };" },
        { prefix: "async function outerAsyncFunction() { ", suffix: " }" },
        { prefix: "var outerAsyncFuncExpr = async function() { ", suffix: " };" },
        { prefix: "var outerAsyncArrowFunc = async () => { ", suffix: " };" },
        { prefix: "var outerObject = { async outerAsyncMethod() { ", suffix: " } };" },
        { prefix: "var outerClassExpr = class C { async outerAsyncMethod() { ", suffix: " } };" },
        { prefix: "var outerClassExpr = class C { static async outerStaticAsyncMethod() { ", suffix: " } };" },
        { prefix: "class outerClass { async outerAsyncMethod() { ", suffix: " } };" },
        { prefix: "class outerClass { static async outerStaticAsyncMethod() { ", suffix: " } };" },
    ];

    var testData = [
        `var async = 1; return async;`,
        `let async = 1; return async;`,
        `const async = 1; return async;`,
        `function async() {} return async();`,
        `var async = async => async; return async();`,
        `function foo() { var await = 1; return await; }`,
        `function foo(await) { return await; }`,
        `function* foo() { var await = 1; return await; }`,
        `function* foo(await) { return await; }`,
        `var f = () => { var await = 1; return await; }`,
        `var O = { method() { var await = 1; return await; } };`,
        `var O = { method(await) { return await; } };`,
        `var O = { *method() { var await = 1; return await; } };`,
        `var O = { *method(await) { return await; } };`,

        `(function await() {})`,
    ];

    for (let context of contextData) {
        for (let test of testData) {
            let script = context.prefix + test + context.suffix;
            testSyntax(script);
            testSyntax(`"use strict"; ${script}`);
        }
    }
})();


(function testTopLevelAsyncAwaitSyntaxSloppyMode() {
    testSyntaxError(`var asyncFn = async function await() {}`);
    testSyntaxError(`var asyncFn = async () => var await = 'test';`);
    testSyntaxError(`var asyncFn = async () => { var await = 'test'; };`);
    testSyntaxError(`var asyncFn = async await => await + 'test'`);
    testSyntaxError(`var asyncFn = async function(await) {}`);
    testSyntaxError(`var asyncFn = async function withName(await) {}`);
    testSyntaxError(`var asyncFn = async (await) => 'test';`);
    testSyntaxError(`async function asyncFunctionDeclaration(await) {}`);

    // FIXME: MethodDefinitions do not apply StrictFormalParameters restrictions
    //        in sloppy mode (https://bugs.webkit.org/show_bug.cgi?id=161408)
    //testSyntaxError(`var outerObject = { async method(a, a) {} }`);
    //testSyntaxError(`var outerObject = { async ['meth' + 'od'](a, a) {} }`);
    //testSyntaxError(`var outerObject = { async 'method'(a, a) {} }`);
    //testSyntaxError(`var outerObject = { async 0(a, a) {} }`);

    testSyntaxError(`var asyncArrowFn = async() => await;`);

    testSyntaxError(`var asyncFn = async function*() {}`);
    testSyntaxError(`async function* asyncGenerator() {}`);
    testSyntaxError(`var O = { *async asyncGeneratorMethod() {} };`);
    testSyntaxError(`var O = { async *asyncGeneratorMethod() {} };`);
    testSyntaxError(`var O = { async asyncGeneratorMethod*() {} };`);

    testSyntaxError(`var asyncFn = async function(x = await 1) { return x; }`);
    testSyntaxError(`async function f(x = await 1) { return x; }`);
    testSyntaxError(`var f = async(x = await 1) => x;`);
    testSyntaxError(`var O = { async method(x = await 1) { return x; } };`);

    testSyntaxError(`function* outerGenerator() { var asyncArrowFn = async yield => 1; }`);
    testSyntaxError(`function* outerGenerator() { var asyncArrowFn = async(yield) => 1; }`);
    testSyntaxError(`function* outerGenerator() { var asyncArrowFn = async(x = yield) => 1; }`);
    testSyntaxError(`function* outerGenerator() { var asyncArrowFn = async({x = yield}) => 1; }`);

    testSyntaxError(`class C { async constructor() {} }`);
    testSyntaxError(`class C {}; class C2 extends C { async constructor() {} }`);
    testSyntaxError(`class C { static async prototype() {} }`);
    testSyntaxError(`class C {}; class C2 extends C { static async prototype() {} }`);

    testSyntaxError(`var f = async() => ((async(x = await 1) => x)();`);

    // Henrique Ferreiro's bug (tm)
    testSyntaxError(`(async function foo1() { } foo2 => 1)`);
    testSyntaxError(`(async function foo3() { } () => 1)`);
    testSyntaxError(`(async function foo4() { } => 1)`);
    testSyntaxError(`(async function() { } foo5 => 1)`);
    testSyntaxError(`(async function() { } () => 1)`);
    testSyntaxError(`(async function() { } => 1)`);
    testSyntaxError(`(async.foo6 => 1)`);
    testSyntaxError(`(async.foo7 foo8 => 1)`);
    testSyntaxError(`(async.foo9 () => 1)`);
    testSyntaxError(`(async().foo10 => 1)`);
    testSyntaxError(`(async().foo11 foo12 => 1)`);
    testSyntaxError(`(async().foo13 () => 1)`);
    testSyntaxError(`(async['foo14'] => 1)`);
    testSyntaxError(`(async['foo15'] foo16 => 1)`);
    testSyntaxError(`(async['foo17'] () => 1)`);
    testSyntaxError(`(async()['foo18'] => 1)`);
    testSyntaxError(`(async()['foo19'] foo20 => 1)`);
    testSyntaxError(`(async()['foo21'] () => 1`);
    testSyntaxError("(async`foo22` => 1)");
    testSyntaxError("(async`foo23` foo24 => 1)");
    testSyntaxError("(async`foo25` () => 1)");
    testSyntaxError("(async`foo26`.bar27 => 1)");
    testSyntaxError("(async`foo28`.bar29 foo30 => 1)");
    testSyntaxError("(async`foo31`.bar32 () => 1)");

    // assert that errors are still thrown for calls that may have been async functions
    testSyntaxError(`function async() {}
                     async({ foo33 = 1 })`);
})();

(function testTopLevelAsyncAwaitSyntaxStrictMode() {
    testSyntaxError(`"use strict"; var asyncFn = async function await() {}`);
    testSyntaxError(`"use strict"; var asyncFn = async () => var await = 'test';`);
    testSyntaxError(`"use strict"; var asyncFn = async () => { var await = 'test'; };`);
    testSyntaxError(`"use strict"; var asyncFn = async await => await + 'test'`);
    testSyntaxError(`"use strict"; var asyncFn = async function(await) {}`);
    testSyntaxError(`"use strict"; var asyncFn = async function withName(await) {}`);
    testSyntaxError(`"use strict"; var asyncFn = async (await) => 'test';`);
    testSyntaxError(`"use strict"; async function asyncFunctionDeclaration(await) {}`);

    testSyntaxError(`"use strict"; var outerObject = { async method(a, a) {} }`);
    testSyntaxError(`"use strict"; var outerObject = { async ['meth' + 'od'](a, a) {} }`);
    testSyntaxError(`"use strict"; var outerObject = { async 'method'(a, a) {} }`);
    testSyntaxError(`"use strict"; var outerObject = { async 0(a, a) {} }`);

    testSyntaxError(`"use strict"; var asyncArrowFn = async() => await;`);

    testSyntaxError(`"use strict"; var asyncFn = async function*() {}`);
    testSyntaxError(`"use strict"; async function* asyncGenerator() {}`);
    testSyntaxError(`"use strict"; var O = { *async asyncGeneratorMethod() {} };`);
    testSyntaxError(`"use strict"; var O = { async *asyncGeneratorMethod() {} };`);
    testSyntaxError(`"use strict"; var O = { async asyncGeneratorMethod*() {} };`);

    testSyntaxError(`"use strict"; var asyncFn = async function(x = await 1) { return x; }`);
    testSyntaxError(`"use strict"; async function f(x = await 1) { return x; }`);
    testSyntaxError(`"use strict"; var f = async(x = await 1) => x;`);
    testSyntaxError(`"use strict"; var O = { async method(x = await 1) { return x; } };`);

    testSyntaxError(`"use strict"; function* outerGenerator() { var asyncArrowFn = async yield => 1; }`);
    testSyntaxError(`"use strict"; function* outerGenerator() { var asyncArrowFn = async(yield) => 1; }`);
    testSyntaxError(`"use strict"; function* outerGenerator() { var asyncArrowFn = async(x = yield) => 1; }`);
    testSyntaxError(`"use strict"; function* outerGenerator() { var asyncArrowFn = async({x = yield}) => 1; }`);

    testSyntaxError(`"use strict"; class C { async constructor() {} }`);
    testSyntaxError(`"use strict"; class C {}; class C2 extends C { async constructor() {} }`);
    testSyntaxError(`"use strict"; class C { static async prototype() {} }`);
    testSyntaxError(`"use strict"; class C {}; class C2 extends C { static async prototype() {} }`);

    testSyntaxError(`"use strict"; var f = async() => ((async(x = await 1) => x)();`);

    // Henrique Ferreiro's bug (tm)
    testSyntaxError(`"use strict"; (async function foo1() { } foo2 => 1)`);
    testSyntaxError(`"use strict"; (async function foo3() { } () => 1)`);
    testSyntaxError(`"use strict"; (async function foo4() { } => 1)`);
    testSyntaxError(`"use strict"; (async function() { } foo5 => 1)`);
    testSyntaxError(`"use strict"; (async function() { } () => 1)`);
    testSyntaxError(`"use strict"; (async function() { } => 1)`);
    testSyntaxError(`"use strict"; (async.foo6 => 1)`);
    testSyntaxError(`"use strict"; (async.foo7 foo8 => 1)`);
    testSyntaxError(`"use strict"; (async.foo9 () => 1)`);
    testSyntaxError(`"use strict"; (async().foo10 => 1)`);
    testSyntaxError(`"use strict"; (async().foo11 foo12 => 1)`);
    testSyntaxError(`"use strict"; (async().foo13 () => 1)`);
    testSyntaxError(`"use strict"; (async['foo14'] => 1)`);
    testSyntaxError(`"use strict"; (async['foo15'] foo16 => 1)`);
    testSyntaxError(`"use strict"; (async['foo17'] () => 1)`);
    testSyntaxError(`"use strict"; (async()['foo18'] => 1)`);
    testSyntaxError(`"use strict"; (async()['foo19'] foo20 => 1)`);
    testSyntaxError(`"use strict"; (async()['foo21'] () => 1)`);
    testSyntaxError('"use strict"; (async`foo22` => 1)');
    testSyntaxError('"use strict"; (async`foo23` foo24 => 1)');
    testSyntaxError('"use strict"; (async`foo25` () => 1)');
    testSyntaxError('"use strict"; (async`foo26`.bar27 => 1)');
    testSyntaxError('"use strict"; (async`foo28`.bar29 foo30 => 1)');
    testSyntaxError('"use strict"; (async`foo31`.bar32 () => 1)');

    // assert that errors are still thrown for calls that may have been async functions
    testSyntaxError(`"use strict"; function async() {}
                     async({ foo33 = 1 })`);

    testSyntaxError(`"use strict"; var O = { async method(eval) {} }`);
    testSyntaxError(`"use strict"; var O = { async ['meth' + 'od'](eval) {} }`);
    testSyntaxError(`"use strict"; var O = { async 'method'(eval) {} }`);
    testSyntaxError(`"use strict"; var O = { async 0(eval) {} }`);

    testSyntaxError(`"use strict"; var O = { async method(arguments) {} }`);
    testSyntaxError(`"use strict"; var O = { async ['meth' + 'od'](arguments) {} }`);
    testSyntaxError(`"use strict"; var O = { async 'method'(arguments) {} }`);
    testSyntaxError(`"use strict"; var O = { async 0(arguments) {} }`);

    testSyntaxError(`"use strict"; var O = { async method(dupe, dupe) {} }`);
})();
