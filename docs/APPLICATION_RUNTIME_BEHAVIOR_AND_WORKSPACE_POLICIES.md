<!--
Umicom Framework
File: docs/APPLICATION_RUNTIME_BEHAVIOR_AND_WORKSPACE_POLICIES.md

PURPOSE:
Explain reusable panel behavior and workspace runtime policies to new
developers in plain language.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Runtime Behavior and Workspace Policies

This part of Umicom Framework answers a simple question: after a panel has
been placed in a window, how should it behave while the application is running?

Think of a Umicom application as a Lego model. A panel is one brick. A recipe
says where the bricks go. A behavior profile says how one brick behaves. A
workspace runtime policy says how the whole model behaves.

## The three layers

1. A **panel specification** describes what the panel is called and where it
   may be placed.
2. A **surface behavior** describes refresh timing, selection, saved state,
   commands and shared context for one panel.
3. A **workspace runtime policy** describes startup, focus, checkpoints and
   background activity for one complete recipe.

Keeping these jobs separate is important. A trading chart can be reused in
Trader, TMS or a future banking application without copying its live behavior.
Each application may still choose a different workspace recipe.

## Panel behavior in plain language

Every reusable panel has exactly one behavior record.

- **Refresh** can be manual, when focused, at an interval, or streaming.
- **Selection** can be absent, single, multiple or a continuous range.
- **Persistence** says whether state lasts for a moment, a session or the saved
  workspace.
- **Command mode** can be read-only, editable, transactional or guarded.
- **Context** lets compatible panels share values such as the selected file or
  financial instrument.

A guarded command needs a product controller. This is especially useful for an
order ticket: declaring the panel is not enough to authorise an order action.
The product must deliberately provide the controller that handles it.

Behavior records live under:

```text
src/application/presentation/behavior_profiles/
```

There is one small file per component. The master `behavior_records.inc` file
assembles them into the catalogue.

## Workspace behavior in plain language

Every workspace recipe has exactly one runtime policy.

- **Startup** decides whether to restore state, show the primary work first or
  reveal every learning panel.
- **Checkpoint** decides when layout and session state should be saved.
- **Focus** decides which visible panel should receive keyboard attention.
- **Background** decides whether timed work pauses, slows down or continues at
  full rate when the application is not in front.
- **Background commands** can be allowed or blocked independently.

Policy records live under:

```text
src/application/presentation/workspace_policies/
```

The standard Studio workspace saves every 30 seconds and reduces timed work in
the background. The standard Trader workspace saves every 15 seconds and keeps
market panels at full rate. Focus workspaces pause timed background work and
block background commands. Learning workspaces open all teaching panels.

## How the runtime uses the rules

The application calls
`umi_application_presentation_surface_runtime_advance()` with the number of
elapsed seconds. The runtime refreshes only panels that are due. It preserves
the remaining fraction of an interval, so uneven timer ticks do not gradually
shift the schedule.

When a window moves to the background, call
`umi_application_presentation_surface_runtime_set_background()`. The recipe
policy then decides whether refreshes continue, slow down or pause.

Use `umi_application_presentation_surface_runtime_context_changed()` when a
compatible panel receives a new shared value. Use
`umi_application_presentation_surface_runtime_checkpoint_due()` before writing
a workspace checkpoint.

These functions contain no GTK4, Windows or browser code. A frontend connects
its own timer and window-focus events to the same portable API.

## Adding a reusable panel

Follow this order:

1. Add or reuse a component contract.
2. Add its panel specification.
3. Add one behavior profile with the same component identifier.
4. Include the profile in `behavior_records.inc`.
5. Place the component in one or more recipes.
6. Add a product controller only when product-specific behavior is needed.
7. Add catalogue and runtime tests.

Do not put broker credentials, database connections or operating-system handles
in a behavior profile. It is portable metadata, not a service implementation.

## Adding a new application

A new application normally supplies standard, focus and learning recipes. Add
one workspace runtime policy for each recipe. The application then creates a
Framework surface, registers its few product controllers and binds a frontend
host. Most lifecycle, scheduling, context and checkpoint logic remains shared.

## Inspecting the catalogue

After the user builds the tools, these commands explain the policies:

```text
umicom-application-runtime-policy
umicom-application-runtime-policy umicom.trading.order-ticket
umicom-application-runtime-policy org.umicom.workspace.studio.standard
```

The first command prints catalogue totals. The other commands explain one
component or recipe in plain language.

## Safety rules

- A read-only panel never accepts a command through the surface runtime.
- A guarded panel needs a registered product controller.
- A focus workspace blocks commands while it is in the background.
- A long suspension produces at most one refresh per panel when time advances,
  preventing a burst of stale work.
- The runtime saturates elapsed-time counters instead of allowing integer
  overflow.

These rules do not replace domain security. Trading limits, permissions,
approvals and live-order arming still belong to the established trading and
security services.
