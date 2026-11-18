![Umicom](../resources/brand/umicom-logo.svg)

# Acknowledge preview work without losing newer edits

The Framework frontend reload service records bounded path/revision changes.
A preview host can capture work, process it and acknowledge only the revisions
it actually applied. The queue retains changes that arrived during that work.

The service does not compile C, watch files, load GtkBuilder descriptions,
refresh CSS, manage child processes or replace live code. Applications supply
those operations and call the service from one serial owner.

## Follow one update

1. Initialise a UmiFrontendHotReload with reload enabled.
2. Queue each changed path with a revision that identifies its content.
3. Capture a UmiFrontendReloadBatch. No work is removed.
4. Process exactly that batch. Keep the last usable preview if processing fails.
5. Acknowledge only after successful application. Inspect the receipt's
   acknowledged and remaining counts.
6. If work remains, capture it for a subsequent update.

For example, a batch captures counter.c revision 10 and theme.css revision 20.
Before it finishes, counter.c becomes revision 11 and dialog.ui revision 30 is
added. Acknowledging the batch removes only theme.css revision 20. The newer
counter and dialog remain queued.

## Ownership and failure rules

The queue and batch contain bounded path arrays. Allocate them with awareness
of their size; a small thread stack is unsuitable for casually nesting several
copies. Framework performs no allocation in these queue operations.

The caller owns the queue, batch and receipt. Their storage must be disjoint.
Keep the batch unchanged until it is acknowledged or discarded. It borrows its
owner's identity and cannot survive moving or reinitialising that owner. The
pointer is process-local and must not be serialised.

All operations require external serialisation. They are not a lock-free queue
and do not permit unsynchronised worker access. Deliver a worker's result back
to the owning thread through the application's supported scheduling mechanism.

A second completion advances the generation. A competing batch captured under
the previous generation returns INVALID_STATE without changing the queue or
receipt. Invalid counts, unterminated bounded paths and duplicate entries are
refused before traversal or mutation. Generation overflow returns
CAPACITY_EXCEEDED. An empty capture returns NOT_FOUND and preserves its output.

A successful old batch may acknowledge zero paths if every captured path has
been edited again. The receipt reports the new generation and all remaining
work. A generation identifies bookkeeping completion, not a claim that the
latest source is visible in a GUI.

## Compatibility

The existing umi_frontend_hot_reload_complete function retains its synchronous
consume-all behaviour. It now validates bounded state and refuses generation
overflow. Existing callers are not silently switched to asynchronous semantics.

Asynchronous hosts should use capture and acknowledge. Reusing a revision for
different content cannot detect a newer edit. Path comparison remains exact
and case-sensitive; callers should use one consistent canonical path spelling.

## Qualify the service

A small test project compiles the production service directly:

~~~powershell
cmake -S tests/frontend_reload -B build/reload-checks -G Ninja
cmake --build build/reload-checks --parallel 2
ctest --test-dir build/reload-checks --output-on-failure
~~~

Run from the Framework repository root with a compatible C23 toolchain. The
cases cover interleaved edits, failed-work retention, competing completions,
all-newer content, invalid bounded state, capacity and legacy completion.

The Bits2Banking framework-preview-revisions lab supplies a complete consumer.
Its source-study mode and installed-SDK mode are separate qualification paths.
A GUI integration must additionally prove callback lifetime, main-context
delivery, runtime identity, failure feedback and close-during-update behaviour.
