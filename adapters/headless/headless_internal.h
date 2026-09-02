/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/headless_internal.h
 *
 * PURPOSE:
 *   Share the private headless adapter state and rendering helpers between small
 *   component-focused implementation units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_HEADLESS_INTERNAL_H
#define UMICOM_UI_HEADLESS_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/platform/threading.h"
#include "umicom/ui/headless.h"

struct UmiUiHeadlessAdapter {
    UmiUiApplicationShell *shell;
    char render[UMI_UI_HEADLESS_RENDER_CAPACITY];
    size_t length;
    uint64_t revision;
    size_t pane_count;
    size_t document_count;
    size_t notification_count;
    UmiMutex *mutex;
};

/**
 * Add headless only after its inputs and available capacity have been checked.
 */
UmiStatus umi_headless_append(UmiUiHeadlessAdapter *adapter,
                              const char *format,
                              ...);
/**
 * Provide the headless render shell operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_shell(UmiUiHeadlessAdapter *adapter);
/**
 * Provide the headless render workbench operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_workbench(UmiUiHeadlessAdapter *adapter,
                                        UmiUiWorkbench *workbench);
/**
 * Provide the headless render panes operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_panes(UmiUiHeadlessAdapter *adapter,
                                    UmiUiWorkbench *workbench);
/**
 * Provide the headless render pane view operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_pane_view(UmiUiHeadlessAdapter *adapter,
                                        UmiUiWorkbench *workbench,
                                        const UmiUiPaneSnapshot *pane);
/**
 * Provide the headless render documents operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_documents(UmiUiHeadlessAdapter *adapter,
                                        UmiUiWorkbench *workbench);
/**
 * Provide the headless render status operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_status(UmiUiHeadlessAdapter *adapter,
                                     UmiUiWorkbench *workbench);
/**
 * Provide the headless render notifications operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_notifications(UmiUiHeadlessAdapter *adapter,
                                            UmiUiWorkbench *workbench);
/**
 * Provide the headless render dialogs operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_dialogs(UmiUiHeadlessAdapter *adapter,
                                      UmiUiWorkbench *workbench);
/**
 * Provide the headless render selection operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_selection(UmiUiHeadlessAdapter *adapter,
                                        UmiUiWorkbench *workbench);

#endif
