/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: resources/web/reactive/view.mjs
 *
 * PURPOSE:
 *   Refresh explicitly bound browser text from a revisioned native view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

const schema = "umicom.reactive.view/1";
const maxRevision = 18446744073709551615n;
const minInteger = -9223372036854775808n;
const maxInteger = 9223372036854775807n;
const maximumRows = 256;
const htmlNamespace = "http://www.w3.org/1999/xhtml";
const textElements = new Set(["dd", "div", "dt", "li", "output", "p", "span", "td", "th"]);

function revision(text) {
    if (typeof text !== "string" || !/^(0|[1-9][0-9]{0,19})$/.test(text))
        throw new TypeError("Invalid reactive view revision");
    const value = BigInt(text);
    if (value > maxRevision) throw new RangeError("Reactive view revision is too large");
    return value;
}

function display(row) {
    const value = row.value;
    switch (row.kind) {
    case "none":
        if (value === null) return "";
        break;
    case "boolean":
        if (typeof value === "boolean") return String(value);
        break;
    case "integer":
        if (typeof value === "string" && /^(0|-?[1-9][0-9]{0,18})$/.test(value)) {
            const integer = BigInt(value);
            if (integer >= minInteger && integer <= maxInteger) return value;
        }
        break;
    case "real":
        if (typeof value === "number" && Number.isFinite(value))
            return Object.is(value, -0) ? "-0" : String(value);
        break;
    case "string":
        if (typeof value === "string" && value.length <= 511) return value;
        break;
    }
    throw new TypeError("Invalid reactive view value");
}

function stage(frame, since, bindings) {
    const previous = revision(since);
    if (frame === null || typeof frame !== "object" || frame.schema !== schema ||
        frame.since !== since || !Array.isArray(frame.rows) ||
        frame.rows.length > bindings.size || (previous === 0n && frame.rows.length !== bindings.size))
        throw new TypeError("Invalid reactive view frame");
    const observed = revision(frame.revision);
    if (observed < previous) throw new RangeError("Reactive view moved backward");
    const seen = new Set();
    const updates = [];
    for (const row of frame.rows) {
        if (row === null || typeof row !== "object" || !bindings.has(row.key) || seen.has(row.key))
            throw new TypeError("Unexpected reactive view key");
        const changed = revision(row.revision);
        if (changed <= previous || changed > observed) throw new RangeError("Invalid row revision");
        seen.add(row.key);
        updates.push([bindings.get(row.key), display(row)]);
    }
    return updates;
}

function sameOriginPath(endpoint, purpose) {
    const url = new URL(endpoint, document.baseURI);
    if (url.origin !== location.origin || !["http:", "https:"].includes(url.protocol) ||
        url.username || url.password || url.search || url.hash)
        throw new TypeError(`${purpose} requires a same-origin HTTP path`);
    return url;
}

/**
 * Bind every key selected by the admitted native view to a distinct existing
 * text-only DOM element. The application controls placement, titles and styling.
 * Concurrent refresh calls share one request. After a store lifetime change,
 * wait for the pending refresh to settle and call reset before reading again.
 */
export function createReactiveView(endpoint, elements) {
    // Keep the earlier inline check for review. The shared admission now gives
    // read and edit endpoints the same origin, credentials and URL rules.
    if (false) {
        const url = new URL(endpoint, document.baseURI);
        if (url.origin !== location.origin || !["http:", "https:"].includes(url.protocol) ||
            url.username || url.password || url.search || url.hash)
            throw new TypeError("Reactive view requires a same-origin HTTP path");
    }
    const url = sameOriginPath(endpoint, "Reactive view");
    const bindings = new Map(elements);
    if (bindings.size > maximumRows || new Set(bindings.values()).size !== bindings.size)
        throw new TypeError("Invalid reactive view bindings");
    for (const [key, element] of bindings)
        if (typeof key !== "string" || !key.length || key.length > 255 ||
            element?.namespaceURI !== htmlNamespace || !textElements.has(element.localName) ||
            element.childElementCount !== 0)
            throw new TypeError("Reactive view requires a named text-only HTML element");

    let since = "0";
    let pending = null;
    async function receive() {
        const request = new URL(url);
        request.searchParams.set("since", since);
        const result = await fetch(request, {cache: "no-store", credentials: "same-origin", redirect: "error"});
        if (!result.ok) {
            const error = new Error(`Reactive view request failed (${result.status})`);
            error.status = result.status;
            throw error;
        }
        const type = result.headers.get("content-type")?.split(";", 1)[0].trim().toLowerCase();
        if (type !== "application/json") throw new TypeError("Reactive view requires JSON");
        const frame = await result.json();
        const updates = stage(frame, since, bindings);
        // Validate the entire frame before changing any node. Text protects
        // existing application markup even when a valid string contains HTML.
        for (const [element, value] of updates) element.textContent = value;
        since = frame.revision;
        return since;
    }

    return Object.freeze({
        refresh() {
            if (!pending) pending = receive().finally(() => { pending = null; });
            return pending;
        },
        reset() {
            if (pending) throw new Error("Wait for the current reactive view refresh");
            since = "0";
        },
        get revision() { return since; },
    });
}

/* Both form types use the same origin, exact revision and acknowledgement
 * checks. Only the admitted literal differs between their input controls. */
function createReactiveEdit(endpoint, encode) {
    const url = sameOriginPath(endpoint, "Reactive edit");
    return async function submitEdit(viewRevision, value) {
        const baseRevision = revision(viewRevision);
        const text = encode(value);
        const result = await fetch(url, {
            method: "POST", cache: "no-store", credentials: "same-origin", redirect: "error",
            headers: {"Content-Type": "application/x-umicom-reactive-edit"},
            body: `revision=${viewRevision}&value=${text}`,
        });
        if (!result.ok) {
            const error = new Error(`Reactive edit request failed (${result.status})`);
            error.status = result.status;
            throw error;
        }
        const type = result.headers.get("content-type")?.split(";", 1)[0].trim().toLowerCase();
        if (result.status !== 200 || type !== "application/json")
            throw new TypeError("Reactive edit requires a JSON acknowledgement");
        const reply = await result.json();
        if (reply?.schema !== "umicom.reactive.edit/1" || revision(reply.revision) !== baseRevision + 1n)
            throw new TypeError("Invalid reactive edit acknowledgement");
        return reply.revision;
    };
}

/** Bind a configured native edit route. Pass the revision the user reviewed
 * and a signed integer as decimal text. The acknowledgement describes the
 * commit only. The view can refresh independently and handle conflicts. */
export function createReactiveIntegerEdit(endpoint) {
    return createReactiveEdit(endpoint, value => display({kind: "integer", value}));
}

/** Bind a host-configured Boolean route and pass the checkbox's checked
 * value. Keep the reviewed revision separate from the checkbox draft. */
export function createReactiveBooleanEdit(endpoint) {
    return createReactiveEdit(endpoint, value => {
        if (typeof value !== "boolean") throw new TypeError("Reactive Boolean edit requires a Boolean");
        return value ? "true" : "false";
    });
}
