/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/coordinator.h
 *
 * PURPOSE:
 *   Define the authoritative bridge between DocumentStore working copies and
 *   toolkit-neutral UI document views, including history and conflict checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_COORDINATOR_H
#define UMICOM_DOCUMENT_COORDINATOR_H

#include "umicom/document/provider.h"
#include "umicom/platform/document_store.h"
#include "umicom/ui/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY 32U

/**
 * Represent the document working copy snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDocumentWorkingCopySnapshot {
    UmiDocumentId document_id;
    char view_id[UMI_UI_ID_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char uri[UMI_DOCUMENT_URI_CAPACITY];
    char display_name[UMI_DOCUMENT_NAME_CAPACITY];
    char language_id[UMI_DOCUMENT_LANGUAGE_CAPACITY];
    UmiDocumentTextEncoding encoding;
    UmiDocumentLineEnding line_ending;
    UmiDocumentConflictState conflict;
    size_t text_length;
    size_t undo_count;
    size_t redo_count;
    uint64_t revision;
    int dirty;
    int has_path;
    int active;
} UmiDocumentWorkingCopySnapshot;

/**
 * Represent the document coordinator data shared with callers of this public contract.
 */
typedef struct UmiDocumentCoordinator UmiDocumentCoordinator;

/**
 * Initialise document coordinator from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_document_coordinator_create(
    UmiDocumentStore *store,
    UmiUiWorkbench *workbench,
    const UmiDocumentProvider *provider,
    UmiDocumentCoordinator **out_coordinator);
/**
 * Release or reset state held by document coordinator so the same storage can be reused
 * safely.
 */
void umi_document_coordinator_destroy(UmiDocumentCoordinator *coordinator);
/**
 * Provide the document coordinator new operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_new(UmiDocumentCoordinator *coordinator,
                                       const char *suggested_name,
                                       char *out_view_id,
                                       size_t capacity);
/**
 * Provide the document coordinator open operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_open(UmiDocumentCoordinator *coordinator,
                                        const char *path,
                                        char *out_view_id,
                                        size_t capacity);
/**
 * Provide the document coordinator save active operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_save_active(
    UmiDocumentCoordinator *coordinator);
/**
 * Provide the document coordinator save active as operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_save_active_as(
    UmiDocumentCoordinator *coordinator,
    const char *path);
/**
 * Provide the document coordinator close active operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_close_active(
    UmiDocumentCoordinator *coordinator,
    int force);
/**
 * Provide the document coordinator sync active operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_sync_active(
    UmiDocumentCoordinator *coordinator);
/**
 * Provide the document coordinator undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_undo(UmiDocumentCoordinator *coordinator);
/**
 * Provide the document coordinator redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_redo(UmiDocumentCoordinator *coordinator);
/**
 * Find document coordinator while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_document_coordinator_find(UmiDocumentCoordinator *coordinator,
                                        const char *needle,
                                        size_t *out_offset);
/**
 * Provide the document coordinator replace operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_replace(UmiDocumentCoordinator *coordinator,
                                           const char *needle,
                                           const char *replacement,
                                           size_t *out_offset);
/**
 * Provide the document coordinator go to line operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_go_to_line(
    UmiDocumentCoordinator *coordinator,
    size_t one_based_line,
    size_t *out_offset);
/**
 * Provide the document coordinator check external change operation used by this module and
 * its client applications.
 */
UmiStatus umi_document_coordinator_check_external_change(
    UmiDocumentCoordinator *coordinator,
    int *out_changed);
/**
 * Provide the document coordinator active snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_active_snapshot(
    const UmiDocumentCoordinator *coordinator,
    UmiDocumentWorkingCopySnapshot *out_snapshot);
/**
 * Find document coordinator while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_document_coordinator_at(
    const UmiDocumentCoordinator *coordinator,
    size_t index,
    UmiDocumentWorkingCopySnapshot *out_snapshot);
/**
 * Return the number of records represented by document coordinator without changing their
 * state.
 */
size_t umi_document_coordinator_count(
    const UmiDocumentCoordinator *coordinator);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_COORDINATOR_H */
