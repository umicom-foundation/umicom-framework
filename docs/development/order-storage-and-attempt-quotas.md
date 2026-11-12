![Umicom](../../resources/brand/umicom-logo.svg)

# Order identity storage and submission attempt quotas

The reference order management system (OMS) stores bounded order records for simulations and tests. Framework owns its identity lookup and fixed-window rate limit so clients can share the same rules. These services do not submit an order to a broker.

## Look up an order without exposing malformed storage

1. Initialize a complete UmiOrderStore with **umi_order_store_init**.
2. Supply an order with a terminated, nonempty client order identifier.
3. Add it through **umi_order_store_add**. The store validates every active identifier and rejects duplicates before appending.
4. Use **umi_order_store_find_const** for read-only access. Use the existing mutable lookup only when the caller owns a legitimate lifecycle mutation.
5. Keep the store alive and serialize access for the complete use of any borrowed pointer.

~~~c
const UmiOrder *order = NULL;
UmiStatus status = umi_order_store_find_const(store, "ORDER-1", &order);
if (status == UMI_STATUS_OK) {
    /* Read the borrowed order while its owner remains alive and serialized. */
}
~~~

Storage validation checks counts, identity termination and uniqueness. It does not decide whether an order's price, quantity, risk or lifecycle state is acceptable. Those policies remain in request validation, risk evaluation and the lifecycle services. A valid identity is not execution permission.

The lookup scans at most the identifier field capacity. Supply readable bytes through a terminator or that capacity. A missing terminator returns **CAPACITY_EXCEEDED**; an empty query returns **INVALID_ARGUMENT**. An oversized count, duplicate active identity or malformed active identity returns **INVALID_STATE**, even if an earlier row matches.

For non-null arguments and separate output storage, lookup failure writes a null output pointer, preserving the existing mutable API behavior. Missing arguments and output storage overlapping the store are rejected without writing that output. Query bytes are captured before output publication, so a query may borrow a separate output allocation.

Inactive rows are ignored. Appending a valid specification borrowed from an inactive slot captures it before changing the destination. In a valid full store, an existing identifier returns **ALREADY_EXISTS** and a new identifier returns **CAPACITY_EXCEEDED**. Refused additions leave the store unchanged.

## Treat timestamp zero as real time

A fixed-window throttle admits at most max_count attempts during its configured interval. For a limit of two attempts per 1,000 milliseconds:

| Time | Result | Accepted count |
| --- | --- | --- |
| 0 | accept | 1 |
| 0 | accept | 2 |
| 0 | refuse | 2 |
| 999 | refuse | 2 |
| 1000 | accept in a new window | 1 |

The accepted count distinguishes an unused throttle from a window that started at zero. The former zero-time sentinel is retained in disabled source for comparison because it restarted the window on repeated zero-time calls.

Use nonnegative timestamps from one monotonic clock domain. The implementation rejects a timestamp earlier than the active window start before subtracting. With nonnegative ordered int64_t operands, elapsed subtraction is representable. Invalid configuration, negative stored time, count above the configured maximum or exhausted quota returns zero without mutation. A zero count starts a new window at the next accepted timestamp.

The record stores the window start, not the most recent observation. It cannot detect every backwards movement inside a window. A caller needing that stronger guarantee must enforce it in its clock owner. This service supplies no lock or distributed rate-limit coordination.

## Understand what consumes the quota

The OMS checks the kill switch before the throttle. An engaged kill switch therefore consumes no attempt. Once the throttle accepts, later request-validation, risk or storage refusal still consumes that attempt. This existing behavior limits submission attempts rather than accepted orders. The repair preserves it.

Do not move the throttle after validation merely to make an invalid-request test pass. That would change the policy. The regression intentionally sends an invalid request at zero, observes one consumed attempt, then expects a second request at zero to return **BUSY**. A later window can admit another attempt.

## Qualify the changes

From the Framework source root, run each command separately and stop on errors. Use the repository's documented toolchain environment and prerequisites.

~~~powershell
cmake -S . -B build/order-admission -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build/order-admission --target umicom-trading-order_storage_admission-test umicom-trading-order_throttle_boundaries-test umicom-trading-workspace_order_lookup-test
ctest --test-dir build/order-admission -R "^framework[.]trading[.]admission[.](order_storage_admission|order_throttle_boundaries|workspace_order_lookup)$" --output-on-failure
~~~

<!-- The previous broad CTest selector is retained for command-history review:
ctest --test-dir build/order-admission -R "^framework[.]trading[.]admission[.]" --output-on-failure
The explicit selector above covers exactly the three targets built in these steps;
additional market lifecycle checks now share the admission namespace. -->

The three executables cover full-store precedence, malformed later records, oversized counts, query byte limits, borrowed input, overlapping output refusal, zero-time quotas, signed-time boundaries, OMS attempt accounting and workspace selection. They perform memory-only operations. Expected successful results are not an assertion that the suite has been executed.

The workspace's existing index helper maps lookup refusal to no match, preserving its selection API. The public store API exposes the detailed reason. Returning no match safely is not a diagnosis of the rest of a damaged workspace.

## Practice diagnosis

Create a store with two valid rows, then remove the terminator from the second row's identifier in a controlled test fixture. Looking up the first row should now return **INVALID_STATE**. This demonstrates why an early match must not bypass admission of later active storage.

Next, repeat the throttle's zero-time example and compare every refused call against a copied prior record. Check both the status and state. Preserve the first failing input, source identity and test output when reporting a defect. A passing memory check establishes no broker connectivity, live-order execution or native trading interface behavior.

Continue with [market identity and chart lifetime](market-identity-and-chart-lifetime.md) for watchlist lookup and per-instrument navigation qualification.
