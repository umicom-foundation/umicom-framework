/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_browser_reactive_view.mjs
 *
 * PURPOSE:
 *   Verify browser view admission, cursor handling and atomic text publication.
 *   Verify canonical integer edits and independent refresh after a commit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

import assert from "node:assert/strict";
import test from "node:test";
import {createReactiveView, createReactiveIntegerEdit, createReactiveBooleanEdit} from "../../resources/web/reactive/view.mjs";

const origin = "http://127.0.0.1:4111";
globalThis.document = {baseURI: origin + "/dashboard/"};
globalThis.location = new URL(origin);

function element(textContent = "before") {
    return {namespaceURI: "http://www.w3.org/1999/xhtml", localName: "output",
        childElementCount: 0, textContent};
}

function row(key, revision, kind, value) {
    return {key, revision, kind, value};
}

function frame(since, revision, rows) {
    return {schema: "umicom.reactive.view/1", since, revision, rows};
}

function reply(document, status = 200, type = "application/json") {
    return new Response(JSON.stringify(document), {status, headers: {"Content-Type": type}});
}

test("text, signed 64-bit values and full unsigned revisions stay exact", async () => {
    const message = element();
    const amount = element();
    let requested = 0;
    globalThis.fetch = async (url, options) => {
        ++requested;
        assert.equal(url.href, origin + "/state?since=0");
        assert.deepEqual(options, {cache: "no-store", credentials: "same-origin", redirect: "error"});
        return reply(frame("0", "18446744073709551615", [
            row("message", "18446744073709551614", "string", '<em id="other">literal</em>'),
            row("amount", "18446744073709551615", "integer", "-9223372036854775808"),
        ]), 200, "Application/JSON; charset=utf-8");
    };
    const bindings = new Map([["message", message], ["amount", amount]]);
    const view = createReactiveView("/state", bindings);
    bindings.clear();
    assert.equal(await view.refresh(), "18446744073709551615");
    assert.equal(view.revision, "18446744073709551615");
    assert.equal(requested, 1);
    assert.equal(message.textContent, '<em id="other">literal</em>');
    assert.equal(amount.textContent, "-9223372036854775808");
});

test("incremental and empty frames update the cursor without repainting other elements", async () => {
    const label = element();
    const number = element();
    const view = createReactiveView("/state", [["label", label], ["number", number]]);
    const queue = [
        ["0", frame("0", "2", [row("label", "1", "boolean", true), row("number", "2", "real", 1.25)])],
        ["2", frame("2", "3", [row("number", "3", "real", -0)])],
        ["3", frame("3", "4", [])],
        ["4", frame("4", "5", [row("label", "5", "none", null)])],
    ];
    globalThis.fetch = async url => {
        const [since, data] = queue.shift();
        assert.equal(url.search, "?since=" + since);
        // JSON.stringify normalizes -0. Preserve the native wire spelling here.
        return since === "2"
            ? new Response(JSON.stringify(data).replace('"value":0', '"value":-0'),
                {headers: {"Content-Type": "application/json"}})
            : reply(data);
    };
    assert.equal(await view.refresh(), "2");
    assert.equal(label.textContent, "true");
    assert.equal(number.textContent, "1.25");
    assert.equal(await view.refresh(), "3");
    assert.equal(label.textContent, "true");
    assert.equal(number.textContent, "-0");
    assert.equal(await view.refresh(), "4");
    assert.equal(number.textContent, "-0");
    assert.equal(await view.refresh(), "5");
    assert.equal(label.textContent, "");
    assert.equal(queue.length, 0);
});

test("a refused row, scope mismatch or response failure leaves all observed values intact", async () => {
    const first = element();
    const last = element();
    const view = createReactiveView("/state", [["first", first], ["last", last]]);
    const good = row("first", "1", "string", "accepted only with all rows");
    const refused = [
        frame("0", "2", [good]),
        frame("0", "2", [good, row("private", "2", "string", "unselected")]),
        frame("0", "2", [good, row("first", "2", "string", "duplicate")]),
        frame("0", "2", [good, row("last", "0", "boolean", true)]),
        frame("0", "2", [good, row("last", "3", "boolean", true)]),
        frame("0", "2", [good, row("last", "2", "integer", "9223372036854775808")]),
        frame("0", "2", [good, row("last", "2", "integer", "-0")]),
        frame("1", "2", [good, row("last", "2", "boolean", true)]),
        frame("0", "18446744073709551616", [good, row("last", "2", "boolean", true)]),
    ];
    for (const attempt of refused) {
        globalThis.fetch = async () => reply(attempt);
        await assert.rejects(view.refresh());
        assert.equal(first.textContent, "before");
        assert.equal(last.textContent, "before");
        assert.equal(view.revision, "0");
    }
    globalThis.fetch = async () => reply(null, 409);
    await assert.rejects(view.refresh(), {status: 409});
    globalThis.fetch = async () => reply(null, 200, "text/html");
    await assert.rejects(view.refresh(), /requires JSON/);
    globalThis.fetch = async () => reply(frame("0", "2", [good, row("last", "2", "boolean", true)]));
    assert.equal(await view.refresh(), "2");
    assert.equal(first.textContent, good.value);
    assert.equal(last.textContent, "true");
});

test("one pending refresh owns the cursor and reset requests a new full view", async () => {
    const view = createReactiveView("/state", [["title", element()]]);
    let finish;
    let requested = 0;
    globalThis.fetch = url => {
        ++requested;
        assert.equal(url.search, "?since=0");
        return new Promise(resolve => { finish = resolve; });
    };
    const first = view.refresh();
    assert.equal(view.refresh(), first);
    assert.equal(requested, 1);
    assert.throws(() => view.reset(), /current reactive view refresh/);
    finish(reply(frame("0", "1", [row("title", "1", "string", "started")])));
    await first;
    assert.equal(view.revision, "1");
    view.reset();
    const second = view.refresh();
    finish(reply(frame("0", "2", [row("title", "2", "string", "new lifetime")])));
    await second;
    assert.equal(view.revision, "2");
    assert.equal(requested, 2);
});

test("refuse cross-origin, embedded markup destinations and ambiguous bindings", () => {
    const output = element();
    assert.throws(() => createReactiveView("https://foreign.invalid/state", [["a", output]]));
    assert.throws(() => createReactiveView("/state?foreign=1", [["a", output]]));
    assert.throws(() => createReactiveView("/state", [["a", output], ["b", output]]));
    assert.throws(() => createReactiveView("/state", [["a", {...output, localName: "script"}]]));
    assert.throws(() => createReactiveView("/state", [["a", {...output, childElementCount: 1}]]));
});

test("an edit sends exact integer and revision text while the read cursor waits for refresh", async () => {
    const amount = element();
    const view = createReactiveView("/state", [["amount", amount]]);
    const edit = createReactiveIntegerEdit("/edit");
    const initial = "18446744073709551614";
    const updated = "18446744073709551615";
    globalThis.fetch = async url => {
        assert.equal(url.search, "?since=0");
        return reply(frame("0", initial, [row("amount", initial, "integer", "4")]));
    };
    await view.refresh();
    globalThis.fetch = async (url, options) => {
        assert.equal(url.href, origin + "/edit");
        assert.deepEqual(options, {method: "POST", cache: "no-store", credentials: "same-origin",
            redirect: "error", headers: {"Content-Type": "application/x-umicom-reactive-edit"},
            body: `revision=${initial}&value=-9223372036854775808`});
        return reply({schema: "umicom.reactive.edit/1", revision: updated});
    };
    assert.equal(await edit(view.revision, "-9223372036854775808"), updated);
    assert.equal(view.revision, initial);
    assert.equal(amount.textContent, "4");
    globalThis.fetch = async url => {
        assert.equal(url.search, "?since=" + initial);
        return reply(frame(initial, updated, [row("amount", updated, "integer", "-9223372036854775808")]));
    };
    assert.equal(await view.refresh(), updated);
    assert.equal(amount.textContent, "-9223372036854775808");
});

test("noncanonical and cross-origin edits are refused before fetch", async () => {
    for (const endpoint of ["https://foreign.invalid/edit", "/edit?other=true", "/edit#other"])
        assert.throws(() => createReactiveIntegerEdit(endpoint), /same-origin/);
    const edit = createReactiveIntegerEdit("/edit");
    let requested = 0;
    globalThis.fetch = async () => { ++requested; };
    for (const [revision, value] of [["00", "0"], [0, "0"], ["-1", "0"], ["18446744073709551616", "0"],
        ["1", 0], ["1", "+1"], ["1", "-0"], ["1", "01"], ["1", "9223372036854775808"], ["1", "-9223372036854775809"]])
        await assert.rejects(edit(revision, value));
    assert.equal(requested, 0);
});

test("stale edits stay visible to the caller and no response retries the request", async () => {
    const edit = createReactiveIntegerEdit("/edit");
    const accepted = {schema: "umicom.reactive.edit/1", revision: "3"};
    const replies = [reply(null, 409), reply(null, 403), reply(accepted, 200, "text/html"),
        reply(accepted, 201), reply({...accepted, revision: 3}), reply({...accepted, revision: "4"}),
        reply({...accepted, revision: "2"}), reply({...accepted, schema: "other"}), reply(accepted)];
    globalThis.fetch = async () => replies.shift();
    await assert.rejects(edit("2", "0"), {status: 409});
    await assert.rejects(edit("2", "0"), {status: 403});
    for (let index = 0; index < 6; ++index) await assert.rejects(edit("2", "0"));
    assert.equal(await edit("2", "0"), "3");
    assert.equal(replies.length, 0);
});

test("a Boolean control sends canonical true and false without advancing the view", async () => {
    const value = element();
    const sibling = element();
    const view = createReactiveView("/state", [["enabled", value], ["other", sibling]]);
    const edit = createReactiveBooleanEdit("/enabled");
    globalThis.fetch = async () => reply(frame("0", "2", [
        row("enabled", "1", "boolean", false), row("other", "2", "integer", "9223372036854775807"),
    ]));
    await view.refresh();
    for (const [base, next, checked] of [["2", "3", true], ["3", "4", false]]) {
        globalThis.fetch = async (url, options) => {
            assert.equal(url.href, origin + "/enabled");
            assert.deepEqual(options, {method: "POST", cache: "no-store", credentials: "same-origin",
                redirect: "error", headers: {"Content-Type": "application/x-umicom-reactive-edit"},
                body: `revision=${base}&value=${checked}`});
            return reply({schema: "umicom.reactive.edit/1", revision: next});
        };
        assert.equal(await edit(view.revision, checked), next);
        assert.equal(view.revision, base);
        assert.equal(value.textContent, String(!checked));
        globalThis.fetch = async url => {
            assert.equal(url.search, "?since=" + base);
            return reply(frame(base, next, [row("enabled", next, "boolean", checked)]));
        };
        assert.equal(await view.refresh(), next);
        assert.equal(value.textContent, String(checked));
        assert.equal(sibling.textContent, "9223372036854775807");
    }
});

test("a Boolean control rejects coerced values and leaves conflicts to its caller", async () => {
    assert.throws(() => createReactiveBooleanEdit("https://foreign.invalid/enabled"), /same-origin/);
    const edit = createReactiveBooleanEdit("/enabled");
    let requests = 0;
    globalThis.fetch = async () => { ++requests; return reply(null, 409); };
    for (const value of ["true", "false", 0, 1, null, undefined, {checked: true}])
        await assert.rejects(edit("2", value), /requires a Boolean/);
    await assert.rejects(edit("02", true));
    assert.equal(requests, 0);
    await assert.rejects(edit("2", true), {status: 409});
    assert.equal(requests, 1);
});
