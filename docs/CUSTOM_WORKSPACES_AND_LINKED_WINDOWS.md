<!--
Umicom Framework
File: docs/CUSTOM_WORKSPACES_AND_LINKED_WINDOWS.md

PURPOSE:
Explain custom layouts, the searchable window catalogue and linked context
groups to new Umicom contributors.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Custom Workspaces and Linked Windows

## What this feature is for

An Umicom application should feel like a box of useful building blocks. A user
chooses the windows they need, places them where they work best, links related
windows, and saves the result as a named layout. The application supplies safe
defaults, but it does not force every person to work in the same way.

The design combines familiar features found in professional trading,
development, treasury, banking and media workstations:

- a searchable **New Window** catalogue;
- named layout tabs and predefined layout templates;
- an explicit edit mode followed by locking;
- panels that can be grouped, moved and displayed on more than one monitor;
- colour-linked windows that share the current subject;
- saved settings and recovery of an earlier arrangement.

Umicom expresses these familiar concepts through original, product-neutral
Framework contracts and its own visual language.

## The five ideas to remember

### 1. A layout is a named desk

A layout remembers which windows are open and where they belong. A developer
might use `Development`, `Debugging` and `Review`. A trader might use
`Monitoring`, `Options` and `Order Management`.

`UmiUiWorkspaceCustomisation` owns the small frontend-ready model used by
current native application shells. `UmiWorkbenchLayoutService` remains the
larger persistent semantic model for history, recovery, multi-monitor placement
and durable storage. New applications should not create a third layout model.

### 2. Editing is a guarded session

A locked layout cannot accidentally lose or move a window. The lifecycle is:

1. Call `umi_ui_workspace_customisation_begin_edit`.
2. Add, move, resize, show or hide windows.
3. Call `umi_ui_workspace_customisation_commit_edit` to validate and lock it.
4. Or call `umi_ui_workspace_customisation_cancel_edit` to restore the exact
   arrangement captured at step 1.

Switching to another layout is refused while an edit session is open. This is
important because otherwise it would be unclear which layout should receive
the unsaved changes.

### 3. New Window is a catalogue, not a hard-coded menu

`UmiUiWindowCatalogue` contains reusable window descriptors. The extended query
API can:

- search the title, description and stable tool identifier;
- filter by category;
- show whether another instance is allowed;
- detect a singleton already present in the active layout;
- rank tools by recent use;
- limit the result count for a small popover or a large catalogue screen.

Call `umi_ui_workspace_customisation_open_window` to create a correctly sized
window from one of these descriptors. Do not create product-specific copies of
the catalogue search algorithm.

### 4. A context link shares meaning, not pixels

Moving a window and linking a window are different operations. A window group
controls where panels appear. A context group tells several panels which
subject is active.

Examples:

| Application | Shared context | Source window | Destination windows |
|---|---|---|---|
| Trader | Instrument | Watchlist | Chart, Order Entry, News |
| Studio | File or symbol | Explorer/Editor | Outline, Problems, AI |
| TMS | Deal or book | Trade Search | Cashflows, Risk, Accounting |
| Bank | Customer/account | Customer Search | Balance, Payments, Compliance |
| Music Studio | Track/timeline | Arrangement | Mixer, Effects, Inspector |

The Framework supports source-only, destination-only and bidirectional roles.
This prevents a destination panel from starting an unwanted feedback loop.
Use `umi_ui_workspace_customisation_assign_context_group` for normal
user-facing group changes. It keeps the active layout and group membership in
step, and moves the window out of any previous context group before applying
the new role. Use `umi_ui_window_group_for_window` to read the current role and
`umi_ui_workspace_customisation_clear_context_group` to choose "No group".

### 5. Frontends render; Framework owns the rules

GTK4, a web frontend or another native adapter may present different widgets,
but each should consume the same catalogue, layout and context contracts. The
application repository contributes product panels and commands. It must not
duplicate Framework locking, searching, layout history or context routing.

## What is implemented now

- New Window query, recent-use ranking and singleton availability.
- Begin, commit and cancel layout editing.
- One-call loading of every declared layout and catalogue panel for any Umicom
  application experience.
- Explicit product metadata for safe multi-instance panels, with Framework-
  generated collision-free window identities.
- Clone, rename, activate and remove named layouts.
- Transactional dock, undock, close, auto-hide, show, group, move and resize
  operations which roll back if any step fails.
- Shared experience-policy checks which prevent unsupported floating,
  auto-hide or closing of a critical operational panel.
- Generic GTK4 Application Suite controls used by Umicom Trader.
- A shared GTK panel editor for region, floating, auto-hide and linked-context
  changes, with one atomic apply and a clear rollback message on failure.
- Studio's searchable real-surface manager for left, right, bottom and centre
  regions, plus detachable native windows which redock when closed.
- Generic context kinds for instruments, orders, positions, deals, books,
  customers, documents, tracks and timelines.
- Exclusive context-group assignment with source, destination and
  bidirectional roles.
- Separate placement, tab-stack and linked-context identities on every window.
- Portable schema 3 persistence with schema 2 read compatibility.
- Reusable semantic panel actions and working GTK move, context, settings, pin,
  float, maximise and close controls.

## Next improvements

The following work should build on the current contracts:

1. Project lightweight frontend edits through `UmiWorkbenchLayoutService` so a
   custom layout survives restart without application-specific files.
2. Add GTK drag previews and drop targets to Studio's outer tool regions.
3. Extend the generic suite host from its current floating overlay to native
   detached windows, using Studio's completed redocking lifecycle as evidence.
4. Map detached windows to monitor identifiers and recover them from a removed
   monitor.
5. Add layout import, export and team sharing with an explicit trust prompt.
6. Connect each panel header to `UmiWorkbenchContextHost` colour-group controls.
7. Add keyboard-accessible panel movement and screen-reader announcements.

## A small exercise for a new developer

Start with a copy of an existing window descriptor. Give it a stable `tool_id`,
a plain title, a one-sentence description, a category and sensible default
size. Register it in the Framework catalogue. Add a headless test that searches
for it. Then add its product renderer. If the renderer is missing, the GTK host
will display a clear placeholder instead of crashing.

That sequence keeps the contract testable before a graphical screen is ready.
