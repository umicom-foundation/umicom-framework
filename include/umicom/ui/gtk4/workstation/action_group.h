/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/action_group.h
 *
 * PURPOSE:
 *   Keep command controls reachable as a tool panel changes width.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_ACTION_GROUP_H
#define UMICOM_UI_GTK4_WORKSTATION_ACTION_GROUP_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/* GTK-owner-thread presentation only: commands, trust decisions and asynchronous
 * jobs remain owned by the existing Framework services and the host's bindings.
 * The returned floating widget wraps controls into additional rows. Place it in
 * a vertically scrollable tool panel when the panel also has limited height. */
GtkWidget *UmiGtk4ActionGroupCreate(void);
/* Append an unparented GtkButton (including a toggle button). Success gives the
 * group normal GTK parent ownership. A failed admission does not consume the
 * button. The caller retains responsibility for safe signal/context lifetime.
 * Controls retain their identity, labels and keyboard focus when reflowed.
 * The group supports at most 64 controls; it does not duplicate dispatch. */
UmiStatus UmiGtk4ActionGroupAppend(GtkWidget *group, GtkWidget *button);

#ifdef __cplusplus
}
#endif
#endif
