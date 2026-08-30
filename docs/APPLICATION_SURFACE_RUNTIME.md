<!--
Umicom Framework
File: docs/APPLICATION_SURFACE_RUNTIME.md

PURPOSE:
Explain the portable application surface runtime to new contributors and show
how an application can assemble reusable panels without owning layout logic.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Application Surface Runtime

An application surface is the part of an application that a person can see and
interact with. It includes the main window, panels such as an editor or chart,
which panel is visible, which panel has focus, and messages such as “loading” or
“no items yet”.

Umicom Framework now owns this shared behavior. An application chooses a
workspace recipe, adds small controllers for product-specific data and actions,
and connects a frontend host. The application does not need to invent another
panel manager.

## The simple mental model

Think of application construction as a box of building blocks:

1. A **component** says what a reusable block is, such as an editor, chart, risk
   panel or terminal.
2. A **recipe** says which blocks an application uses and where they belong.
3. A **presentation specification** says how each block should look and how
   large it needs to be.
4. A **surface session** remembers which blocks are visible, focused, busy,
   empty or in error.
5. A **controller** connects a reusable block to product data and actions.
6. A **host** turns the portable plan into a real frontend. The supplied
   headless host does this in memory; GTK4 and other adapters can use the same
   contract.

The recipe is the Lego instruction sheet. Components are the bricks. A host is
the table on which the finished model is presented.

## What the runtime provides

The public include is:

```c
#include "umicom/application/presentation/presentation.h"
```

The runtime provides:

- recipe projection and validation before a frontend starts;
- bounded storage for panel state, with no hidden heap allocation;
- activation, deactivation and focus rules;
- loading, ready, empty, busy, warning, error, offline and permission states;
- controller callbacks for mount, refresh, focus, commands and shutdown;
- frontend callbacks for mounting, updating, focusing and unmounting panels;
- portable checkpoints for visibility, focus and unsaved-change markers;
- a bounded event journal for diagnosis;
- a headless host for tests and command-line tools.

## Starting a surface

```c
UmiApplicationPresentationSurfaceRuntime runtime;
UmiApplicationPresentationHeadlessSurfaceHost headless;

umi_application_presentation_surface_runtime_init(
    "org.umicom.workspace.studio.standard", &runtime);
umi_application_presentation_headless_surface_host_init(&headless);
umi_application_presentation_surface_runtime_bind_host(
    &runtime,
    umi_application_presentation_headless_surface_host_interface(&headless));
umi_application_presentation_surface_runtime_start(&runtime);
```

Always stop a started runtime:

```c
umi_application_presentation_surface_runtime_stop(&runtime);
```

## Adding a new application

Start with metadata, not widget code:

1. Reuse existing component identifiers wherever their meaning fits.
2. Add a Framework component only when the behavior is genuinely new.
3. Create standard, focus and learning recipes from those components.
4. Add presentation specifications for any new components and windows.
5. Create a thin application surface module that selects its recipe.
6. Register controllers only for the application-specific data or actions.
7. Use default Framework controllers for fully declarative panels.
8. Add headless tests before connecting GTK4 or another native frontend.

Do not store GTK widgets, database connections or broker handles in a surface
session. Those objects belong to frontend adapters and Framework services. The
surface session stores only portable state.

## Compatibility

The application surface runtime does not replace the established
experience-based workspace runtime. Existing applications can keep their
current workbench while adopting component recipes one product at a time.
Studio and Trader demonstrate this additive path.
