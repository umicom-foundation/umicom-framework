<!--
Umicom Framework
File: docs/WORKSPACE_PANEL_COMPOSITION.md

PURPOSE:
Explain how Umicom applications compose movable, linked and persistent panels
from one Framework model.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Workspace Panel Composition

## The simple idea

Think of an Umicom workspace as a desk. A panel is one tool on that desk. The
Framework remembers where each tool belongs, which tools share a tab group and
which tools follow the same subject.

These are three separate questions:

1. **Where is it?** `placement_id` can be left, centre, right, top, bottom or
   floating.
2. **Which tabs is it with?** `stack_id` identifies the visual tab stack.
3. **What does it follow?** `context_group_id` identifies a linked subject such
   as a file, instrument, customer, deal or media track.

Earlier layout files used `group_id` for visual grouping and sometimes inferred
placement from it. The field remains available so older application code and
saved files continue to work. New code should use the three explicit fields.

## Why the separation matters

Imagine a Chart panel docked on the right. It shares a tab stack with News and
follows the instrument selected in a Watchlist:

```text
placement_id     = right
stack_id         = market-research
context_group_id = instrument-blue
```

Moving the Chart to the left must not disconnect it from the instrument. Moving
it into another tab stack must not change the instrument either. Each operation
updates only the identity it owns.

The same rule works outside trading:

| Application | Example linked subject |
|---|---|
| Studio | Project, file, symbol or diagnostic |
| TMS | Deal, book, position or settlement instruction |
| Bank | Customer, account or payment |
| Music | Track, clip or timeline position |
| Video | Scene, asset or timeline position |

## Panel header contract

`UmiWsPanelChrome` is the reusable, toolkit-neutral description of a panel
header. It contains:

- title, subtitle and badge text;
- panel, placement and linked-context identity;
- pinned, locked, floating and maximised state;
- visibility flags for close, pin, move, float, maximise, context and settings
  actions.

`UmiWsPanelAction` gives every frontend the same action names. A GTK, Qt, web or
future frontend can draw different controls while dispatching the same intent.

The shared GTK frame renders only actions that have a real handler. A button is
not shown merely because a future contract exists. This prevents visible
controls that do nothing.

## Layout editing

Normal layouts are locked. A safe edit follows this order:

1. Begin an edit with `umi_ui_workspace_customisation_begin_edit`.
2. Change placement, stack, visibility, pinned state or linked context.
3. Apply with `umi_ui_workspace_customisation_commit_edit`.
4. Or restore the exact baseline with
   `umi_ui_workspace_customisation_cancel_edit`.

Mutation functions return a permission error while the layout is locked. An
application should explain that result in plain language: “Unlock the layout
before changing panels.”

## Linked contexts

`UmiUiWindowGroupStore` owns typed linked groups. The group records a colour,
context kind and source/destination role. Use
`umi_ui_workspace_customisation_assign_context_group` so the group membership
and the window layout are updated together. Use
`umi_ui_workspace_customisation_clear_context_group` for “No group”.

A linked context shares meaning, not widgets. A source panel publishes a typed
subject. Destination panels decide how to display it. This keeps application
logic out of the graphical toolkit.

## Saved layout compatibility

Portable layout schema 3 stores:

- placement and tab stack;
- linked context group;
- pinned and resizable state;
- existing geometry, visibility, floating, maximised and close state.

The decoder still reads schema 2. When it sees an older file, it copies the old
visual group into `stack_id` and supplies a safe resizable default. Saving can
remain schema 2 when an older consumer must read the file, or use schema 3 for
the complete model.

## Application responsibilities

Framework owns layout rules, persistence formats, linked-context rules and
reusable panel actions. An application owns only:

- the useful content inside a panel;
- product commands and permissions;
- the panel catalogue contribution;
- honest loading, empty, unavailable and error states.

Studio now wraps its real tool surfaces with the shared panel header and uses a
working close action while layout editing is unlocked. Trader receives working
pin, float and close actions through the shared Application Suite workstation.
Other Umicom applications can adopt the same host without copying this logic.

## Checklist for a new panel

Before calling a panel complete, check that it:

- has a stable identifier and plain title;
- declares a placement and stack;
- declares linked context only when it publishes or receives a real subject;
- respects layout locking;
- can be found through the shared window catalogue;
- shows no button without a working action;
- survives a schema 3 save and restore;
- has keyboard labels and useful tooltips;
- explains unavailable providers instead of inventing data.
