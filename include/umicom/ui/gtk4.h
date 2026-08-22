/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4.h
 *
 * PURPOSE:
 *   Define the public GTK4 reference-adapter boundary without exposing GTK object
 *   types through product-neutral Framework headers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_H
#define UMICOM_UI_GTK4_H

#include <stdint.h>

#include "umicom/ui/adapter.h"
#include "umicom/ui/application_shell.h"
#include "umicom/ui/gtk4/widget_catalogue.h"
#include "umicom/desktop/shell_model.h"

#ifdef __cplusplus
extern "C" {
#endif

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

UmiStatus umi_gtk4_adapter_create(void *native_gtk_application,
                                  UmiGtk4Adapter **out_adapter);
void umi_gtk4_adapter_destroy(UmiGtk4Adapter *adapter);
UmiUiAdapter umi_gtk4_adapter_interface(UmiGtk4Adapter *adapter);
UmiStatus umi_gtk4_adapter_present(UmiGtk4Adapter *adapter,
                                   UmiUiApplicationShell *shell);
UmiStatus umi_gtk4_adapter_bind_desktop_shell(
    UmiGtk4Adapter *adapter,
    UmiDesktopShellModel *desktop_shell);
UmiStatus umi_gtk4_adapter_bind_context_interactions(
    UmiGtk4Adapter *adapter,
    const UmiGtk4ContextInteractionSink *sink);
void umi_gtk4_adapter_unbind_context_interactions(
    UmiGtk4Adapter *adapter);
UmiStatus umi_gtk4_adapter_refresh(UmiGtk4Adapter *adapter);
void *umi_gtk4_adapter_native_window(UmiGtk4Adapter *adapter);

#ifdef __cplusplus
}
#endif

#endif
