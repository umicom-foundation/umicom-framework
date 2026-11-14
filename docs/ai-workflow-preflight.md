<!--
Umicom Local Model Router / Umicom Framework
File: framework/docs/ai-workflow-preflight.md

PURPOSE:
  Explain the typed static workflow contract and canonical planner reuse.

EDUCATIONAL OVERVIEW:
  Structural validity, declared availability and actual execution are separate
  outcomes. Publication happens only after bounded validation succeeds.

AUTHOR AND ORGANISATION:
  Sammy Hegab
  Umicom Foundation

LICENCE:
  MIT for this original Umicom contribution. Referenced repositories,
  model weights and dependencies retain their own licences.
-->

# Typed AI workflow preflight

## Public surface

Include `umicom/ai_workflow/preflight.h` and link the opt-in
`Umicom::ai_workflow_preflight` target. The new module composes the existing
Integration Fabric value types, cycle detector and planner. It does not replace
any public type, scheduler or existing target.

```c
UmiStatus umi_ai_workflow_preflight(
    const UmiAiWorkflowDescription *description,
    uint32_t allowed_effects,
    UmiAiWorkflowPlan *out_plan,
    UmiAiWorkflowDiagnostic *diagnostic);
```

The caller owns every input array for the duration of the call. Inputs and outputs
must not overlap; pointers/counts must describe readable, correctly aligned storage.
Calls must be serialised with mutations of the same input/output. The function is
not a memory sandbox or a lock. It retains no input pointer and makes no I/O calls.

## Description

The trusted host supplies operation schemas with named semantic input/output ports,
required-input flags, permitted effect classes and an availability snapshot.
Workflow nodes select a type and receive stable ASCII IDs. External supplied-input
bits mean values have already been validated by the host. Links describe exactly
one source port and one destination port. Requested outputs are NODE indices,
not output-port indices.

The first contract supports text, model handles, conditioning, latents, images,
masks, audio, video, integer, real, Boolean and generic asset kinds. These names
are semantic tags only. No image bytes, tensor dimensions, file path, actual model
architecture or sample rate is checked here. Exact tag matching is deliberate:
a later conversion must be an explicit operation, not silent coercion.

## Admission sequence

1. Validate pointers and bounded collection counts before reading array members.
2. Check trusted schema identifiers, port names, enum kinds and effect masks.
3. Check node identifiers/type indices/external-input masks and requested outputs.
4. Check link indices, exact port types and single-writer destination rules.
5. Construct a canonical Fabric value; collapse parallel port links only for
   ordering edges. Every port-level contract was already checked.
6. Reuse the canonical cycle detector and planner for the whole graph.
7. Select the reverse dependency closure of requested outputs.
8. Check required inputs, approved effects and declared availability for that closure.
9. Ask the canonical planner to order the selected graph, then publish once.

Whole-graph malformed links and cycles are errors even in unselected branches.
Missing required inputs and unavailable providers in otherwise valid unselected
branches do not block another selected output. This is a documented static subset,
not an implementation of ComfyUI lazy nodes or loop expansion.

The canonical inline graph is constructed from checked descriptors, so the focused
build does not require graph mutation/archive implementation hooks. The original
cycle and topological-order code executes unchanged.

## Result and ownership

A non-OK status preserves every byte of `out_plan`. A supplied diagnostic is reset
and then populated with numeric issue/node/link/port locations. It does not contain
prompt strings, paths or credentials. A bounded heap candidate avoids large graph
objects on small application stacks and is released on all paths.

A structurally valid graph with unavailable providers returns OK and publishes
`execution_ready=false`, plus the relevant node indices. That flag means only
that the host declared the selected schemas available; it is never a live model
probe, permission token or resource guarantee. The eventual executor must repeat
provider/policy/resource checks on its execution snapshot.

`order.fingerprint` is the existing Fabric ordering fingerprint over ordered IDs.
Do not use it as a complete workflow cache key, model identity, authorisation,
cryptographic digest or promise of reproducibility.

## Capacity and side effects

The inherited graph limit is 64 nodes and 128 links. The initial registry limit is
64 operation schemas with 8 input and 8 output ports each. Tests include a full
64-node chain. Read-asset, write-asset, network and device effect bits are declared
and checked; none is performed by preflight. Unknown effect bits are refused.
No network request, file scan, weight load, provider callback or GPU work occurs.

## CMake composition

Once an existing SDK or parent build supplies `Umicom::integration`:

```cmake
include("${UMICOM_FRAMEWORK_SOURCE_DIR}/cmake/UmicomAiWorkflowPreflight.cmake")
umicom_add_ai_workflow_preflight()
target_link_libraries(my_existing_consumer PRIVATE Umicom::ai_workflow_preflight)
```

This is an opt-in addition. It does not register itself globally, edit exports or
reconfigure other products. The optional `DEPENDENCY_TARGET` argument is used by
the focused source-reference harness; production consumers should use the canonical
integration target. A matching full SDK build must be qualified before export.

## Contributor reading order

Read the header, then `validate_shape`, `validate_types`, `validate_nodes`,
`validate_links`, `build_fabric_graph`, `select_required_nodes`,
`check_active_contracts`, and `umi_ai_workflow_preflight`. Each function has a
preceding purpose/contract explanation. Study error tests together with normal
chain tests; a successful demo alone would not test rejection or transactional
publication.
