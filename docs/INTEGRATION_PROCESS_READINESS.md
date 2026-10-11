# Umicom Framework — Process-Instance-Bound Application Readiness

**Author and project originator:** Sammy Hegab  
**Organisation:** Umicom Foundation  
**Implementation language:** C23  
**Owning repository:** `umicom-framework`  
**Canonical Windows development path:** `C:\umicom\Umicom-Applications\framework`

## Purpose

Umicom Framework already owns application launch planning, process supervision,
launch receipts, native executable discovery, and the Integration Fabric's suite
lifecycle. This addition deliberately **does not create another registry,
launcher, socket protocol, process manager or secret provider**. It attaches an
optional **one-use readiness gate** to one existing `UmiIntegrationSuiteRuntime`
and helps its supervising host refuse stale, replayed or unverified readiness
observations.

A process can be created successfully without being ready to serve commands.
In particular, executable discovery, a process ID, a successful `CreateProcess`,
or a copied launch plan do not prove that Bank, TMS, Trader or Studio has
completed its application initialisation. This gate records that distinction.

## Ownership and data flow

```text
Umicom Desk / owning Master Controller
    |
    |  1. Existing launcher + OS process adapter creates child
    |  2. Existing Desk catalogue stores current process token/generation
    |  3. Trusted host supplies OS-CSPRNG challenge and bounded deadline
    v
Umicom Framework — Integration Readiness Gate
    |
    |  PENDING: process created, readiness not established
    |  Supervisor sends challenge on a separately authenticated channel
    |  4. OS peer identity + exact process instance checked by HOST
    |  5. One-use response verified; deadline not expired
    v
Existing Integration Suite Runtime
    |
    +--- RUNNING: only after a valid, current readiness observation
    +--- STARTING: a required member is not yet confirmed ready
    +--- DEGRADED: only optional members absent/failed
    +--- FAILED: current required member has failed
    +--- STOPPED: confirmed current-process exit removes readiness
```

**Neither this library nor this document asserts that an authenticated
inter-process transport is already installed.** The transport, OS peer checking,
child process lifecycle and random byte generation remain responsibilities of
the authoritative host/adapter. Until those are connected end-to-end, this is
an independently testable protection layer for that future integration.

## Public contracts

Include `umicom/integration/readiness_gate.h`, or the updated aggregate
`umicom/integration/integration.h`. New functions use UpperCamelCase names and
existing `UmiStatus` codes; the older public interfaces remain unchanged.

| Function | Responsibility |
| --- | --- |
| `UmiIntegrationReadinessGateCreate` | Validate and borrow an existing prepared suite, retaining member identities and kinds. |
| `UmiIntegrationReadinessGateArm` | Bind an OS-owned process token, strictly increasing process generation, unpredictable challenge and bounded monotonic deadline to one member; transition to STARTING. |
| `UmiIntegrationReadinessGateConfirm` | Verify one current challenge and the **trusted host's separately established peer-authentication result**, then transition the existing suite member to RUNNING. |
| `UmiIntegrationReadinessGateExit` | Reconcile the current process's confirmed exit into the existing suite without accepting stale tokens or generations. |
| `UmiIntegrationReadinessGateIsReady` | Enforce gate-specific readiness; all required members must have a confirmed current-process proof, even if an earlier registry marked them running. |
| `UmiIntegrationReadinessGateSnapshot` | Copy non-secret status/identity for status displays. The challenge is never returned. |
| `UmiIntegrationReadinessGateDestroy` | Zero private challenge storage and release the gate without owning or stopping the suite or process. |

No existing public enum values, structs or function names are renamed. The
public gate itself is opaque; its private memory is not persisted to disk.

## Host adapter requirements

1. Obtain `processToken` and `processGeneration` from the **current, trusted
   application-process owner**. Reject PID-only assertions from messages or
   calls that refer to a previous application instance.
2. Generate a fresh **32-byte OS cryptographically secure random challenge**
   per process start. Never derive it from time, process ID, `rand()`, an
   application name or a predictable incrementing counter.
3. Call `Arm` only after a successful current-process start. `Arm` validates
   the member, instance and timeout, then publishes a suite STARTING observation.
4. Deliver the challenge only over the peer-checked channel appropriate to the
   platform. On Windows, bind authenticated local IPC to the process identity
   and restrict pipe access. On Linux, use local socket peer credentials or a
   mutually authenticated channel. Cryptography must use vetted system/native
   primitives; this small readiness gate does **not** implement encryption.
5. On a response, independently validate that the transport peer is the
   expected current child. **Only then** pass `peerVerifiedByTransport=true`
   to `Confirm`. The boolean is a statement by trusted host code, not an
   authentication mechanism. A malicious client-supplied boolean is invalid.
6. Reject expired, repeated, wrong-process, wrong-generation and wrong-challenge
   responses. A timeout does not silently kill an application or claim it
   exited. The owning supervisor chooses cancellation/escalation, and must
   reconcile the exact exit before calling `Exit`.
7. For a confirmed exit, reconcile the exact process token through
   `UmiDeskRuntimeReconcileProcessExit` first; only if accepted call
   `UmiIntegrationReadinessGateExit` for the matching token/generation. A
   nonzero exit denotes failure, and a zero exit denotes stopped.
8. Recreate the gate after re-preparing a suite or changing membership. The
   gate checks that its borrowed suite still has the same identity/member
   layout; it must not outlive the borrowed runtime. Its state is not an OS
   authentication cache and must not be restored from an untrusted file.
9. Run all gate calls on the suite owner thread, or serialise access around
   the entire Desk/suite operation. A UI thread must not hold this borrowed
   instance while a background supervisor mutates it concurrently.

## Illustrative controller flow

The example below shows operation sequencing. Values marked `trusted...` must
come from the owning host and its authenticated transport; they are not
values to accept from an external message or hard-code into production source.

```c
/* Set up a gate once, after preparing the existing suite. */
UmiIntegrationReadinessGate *gate = NULL;
UmiStatus status = UmiIntegrationReadinessGateCreate(suiteRuntime, &gate);

/* A confirmed process start: all inputs are already validated by Desk. */
if (status == UMI_STATUS_OK) {
    status = UmiIntegrationReadinessGateArm(
        gate, applicationId,
        trustedProcessToken, trustedProcessGeneration,
        trustedFreshChallenge, monotonicMilliseconds, timeoutMilliseconds);
}

/* Later: only the authenticated transport host calls Confirm. */
if (status == UMI_STATUS_OK && peerAuthenticatedAndCurrent) {
    status = UmiIntegrationReadinessGateConfirm(
        gate, applicationId,
        trustedProcessToken, trustedProcessGeneration,
        verifiedResponse, monotonicMilliseconds, true);
}

/* The existing process supervisor must confirm the exact current exit first. */
if (processExitWasAcceptedByDesk) {
    status = UmiIntegrationReadinessGateExit(
        gate, applicationId, trustedProcessToken,
        trustedProcessGeneration, observedExitCode);
}

UmiIntegrationReadinessGateDestroy(gate);
```

This snippet is an **integration sketch**, not a standalone programme. The
Framework does not generate the `trusted...` values automatically here.

## Engineering invariants and acceptance criteria

- A suite's **gate-specific** `UmiIntegrationReadinessGateIsReady` is false
  merely because its executable exists, a process was started, or a legacy
  registry entry said RUNNING. All required members must present an accepted
  current-process proof. The older suite runtime usability function remains
  available for ungated/legacy callers and must **not** be interpreted as an
  authenticated readiness check.
- Duplicate confirmations never increment running counts.
- A failed or stopped required child removes gate-specific readiness and
  ordinary suite usability.
- Optional absence makes an otherwise ready suite DEGRADED, not FAILED.
- No stale process token or older generation can complete a new launch.
- Challenge mismatches, absent host peer verification, replay and expiry
  never publish a new suite state.
- Timeouts use caller-provided monotonic milliseconds; overflow and clock
  rewind are explicitly refused. The maximum wait is five minutes.
- Public snapshots omit challenges. Opaque memory is wiped before release.
- A malformed or altered suite, missing membership or invalid input fails
  before publication. Caller-provided output remains unchanged on failure.
- Existing application launch/discovery, finance, Studio, Kernel and OS code
  is untouched; no real broker order or money transfer is enabled.

The registered CTest is `framework.integration.suite_runtime`. The extended
regression exercises the new gate using deterministic **test-only** challenge
bytes and fake process identity. It does not assert that real Windows named
pipes, Linux peer credentials or a full application build were exercised.

## Repository and operating-system synchronisation

The source is owned once by Umicom Framework at
`C:\umicom\Umicom-Applications\framework`. After build, test, commit and push:

- From `C:\umicom\Umicom-Applications`, use the existing Framework-native
  `umicom repo verify .` then `umicom repo lock .` to stage the parent's
  exact Framework gitlink. Commit and push the Applications parent.
- From `C:\umicom\umicomOS\framework`, fast-forward `main` to the same
  Framework commit with `git pull --ff-only origin main`; **do not copy a second
  Framework implementation**. Build/test the OS host and use
  `umicom repo verify .` / `umicom repo lock .` from the OS root, then
  commit and push the OS parent's new gitlink.
- Additional *separately cloned standalone applications* containing their own
  `framework` submodules require the same fast-forward and parent-gitlink
  procedure. The thin modules already inside `Umicom-Applications\applications`
  consume their parent's Framework and must not be given nested Framework copies.
- The independent `umicom-kernel` has no Framework submodule and needs no
  Framework pull.

See the package-level merge guide for explicit PowerShell commands and the
source-preservation/validation requirements.
