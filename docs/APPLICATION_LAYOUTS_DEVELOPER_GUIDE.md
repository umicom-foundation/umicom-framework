<!--
Umicom Framework
File: docs/APPLICATION_LAYOUTS_DEVELOPER_GUIDE.md

PURPOSE:
Explain how every application uses Framework-owned panels, layouts and
customisation rules without copying presentation logic into thin clients.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Application Layouts Developer Guide

This guide explains how layouts work across Umicom Applications, starting with
the visible panels and following them back to the Framework code that owns the
behaviour.

## What a layout is

A layout is a starting arrangement of panels. For example, one layout may put
an explorer on the left, the main editor in the centre and output at the
bottom. A different layout can show research or monitoring tools instead.

A layout does not own the tools, data or business rules inside those panels.
Those belong to reusable Framework components and services. This separation is
important because it lets every application use the same reliable docking,
floating, grouping, linking, locking, persistence and multi-monitor behaviour.

Users may start with a canonical layout and then:

- move a panel to another region;
- place several panels in one tab group;
- float a panel as a separate window;
- move a floating window to another monitor;
- hide or reopen a panel;
- link panels that should share selection context;
- lock a completed arrangement;
- save a personal copy without changing the canonical layout.

## Where layouts are defined

Canonical application experiences are stored in:

```text
framework/src/application/experiences
```

Each experience contains three kinds of metadata:

1. `PANELS` lists tools the application may present.
2. `LAYOUTS` groups panel identifiers into useful starting arrangements.
3. `FEATURES` records truthful implementation and roadmap maturity.

The thin application asks Framework to project this metadata. It must not copy
the layout or build another private docking engine.

`SUITE_LAYOUT_ARCHITECTURE.md` follows this process across individual headers
and implementation files, including pointer ownership and failure behaviour.

## Current application layouts

The canonical catalogue currently contains 25 experiences, 282 panels and 68
layouts. The list below includes the Framework-owned Author experience as well
as every independently versioned application module.

| Application | Canonical starting layouts |
|---|---|
| Accountant | Bookkeeping, Payroll, Controller |
| Creator | Create, Storyboard, Review |
| Author | Research, Author, Publish |
| Bank | Banking, Global Money, Digital Assets |
| CAD | Design, Inspect |
| Database Studio | Database, Operations |
| Desktop | Flow, Workstation |
| Education | Course, Assessment |
| Exchange | Marketplace, Supplier, Operations |
| Games | World, Gameplay, Debug |
| Integration Studio | Design, Operate |
| Kitchen | Design, Present |
| LLM | Chat, Models, Evaluation |
| Marketplace | Marketplace, Manage |
| Media | Edit, Director, Deliver |
| Mobile Studio | Design, Test |
| Music | Compose, Arrange, Master |
| Operations | Operations, Resilience |
| OS | System, Developer |
| RAG | Knowledge, Evaluation |
| Security Centre | Security, Trust |
| Studio IDE | Development, Review, AI Assisted, Engine Development, Data Development, Remote Development |
| TMS | Front Office, Middle Office, Back Office |
| Trader | Trading, Research, Strategy Development, Market Analysis, Options, Strategy Analysis |
| Web Studio | Design, Test |

## How Framework checks every layout

`UmiApplicationSuiteLayoutSummary` validates one named layout and reports:

- how many panels it contains;
- how many panels begin in each screen region;
- whether it is the default layout;
- whether it can be locked;
- whether it supports multiple monitors;
- whether it is responsive;
- whether its panels use linked context;
- whether every panel was placed successfully.

`UmiApplicationSuiteLayoutDescriptor` contains summaries for every layout in
one application. `UmiApplicationSuiteLayoutCatalogue` then combines every
application descriptor into one suite-wide view.

This design means a new layout cannot be hidden behind a valid default layout.
Every named alternative must reference real panels and produce a usable render
plan.

## Reading a layout choice in C

The following example creates a frontend-neutral selector. A native window,
console tool or automated test can use the same model.

```c
const UmiApplicationExperienceDefinition *experience;
UmiApplicationSuiteLayoutSelectorModel selector;
const UmiApplicationSuiteLayoutChoice *choice;

experience = umi_application_experience_catalogue_find("org.umicom.studio");
if (experience == NULL) {
    return UMI_STATUS_NOT_FOUND;
}

if (umi_application_suite_layout_selector_build(
        experience, NULL, &selector) != UMI_STATUS_OK) {
    return UMI_STATUS_INVALID_STATE;
}

choice = umi_application_suite_layout_selector_current(&selector);
if (choice == NULL) {
    return UMI_STATUS_NOT_FOUND;
}

/* A frontend can show these facts before the user applies the layout. */
printf("%s contains %zu panels\n", choice->title, choice->panel_count);
```

The selector borrows canonical metadata. Do not free or modify pointers owned
by the experience catalogue.

## Adding a layout safely

Follow these steps:

1. Decide which existing panels support the work pattern.
2. Add a static array of their stable panel identifiers.
3. Add one `UmiExperienceLayoutDefinition` record.
4. Give the layout a stable lowercase identifier that describes its purpose.
5. Write a short title and a plain-language description.
6. Choose flags that describe real behaviour, not hoped-for functionality.
7. Keep the existing default unless the product decision explicitly changes.
8. Run the suite-layout summary and render-all tests.
9. Open the application and verify docking, floating, grouping and persistence.

Do not duplicate a panel implementation just to place it in another layout.
Refer to its existing stable panel identifier.

## Adding a panel safely

A new panel needs more than a visible name:

1. Register or reuse a Framework capability.
2. Register or reuse a Framework component for that capability.
3. Add the panel to the canonical application experience.
4. Add the panel identifier to suitable layout recipes.
5. Keep sensitive operations behind explicit permissions and service APIs.
6. Add comments explaining ownership, safety boundaries and intended reuse.
7. Verify capability and component closure before treating the panel as ready.

## Reading an experience file

Choose one application and read its experience file. Write down:

- the default layout identifier;
- every panel in that layout;
- the starting region of each panel;
- which panels are dockable or floatable;
- which capability each panel requires.

Next, create a paper sketch of a personal arrangement. Notice that the panels
do not need to change—the layout only changes where they appear. That is the
central idea behind reusable Umicom application building blocks.
