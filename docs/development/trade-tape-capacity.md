![Umicom](../../resources/brand/umicom-logo.svg)

# Handle capacity in a public trade tape

Time and Sales shows public market prints. These records are separate from executions belonging to an account. Framework retains a bounded window, checks provider sequence order, and keeps display filters and pause state in the same toolkit-neutral owner.

The tape has two different limits. Filling its row window discards the oldest retained print as documented. Exhausting its revision number does not discard a row: it refuses a new mutation before changing retained data or diagnostics.

## Preserve evidence before returning a refusal

The tape revision no longer restarts at 1 after `UINT64_MAX`. The following rules apply:

| Operation | Behavior at the maximum revision |
| --- | --- |
| Append a valid newer record | `CAPACITY_EXCEEDED`; records, sequence and gap/drop counters remain unchanged |
| Change filter or minimum size | `CAPACITY_EXCEEDED` with the previous view intact |
| Change pause state | `CAPACITY_EXCEEDED` with the prior pause boundary intact |
| Remove a retained instrument | `CAPACITY_EXCEEDED` before compaction |
| Repeat a valid filter/pause value | `OK` without changing state |
| Remove an absent instrument | `OK` without changing state |
| Observe provider availability | Retain the new flag and saturate the revision |

An invalid record or stale sequence retains its existing validation error. The owner checks those conditions before revision admission. A refused gap does not increment the missing-sequence count, and a refused append to a full row window does not increment the dropped-trade count.

Provider readiness remains observable because a failed connection must not stay displayed as ready solely because a counter is full. At the maximum revision, refresh the readiness field instead of treating revision equality as proof that nothing changed.

## Integrate the result in an adapter

1. Construct the complete record using `umi_trading_trade_tape_record_init`.
2. Pass it to the canonical append operation.
3. Publish success to the frontend only after receiving `OK`.
4. On invalid or stale input, report the specific validation failure and investigate the adapter's sequence or data conversion.
5. On revision capacity, stop publishing new data into that owner. Preserve the retained evidence and use an explicit session-replacement workflow.
6. Keep provider sequence continuity in the adapter. Creating a fresh tape does not prove that the provider stream contains no gap or replay.

~~~c
UmiStatus status = umi_trading_trade_tape_append(tape, &record);
if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
    /* Retained data is unchanged. Surface the failure and retire the owner
     * through the application's feed lifecycle; do not reset its counter. */
}
~~~

A paused tape still accepts ingestion when capacity permits; pause freezes the visible sequence, not the provider connection. A filter never deletes stored records. Those existing behaviors remain intact.

## Respect the outer workspace

The workspace reserves its own revision before changing a child tape filter or pause state. If the child refuses, the outer revision stays unchanged. Workspace provider-availability updates use the shared protective observation path and remain available when the outer or child revision is full.

A workspace command is still an outer edit even if its child operation would be a no-op. At exhausted outer capacity, filter and pause commands return `CAPACITY_EXCEEDED` before entering the child. Direct tape no-op semantics do not grant a new outer revision.

This focused change does not qualify all workspace market-data publication paths or unrelated chart registries.

## Qualify retained data and delegation

After integrating and configuring the Framework source, run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-tape-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.]trading_execution[.]tape[.]" --output-on-failure
~~~

The expected result is 29 passing cases. They cover ordinary/final/exhausted revisions, a full retained window, gap diagnostics, invalid and stale input, no-op commands, removal before compaction, readiness saturation and both workspace/child refusal boundaries.

The fixture includes the real opaque owners for fault injection. It does not add mutable testing fields to the public API. These cases are authored and statically reviewed; compiler, linker, runtime and native Time and Sales behavior remain owner-qualified.

Continue with [protective controls](protective-trading-controls.md) and [observation revisions](trading-observation-revisions.md).
