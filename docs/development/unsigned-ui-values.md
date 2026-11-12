![Umicom](../../resources/brand/umicom-logo.svg)

# Present unsigned observations without losing digits

Many owner revisions use `uint64_t`. The existing UI value layout has a signed integer and a string kind, but no unsigned integer kind. Casting a large revision to `int64_t` does not preserve its intended numeric meaning. Converting it to a floating-point value can lose individual digits.

`UmiUiValueSetUnsigned` uses the existing kinds without changing the ABI-stable value layout:

| Input | Stored UI kind | Meaning |
| --- | --- | --- |
| 0 through INT64_MAX | INTEGER | Exact signed representation of the nonnegative value |
| INT64_MAX + 1 through UINT64_MAX | STRING | Exact decimal digits, without sign, grouping or leading zeroes |

The function never uses REAL. A property adopting this contract must allow both integer and decimal-string observations. Consumers should format these as identity values rather than performing arithmetic on a display field.

~~~c
UmiUiValue revision;
UmiStatus status = UmiUiValueSetUnsigned(&revision, owner_revision);
/* Check status, then publish the value through the existing view model. */
~~~

The function copies its result into caller-owned storage and returns INVALID_ARGUMENT for a null destination. Zero uses the integer kind. Replacing an earlier large string value with zero clears the previous string through the existing value initializer.

## Trading projections

The following properties use the shared conversion:

- `trading.revision`
- `context.workspace-revision`
- `research.workspace-revision`

Their values identify the trading workspace owner, as described in [observation revisions](trading-observation-revisions.md). They do not authorize a command and do not combine chart or trade-tape revisions.

The generic GTK property renderer already handles both kinds: it formats integers as decimal text and copies strings as display text. This is source evidence of a compatible rendering path, not a newly observed GUI result. External consumers that assumed these properties are always signed integers must accept the documented string form for the upper half of the unsigned range.

## Owner qualification

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-unsigned-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]trading_execution[.]unsigned[.]"
~~~

Run commands separately and stop on errors. The authored checks cover zero, decimal boundaries, values around the exact-integer limit of common binary floating-point, the signed limit and the unsigned maximum. They also create the trading view models at small and large revisions and inspect their property kinds and exact values.

This target exercises shared values and view factories; it does not open a native window. Compiler, linker, test and GUI results remain unverified until the owner executes the relevant qualification.

Keep numeric input and identity display contracts separate. A displayed decimal string is not permission to parse it back and mutate the owner's revision.
