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

typedef struct UmiStudioRuntimeDocumentTabs {
    UmiStudioRuntimeDocumentTab tabs[UMI_STUDIO_DOCUMENT_TAB_CAPACITY];
    size_t count;
    size_t active_index;
    uint64_t revision;
    int has_active;
} UmiStudioRuntimeDocumentTabs;

void umi_studio_document_tabs_init(
    UmiStudioRuntimeDocumentTabs *tabs);

UmiStatus umi_studio_document_tabs_refresh(
    UmiStudioRuntimeDocumentTabs *tabs,
    const UmiDocumentCoordinator *documents);

#ifdef __cplusplus
}
#endif
#endif
