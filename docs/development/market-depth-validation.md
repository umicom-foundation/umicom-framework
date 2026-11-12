![Umicom](../../resources/brand/umicom-logo.svg)

# Validate a market-depth snapshot

A depth snapshot carries an instrument and bounded arrays of bid and ask levels. Before a consumer looks up that instrument or reads prices, it must know that the identity is valid and the active levels contain usable numbers.

Framework keeps this validation in `umi_market_depth_valid`. The workspace calls it before comparing the instrument identifier. Code that constructs depth values directly receives the same checks as a provider adapter.

## What the shared validator checks

- The instrument satisfies the canonical instrument contract, including bounded, terminated text and a finite positive contract multiplier.
- Bid and ask counts fit their fixed arrays.
- Every active level has a finite price.
- Every active size is finite and nonnegative.

The validator examines only levels below their corresponding counts. Unused array tails are not active market evidence. A snapshot may have no bids, no asks or neither, provided its instrument is valid.

A finite negative price is allowed. A zero price or zero size is also allowed. This generic data contract does not impose a positive-price risk model on every instrument. Risk evaluation and an adapter's accepted feed conventions remain separate responsibilities.

## What it does not prove

The validator does not sort levels, reject crossed books, require monotonic event times or establish freshness. An adapter must declare and enforce those policies for its feed. The existing best-bid and best-ask accessors read the first active level; they do not search for a better price elsewhere in the array.

Both accessors return zero when the input is invalid or the requested side is empty. Zero can also be a valid price. Inspect validity and the side count before using that result when absence matters.

~~~c
if (!umi_market_depth_valid(&depth)) {
    /* Reject the snapshot before any instrument lookup or price use. */
} else if (depth.bid_count == 0U) {
    /* No bid is available in this snapshot. */
} else {
    double first_bid = umi_market_depth_best_bid(&depth);
    /* The adapter owns the ordering contract for this first level. */
}
~~~

## Publish into a workspace

1. Construct a complete instrument and active levels in temporary storage.
2. Validate the snapshot with the shared function.
3. Pass it to `umi_trading_workspace_update_depth`.
4. Inspect the returned status before updating the frontend.
5. Retain the prior displayed depth when the workspace refuses the value.

The workspace compares the full validated instrument against the existing market using the same metadata matcher used for quotes. Reusing an identifier with a different symbol, venue, currency, multiplier or expiry is rejected with `INVALID_ARGUMENT`. A valid unknown identifier returns `NOT_FOUND`.

After validation and lookup, the workspace reserves its own revision and the market-row revision before publication. Rejected depth leaves the complete owner unchanged. Successful publication does not establish that the data came from an authenticated or current provider.

## Diagnose a rejection

For `INVALID_ARGUMENT`, inspect bounded identity fields, the contract multiplier, active counts and active price/size values. If standalone validation succeeds but workspace publication rejects the value, compare its contract metadata with the instrument already stored in the workspace.

Do not repair an identity mismatch by silently replacing the stored contract. Resolve the provider mapping explicitly. Do not substitute zero for NaN or infinity and call that a validated market value.

For `CAPACITY_EXCEEDED`, use the workspace's retirement workflow described in [mutation admission](workspace-mutation-admission.md); this is a separate observation-capacity failure.

## Owner qualification

After integrating and configuring Framework, run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-depth-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.]trading_execution[.]depth[.]" --output-on-failure
~~~

The expected result is 39 passing cases. They cover malformed bounded strings, invalid counts, nonfinite prices/sizes, negative sizes, contract mismatches, missing instruments and null input. Positive cases preserve finite negative prices, zero values, empty sides, inactive tails, unordered/crossed values and negative timestamps because those policies are deliberately outside this validator.

These checks are authored and statically reviewed. Compiler, linker, test execution and native depth rendering remain owner-qualified. An accepted structural value is not evidence that a provider's market-data policies or financial calculations have been qualified.
