<!--
-----------------------------------------------------------------------------
Umicom Framework
File: docs/COMPONENT_GOVERNANCE_AND_RELEASE_GATES.md

PURPOSE:
Document the MB60 component inventory, lifecycle, evidence, query, reporting
and release-gate contracts used to turn reusable surfaces into verified parts.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-----------------------------------------------------------------------------
-->

# Component governance and release gates

The application component catalogue contains 132 reusable surfaces across 31
domains. MB60 adds a governance projection over those existing definitions; it
does not remove, rename or copy them.

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

The initial MB60 projection is intentionally conservative:

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
required evidence or frontend conformance. This is expected during MB60 and is
the actionable promotion backlog, not a reason to delete incomplete features.

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

## MB60 acceptance intent

Every current catalogue entry is validated, governed, queryable and included in
the native report. The resulting gaps should drive MB61–MB70 implementation and
promotion. The count of public contracts alone is no longer a completion
measure; a release-ready component needs behavior, conformance and evidence.
