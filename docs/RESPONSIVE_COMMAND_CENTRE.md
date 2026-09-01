# Responsive Command Centre

An application can contain hundreds of commands, windows, layouts and settings.
Menus make these features visible, but a person should not have to remember
which menu contains every action. Umicom Framework therefore provides one
searchable command centre that applications can place in their top bar.

The command centre does not replace menus, shortcuts or toolbar buttons. All of
those controls continue to route to the same stable command identifier. It is
another way to discover and run the same application behaviour.

## What Framework owns

The portable model in `umicom/ui/workstation/command_bar.h` owns:

- copied action titles, descriptions, identifiers and search words;
- action scope, priority and enabled state;
- parsing and filtering of a search query;
- stable ordering of matching results;
- keyboard selection state; and
- the expanded, compact and button-only width policy.

No pointer inside the model refers to an application's temporary memory. A
frontend may copy the whole model without creating dangling pointers.

The native renderer in `umicom/ui/gtk4/workstation/command_bar.h` owns the GTK
widgets. It shows the search field, scope label and a bounded result popover. It
reports a selected command to the application through a callback. The renderer
does not execute business logic itself.

## Search prefixes

Typing normal words searches every registered action. A one-character prefix
narrows the kind of result:

| Prefix | Searches for |
| --- | --- |
| `>` | commands |
| `@` | symbols |
| `#` | text |
| `:` | line navigation |
| `/` | settings |
| `+` | windows and panels |
| `?` | assistant actions |

For example, `+ terminal` searches only windows and panels whose metadata
contains “terminal”. The prefix is removed before matching, so the application's
search service receives clean text.

## Registering actions

The model starts empty. Each application composes it from existing command and
layout metadata rather than writing a second command list.

```c
UmiWsCommandBarModel model;

umi_ws_command_bar_model_init(&model);
(void)umi_ws_command_bar_model_add(
    &model,
    "workspace.window.output",
    "Output",
    "Open the build and task output window.",
    "workspace.output.open",
    "window panel build log",
    UMI_WS_COMMAND_SCOPE_PANEL,
    80U);
```

All text is copied into fixed-capacity fields. Registration first fills a local
candidate and publishes it only after every copy succeeds. A long value or
duplicate identifier therefore cannot leave a half-created entry behind.

Priority controls ordering, not permission. A larger number places a result
before a lower-priority match. Entries with the same priority keep registration
order, which makes keyboard behaviour predictable.

## Enabling and disabling actions

An action can be temporarily disabled when its required context is absent. For
example, Restore Layout is disabled until a checkpoint exists. The item remains
in search results so the user can learn that the feature exists, but the native
row cannot be activated.

```c
(void)umi_ws_command_bar_model_set_enabled(
    &model,
    "workspace.layout.restore",
    has_saved_layout);
```

Permission and safety checks still belong to the command handler. Enabled state
is presentation guidance; it must never be treated as an authorization bypass.

## Creating the native component

```c
UmiGtk4WorkstationCommandBarConfig config =
    umi_gtk4_ws_command_bar_config_default();
UmiGtk4WorkstationCommandBar *command_bar = NULL;

config.placeholder = "Search commands and windows";
config.initial_available_width = 240;

if (umi_gtk4_ws_command_bar_create_managed(
        &config, &model, &command_bar) == UMI_STATUS_OK) {
    gtk_box_append(
        GTK_BOX(application_bar),
        umi_gtk4_ws_command_bar_widget(command_bar));
}
```

The controller copies the model. The application may release or reuse its local
model after creation. If action availability changes, update the local model and
pass it again with `umi_gtk4_ws_command_bar_set_model`.

The activation callback receives a borrowed item that is valid only while the
callback runs. It should copy any value it needs to keep. Most applications only
need to route `item->command_id` through their existing command registry.

## Responsive behaviour

The same action catalogue is preserved at every width:

- expanded shows a scope label and a wide search field;
- compact shows a shorter search field; and
- button shows one Commands control whose popover keeps all results reachable.

Call `umi_gtk4_ws_command_bar_set_available_width` when the containing header
changes size. The portable width policy selects the presentation, which keeps
client applications from inventing different breakpoints.

## Current adoption

Umicom Studio IDE builds its inline catalogue from its existing command and
layout profiles. Activation returns to the same runtime command router used by
menus, toolbar buttons and keyboard shortcuts. Its larger command palette is
preserved and now uses the shared prefix parser.

The Application Suite workstation builds actions from canonical layout choices
and shared layout-editing operations. Umicom Trader and every other thin suite
workstation therefore inherit searchable layout switching, checkpoint recovery
and window discovery without product-specific copies.

## Ownership and shutdown

After the root widget is appended, GTK owns its place in the visible tree. The
application must still call `umi_gtk4_ws_command_bar_destroy` to release the
Framework controller. Disconnect the activation handler first when it refers to
application state that is about to be released.

`umi_gtk4_ws_command_bar_snapshot` copies query, scope, presentation, counts,
selection and revision. Tests and diagnostics can inspect those facts without
reaching into GTK child widgets.
