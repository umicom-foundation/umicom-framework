/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4.h
 *
 * PURPOSE:
 *   Define the public GTK4 reference-adapter boundary without exposing GTK object
 *   types through product-neutral Framework headers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_H
#define UMICOM_UI_GTK4_H

#include <stdint.h>

#include "umicom/ui/adapter.h"
#include "umicom/ui/application_shell.h"
#include "umicom/ui/gtk4/widget_catalogue.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/desktop/shell_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the gtk4 adapter data shared with callers of this public contract.
 */
typedef struct UmiGtk4Adapter UmiGtk4Adapter;

/*
 * Products remain responsible only for composition.  The GTK4 adapter owns
 * native signal discovery and translates interaction details into these
 * toolkit-neutral callbacks.  Studio, Trader and future products can therefore
 * connect their Framework context/event services without exposing GtkWidget,
 * GtkTextBuffer or GtkGesture objects outside this adapter boundary.
 */
typedef struct UmiGtk4ContextInteractionSink {
    uint32_t structure_size;
    void *context;

    UmiStatus (*editor_location)(
        void *context,
        const char *view_id,
        const char *uri,
        const char *language_id,
        uint32_t line,
        uint32_t column,
        uint32_t selection_length,
        uint64_t timestamp_ms);

    UmiStatus (*document_activated)(
        void *context,
        const char *view_id,
        const char *uri,
        const char *language_id,
        uint64_t timestamp_ms);

    UmiStatus (*problem_selected)(
        void *context,
        const char *row_text,
        uint64_t timestamp_ms);

    UmiStatus (*source_control_selected)(
        void *context,
        const char *view_kind,
        const char *row_text,
        uint64_t timestamp_ms);

    UmiStatus (*generic_selected)(
        void *context,
        const char *source_role,
        const char *subject_id,
        const char *secondary_id,
        uint64_t timestamp_ms);
} UmiGtk4ContextInteractionSink;

/*
 * A host may embed the reference adapter inside a richer Framework-owned shell.
 * These flags control only native GTK chrome visibility; they do not remove or
 * mutate the underlying toolkit-neutral workbench models, commands, panes or
 * session state. Zero is therefore a valid "editor/workbench content only"
 * configuration, while UMI_GTK4_CHROME_ALL preserves the historical adapter UI.
 */
typedef uint32_t UmiGtk4ChromeFlags;
enum {
    UMI_GTK4_CHROME_MENU = 1U << 0,
    UMI_GTK4_CHROME_TOOLBAR = 1U << 1,
    UMI_GTK4_CHROME_ACTIVITY = 1U << 2,
    UMI_GTK4_CHROME_PRIMARY_SIDEBAR = 1U << 3,
    UMI_GTK4_CHROME_SECONDARY_SIDEBAR = 1U << 4,
    UMI_GTK4_CHROME_BOTTOM_PANEL = 1U << 5,
    UMI_GTK4_CHROME_STATUS = 1U << 6,
    UMI_GTK4_CHROME_DESKTOP_LAYOUT = 1U << 7,
    UMI_GTK4_CHROME_ALL =
        UMI_GTK4_CHROME_MENU |
        UMI_GTK4_CHROME_TOOLBAR |
        UMI_GTK4_CHROME_ACTIVITY |
        UMI_GTK4_CHROME_PRIMARY_SIDEBAR |
        UMI_GTK4_CHROME_SECONDARY_SIDEBAR |
        UMI_GTK4_CHROME_BOTTOM_PANEL |
        UMI_GTK4_CHROME_STATUS |
        UMI_GTK4_CHROME_DESKTOP_LAYOUT
};

/**
 * Initialise gtk4 adapter from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_gtk4_adapter_create(void *native_gtk_application,
                                  UmiGtk4Adapter **out_adapter);
/**
 * Release or reset state held by gtk4 adapter so the same storage can be reused safely.
 */
void umi_gtk4_adapter_destroy(UmiGtk4Adapter *adapter);
/**
 * Provide the gtk4 adapter interface operation used by this module and its client
 * applications.
 */
UmiUiAdapter umi_gtk4_adapter_interface(UmiGtk4Adapter *adapter);
/**
 * Provide the gtk4 adapter present operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_adapter_present(UmiGtk4Adapter *adapter,
                                   UmiUiApplicationShell *shell);
/**
 * Provide the gtk4 adapter bind desktop shell operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_adapter_bind_desktop_shell(
    UmiGtk4Adapter *adapter,
    UmiDesktopShellModel *desktop_shell);
/**
 * Provide the gtk4 adapter bind context interactions operation used by this module and its
 * client applications.
 */
UmiStatus umi_gtk4_adapter_bind_context_interactions(
    UmiGtk4Adapter *adapter,
    const UmiGtk4ContextInteractionSink *sink);
/**
 * Provide the gtk4 adapter unbind context interactions operation used by this module and
 * its client applications.
 */
void umi_gtk4_adapter_unbind_context_interactions(
    UmiGtk4Adapter *adapter);
/**
 * Provide the gtk4 adapter refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_adapter_refresh(UmiGtk4Adapter *adapter);
/**
 * Provide the gtk4 adapter native window operation used by this module and its client
 * applications.
 */
void *umi_gtk4_adapter_native_window(UmiGtk4Adapter *adapter);

/**
 * Provide the gtk4 adapter set chrome visibility operation used by this module and its
 * client applications.
 */
UmiStatus umi_gtk4_adapter_set_chrome_visibility(
    UmiGtk4Adapter *adapter,
    UmiGtk4ChromeFlags visible_chrome);

/**
 * Provide the gtk4 adapter chrome visibility operation used by this module and its client
 * applications.
 */
UmiGtk4ChromeFlags umi_gtk4_adapter_chrome_visibility(
    const UmiGtk4Adapter *adapter);

#ifdef __cplusplus
}
#endif

#endif
