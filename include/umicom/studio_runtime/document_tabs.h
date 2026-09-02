/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/document_tabs.h
 *
 * PURPOSE:
 *   Project authoritative DocumentCoordinator working copies into editor-tab
 *   presentation records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_DOCUMENT_TABS_H
#define UMICOM_STUDIO_RUNTIME_DOCUMENT_TABS_H

#include "umicom/document/coordinator.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_DOCUMENT_TAB_CAPACITY \
    UMI_DOCUMENT_MAX_WORKING_COPIES

/**
 * Represent the studio runtime document tab data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeDocumentTab {
    UmiDocumentId document_id;
    char view_id[UMI_UI_ID_CAPACITY];
    char display_name[UMI_DOCUMENT_NAME_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char language_id[UMI_DOCUMENT_LANGUAGE_CAPACITY];
    int active;
    int dirty;
    int external_conflict;
    uint64_t revision;
} UmiStudioRuntimeDocumentTab;

/**
 * Represent the studio runtime document tabs data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeDocumentTabs {
    UmiStudioRuntimeDocumentTab tabs[UMI_STUDIO_DOCUMENT_TAB_CAPACITY];
    size_t count;
    size_t active_index;
    uint64_t revision;
    int has_active;
} UmiStudioRuntimeDocumentTabs;

/**
 * Initialise studio document tabs from caller-provided values so later operations receive
 * a known state.
 */
void umi_studio_document_tabs_init(
    UmiStudioRuntimeDocumentTabs *tabs);

/**
 * Provide the studio document tabs refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_document_tabs_refresh(
    UmiStudioRuntimeDocumentTabs *tabs,
    const UmiDocumentCoordinator *documents);

#ifdef __cplusplus
}
#endif
#endif
