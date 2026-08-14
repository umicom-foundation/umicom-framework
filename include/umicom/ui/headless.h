/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/headless.h
 *
 * PURPOSE:
 *   Define the reference headless frontend adapter used by automated tests, CLI
 *   inspection, accessibility validation and non-graphical Umicom products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_HEADLESS_H
#define UMICOM_UI_HEADLESS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ui/adapter.h"
#include "umicom/ui/application_shell.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_HEADLESS_RENDER_CAPACITY 131072U

typedef struct UmiUiHeadlessAdapter UmiUiHeadlessAdapter;

typedef struct UmiUiHeadlessSnapshot {
    uint64_t render_revision;
    size_t pane_count;
    size_t document_count;
    size_t notification_count;
    char text[UMI_UI_HEADLESS_RENDER_CAPACITY];
} UmiUiHeadlessSnapshot;

UmiStatus umi_ui_headless_adapter_create(UmiUiHeadlessAdapter **out_adapter);
void umi_ui_headless_adapter_destroy(UmiUiHeadlessAdapter *adapter);
UmiUiAdapter umi_ui_headless_adapter_interface(UmiUiHeadlessAdapter *adapter);
UmiStatus umi_ui_headless_adapter_present(UmiUiHeadlessAdapter *adapter,
                                          UmiUiApplicationShell *shell);
UmiStatus umi_ui_headless_adapter_refresh(UmiUiHeadlessAdapter *adapter);
UmiStatus umi_ui_headless_adapter_snapshot(const UmiUiHeadlessAdapter *adapter,
                                           UmiUiHeadlessSnapshot *out_snapshot);
UmiStatus umi_ui_headless_adapter_complete_next_dialog(
    UmiUiHeadlessAdapter *adapter,
    int accepted,
    int32_t button_index,
    const char *value
);

#ifdef __cplusplus
}
#endif

#endif
