<!--
Umicom Framework
File: docs/SUITE_LAYOUT_ARCHITECTURE.md

PURPOSE:
Map the application suite-layout contracts to their implementation files and
explain how thin applications move from metadata to a customisable workspace.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Application Suite Layout Architecture

The suite-layout module turns an application experience into a workspace that
can be displayed, moved, grouped, floated, linked, locked and saved. The module
does not implement an editor, chart, account service or trading service. Those
tools remain reusable components. Layout code decides where their panels start
and how the user may arrange them.

## Main data flow

```text
Application experience
        |
        v
Layout summary and descriptor
        |
        +--------> catalogue and launcher guidance
        |
        v
Workspace projection
        |
        +--------> customisation and persistence
        |
        +--------> workbench profile projection
        |
        v
Render plan
        |
        v
Native or browser frontend adapter
```

The experience is the source of truth. Every later object is derived from it.
A thin application selects an application ID and supplies panel factories, but
it does not copy panel recipes or maintain a private docking engine.

## Public files and their relationships

| Public header | Responsibility | Used by |
|---|---|---|
| `layout_summary.h` | Validates one named layout and counts its panels, regions and customisation features. | Descriptors, selectors, generated reference tools and diagnostics. |
| `descriptor.h` | Combines all summaries for one application and preserves default-layout compatibility fields. | Suite catalogue, launchers and readiness reports. |
| `catalogue.h` | Builds a suite-wide view of every application layout. | Launchers, portfolio reports and release checks. |
| `geometry.h` | Maps portable placement names to normalized rectangles. | Projection and render planning. |
| `projection.h` | Copies an experience layout into editable workspace-window state. | Runtime, customisation and tests. |
| `customisation.h` | Loads panels and layouts, then checks panel permissions before user changes. | Application shells and native workstations. |
| `profile_projection.h` | Converts layouts into smaller workbench profiles and optionally maps composed pane IDs. | Workbench bridge and thin product composition. |
| `workbench_bridge.h` | Installs projected profiles in the authoritative workbench profile model. | Studio and other workbench-based applications. |
| `render_plan.h` | Groups visible windows into bounded region and tab-stack instructions. | Frontend adapters. |
| `selector_model.h` | Creates owned display choices for every canonical layout. | Menus, welcome screens and layout switchers. |
| `runtime.h` | Owns the active projected layout for one application session. | Application shells and workstation adapters. |
| `snapshot.h` | Carries small read-only runtime facts without exposing mutable state. | Status bars, diagnostics and tests. |
| `gtk4_workstation.h` | Connects the shared runtime and customisation services to one native widget adapter. | Thin native application entry points. |
| `suite_layout.h` | Includes the toolkit-neutral suite-layout contracts from one stable umbrella header. | Applications that need the complete module. |

Each public header has a matching implementation under:

```text
framework/src/application/suite_layout
```

The native workstation implementation lives in the corresponding adapter
area because toolkit widgets are not part of the portable application model.

## Loading an application

`umi_application_suite_layout_runtime_load` follows these steps:

1. Find the immutable experience using its stable application identifier.
2. Find the experience's canonical default layout.
3. Project each referenced panel into owned workspace-window state.
4. Validate the complete workspace.
5. Publish the experience pointer and active layout only after projection
   succeeds.

The runtime borrows the experience because catalogue records remain valid for
the process lifetime. It owns the projected layout because the user may later
select another arrangement without modifying catalogue data.

## Opening the layout editor

`umi_application_suite_customisation_load_experience` creates candidate state
away from the current workspace. It registers every panel in the New Window
catalogue, defines linked context groups, projects every named layout and
activates the default. Only a complete candidate replaces the caller's
previous state.

Dock, float, auto-hide and context-link operations check the flags on the
canonical panel before changing the base workspace. A visible button or menu
does not grant a permission that the application experience did not declare.

## Rendering a workspace

Projection creates windows. Render planning groups the visible windows by
placement and stack identifier. Every stack stores indexes into the source
layout instead of copying whole window records. A frontend therefore reads:

- the stack's region and normalized rectangle;
- the ordered source-window indexes;
- total visible and floating window counts;
- the source revision used to build the plan.

Windows in one tab stack must share geometry. Conflicting geometry is rejected
because silently choosing one rectangle would make adapters behave
differently.

## Ownership summary

- Experience, panel and layout definitions are borrowed from Framework.
- Summaries borrow their layout definition.
- Descriptors and catalogues own their fixed-capacity summary arrays.
- Selector models own copied text and flags.
- Runtime owns its projected active layout.
- Render plans own stacks and indexes but not the source layout.
- Native workstation objects own their widgets and services.
- Snapshot values are self-contained and require no cleanup.

## Documentation protection

The source-only target below checks that every public suite-layout function has
its own structured comment:

```powershell
& "C:\msys64\ucrt64\bin\cmake.exe" `
    --build `
    --preset windows-ucrt64-debug `
    --target umicom-suite-layout-documentation-audit
```

The audit is also registered as
`framework.application_suite.layouts.documentation`. It does not run an
application and does not alter source files. Its purpose is to prevent a new
public function from entering the generated reference without an explanation.
