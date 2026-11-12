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
