![Umicom](../../resources/brand/umicom-logo.svg)

# Publish alert changes together

A price-alert book is caller-owned storage containing bounded rules and one revision. Each rule retains its latest observed value, activation state and occurrence history. Framework validates the retained rules before a book command uses them.

An outer workspace revision and an alert-book revision are different observations. Reserving the workspace revision alone does not grant permission to mutate an exhausted alert book.

## Admit the child mutation

Add, remove, enable, acknowledge and evaluation operations reserve an alert-book revision before changing any retained rule. At `UINT64_MAX`, a required mutation returns `CAPACITY_EXCEEDED` and leaves the book unchanged.

The existing no-op and validation rules remain significant:

| Situation | Result |
| --- | --- |
| Enable a rule to its existing value | `OK` without a new revision |
| Evaluate an instrument with no matching eligible rules | `OK` without mutation |
| Observe an older event than every matching rule | `OK` without mutation |
| Acknowledge an inactive rule | Existing `INVALID_STATE` result |
| Add a duplicate identifier | Existing `ALREADY_EXISTS` result |
| Retained rule is malformed | `INVALID_ARGUMENT` before changing any row |

Evaluation uses the same eligibility predicate during admission and application. A command first determines whether any rule would observe the value, reserves one revision, and then updates all eligible rules. It does not update an earlier row and discover capacity failure at a later one.

Occurrence counters keep their existing saturation behavior. A counter at its maximum remains there; it does not wrap to zero and erase its visible history. An exhausted book revision is a separate condition and refuses a new evaluation.

## Remove one instrument's rules as a batch

`UmiTradingAlertBookRemoveInstrument` removes every matching rule in stable order with one revision. It clears the inactive tail after compaction. If no rule matches, it returns `OK` without changing anything, including at maximum revision.

~~~c
UmiStatus status = UmiTradingAlertBookRemoveInstrument(&book, instrument_id);
if (status != UMI_STATUS_OK) {
    /* The complete book is unchanged. Retain its existing presentation. */
}
~~~

This is a canonical Framework operation rather than a UI loop over individual removals. A batch represents one logical change even when several rules are removed.

## Coordinate workspace removal

Removing an instrument also removes its retained public prints and alerts. The workspace performs the fallible work in this order:

1. Validate the request, find the market and reserve the outer revision.
2. Allocate private storage for a copy of the alert book.
3. Apply the canonical alert-removal batch to that copy.
4. Ask the trade tape to remove its matching records through its failure-atomic API.
5. Publish the prepared alert book, compact market/history/watchlist arrays, advance the workspace revision and reconcile selection.

If allocation, alert admission or tape removal fails, the workspace returns that failure before publishing the prepared alert book. The tape's own refusal contract leaves its records intact. The operation does not ignore individual child failures or announce that an instrument disappeared while its dependent alerts remain.

All calls occur on the workspace's owning thread. This staged operation is not a concurrency mechanism or persistent database transaction.

## Respond to a failure

Retain the current view and display the returned status. A capacity refusal requires an explicit owner-retirement workflow; resetting a counter in place would reuse observation identities. An invalid book requires investigating how caller-owned storage became malformed. Do not remove invalid rules silently to make the command succeed.

A quote update that cannot evaluate a matching alert book returns the child failure before replacing the quote. A book at its final revision with no eligible rules is still a no-op and does not prevent unrelated quote publication.

## Owner qualification

After integrating and configuring Framework, run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-alert-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.]trading_execution[.]alert[.]" --output-on-failure
~~~

The expected result is 38 passing cases. They cover ordinary/final/exhausted book revisions, malformed retained rules, no-op evaluation, occurrence saturation, staged removal, empty child owners and injected allocation failure.

The fixture uses the real workspace and tape implementations, with a local allocation seam enabled only around the tested removal. No production fault-setting API is added. These checks are authored and statically reviewed; compiler, linker, runtime and native alert presentation remain owner-qualified.

Continue with [workspace mutation admission](workspace-mutation-admission.md) and [trade-tape capacity](trade-tape-capacity.md).
