# Own breakpoint intent and adapter confirmation separately

A debugger has two kinds of state: the breakpoint a person wants, and what
the connected adapter has confirmed. Keeping these separate lets an application
explain failed requests without silently losing the person's settings.

## Use the copied edit contract

1. Create or obtain the existing **UmiDebugWorkspace**.
2. Capture a source row with **UmiDebugBreakpointEditCapture**.
3. Read its copied values with **UmiDebugBreakpointEditRead** and show them.
   The capture owns its evidence and remains readable after the workspace closes.
4. Build settings with **UmiDebugBreakpointSettingsInit**. Text is bounded to
   511 bytes per property; empty strings clear values. This API bounds byte
   strings and does not evaluate expressions or validate their language syntax.
5. On an explicit user action, call **UmiDebugBreakpointEditApply** or
   **UmiDebugBreakpointEditRemove** on the same owner thread.
6. Inspect the returned change. A no-op retains revisions and verification.
   A real property edit preserves source identity and location but clears
   verification. Destroy the edit when its interaction ends.

Capture validation binds a unique workspace owner and the breakpoint, session
and configuration generations. Removing and recreating the same ID, replacing
the workspace, or changing another breakpoint makes the capture stale.
Watch and frame updates alone do not. These are concurrency/lifetime guards,
not user authentication or filesystem authorization.

## Synchronize deliberately

The native host calls **umi_debug_runtime_platform_sync_breakpoints** for the
changed source. It sends every enabled breakpoint for that source. An empty
set removes all breakpoints for that source in the adapter.

Before sending, Framework checks advertised condition and logpoint support,
bounded text and signed source coordinates. It clears old confirmation for the
requested set before I/O. Timeout, refusal, or incomplete/malformed responses
therefore cannot leave partly updated confirmations.

Successful replies must contain exactly one result per requested breakpoint
in order. **UmiDebugRuntimeApplyBreakpointReply** validates the captured registry
generation and each row, then publishes verification/location updates as one
batch. It retains user conditions, log messages, identities and other-source
or disabled records. A newer registry generation prevents stale publication.

The platform uses heap-owned bounded buffers for this operation, avoiding
large response arrays on a native application's stack. It does not make the
overall source edit and external debugger into one atomic transaction.
If local editing succeeds but synchronization fails, retain local intent,
report the separate failure, and require deliberate recovery.

## Limits and compatibility

Studio's facade returns **desiredApplied**, **desiredChanged**, and
**adapterSynchronized** so callers can tell these outcomes apart. A successful
exchange does not mean every individual breakpoint was verified.

The runtime supports at most 256 enabled breakpoints per source request and
retains the existing 2,048-record registry capacity. Property changes are
in-memory only. The appended logpoint capability field requires rebuilding all
consumers; old capability bits keep their values.

The transport tests use a controlled DAP peer, not a real debugger. Real
GDB/LLDB qualification, installed-product acceptance and restart persistence
remain separate work.

The wire semantics follow Microsoft's
[Debug Adapter Protocol overview](https://github.com/microsoft/debug-adapter-protocol/blob/main/overview.md)
and [protocol specification](https://github.com/microsoft/debug-adapter-protocol/blob/main/specification.md).
