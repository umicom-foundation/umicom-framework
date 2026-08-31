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

## Applications adopting this contract

The shared contract is currently exposed by the thin productisation modules for:

- Umicom Studio IDE;
- Umicom Trader;
- Umicom Web Studio;
- Umicom TMS;
- Umicom Bank;
- Umicom LLM;
- Umicom RAG.

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
