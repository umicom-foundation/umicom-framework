<!--
-----------------------------------------------------------------------------
Umicom Framework
File: docs/COMPONENT_GOVERNANCE_AND_RELEASE_GATES.md

PURPOSE:
Explain how reusable components are counted, reviewed, tested and approved for
release without removing incomplete work.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-----------------------------------------------------------------------------
-->

# Component governance and release gates

The application component catalogue contains 132 reusable surfaces across 31
domains. Governance adds review information to those existing definitions. It
does not remove, rename or copy them.

Think of governance as a checklist attached to every Lego brick. The brick can
exist before every checklist item is complete, but it cannot be called stable
until the required evidence is present.

## Start here

- **Inventory** means the complete list of known components.
- **Evidence** means proof, such as tests, documentation or frontend checks.
- **Release gate** means a checklist that must pass before a component is
  presented as stable.
- **Conformance** means proof that a frontend follows the component contract.

## Lifecycle

| Status | Meaning | Default source |
|---|---|---|
| Planned | Contract and intent are documented | Planned capability |
| Experimental | A foundation exists but is not a product-ready promise | Foundation capability |
| Candidate | Implementation exists and must now earn release evidence | Implemented capability |
| Stable | Required evidence and frontend conformance are explicitly recorded | Governance override after review |
| Deprecated | Preserved compatibility surface with a documented replacement | Explicit governance decision |

No component becomes stable automatically. A stable decision must be explicit,
versioned and supported by evidence.

The current projection is intentionally conservative:

| Projection | Count |
|---|---:|
| Candidate | 67 |
| Experimental | 25 |
| Planned | 40 |
| Evidence complete at current status | 40 |
| Evidence incomplete at current status | 92 |
| Candidate release blockers | 134 |

The 40 planned records satisfy their current contract-and-documentation gate;
they are not treated as implemented. The 134 candidate blockers represent one
evidence gap and one frontend-conformance gap for each of 67 candidates.

## Product-domain aliases

Application profiles use precise business vocabulary such as `banking`,
`payments`, `accounting`, `digital-assets`, `commodity`, `exchange`,
`logistics` and `payroll`. Several of their reusable components are physically
grouped under broader `finance` or `people` catalogue domains.

Framework resolves those names through canonical capability aliases. It does
not create duplicate component records. Boundary audits can therefore validate
product vocabulary while inventory and ownership remain single-source.

## Evidence contract

Candidate and stable visual components require:

- a toolkit-neutral public contract;
- implementation evidence;
- focused unit tests;
- a headless behavior fixture;
- GTK4 conformance evidence;
- public documentation;
- accessibility evidence;
- at least one executable consumer fixture, with two distinct consumers
  preferred before long-term stabilization.

Qt6 and web support can be added independently. Unsupported adapters remain
unknown rather than being reported as successful.

## Public API

Include the aggregate header:

```c
#include "umicom/application/governance/governance.h"
```

Build the default inventory:

```c
UmiComponentInventory inventory;

if (umi_component_inventory_build(NULL, 0U, &inventory) != UMI_STATUS_OK) {
    /* Treat an invalid catalogue as a configuration error. */
}
```

Applications and release tooling can supply explicit governance overrides.
Overrides never mutate the canonical catalogue and duplicate or unknown IDs are
rejected.

## Native inventory tool

When `UMICOM_BUILD_NATIVE_TOOL` is enabled:

```powershell
umicom-component-inventory --format markdown
umicom-component-inventory --format json --domain trading
umicom-component-inventory --format text --check
```

`--check` returns a non-zero status while candidate or stable components lack
required evidence or frontend conformance. This is expected while work remains.
It creates a useful improvement list; it is not a reason to delete incomplete
features.

## Compatibility policy

- Existing component definitions and catalogue lookup functions are preserved.
- New governance records reference the canonical definitions instead of
  creating a second component catalogue.
- Status may move forward from planned to experimental, candidate and stable.
- Any active status may become deprecated, but a deprecated component cannot
  be silently resurrected and release policy requires a replacement route.
- New frontend claims require conformance evidence; they are not inferred from
  a file or symbol name.
- Reports use a two-pass bounded writer and can be emitted as text, Markdown or
  JSON without Python, PowerShell or application-private code.

## Completion goal

Every current catalogue entry is validated, governed, queryable and included in
the native report. Any reported gap should become a clear implementation or
promotion task. Counting public contracts alone does not prove completion; a
release-ready component also needs working behaviour, frontend conformance and
evidence that another developer can review.
