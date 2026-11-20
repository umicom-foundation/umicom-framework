![Umicom](../../resources/brand/umicom-logo.svg)

# Keep reactive state readable when an update is refused

A reactive state store associates a property path with an owned UI value. Each accepted put advances the store revision and the changed row's revision. These numbers help a client notice accepted changes; they are not timestamps or permission to execute a command.

The public store is a value object. It has no internal lock. The application owns its lifetime and must serialize reads with writes.

## 1. Allocate and initialize the owner

The store has room for 256 rows, each containing fixed text and a complete value. Allocate it deliberately rather than placing several copies on a small thread stack.

~~~c
#include <stdlib.h>
#include "umicom/ui/reactive/observable_store.h"

UmiUiReactiveObservableStore *store = malloc(sizeof(*store));
if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
umi_ui_reactive_observable_store_init(store);
~~~

Keep the pointer in the application model that owns this state. Destroy that model only after callbacks and borrowers have finished. Release the store with free when it is no longer used.

## 2. Publish through the shared value contract

~~~c
UmiUiValue value;
UmiStatus status = umi_ui_value_set_integer(&value, 42);
if (status == UMI_STATUS_OK)
    status = umi_ui_reactive_observable_store_put(store, "order.quantity", &value);
if (status != UMI_STATUS_OK) {
    /* Keep the prior presentation and report the refused update. */
}
~~~

The key must be nonempty and terminate within the 256-byte path field. The value must use a declared kind and a bounded string when STRING is active. The store stages the input before publication, so a value borrowed from one complete row can be used to update another row.

The API copies accepted values. Changing the caller's separate input afterward does not change the row.

## 3. Distinguish refused input from damaged owner state

| Status | Meaning | Response |
| --- | --- | --- |
| INVALID_ARGUMENT | A required argument, key or incoming value is malformed | Correct the request; retain the current owner |
| INVALID_STATE | The active store count, key or value is malformed | Stop publication and investigate the owner or restoration path |
| CAPACITY_EXCEEDED | No insertion slot or no next revision remains | Preserve the accepted state and choose a deliberate lifecycle action |
| NOT_FOUND | A valid lookup has no matching row | Handle absence without inventing a value |

The store examines bounded active rows before lookup. It checks the count before indexing and establishes key terminators before string comparison. An invalid later row is not ignored merely because an earlier row matches the requested key.

These checks do not make an arbitrary pointer safe and do not validate every business rule. Callers still provide complete readable objects. The store does not decide whether a quantity is commercially valid or whether a path is authorized for a command.

## 4. Preserve observation identity at the revision limit

When the revision is UINT64_MAX, another put returns CAPACITY_EXCEEDED before mutation. It does not recycle zero as a new identity. Reads remain available, so the application can show or save the accepted state.

An update from UINT64_MAX - 1 can publish once, reaching the maximum. The following update is refused. Full row capacity is a separate condition: an existing row can still be replaced when all 256 slots are occupied, provided a revision remains available.

Do not reset a live store merely to make an update succeed. A new owner lifetime needs an explicit application decision, correct borrower shutdown and a fresh observation context.

## 5. Read into separate caller storage

~~~c
UmiUiValue observed;
UmiStatus status = umi_ui_reactive_observable_store_get(store, "order.quantity", &observed);
if (status == UMI_STATUS_OK && observed.kind == UMI_UI_VALUE_INTEGER) {
    /* Format observed.integer_value for the current view. */
}
~~~

The output record must be separate from the store. A failed lookup leaves it unchanged. A read at the maximum revision remains valid. Serialize the whole call with updates; the validation pass is not a concurrency lock.

## 6. Admit command and expression envelopes consistently

Change events, command parameters, expression operands and evaluator results contain complete UI values. They reuse umi_ui_value_valid before accepting or encoding those payloads.

Converter descriptors and validators contain value kinds without payloads. They use umi_ui_value_kind_valid to recognize the same declared set, including NONE. This avoids constructing a dummy payload or maintaining a second list of supported kinds.

Archive owners retain their existing fixed-text terminator checks, including inactive fields that the archive serializes. A declared kind is not proof that a converter implementation exists, a command has permission or an expression has executed. Those remain separate service responsibilities.

## 7. Qualify the source

Run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-ui-reactive-value-admission-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]ui_reactive[.]value_admission[.]"
~~~

The authored cases cover new and existing rows, borrowed values, full capacity, malformed counts and text, revision boundaries, output preservation and eight embedded kind positions. No compiler, test or native UI result is implied by this guide. Owner qualification remains unexecuted until the commands run successfully against the recorded source.

Read [UI value publication](ui-value-publication.md) for string aliasing, active payloads and comparison semantics.

## Apply several widget edits as one reaction

A native application can keep a separate observable store for each browser
session or desktop window. Declare the editable properties before serving
the view. When a client submits edits, authorize the action and begin a
`UmiUiReactiveChangeTransaction` using the store revision the client saw.

For example, consider a filter with `filter.low` and `filter.high`. A
client may change both after reviewing the same view. Represent each edit
as a change event with its original and proposed value. Call
`umi_ui_reactive_observable_store_apply` with the complete pair. A successful
commit gives both edited rows the same new revision. The returned batch
contains the observed and published revisions for the next render.

If another request has already updated the store or a supplied original
value differs, the whole event is refused. The client can read the latest
state and review its edit again. Repeated paths and unknown properties are
also refused. The host serializes the operation with other writes and
computes or renders derived output from the committed state.

With Framework configured, run the focused tests from its source folder.

~~~powershell
cmake --build --preset windows-ucrt64-debug --target umicom-ui-reactive-event-commit-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure --no-tests=error -R "^framework[.]ui_reactive[.]event_commit[.]"
~~~

The fixture covers two edited values, stale state, a refused second
property, and revision exhaustion. It also verifies that restarting a
transaction with its own stored ID keeps the ID intact. These cases exercise
the native state owner. A browser or desktop client needs its own event and
rendering checks.

## Refresh the selected properties of a view

Choose the property paths a view is allowed to render. For a panel that shows a
range, send just its lower and upper values even if the same session has other
settings. Keep this selection with the view and pass it to
`umi_ui_reactive_observable_store_read_changes`.

1. Start with revision zero and allow room for every selected key. The first
   read copies the current render values in selection order.
2. Save the returned revision with that view. After accepting an edit, read
   again using that revision. A row changed twice appears once with its latest
   value. A change elsewhere can advance the store revision and yield zero rows.
3. Keep the same store lifetime and selected keys for later reads. Start again
   from zero if the selection or session changes. A failed read preserves the
   old output and cursor. An unknown key usually means the view and state owner
   disagree about the schema.

```powershell
cmake --build --preset windows-ucrt64-debug --target umicom-ui-reactive-view-delta-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure --no-tests=error -R '^framework[.]ui_reactive[.]view_delta[.]'
```

The 13 native cases cover the first paint, incremental reads, a two-field atomic
event, independent ownership and refusal paths. Keep the selection under the
view owner's control and serialize access with state edits. A renderer should
use the active value kinds and its own frontend encoding rules.

## Connect the same state to browser text

1. Link your HTTP host to `Umicom::reactive_web`. In an installed SDK, request
   the `reactive_web` component. Add a store per admitted application session.
2. Choose an exact route such as `/practice-state` and a fixed list of visible
   keys. After the host authorizes the request and finds its session store,
   supply those values in a `UmiWebReactiveView` and call
   `UmiWebReactiveViewHandle` from the service's body handler. Keep the keys
   from the trusted application configuration. A query selects a revision.
3. Serve `resources/web/reactive/view.mjs` from a deliberate same-origin
   static route. Bind the chosen keys to existing empty or text-only HTML
   elements, such as an `output`, `p` or `span`. Set their captions and
   styling in the page. For example, a panel with an existing
   `<output id="quantity"></output>` could use this module:

```javascript
import {createReactiveView} from "/reactive/view.mjs";

const view = createReactiveView("/practice-state", [
    ["order.quantity", document.getElementById("quantity")],
]);
await view.refresh();
```

The first refresh sends `since=0`. Later refreshes publish only the changed
properties. A refresh with no changed properties may still advance the
revision. Revisions and signed 64-bit integers travel in decimal strings so
the browser can show them without rounding. All updated text is validated
before it is written to the selected elements.

If a view request returns HTTP 409 after the application selects a new store,
wait for the refresh to settle, call `view.reset()` and refresh again from
the beginning. The host still owns authentication, session selection and
serialization. The browser view only reads values. Its refresh calls never
submit edits, persist state or run application commands.

## Submit a reviewed integer edit

1. Choose a write route such as `/practice-quantity`. Configure a
   `UmiWebReactiveIntegerEdit` with the existing integer key, its inclusive
   minimum and maximum, and the store used by the read view. Keep those
   choices in the application. The browser supplies the value and revision.
2. Supply an explicit origin policy and an authorization callback. Check that
   the current session can edit this key in this store. Route POST requests
   through `UmiWebReactiveIntegerEditHandle` after selecting the session store.
   Serialize dispatch with all other store reads and writes.
3. Call the browser adapter when the user submits a value they have reviewed.
   Read the input as text to preserve every signed 64-bit integer. Keep the
   revision shown when the user began editing and refresh after a confirmed
   write.

```javascript
import {createReactiveIntegerEdit} from "/reactive/view.mjs";

const submitQuantity = createReactiveIntegerEdit("/practice-quantity");
const reviewedRevision = view.revision;
// Pass the value from the application's existing text input on submit.
await submitQuantity(reviewedRevision, quantityInput.value);
await view.refresh();
```

The server compares the reviewed revision with current state before applying
the change. If the revision changed, the write returns HTTP 409. Refresh the
display and let the user review the new state before submitting again. If the
connection drops after submission, read the current value before deciding
whether to retry. Each accepted edit advances the revision once. This route
updates one existing integer in the host's in-memory state. The application
owns any durable storage and business workflow.

The selected native suite `framework.ui_reactive.web_edit.*` checks the write
boundary. The `framework.ui_reactive.web_view.browser_contract` suite checks
that the browser sends exact decimal text and refreshes independently.
