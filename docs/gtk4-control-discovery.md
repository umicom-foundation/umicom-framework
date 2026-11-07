# Discovering and revealing GTK4 controls

A control's identity, its lifetime and its visible state are separate parts of a user-interface contract. Umicom Framework provides shared discovery services so applications and tests use the same rules for nested panels.

## Choose an identifier

Give new controls a stable automation ID with umi_gtk4_automation_tag_widget. Use an application-specific name such as studio.file.open. Captions and screen coordinates are unsuitable identifiers because translation and layout changes can alter them.

Existing controls with explicit gtk_widget_set_name names can use the named lookup service. Names and automation IDs are separate selectors. A named lookup does not fall back to an automation ID. Default GTK class names are commonly shared by several controls and are not unique identifiers.

## Inspect one caller-owned scope

```c
#include <umicom/ui/gtk4/automation.h>

void *control = NULL;
UmiStatus status = umi_gtk4_automation_find_named_widget_checked(
    native_panel, "account.balance", &control);
if (status == UMI_STATUS_OK) {
    /* Use the borrowed control while its owning panel remains alive. */
}
```

native_panel must point to a live GtkWidget on its GTK owning thread. The caller retains ownership of the panel. Supplying a stale arbitrary address cannot be made safe by a type check.

The checked operation clears its output before reporting a failure:

| Result | Meaning |
| --- | --- |
| OK | The supported search completed with one match. |
| NOT_FOUND | The search completed with no match. |
| ALREADY_EXISTS | Several matches make the selector ambiguous. |
| INVALID_ARGUMENT | A required argument is absent or invalid. |
| CAPACITY_EXCEEDED | The tree exceeds a supported traversal bound. |

Checked tagged lookup has the same result contract. Both selectors use the shared logical traversal, including content retained by a collapsed GtkExpander. The supported limits are 131072 visited widgets and 256 ownership edges. A traversal failure never returns an early partial match.

The older pointer-returning functions remain available. They return NULL for all unsuccessful cases. Prefer the checked form when the reason for failure matters to diagnostics or a test report.

## Reveal separately

Use umi_gtk4_automation_reveal_tagged_widget to explicitly expand sections or select existing stack/notebook pages on the route to one tagged control. Lookup itself never changes layout.

Reveal does not present a window, focus a control, pump events, scroll, enable a disabled control or emit an action. Wait for effective visibility and mapping before a user-facing interaction. Check sensitivity and editability as appropriate.

Notifications during reveal can rebuild the view. The service reports CANCELLED when its captured destination changes; partial navigation can remain. Reacquire the control after notification-capable work instead of retaining a borrowed child pointer across it.

## Keep lifetime and scope explicit

A returned widget is borrowed. Do not unref it merely because lookup returned it. Do not pass the borrowed address to a worker or hold it across an event-loop iteration. Keeping an object reference preserves lifetime but does not establish that it still belongs to the active view.

Limit searches to the intended window or panel. A different application window must not accidentally satisfy a missing control in the current one. Report duplicate identifiers as construction or scope errors; do not choose the first one arbitrarily.

## Review coverage

The native lookup fixtures cover unique and missing names, duplicate names, collapsed and expanded sections, isolated roots, invalid inputs, depth bounds and separation of names from tags. They also verify that inspection leaves action counts and section state unchanged.

These fixtures exercise the service contract. They do not by themselves establish that every product menu is wired correctly, that native pointer input reaches it or that a full application journey works. Pair service checks with visible, application-specific interaction scenarios.

The Bits2Banking Learning GTK4 course includes a control-inspection lab built against the installed Framework SDK. It follows the same APIs without copying their traversal implementation.
