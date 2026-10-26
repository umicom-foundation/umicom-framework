# Edit planning cards with the installed C23 SDK

This example creates a synthetic board entirely in memory. It uses public Framework APIs
to edit private details, record acceptance assertions and link a prerequisite. It does
not read your development board, execute product tests, save files or publish anything.

1. Install an SDK containing the `kanban` component and its dependencies.
2. Configure this example directory with CMake and set `UMICOM_SDK_PREFIX` to the absolute
   installation prefix, for example `C:/umicom/installed/applications`.
3. Build `kanban-card-editing-consumer` and run CTest in that example build directory.
   <!-- Earlier three-scenario wording is retained for review; the public consumer now also covers labels and limits.
   Three scenarios are registered: `roundtrip`, `stale` and `cycle`. -->
   Four scenarios are registered: `roundtrip`, `stale`, `cycle` and `labels-limits`.
4. Run the executable with `roundtrip` to see a checklist prevent premature completion,
   then explicitly complete the synthetic acceptance and round-trip the board archive.
   The expected message is: `Synthetic card details, checklist and prerequisite survived archive roundtrip.`
5. Run with `stale` or `cycle` to inspect the refusal workflows. A stale form keeps the
   current board intact; a cyclic relationship cannot be introduced.

A successful example checks only these synthetic SDK interactions. An acceptance
checkmark is an owner's statement; it is not proof that a build or test completed.
Source identity, independent evidence review and application qualification are separate.

If a header, component or symbol is missing, check that the intended SDK prefix contains
the updated installation. Do not add source-tree include paths or fallback libraries to
make this installed-consumer check pass. No source files are compiled into the consumer
except its own `main.c`.

The `labels-limits` scenario adds private labels, refuses a stale edit and a list limit
below current occupancy, then reopens both labels and the accepted limit from a board
archive. It uses public headers only. This fourth scenario is also registered with
CTest. The expected message is: `Private labels and list limits survived the installed SDK archive roundtrip.`

The `labels-limits` workflow also preserves a deadline of `946684800` UTC seconds
through the archive. Deadline edits retain all other card fields. The public C23 model
stores whole unsigned UTC seconds, with zero meaning no deadline; the GTK date form
supports a narrower human-readable range and requires explicit clearing.
