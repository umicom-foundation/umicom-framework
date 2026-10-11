# Umicom Framework — Authoritative Suite Registry Reconciliation

Author and project lead: Sammy Hegab  
Organisation: Umicom Foundation  
Language: C23; stable C ABI  
Source owner: `umicom-framework`  
Integrated workspace: `C:\umicom\Umicom-Applications\framework`

## Why this extension is necessary

The Framework already owns suite planning, per-member start/stop/exit evidence,
an Integration Registry, native application discovery, an application launcher,
process supervision and Umicom Desk state. **None of those systems should be
reimplemented in an application module.**

The existing suite methods record discrete observations, but the owning host
may also have a complete, trusted registry snapshot after processing a series
of starts, exits, disables or reconnections. Without a single reconciliation
operation, a host has to issue many independent per-member callbacks and may
leave a suite partly updated if one observation is invalid.

This update introduces **one atomic reconciliation operation** on the existing
`UmiIntegrationSuiteRuntime` and its private copy of the existing launch plan.
It introduces no new process supervisor, app catalogue, network daemon, GUI,
file format or product-specific service.

## Public API

```c
UmiStatus umi_integration_suite_runtime_reconcile(
    UmiIntegrationSuiteRuntime *runtime,
    const UmiIntegrationRegistry *registry);
```

The caller provides the existing trusted registry, already updated by the real
process owner. This operation is **read-only to the registry** and publishes
only a complete, validated suite runtime. The suite retains its membership,
front-end preferences, IDs and public structure layout.

A successful `UMI_STATUS_OK` means the snapshot was accepted, **not** that a
suite is ready. Inspect `runtime->state` and
`umi_integration_suite_runtime_is_usable()`; use
`umi_integration_health_from_runtime()` for a current-status display.

### State interpretation

| Registry member state | Suite observation | Counts as running? |
| --- | --- | --- |
| Absent, UNKNOWN or DISCOVERED | Required/optional missing | No |
| Disabled | Disabled and unavailable | No |
| AVAILABLE | Ready to request launch | No |
| STARTING | Observed starting | No |
| RUNNING | Observed running | Yes, after host validation |
| STOPPING | Observed stopping | No |
| STOPPED | Observed cleanly stopped | No |
| FAILED | Observed failed | No |

The optional/required absence totals and ready/running/failed totals are
recomputed from the complete snapshot. Repeat reconciliation is idempotent.
Malformed registry bounds, duplicates, identifiers, states, suite membership
or types are rejected before any caller-owned runtime is changed.

A required disabled/missing/failed member makes the suite unusable. Optional
members may be missing while all required services remain usable, in which
case the runtime is DEGRADED. A STOPPING member never retains its RUNNING
contribution. A subsequent *confirmed* exit can still be recorded by the
existing `umi_integration_suite_runtime_mark_exit()` method.

## Trust boundary: what this does **not** prove

This function does not inspect executable formats, verify signatures, read
provider credentials, perform IPC, launch a child or authenticate a service.
An OS process running is not proof of an application-specific readiness
handshake. The host must first authenticate any process/service identity and
reject stale process tokens through its existing Desk and launch-receipt
contracts. Registry updates and suite reconciliation must run in a coherent,
serialised ownership context. The function itself cannot reject stale external
snapshots, because the current registry has no monotonic source generation.

This is an intentionally narrow, useful correctness step towards end-to-end
inter-application launch/readiness. A later feature must supply an authenticated,
replay-resistant readiness protocol before making stronger service claims.

## Practical use

```c
/* The process owner updates the EXISTING Integration Registry only after
 * verifying the current application identity and its lifecycle evidence.
 * registry and suiteRuntime are owned by the same serialised service. */
UmiStatus status = umi_integration_suite_runtime_reconcile(
    suiteRuntime, registry);
if (status == UMI_STATUS_OK) {
    UmiIntegrationHealthSummary summary;
    umi_integration_health_from_runtime(suiteRuntime, &summary);
    /* A GUI may display summary, but it must distinguish process state
     * from authenticated service readiness. */
}
```

## Validation and ownership

The existing CTest target `umicom-integration-suite-runtime-tests` now tests:
registry admission, startup, required-member gating, optional degradation,
stopping and confirmed clean exit, error/recovery, duplicate and malformed
records, bounded inputs, idempotence and all-or-nothing publication.

The source target remains `Umicom::integration` in the canonical Framework
repository. The original lifecycle implementation is retained in its existing
`#if 0` review block. No application, Kernel or OS implementation is deleted.

Because the source update changes Framework, build and test the pinned
Umicom Applications checkout first; commit and push Framework; then use the
native `umicom repo lock .` to stage updated submodule pointers in the
Applications and OS parents, and commit/push those parents separately.
See `SHARED_FRAMEWORK_SUBMODULE_UPDATE_POLICY.md`.
