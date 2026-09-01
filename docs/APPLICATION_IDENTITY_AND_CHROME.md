# Application Identity and Chrome

Every Umicom application needs to look like part of the same family while
still showing its own name and purpose. Framework now supplies one native
application identity component for this job. Studio, Trader, Desk and future
applications can place the component in a menu bar, workspace header or launch
surface without copying image-loading code.

## What the component shows

The component is deliberately small. It can show:

- the Umicom `<>` SVG mark;
- the application name as real text;
- a short subtitle, such as the active workspace layout; and
- an optional mode badge, such as Simulation or Read Only.

The application name is not baked into an image. Real text stays sharp at any
display scale, can be read by accessibility tools and can be translated later.
If an SVG file is missing from a package, the text remains visible so the
application is still understandable.

## How light and dark appearances work

An `UmiUiAppearanceProfile` names the correct vector for its surface. A light
profile uses the dark mark and a dark profile uses the light mark. The shared
appearance editor publishes a change only after the new profile has passed
validation and its style has been applied.

The shell header listens for that change and reloads the resource. Client
applications do not compare colour values and do not choose filenames. This
keeps appearance policy in Framework and prevents two applications from making
different choices for the same theme.

## Creating an application identity

The following example creates a compact header. The strings are copied during
creation, so local variables can safely go out of scope afterwards.

```c
UmiGtk4WorkstationShellHeaderConfig config =
    umi_gtk4_ws_shell_header_config_default(
        "org.umicom.example", "Umicom Example");
UmiGtk4WorkstationShellHeader *identity = NULL;

config.subtitle = "Standard workspace";
config.mode_badge = "Local";

if (umi_gtk4_ws_shell_header_create_managed(
        &config, &identity) == UMI_STATUS_OK) {
    gtk_box_append(
        GTK_BOX(application_bar),
        umi_gtk4_ws_shell_header_widget(identity));
}
```

The GTK container owns the visible widget after it is appended. The application
still calls `umi_gtk4_ws_shell_header_destroy` to release the small controller.
Release the controller before destroying the widget tree when possible.

## Following appearance changes

An application with the Framework appearance editor connects the two reusable
parts instead of writing its own theme event:

```c
static void on_appearance_changed(
    const UmiUiAppearanceProfile *profile,
    void *user_data)
{
    UmiGtk4WorkstationShellHeader *identity = user_data;

    if (profile == NULL || identity == NULL) return;
    (void)umi_gtk4_ws_shell_header_apply_appearance(identity, profile);
}

(void)umi_gtk4_appearance_editor_set_changed_handler(
    appearance_editor,
    on_appearance_changed,
    identity);
```

The profile pointer is borrowed only while the callback runs. The shell header
copies the resource name it needs. A caller must not store the profile pointer.

## Updating the active workspace name

Layout selection does not rebuild the header. It updates the existing labels:

```c
(void)umi_gtk4_ws_shell_header_set_text(
    identity,
    "Umicom Example",
    selected_layout_name,
    "");
```

The title, subtitle and badge are changed as one checked operation. Text that
does not fit a public capacity is rejected instead of being silently cut off.

Suite workstations can change only the badge with
`umi_application_suite_gtk4_workstation_set_mode_badge`. For example, a trading
workspace shows Simulation, Paper or Live from its authoritative environment
state. The badge explains the mode; it does not grant permission or bypass a
safety policy.

## Inspecting the component

`umi_gtk4_ws_shell_header_snapshot` returns a normal C value. It includes the
application identifier, visible text, selected icon resource, visibility state
and revision. Tests and diagnostics can inspect this value without accessing
GTK child widgets.

The snapshot separates two useful facts:

- `icon_resource` tells us what the active appearance requested; and
- `icon_visible` tells us whether that file was found and displayed.

This distinction makes packaging faults easy to explain. A correct theme can
select the right resource even when an installer accidentally omitted the file.

## Keeping applications thin

An application should provide only:

1. its stable application identifier;
2. its visible product name;
3. optional workspace or mode text; and
4. a normal GTK container where the identity belongs.

Resource resolution, SVG loading, accessibility labelling, appearance changes,
bounded state and graceful text fallback remain Framework responsibilities.
Do not copy these behaviours into an application module.

## Compatibility

The earlier `umi_gtk4_ws_shell_header_create` function remains available. It
creates the same visual structure but returns only its widget, so it cannot be
updated through a controller. Existing code does not need an immediate rewrite.
New workstations should use the managed API.
