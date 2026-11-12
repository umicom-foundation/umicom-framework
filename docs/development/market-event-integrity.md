![Umicom](../../resources/brand/umicom-logo.svg)

# Admit market events before changing retained state

A market identifier locates a record. It does not prove that two records describe the same contract. For example, two events could carry the same identifier but disagree about the currency or contract multiplier. A chart or public-trade tape must not silently attach that event to the existing contract.

Framework's workspace uses `UmiRiskInstrumentMatches` for quotes, depth, bars and public trades. The shared matcher validates both instruments and compares the identifier, symbol, venue, currency, multiplier and expiry. Application clients submit complete values and inspect the returned status; they do not duplicate the matching rules.

## Publish a bar or public print

1. Construct the event in temporary storage.
2. Supply the registered instrument's complete contract metadata.
3. Call `umi_trading_workspace_update_bar` or `umi_trading_workspace_update_trade`.
4. Update presentation only after an accepted result.
5. On refusal, preserve the prior chart, tape and selected instrument while explaining the returned status.

A valid unknown identifier returns `NOT_FOUND`. Invalid fields or different metadata under an existing identifier return `INVALID_ARGUMENT`. Validation and matching precede revision-capacity admission and any change to bar history, the shared tape, market readiness or retained rows.

A bar at the last retained start time updates the forming candle. A newer bar appends to bounded history. An older start time returns `INVALID_STATE`. These existing time policies are unchanged. The generic bar numeric validator still permits finite negative prices; instrument registration is an additional workspace requirement, not a replacement price policy.

## Understand a public tick's numeric contract

`umi_trade_tick_valid` checks a non-null value, a finite positive price, a finite nonnegative size and a nonnegative event time. Zero size and time zero remain allowed. The function validates numeric/time fields; the retaining owner separately validates the instrument and its sequence policy.

The finite checks matter because positive infinity satisfies ordinary comparisons such as `price > 0` and `size >= 0`. NaN and infinity must be refused before they become retained market evidence. Do not convert them to zero as a silent repair.

~~~c
if (!umi_trade_tick_valid(&tick)) {
    /* Explain malformed numeric/time input; retain the previous observation. */
} else {
    UmiStatus status = umi_trading_trade_tape_record_init(
        &record, next_sequence, &tick,
        UMI_TRADING_TRADE_DIRECTION_UNKNOWN, "");
    if (status == UMI_STATUS_OK) {
        status = umi_trading_workspace_update_trade(workspace, &record);
    }
    /* Report status before presenting the candidate as accepted. */
}
~~~

The tape also checks directly populated public records. A provider cannot bypass the numeric contract by skipping `record_init`. This protects the shared store, but it does not authenticate the provider, establish freshness or make a public trade an account execution.

## Validate an enum before accepting a repeat

Market state is an enum: a set of named values. Casting a number to that type does not prove it names a supported state. The transition helper first checks both values, then applies this table:

| Current state | Accepted next states |
| --- | --- |
| Closed | Closed, Preopen |
| Preopen | Preopen, Open, Closed |
| Open | Open, Halted, Closed |
| Halted | Halted, Open, Closed |

An unknown value remains invalid even when the old and new numbers are equal. If a retained workspace contains such a value, a repeated observation is refused without changing it. Investigate the source of that state and restore through a reviewed lifecycle; do not relabel an unknown value as Open.

## Diagnose the boundary that refused the input

For metadata mismatch, compare the complete provider mapping with the registered contract. For invalid numbers, retain the original diagnostic and correct the producer. For an invalid transition, compare both states with the declared table. For exhausted revision capacity, follow [workspace mutation admission](workspace-mutation-admission.md).

The owner retains prior state on these refusals. This does not imply that every possible producer, concurrency pattern or restored-data path is qualified. Adapters remain responsible for their feed-specific ordering, authentication and timestamp conventions.

## Owner qualification

After integrating and configuring the matching Framework source, run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-market-event-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.]trading_execution[.]market-event[.]" --output-on-failure
~~~

The expected result is 94 passing cases. Coverage includes complete metadata mismatches, malformed bounded fields, missing/null input, forming/older bars, finite tick boundaries, direct tape admission, all declared-state pairs, unknown enum values and a damaged retained state. Refusal cases compare the complete workspace and its separately owned tape where applicable.

The source and tests are authored and statically inspected. Compilation, linking, execution and native Trader presentation remain owner-run and unverified. Start the practical continuation with the Bits2Banking trading course's market-observation laboratory, using an installed SDK from the same qualified source.
