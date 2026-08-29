<!-- --------------------------------------------------------------------------
Umicom Framework
File: docs/WORKSPACE_RECIPES_AND_CUSTOMISATION.md

PURPOSE:
Explain how application workspace recipes, safe customisation, validation,
save files and recipe upgrades work in language suitable for new developers.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
----------------------------------------------------------------------------- -->

# Workspace Recipes and Customisation

A workspace recipe is an instruction card for assembling an application from
reusable Framework parts. Imagine opening a Lego box: the components are the
bricks, while the recipe explains which bricks to use and roughly where each
one belongs.

The recipe does not create GTK4 widgets, windows or monitor positions. Those
jobs still belong to frontend adapters and the existing Desktop layout system.
This separation lets the same recipe work in a headless test, a GTK4 desktop,
a Qt application or a web frontend.

## What is included

Framework provides three recipes for every application in its official
portfolio:

- **Learning** shows a small number of essential parts. It is intended for a
  new user who wants to understand the application without being overwhelmed.
- **Standard** is the balanced everyday workspace. It includes the main work
  surface and the supporting tools normally needed by an experienced user.
- **Focus** removes most distractions and keeps the primary task large.

There are 26 applications and three recipes per application, giving 78 recipes
in total. This includes current products such as Studio, Trader and TMS, plus
planned products already declared in the Framework portfolio, such as Author.

Each recipe lives in a short data file below:

```text
src/application/component/recipes/<application>/
    learning.inc
    standard.inc
    focus.inc
```

Keeping each recipe separate makes code review simple. A developer can change
the layout for one application without editing a large shared table.

## The important data types

### Experience profile

A workspace recipe answers “which components go where?” Its matching experience
profile answers “how should this workspace behave?” Every built-in Learning,
Standard and Focus recipe now has a reciprocal profile containing command,
form and UI-state policy. The complete beginner guide is in
[Interaction, Forms and UI State](INTERACTION_FORMS_AND_UI_STATE.md).

`UmiApplicationComponentRecipe` is the read-only instruction card. It contains:

- a stable recipe ID;
- the stable application ID;
- a human-readable title and explanation;
- its intended audience;
- a list of component IDs, regions and size weights.

`UmiApplicationComponentWorkspaceDraft` is an editable copy. Fixed-capacity
storage makes ownership clear: the draft owns its copied text and slots, so it
does not depend on a temporary parser buffer or application repository path.

`UmiApplicationComponentCustomiser` combines a draft with undo and redo
history. It supports:

- adding and removing component instances;
- moving a component earlier or later in layout order;
- moving a component to another region;
- changing its size weight;
- showing or hiding it;
- renaming the workspace;
- marking the current state as saved.

The primary surface in each built-in recipe is locked. This prevents a new user
from accidentally removing the only central work surface and then seeing what
looks like an empty application. Frontends may explain the lock and offer a
reset command rather than silently doing nothing.

## Regions and weights

A recipe uses five toolkit-neutral regions:

| Region | Simple meaning |
| --- | --- |
| Primary | The main place where the user does the job |
| Left | Navigation, lists or supporting context |
| Right | Inspectors, details or actions |
| Bottom | Output, history, logs or secondary tools |
| Floating | A temporary surface above the normal layout |

A weight is a relative size, not a pixel count. If one primary component has a
weight of `6` and another has `3`, the first should receive roughly twice as
much room. The frontend decides the final pixel geometry.

## Finding a recipe

Use the recipe catalogue rather than including recipe data files directly:

```c
const UmiApplicationComponentRecipe *recipe =
    umi_application_component_recipe_catalogue_recommend(
        "org.umicom.studio",
        UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING);
```

The catalogue can also find an exact recipe ID, list the three recipes for an
application, or validate every recipe against a populated component registry.
Applications should store stable IDs, not catalogue array positions.

## Editing a recipe safely

The normal flow is:

1. Find a read-only recipe in the catalogue.
2. Initialise a customiser from that recipe.
3. Apply small user actions such as add, move, resize or hide.
4. Evaluate the draft with a workspace policy.
5. Show every diagnostic to the user, with its suggested correction.
6. Project the valid draft into `UmiApplicationComponentLayout`.
7. Let the selected frontend materialise the layout.
8. Mark the customiser as saved after persistence succeeds.

An edit history stores individual reversible changes rather than copies of the
whole workspace. It holds the most recent 64 changes. When full, it discards
the oldest entry. Starting a new edit after undo correctly drops the abandoned
redo branch.

## Validation that explains the problem

`UmiApplicationComponentWorkspacePolicy` controls simple guardrails. The
default policy allows a full professional workspace. The learning policy caps
the number of parts and does not allow floating components.

Evaluation checks for:

- component IDs that are not registered;
- components unsupported by the chosen frontend;
- duplicate instance IDs;
- a second copy of a single-instance component;
- invalid weights;
- forbidden floating components;
- too many slots;
- a workspace with everything hidden;
- a workspace without a visible primary surface.

The evaluator collects multiple problems in one report. A settings screen can
therefore explain everything the user needs to fix instead of making them
repeat a save-fail-fix cycle for each individual issue.

## Saving a customised workspace

The portable format is a small line-oriented text document. It starts with a
format name and version, followed by metadata and component slots:

```text
umicom-workspace|2
recipe|org.umicom.workspace.studio.learning
application|org.umicom.studio
experience|org.umicom.experience.studio.learning
title|My Studio Workspace
description|A workspace saved by the user.
audience|1
slot|umicom.development.editor|umicom.development.editor|1|4|1|1
end
```

The fields after `slot` are component ID, instance ID, region, weight,
visibility and lock state. A percent escape protects a pipe, percent sign or
line break inside human text. The decoder rejects unknown records, duplicate
metadata, invalid numbers, missing required fields and unexpected content
after `end`.

Version 2 stores the experience profile explicitly, so a customised layout
keeps its command, form and UI-state behavior. Version 1 files remain readable:
Framework resolves their profile from the original built-in recipe while
loading them.

Framework only converts drafts to and from text. The application chooses where
the text is stored, following the platform settings and security policies.

## Upgrading when a recipe changes

A Framework update may add a useful panel to a Standard recipe. Replacing a
user's whole workspace would lose their preferences, so use the rebase
operation instead.

Rebase starts from the new recipe, then preserves the region, weight and
visibility of every matching old component instance. It reports how many parts
were preserved, added and removed. A caller may retain components removed from
the new recipe when a product has promised that behaviour.

The result is marked dirty because it must be reviewed and saved.

## Command-line inspection

The native tool gives developers a quick view without a GUI:

```powershell
.\build\windows-ucrt64-debug\bin\umicom-workspace-recipes.exe list
.\build\windows-ucrt64-debug\bin\umicom-workspace-recipes.exe show org.umicom.workspace.studio.learning
.\build\windows-ucrt64-debug\bin\umicom-workspace-recipes.exe validate
.\build\windows-ucrt64-debug\bin\umicom-workspace-recipes.exe export org.umicom.trader focus
```

The `export` command writes the portable workspace text to standard output, so
PowerShell can redirect it to a file if desired.

## Adding or changing a recipe

1. Reuse component IDs from the canonical component catalogue.
2. If a required reusable component does not exist, add its Framework contract
   first. Do not create an application-local copy merely to complete a recipe.
3. Keep a visible primary component in every profile.
4. Keep Learning small, make Standard practical, and make Focus genuinely
   quieter than Standard.
5. Lock only the surface whose removal would make the application confusing.
6. Update the recipe aggregate and pointer records in the same portfolio order.
7. Run the focused recipe tests and the complete integrated suite.

The recipe is product presentation data. Reusable state, commands, services,
validation and frontend adapters still belong to Framework components.
