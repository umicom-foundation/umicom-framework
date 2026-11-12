![Umicom](../../resources/brand/umicom-logo.svg)

# Distinguish a valid order policy from a supported one

An enum validator answers whether a request uses a declared value. It does not prove that the selected service implements that value's behavior. Framework's canonical request vocabulary includes DAY, GTC, IOC and FOK. The local broker and retained trading workspace do not provide an immediate matching transaction.

The shared `UmiTimeInForceCheckLocalRetention` helper makes that boundary explicit. The existing `umi_time_in_force_valid` validator is unchanged, so other providers can continue validating requests independently of this local capability.

| Policy | Local retention result | Responsibility |
| --- | --- | --- |
| DAY | May retain a resting request | Caller owns session-end cancellation; no automatic calendar expiry |
| GTC | May retain a resting request | Caller controls cancellation and session lifetime |
| IOC | NOT_IMPLEMENTED | Immediate execution and remainder handling are absent |
| FOK | NOT_IMPLEMENTED | An immediate all-or-none matching transaction is absent |
| Undeclared enum value | INVALID_ARGUMENT | Correct the request before selecting a provider |

Retaining a DAY or GTC request does not establish venue scheduling, exchange matching, persistence across restarts or a broker acknowledgement. The existing local semantics remain explicit rather than silently converting IOC/FOK into resting orders.

## Call the shared boundary

~~~c
#include "umicom/trading/time_in_force.h"

UmiStatus capability = UmiTimeInForceCheckLocalRetention(request.tif);
if (capability != UMI_STATUS_OK) {
    /* Explain the unsupported local capability; do not rewrite the request. */
}
~~~

A complete client still checks request validity, environment, health, risk and the operation's return status. This helper performs no I/O and grants no execution permission.

## Observe the broker and workspace behavior

1. The simulation broker validates the request and connected mode.
2. It checks local retention capability before adding an order.
3. A refused broker callback preserves its stored orders and connection.
4. Both workspace preview entry points and submission use the same capability check.
5. For an unsupported workspace policy, they retain a denied preview with no valuation, advance the available observation revision once and return NOT_IMPLEMENTED.
6. They leave the order draft, next order identity, OMS throttle, orders, positions and executions unchanged.

The submit affordance uses the same capability predicate. Preview remains a way to explain the refusal. Direct API callers must still check the returned status because UI state is not authority.

The workspace admits its revision before publishing preview evidence. Submission also admits the next order sequence before changing the ticket or OMS. At an exhausted counter it returns CAPACITY_EXCEEDED without changing the owner or output decision. Do not reset the counter inside the same session to hide exhaustion.

## Qualify the behavior

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-policy-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]trading_execution[.]policy[.]"
~~~

Run each command separately and stop on errors. The regression cases cover enum validity versus capability, both local broker modes, both previews, direct submission, available and exhausted identities, and retained denial evidence. They use synthetic requests and the real owner implementation for counter injection.

These checks are authored and await owner execution. Static review does not establish compiler, linker, native UI or runtime results.

Inspect [time_in_force.c](../../src/trading/time_in_force.c), [simulated_broker.c](../../src/trading/simulated_broker.c) and [workspace.c](../../src/trading/workspace.c). Continue with [execution publication](execution-publication.md) and [cancellation](order-cancellation.md) for the supported local lifecycle. An external adapter must report and implement its own capabilities; this local helper is not a venue capability registry.
