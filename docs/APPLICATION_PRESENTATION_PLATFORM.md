<!-- --------------------------------------------------------------------------
Umicom Framework
File: docs/APPLICATION_PRESENTATION_PLATFORM.md

PURPOSE:
Explain how Framework turns reusable components and workspace recipes into
portable panel and window plans for every Umicom application.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
----------------------------------------------------------------------------- -->

# Application Presentation Platform

## The problem this solves

Umicom Framework already knows which reusable components exist. It also knows
which components belong in each application workspace. A frontend still needs
answers to practical questions:

- Should this component look like an editor, tool, dashboard or inspector?
- What is the smallest useful panel size?
- May the panel float, close or have more than one instance?
- What should an empty panel encourage the user to do?
- How large should the application window start?
- Should navigation be expanded or shown as a compact rail?
- Should the application restore its previous session?

The Application Presentation Platform provides these answers once in
Framework. GTK4, Qt6, web and future frontends can then render the same plan.

## A simple mental model

Think of an application as a Lego model:

1. A component definition names a reusable brick.
2. A panel specification explains how that brick may be presented.
3. A workspace recipe says where selected bricks belong.
4. A window specification describes the outer frame.
5. A presentation plan joins all four parts for a frontend.

Applications do not copy the plan or build their own competing catalogue.
They ask Framework to project a recipe by its stable ID.

## Complete portfolio coverage

The platform contains:

- 132 panel specifications, one for every canonical component;
- 78 window specifications, one for every workspace recipe; and
- three window choices for each of the 26 applications.

The choices are named Learning, Standard and Focus:

| Choice | Intended experience | Navigation | Density |
| --- | --- | --- | --- |
| Learning | More space and visible guidance | Expanded | Comfortable |
| Standard | Balanced daily work | Expanded | Balanced |
| Focus | More content with fewer distractions | Rail | Compact |

Large professional workspaces, including Studio, Trader, TMS, CAD, media and
development tools, receive a larger initial working area. Frontends may adapt
the dimensions to the real monitor while retaining the policy intent.

## Panel specifications

Every panel record includes:

- the canonical component ID;
- a logical icon ID;
- surface kind and chrome style;
- preferred region and minimum dimensions;
- an empty-state intent such as create, open, select, refresh or connect;
- floating and closing policy;
- multiple-instance policy; and
- default visibility.

The multiple-instance value must agree with the canonical component
definition. Validation rejects a presentation record that changes that
lifecycle promise.

Logical icon IDs do not name local image files. A GTK4, Qt6 or web adapter maps
the logical ID to its own icon resources without changing application code.

## Window specifications

A window record belongs to one workspace recipe. It describes:

- initial width and height;
- navigation, side-panel and bottom-panel sizes;
- comfortable, balanced or compact density;
- expanded, rail or hidden navigation;
- command bar and status bar visibility;
- session restoration; and
- whether multiple top-level windows are allowed.

Umicom Desk and Umicom OS use a single top-level workspace policy. Other
professional applications allow multiple windows so users can spread work
across monitors.

## Creating a presentation plan

~~~c
UmiApplicationPresentationPlan plan;

if (umi_application_presentation_project(
        "org.umicom.workspace.studio.standard", &plan) != UMI_STATUS_OK) {
    /* Explain that the selected presentation could not be resolved. */
}
~~~

The plan contains the recipe, window and an ordered placement for every recipe
slot. Each placement connects the original slot to its reusable panel
specification. No frontend object is created at this stage.

## Querying a region

A frontend can count or inspect the panels assigned to Primary, Left, Right,
Bottom or Floating regions. It does not need to repeat traversal code.

~~~c
size_t primary_count = umi_application_presentation_plan_region_count(
    &plan, UMI_APPLICATION_COMPONENT_REGION_PRIMARY);
~~~

## Inspecting the catalogue

After building Framework tools:

~~~powershell
& ".\build\windows-ucrt64-debug\bin\umicom-application-presentation.exe" validate

& ".\build\windows-ucrt64-debug\bin\umicom-application-presentation.exe" `
    list "org.umicom.studio"

& ".\build\windows-ucrt64-debug\bin\umicom-application-presentation.exe" `
    show "org.umicom.workspace.studio.standard"
~~~

The `show` command prints window policy and every ordered panel without opening
a graphical desktop session.

## Adding a future component

1. Add the canonical component record.
2. Add one matching panel specification using the same component ID.
3. Choose a logical icon, surface kind, chrome, minimum size and empty action.
4. Keep multiplicity consistent with the component definition.
5. Add the component to recipes that genuinely need it.
6. Run complete presentation validation.

A release is incomplete when a component lacks a panel specification.

## Adding a future application layout

1. Add Learning, Standard and Focus workspace recipes.
2. Add one window specification for each recipe.
3. Use stable `org.umicom.window...` identities.
4. Select dimensions and density that suit the actual work.
5. Project each recipe in a headless test.

Do not place GTK4, Qt6, HTML or operating-system window objects in these
records. Those belong in frontend adapters.

## What this platform does not do

This layer does not draw widgets. It does not replace the existing component
factory, workbench layout engine, cross-application panels or frontend
adapters. It supplies the shared presentation plan those systems can consume.

That boundary keeps the Framework modular: definitions describe what exists,
recipes describe composition, presentation describes intent, and adapters draw
the result.
