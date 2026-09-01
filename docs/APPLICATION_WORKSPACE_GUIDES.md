<!--
Umicom Framework
File: docs/APPLICATION_WORKSPACE_GUIDES.md

PURPOSE:
Explain how a thin application presents Framework-owned workspace choices and
honest delivery guidance on a welcome page, launcher or application menu.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Application Workspace Guides

## What problem this solves

A new user should not have to understand source code before opening an
application. The first screen can explain which ready-made workspaces exist,
what each workspace is for and which one is the recommended starting point.

This information must not be copied into every application. If a layout is
renamed or receives another panel, a copied welcome page would become stale.
Umicom Framework therefore builds the guide directly from the canonical
application experience.

The reusable contract is:

```c
#include "umicom/application/productisation/workspace_guide.h"
```

## What the guide contains

`UmiProductWorkspaceGuide` is a fixed-size snapshot. It contains:

- the application, display and executable identifiers;
- every canonical layout as a user-readable choice;
- the number of panels in each layout;
- the recommended starting layout;
- counts for planned, foundation, implemented and verified features;
- the most urgent unfinished feature;
- readiness and product acceptance evidence.

The snapshot owns its text. A graphical toolkit, console or web adapter may
keep it for the lifetime of one rendered welcome view without borrowing
pointers from an application repository.

## How a thin application uses it

An application contributes only its existing adoption record. Framework owns
the lookup, copying, bounds checks, recommendation and validation logic.

```c
UmiProductWorkspaceGuide guide;
UmiStatus status = umi_product_workspace_guide_build(
    umi_example_productisation_contribution(),
    &guide);

/* Stop before rendering when the canonical experience is incomplete. */
if (status != UMI_STATUS_OK) {
    return status;
}
```

The application may expose a small named wrapper so its welcome screen does not
need to know where its adoption value is stored. That wrapper must not create a
second layout list or change the Framework recommendation.

## Building a suite portfolio

A launcher usually needs a small summary for every installed application, not
every panel of every layout. Register the available thin contributions and ask
Framework to build the bounded portfolio:

```c
UmiProductAdoptionRegistry registry;
UmiProductWorkspaceGuidePortfolio portfolio;

umi_product_adoption_registry_init(&registry);

/* Register only applications that are installed and available to this user. */
status = umi_product_adoption_registry_register(
    &registry,
    umi_example_productisation_contribution());
if (status != UMI_STATUS_OK) {
    return status;
}

status = umi_product_workspace_guide_portfolio_build(
    &registry,
    &portfolio);
```

Each portfolio row contains application identity, executable identity, the
recommended layout, workspace totals, readiness and the next unfinished
feature. It does not copy complete layouts. Framework independently validates
the aggregate totals and rejects duplicate application identities.

Building a portfolio does not launch a program. A launcher must still wait for
an explicit user command and apply the normal execution and permission policy.

## Previewing several applications before launch

A suite launcher can join its current checkbox selection to the guide
portfolio. The result explains which applications will start, which running
applications will be activated, and which choices need attention. It does not
start a process.

```c
UmiProductGuidedLaunchPlan plan;

status = umi_product_guided_launch_plan_build(
    selection,
    &portfolio,
    &plan);

/* Do not render or execute a plan that failed validation. */
if (status != UMI_STATUS_OK) {
    return status;
}
```

Each `UmiProductGuidedLaunchEntry` owns its application name, recommended
layout and beginner-readable explanation. A missing guide is reported as a
warning, but it does not secretly override the runtime launch policy. An
unavailable application remains visible with an explanation and cannot become
ready to execute.

The plan is a preview only. After a user confirms it, call the existing
`umi_application_launch_selection_execute` function. That function remains the
single path to the governed launcher and process adapter.

## Rendering rules

1. Show the recommended layout first, but let the user choose another layout.
2. Explain each layout using its supplied title and description.
3. Treat a layout as a starting arrangement, not a restriction. Docking,
   floating, grouping, locking and multiple-monitor placement remain available.
4. Use feature maturity exactly as reported. A visible panel is not proof that
   its complete user journey is production-ready.
5. Never execute a program merely because the guide was displayed. Launching
   remains an explicit user command handled by the application launcher.
6. Validate snapshots received across module or process boundaries before
   rendering them.
7. Rebuild the preview after selection or runtime state changes; do not execute
   a stale preview.

## Applications adopting this contract

The shared contract is currently exposed by every thin application
productisation module in the suite:

- Umicom Accountant, Bank and TMS;
- Umicom CAD, Kitchen, Creator, Media and Music;
- Umicom Studio IDE, Web Studio, Mobile Studio, Database Studio and Integration
  Studio;
- Umicom Trader and Exchange;
- Umicom LLM and RAG;
- Umicom Desktop and OS;
- Umicom Education and Games;
- Umicom Operations, Security Centre and Marketplace.

Future applications should use the same Framework function. Adding an
application must not require another workspace-guide engine.

## How to extend it safely

To add a workspace choice, update the application experience in Framework and
reference existing Framework panel identifiers. To improve the roadmap message,
update the canonical feature state, priority and ownership. The guide will then
change automatically.

Keep provider connections, credentials, financial routing, model execution and
other specialist operations behind their own adapters. The workspace guide is
descriptive data only; it deliberately performs no external action.
