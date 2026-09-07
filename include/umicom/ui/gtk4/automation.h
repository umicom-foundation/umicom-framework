/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/automation.h
 *
 * PURPOSE:
 *   Connect toolkit-neutral Umicom acceptance scenarios to a live GTK4 widget
 *   tree without exposing GTK types through the reusable UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_AUTOMATION_H
#define UMICOM_UI_GTK4_AUTOMATION_H

#include "umicom/ui/automation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiGtk4AutomationDriver UmiGtk4AutomationDriver;

/**
 * Create an in-process driver for a GTK4 window or container. native_root is a
 * GtkWidget pointer supplied as void so product-neutral headers remain clean.
 * The driver keeps its own reference until it is destroyed.
 */
UmiStatus umi_gtk4_automation_driver_create(
    void *native_root,
    UmiGtk4AutomationDriver **out_driver);

/** Add one separate titlebar/container scope, retained until driver destruction.
 * GtkWindow scopes are rejected to avoid window-owned-controller reference
 * cycles. Duplicate/second scopes return ALREADY_EXISTS; overlapping trees or
 * scopes already rooted in another window return INVALID_ARGUMENT. No prior
 * scope is replaced or silently dropped. Later nested trees are visited once.
 * With an explicit scope, resolution is confined to these trees and related
 * transient windows; an unparented pair never searches unrelated top levels.
 * GTK owning thread only; this does not present or activate any widget.
 */
UmiStatus umi_gtk4_automation_driver_add_observed_scope(
    UmiGtk4AutomationDriver *driver, void *native_scope);
/** Release the retained GTK root, optional observed scope and driver itself. */
void umi_gtk4_automation_driver_destroy(UmiGtk4AutomationDriver *driver);

/** Return the toolkit-neutral callback interface consumed by the UAT runner. */
UmiUiAutomationDriver umi_gtk4_automation_driver_interface(
    UmiGtk4AutomationDriver *driver);

/**
 * Give a native widget a stable identifier such as studio.file.open. Tests use
 * this identifier instead of fragile captions, positions or child indexes.
 */
UmiStatus umi_gtk4_automation_tag_widget(
    void *native_widget,
    const char *automation_id);

#ifdef __cplusplus
}
#endif

#endif
