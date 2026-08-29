<!-- --------------------------------------------------------------------------
Umicom Framework
File: docs/COMPONENT_MODEL_AND_COMPOSITION.md

PURPOSE:
Explain how reusable Framework components are described, found, created,
displayed and arranged into application layouts.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
----------------------------------------------------------------------------- -->

# Component Model and Composition

This system makes application development feel like building with Lego.
Framework provides reusable pieces such as an editor, explorer, terminal,
chart, order book or notification panel. An application chooses the pieces it
needs and describes where they should appear. It does not copy their code.

The existing catalogue contains 132 component descriptions. Each description
has a stable ID, such as a permanent label on a Lego brick. Framework uses that
ID to find the component, choose a screen technology, create it, manage its
lifetime and place it in a layout.

## Start here

Four words appear often in this guide:

- **Component:** one reusable part of an application, such as a panel or tool.
- **Contract:** the rules a component promises to follow.
- **Factory:** the code that creates a component for a particular screen
  technology.
- **Layout:** a toolkit-neutral description of where components appear.

"Toolkit-neutral" means the description does not contain GTK4, Qt6 or web
objects. The same component plan can therefore be used by different frontends.

## The five main parts

The model separates five jobs so that each part is easy to understand and test:

1. `UmiApplicationComponentContract` describes what a component is, which
   version it has, which frontends it supports and where it normally appears.
2. `UmiApplicationComponentRegistry` is the searchable catalogue used to find
   contracts by ID.
3. `UmiApplicationComponentFactoryRegistry` connects a contract to the code
   that can create it for headless tests, GTK4, Qt6 or the web.
4. `UmiApplicationComponentInstance` represents one created copy and controls
   its states: created, active, suspended, closed or failed.
5. `UmiApplicationComponentLayout` turns a list of component IDs into a working
   workspace. If one part fails, it cleans up parts already created.

The 132 catalogue entries are now separate include records under
`src/application/component_catalogue/<domain>/`. Their compiled order and public
metadata remain unchanged, but each reusable component can be reviewed and
governed independently.

## Reusable bundles

Framework provides common, Studio and Trader bundles as initial reference
recipes. They are not product-owned widget trees: each bundle is a list of
component IDs and regions that any supported frontend can materialise.

Run the headless proof with:

```powershell
.\build\windows-ucrt64-debug\bin\umicom-component-composer.exe studio
.\build\windows-ucrt64-debug\bin\umicom-component-composer.exe trader
```

Application repositories should select Framework component IDs, assign stable
instance IDs and provide presentation metadata. New reusable behavior belongs
in Framework; application-local code should contain only product-specific
orchestration and branding.

## Application workspace recipes

Every recipe now also names an application experience profile. The profile
adds parameterised commands, form-validation policy and standard UI-state
behavior to the visual component arrangement. See
[Interaction, Forms and UI State](INTERACTION_FORMS_AND_UI_STATE.md).

Bundles are small reference collections. The workspace recipe catalogue builds
on them with Learning, Standard and Focus layouts for every application in the
official portfolio. Recipes can be copied into an editable draft, validated,
saved as portable text, upgraded when Framework changes, and edited with undo
and redo.

Read [Workspace Recipes and Customisation](WORKSPACE_RECIPES_AND_CUSTOMISATION.md)
for a beginner-friendly guide and the rules for adding future application
layouts.

Every component and recipe also has a portable panel or window presentation
contract. Read [Application Presentation Platform](APPLICATION_PRESENTATION_PLATFORM.md)
to learn how a frontend turns these catalogue records into one ordered plan.

## Contract and ownership rules

- Component IDs and factory IDs are stable logical identifiers. They are not
  GTK4 widget names, file-system paths or application repository paths.
- A contract uses semantic version compatibility: the major version must match,
  while the available minor and patch versions may be newer than requested.
- Catalogue definitions and factory descriptors must outlive registries that
  reference them. Built-in definitions have process lifetime; plug-ins must keep
  contributed metadata alive until they unregister or shut down.
- A component instance owns the handle returned by its selected factory. The
  instance must be closed exactly once; a repeated close is deliberately safe.
- Failed create or activate operations roll back every instance already created
  for that materialisation request. Applications do not need a second cleanup
  path for partially composed workspaces.
- Fixed-capacity structures reject identity values that do not fit. Presentation
  and diagnostic text may explicitly retain a valid terminated prefix through
  the shared `umicom/base/text.h` helpers.

## How an application creates a workspace

An application or Framework launcher follows these steps:

1. Initialise `UmiApplicationComponentRegistry` and seed the Framework
   catalogue.
2. Initialise `UmiApplicationComponentFactoryRegistry` and register factories
   for the active frontend. The headless seed is suitable for tests and tools.
3. Find a Framework bundle or initialise an application-specific portable
   layout containing only component IDs, instance IDs, regions and weights.
4. Validate the layout against component multiplicity and frontend support.
5. Create the real workspace as an `UmiApplicationComponentWorkspace`.
6. Destroy the workspace during application or window shutdown.

The application does not need `if` statements for GTK4, Qt6, web or headless
types. A frontend
adapter implements create, activate, suspend and destroy callbacks and registers
the descriptor against the contract's factory ID.

## How to add a reusable component

1. Add one catalogue record below
   `src/application/component_catalogue/<domain>/<component>.inc` using a stable
   `umicom.<domain>.<component>` ID and the standard Framework file header.
2. Include the record once from `component_catalogue/records.inc`. Keep the
   aggregate order stable unless a deliberate catalogue migration requires a
   versioned ordering change.
3. Select the most specific existing role. A role is a simple category, such as
   panel, editor or command surface. It determines the default factory
   family and placement; introduce a new role only when no existing lifecycle
   contract is valid.
4. Put reusable state, commands and validation in Framework-owned public
   contracts. Toolkit code belongs in a frontend factory, never in the portable
   component definition.
5. Add the component ID to a reusable bundle only when it is a normal part of
   that workspace. Optional product surfaces should remain an application
   layout choice.
6. Add registry, lifetime, layout and adapter tests. At least one test must use
   the headless frontend so CI does not require a desktop session.

## Current reference bundles

| Bundle | Purpose | Initial component count |
| --- | --- | ---: |
| `umicom.bundle.common` | Shared shell, commands, notifications, tasks and audit | 5 |
| `umicom.bundle.studio` | Explorer, editor, build, debug, test, VCS, terminal and designer | 8 |
| `umicom.bundle.trader` | Market, execution, portfolio and risk workstation | 8 |

These are reference recipes, not closed product definitions. Future applications
may combine the same component contracts in different regions without copying
their implementation.

## Release validation

Before accepting a component-model update:

```powershell
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --parallel 2
ctest --test-dir .\build\windows-ucrt64-debug `
    -R "framework.base_text|framework.application_component|framework.application_productisation.execution" `
    --output-on-failure
```

The catalogue preservation audit must report the same definition count before
and after a mechanical record split. Configure output must contain no unsafe
object-path warning, and public descriptive test names must remain stable even
when an internal compiled target needs a shorter object-directory identifier.
