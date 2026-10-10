# Umicom Framework — Integration Suite Exit, Restart and Health Evidence

Author and project lead: Sammy Hegab  
Organisation: Umicom Foundation  
Language: C23; stable C ABI  
Repository owner: `umicom-framework` (submodule `C:\umicom\Umicom-Applications\framework`)  
Source baseline: `d9737da7f2f69e3582c57d45e5facae6117b1437` (10 October 2026)

## 1. Purpose and existing implementation

The Framework already contains `UmiApplicationRuntimeCatalogue`, the application launcher,
Umicom Desk runtime, a process-supervisor service, launch receipts, native executable
installation discovery, and Integration Fabric suite planning. Those components are NOT
replaced by this work.

The preceding suite lifecycle correction prevented a duplicate RUNNING or FAILED callback
from inflating aggregate counts. However, a real product can later start, exit cleanly,
crash, or restart. An aggregate that never observes a confirmed exit may continue to
report a suite as usable after its required application has stopped. A plan's availability
summary also must not be mistaken for live service readiness.

This update extends **the existing suite runtime's copy of the launch plan** with new
observed member dispositions. No duplicate application registry, process supervisor,
socket implementation or service owner is introduced.

## 2. API additions

Existing public entry points, enum numeric values and public structure layouts are
retained. New functions use the established `umi_integration_` prefix to maintain
compatibility with existing Framework consumers:

| Operation | Meaning | Proof required from the host |
| --- | --- | --- |
| `umi_integration_suite_runtime_mark_starting` | A supervisor has started (or deliberately restarted) this member. | Accepted process start, not mere binary presence. |
| `umi_integration_suite_runtime_mark_running` | Existing API; a required member may now contribute to suite readiness. | A separate running/ready signal from the owning product supervisor. |
| `umi_integration_suite_runtime_mark_stopped` | A member has ended cleanly; it is no longer counted as running. | Confirmed exit of the current process instance. |
| `umi_integration_suite_runtime_mark_exit` | Zero exit -> stopped; nonzero exit -> failed. | Same confirmed current-instance check before call. |
| `umi_integration_health_from_runtime` | Copy a read-only summary of the live suite's observations. | An owned, coherent `UmiIntegrationSuiteRuntime` snapshot. |

A clean exit is not a failure, but a stopped required member is not RUNNING and
therefore the suite is not usable. A nonzero exit of a required member makes the
suite FAILED until the affected member is explicitly started and observed running
again. Missing required applications still prevent suite readiness. Optional
failures yield DEGRADED only if required applications are actually running.

New enum entries are **appended** after the pre-existing entries:
`UMI_INTEGRATION_LAUNCH_OBSERVED_STARTING` and
`UMI_INTEGRATION_LAUNCH_OBSERVED_STOPPED`.

The original `umi_integration_health_from_plan()` remains unchanged in purpose:
its `healthy` field relates to declared **plan availability**. A plan with a READY
item does not prove a running process. GUI or command-line views of actual suite
uptime should use `umi_integration_health_from_runtime()`.

## 3. How an owning Desk controller should use the result

The following is an **integration sketch**, not a claim that Studio, Trader, Bank
or TMS already sends a readiness handshake across processes:

```c
/* The existing Desk runtime must first accept the exact process-token exit.
 * This prevents an exit from an older Studio instance from stopping a newer one.
 * Assume `desk`, `suiteRuntime`, `applicationId`, `processToken`, `exitCode`
 * and `message` are supplied by their authoritative owning controller. */
UmiStatus status = UmiDeskRuntimeReconcileProcessExit(
    desk, applicationId, processToken, exitCode, message);
if (status == UMI_STATUS_OK) {
    status = umi_integration_suite_runtime_mark_exit(
        suiteRuntime, applicationId, exitCode);
}
/* Handle any non-OK status; a failure must not be ignored or treated as PASS. */
```

Before launching a required product, a host may accept an explicit STARTING
observation, but it must not mark the product RUNNING simply because the OS
created a process. A READY signal needs its own authenticated protocol and
health-gate contract. That is future Framework-owned integration work.

The public suite runtime does **not** store process tokens. It relies on the
current authorised application-process owner to refuse stale exit callbacks
before they are relayed. This batch does not add an IPC transport, modify UI
or silently enable live financial operations.

## 4. State illustration

```text
PLAN READY ── accepted launch ──> STARTING ── readiness proof ──> RUNNING
     │                               │                              │
     │ launch failure                │ early nonzero exit           │ exit=0
     ▼                               ▼                              ▼
   FAILED <──── exit nonzero ───── FAILED                      STOPPED
     │                                                          │
     └──── explicitly approved restart ──> STARTING <───────────┘
```

The full suite becomes RUNNING only when ALL required members are observed
RUNNING. An optional missing/failed member makes a **usable** suite DEGRADED.
Otherwise the suite is STARTING, STOPPED or FAILED as appropriate. Duplicate
lifecycle notifications return `UMI_STATUS_INVALID_STATE` without changes.

## 5. Source preservation and ownership

- Existing `#if 0` source-preservation blocks remain untouched and available
  for engineering review.
- Previously declared APIs and public structure sizes remain unchanged.
- Only the canonical Framework `src/integration` and public `include/umicom`
  implementations own these state transitions.
- No product-specific copies were added in Studio, Trader, Bank, TMS, OS or Kernel.
- The Kernel does not depend on Umicom Framework; the OS's root-level
  `framework/` is a submodule pointer to the same canonical Framework remote.

## 6. Validation gates

The existing CTest targets remain:

- `umicom-integration-suite-runtime-tests` / `framework.integration.suite_runtime`
- `umicom-integration-launch-plan-tests` / `framework.integration.launch_plan`
- `umicom-integration-health-tests` / `framework.integration.health`

Regression scenarios exercise prepared, starting, running, degraded, failed and
stopped states; optional failures; stopped required members; clean/nonzero
exits; explicit recovery; duplicates; malformed bounds; and the difference
between plan availability and observed runtime health.

The focused source subset was compiled with strict warnings on GCC 14.2 and
Clang 17; Clang also ran AddressSanitizer/UndefinedBehaviorSanitizer. Those
checks used source-compatible dependency fixtures **rather than the entire
remote Framework checkout**. All 3 focused tests passed in each configuration.
The complete MSYS2 Windows build and the full Umicom Applications CTest suite
must still be run by a developer on the actual integrated repository.

See the separately supplied Batch R02 merge guide for the complete explicit
PowerShell commit and Framework-submodule update order.
