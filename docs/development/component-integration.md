# Component integration: one test definition, more than one build route

Umicom Framework · Sammy Hegab · Umicom Foundation · MIT

## Purpose

Market Tape, Research Replay and the read-only IBKR Connection can be composed
only after their canonical dependencies exist. The complete Applications build
requests their integration early and finishes it through callbacks scheduled in
the root CMake directory. A standalone example can declare those dependencies
first and include the same component immediately. Both routes must work.

The original component definitions created test subdirectories. That is legal
during ordinary traversal but not during deferred execution. A successful
standalone build therefore did not establish that the complete integration
route was valid. The new regression host explicitly reproduces that difference.

## Registration contract

The existing `*Integration.cmake` callbacks, include guards, dependency checks
and public target names are unchanged. Each component includes its own test
list instead of asking CMake to create a late directory. The former
`add_subdirectory` statement is retained inside an explained `if(FALSE)` block.

Test sources are rooted at the list file that declares them, for example:

```cmake
add_executable(umicom-market-tape-test
    "${CMAKE_CURRENT_LIST_DIR}/test_tape.c")
```

This is essential: including a list does not turn its location into a new source
directory scope. Relative names such as `test_tape.c` would otherwise refer to
the including directory. Existing names, arguments, labels, timeouts, compiler
conditions, assertions and native regression source files are retained.

Tests now belong to the including directory's CTest scope. Use their stable
`framework.market_tape.*`, `framework.research_replay.*` and
`framework.ibkr_connection.*` identities, not an assumed old binary subdirectory,
to select them. Generated object and CTest file locations can consequently move.
The old generated build directory does not need to be erased: configure again
and let CMake regenerate its own files. Do not edit `build.ninja` by hand.

## Ten configuration checks

`UmicomComponentIntegrationChecks.cmake` registers these only when
`BUILD_TESTING` is enabled. It also makes the public-API lesson available after
all three components exist. It does not add alternative production services.

| Case | What is exercised |
|---|---|
| `deferred` | The real integration callbacks are requested in a child directory before canonical targets exist. |
| `immediate` | All three production component definitions are included after their dependencies exist. |
| `mixed` | One component is composed immediately and the others through deferred callbacks. |
| `repeated` | Repeated includes and explicit completion calls do not duplicate targets or tests. |
| `missing` | A minimal host without the required targets does not gain substitute components. |
| `partial` | A host with the IBKR dependencies, but not all market/research dependencies, composes only the available component. |
| `tests_off` | An explicitly test-disabled host builds its tools without registering tests. This is a compatibility check, not the recommended repair. |
| `reconfigure` | One build tree alternates between immediate and deferred composition four times. |
| `regenerate` | Editing a private copy of the probe's CMake file forces regeneration during the next build. |
| `installed` | An independently configured client links and runs against the installed focused SDK. |

Every test creates a new randomly named directory under
`component-integration-checks`, preserves its logs, and compares the exact
platform-applicable test inventory with `expected-tests.txt`. The inventory was
captured from the original components under a valid immediate configuration.
Counts alone would not catch replacement of a missing test by an unrelated one.

The cases invoke CMake/CTest using argument lists. They do not run a shell
command assembled from a log or connect to a real broker. Native test bodies
are the existing canonical tests. The two inherited Linux socket tests use an
inert loopback peer; requested Paper and Live modes do not imply an IBKR login.

Nested runs set `PROBE_REGISTER_MATRIX=OFF`; otherwise each integration test
could register and recursively run the same ten tests again. Each outer case
reserves two CTest processor slots and its build uses two jobs. Logs and build
products are intentionally retained, so repeated runs consume disk space.
Remove only known disposable build outputs after reviewing them, never source
or an installation selected by a user.

## Toolchains and verification limits

The host propagates its selected generator, C compiler or toolchain file, and
applicable platform/toolset. It explicitly selects Release for child builds and
CTest, including multi-configuration generators. When cross-compiling, native
execution is not attempted; an `execution-not-run.txt` record makes that limit
visible. This laboratory does not claim a cross-built executable ran on its
target or that every arbitrary toolchain variable is propagated.

`PROBE_SANITIZERS=ON` instruments the parent dependency subset and its native
regressions. The ten configuration cases create separate Release builds; those
child builds are not silently presented as sanitizer-instrumented. Full-product
sanitizer policy remains owned by the complete Framework configuration.

The focused host compiles canonical status, threading, quotes, market quality,
broker mapping, backtest and hashing sources with the three actual components.
It is not the full Framework library. The GTK adapters are not included in this
headless qualification and their lifecycle, accessibility and native startup
checks remain separate. No production runtime `.c` or public `.h` file changes
in this delivery.

## Complete C example

`examples/component_integration/main.c` links all three public components. It
builds an OHLCV bar from four fictional trade ticks, processes a historical
long/flat strategy with the replay service, then validates Paper and Live
read-only options without creating a connection. The replay owns a copied
input dataset; the example owns its tape snapshot. Both opaque services and
the snapshot are released on the common exit path.

The callback only requests a simulated position. It has no broker handle. A
paper/live configuration result is neither account-environment attestation nor
permission to transmit an order. The example is a reproducible consumer, not a
new operational trading application.

## Reading

- Public lesson: `../learning/build-integrated-components.html`.
- CMake deferred calls: <https://cmake.org/cmake/help/latest/command/cmake_language.html#deferring-calls>.
- Source directories: <https://cmake.org/cmake/help/latest/command/add_subdirectory.html>.
- List-file location: <https://cmake.org/cmake/help/latest/variable/CMAKE_CURRENT_LIST_DIR.html>.
