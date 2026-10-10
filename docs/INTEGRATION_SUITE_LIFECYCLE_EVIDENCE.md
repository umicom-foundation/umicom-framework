# Umicom Framework — Integration Suite Lifecycle Evidence

Author and project originator: Sammy Hegab
Organisation: Umicom Foundation
Primary language: C23 · Stable C ABI · Toolkit-neutral

## Purpose

An application suite is a named set of required and optional Umicom products.
For example, a financial suite can contain Umicom Bank and Umicom TMS as
required members and Umicom Trader as optional. Whether a product is present in
the installation is **not** proof that its executable was started, its native
window appeared, its IPC channel is authenticated, or its services are ready.

This update corrects the existing Framework Integration Fabric's lifecycle
accounting, without creating a new platform repository or a second application
launcher. The authoritative public interfaces remain:

- `include/umicom/integration/launch_plan.h`;
- `include/umicom/integration/suite_runtime.h`;
- `include/umicom/integration/health.h`.

The owning implementation remains under `src/integration/`. `Umicom Desk` and
individual application modules may use these contracts, but process ownership
is still handled by the existing Framework application launcher and its native
platform adapter.

## What was wrong with the previous state

The pre-existing `umi_integration_suite_runtime_mark_running()` incremented a
count for every call, even for the same application. Repeating the same callback
could therefore claim two active required members from one process. Moreover,
`umi_integration_suite_runtime_is_usable()` accepted a PREPARED suite whose
required members had not actually been observed running.

A Suite Runtime is **not** an API for placing financial orders, authorising bank
payments or starting an operating system. It may only project observations made
by the lifecycle owner.

## Data flow

```text
          Umicom Applications suite composition
                         |
                         v
              Framework integration registry
                application IDs / policy
                         |
                         v
              Framework launch plan builder
              REQUIRED | OPTIONAL | DISABLED
                         |
                         v
                  Suite Runtime copy
           PREPARED (no required evidence)
                         |
           process supervisor observations
                  /              \
       running(member ID)      failed(member ID)
                  \              /
                   v            v
                per-member observation
                         |
                         v
              recomputed unique counters
                         |
          +--------------+----------------+
          |              |                |
       STARTING        RUNNING          FAILED
          |              |
          |         DEGRADED if optional
          |           members unavailable
          v
   does not satisfy
    suite usability
```

## Important lifecycle rules

1. The builder checks the full capacity of suite and registry records, required
   NUL termination, allowed dependency values and duplicate IDs **before** it
   publishes an output plan. Invalid manifests leave caller output unchanged.
2. A required member missing from the registry still returns
   `UMI_STATUS_UNAVAILABLE` **with a complete diagnostic plan**. This preserves
   the established planning contract.
3. A member already observed RUNNING in the registry is counted **once** at
   preparation. A member merely READY to launch is **not** running.
4. `mark_running()` and `mark_failed()` change only the private copy of the
   launch plan held inside one `UmiIntegrationSuiteRuntime`. The independently
   built plan is not modified.
5. Duplicate notifications return `UMI_STATUS_INVALID_STATE`. They do not
   modify any counters or statuses. Failure after running removes exactly
   that member's running evidence.
6. A later, **explicit** running observation may recover a previously failed
   member; the Framework never silently retries a broker operation or financial
   transfer. Recovery is evidence supplied by the owning supervisor.
7. The suite is usable only when **all required members** have running evidence
   and **at least one member** is running. Missing or failed required members
   make the suite unusable. Failed or absent optional members may make an
   otherwise running suite DEGRADED.
8. The public `UmiIntegrationSuiteRuntime` and
   `UmiIntegrationHealthSummary` structure sizes are unchanged. The two new
   disposition enumerators are appended; original numeric values are retained.
9. A health summary produced from a *launch plan* reports readiness to launch;
   a health summary from a *suite runtime's copied plan* also accounts for
   observed failures and running processes. Neither indicates GUI/IPC readiness.
10. All functions are synchronous C23 value operations. They do not allocate
    background tasks, invoke shell commands, start another application or modify
    saved state. The caller must serialise access on the owning thread.

## Source preservation

Superseded implementations are retained behind documented `#if 0` blocks in
`launch_plan.c`, `suite_runtime.c` and `health.c`. They remain available for
review and are not compiled or executed. Existing comments, authorship,
public identifiers and exported symbols are retained. No automatic repository
or submodule migration is performed.

## Relevant tests

Run the three existing Framework CTest cases from the *integrated*
`C:\umicom\Umicom-Applications` workspace:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'

cmake --preset windows-ucrt64-all-debug -DBUILD_TESTING=ON

cmake --build --preset windows-ucrt64-all-debug `
  --target umicom-integration-launch-plan-tests `
           umicom-integration-suite-runtime-tests `
           umicom-integration-health-tests `
  --parallel 2

ctest --preset windows-ucrt64-all-debug `
  --output-on-failure `
  -R '^framework\.integration\.(launch_plan|suite_runtime|health)$'
```

These tests exercise malformed input refusal, missing required and optional
members, multiple required processes, repeated notifications, process failure,
explicit recovery, pre-existing running evidence, optional degradation and
health projection. The test doubles do not execute an operating-system process.

## Still outstanding

- Attach the Framework process launcher and IPC transport to the suite member
  supervisor, and use real process tokens and health handshakes.
- Add a per-member stop/restart receipt and check that it agrees with the
  launcher and the application runtime catalogue.
- Provide a read-only, installation-level executable verification report by
  consuming the established native discovery API rather than introducing a
  competing application registry.
- Execute Windows UCRT64, Linux GTK4, Studio, Trader and TMS integration smoke
  tests on the user's real checkout; those end-to-end results cannot be inferred
  from this focused suite-value test.

## Architectural ownership

The owner of this fix is `umicom-framework` at
`C:\umicom\Umicom-Applications\framework`. Only its Git submodule commit
pointer changes in the `Umicom-Applications` parent; `umicom-kernel` and
`umicomOS` are left alone. No `umicom-platform` clone or directory is required.
