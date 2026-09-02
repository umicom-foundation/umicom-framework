/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/explorer.h
 *
 * PURPOSE:
 *   Define a bounded toolkit-neutral tree model for project, workspace and resource explorers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The Explorer does not access the filesystem itself. Platform/file-index services provide paths; this model owns only presentation-neutral navigation state.
 */

#ifndef UMICOM_UI_EXPLORER_H
#define UMICOM_UI_EXPLORER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"
#include "umicom/ui/breadcrumb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_EXPLORER_MAX_NODES 1024U
#define UMI_UI_EXPLORER_MAX_RESULTS 64U

/**
 * List the named ui explorer node kind values accepted by this public contract.
 */
typedef enum UmiUiExplorerNodeKind {
    UMI_UI_EXPLORER_WORKSPACE = 1,
    UMI_UI_EXPLORER_FOLDER = 2,
    UMI_UI_EXPLORER_FILE = 3,
    UMI_UI_EXPLORER_VIRTUAL = 4
} UmiUiExplorerNodeKind;

/**
 * Represent the ui explorer node data shared with callers of this public contract.
 */
typedef struct UmiUiExplorerNode {
    char node_id[UMI_UI_ID_CAPACITY];
    char parent_id[UMI_UI_ID_CAPACITY];
    char name[UMI_UI_TEXT_CAPACITY];
    char path[UMI_UI_PATH_CAPACITY];
    UmiUiExplorerNodeKind kind;
    uint16_t depth;
    int expanded;
    int selected;
    int modified;
} UmiUiExplorerNode;

/**
 * Represent the ui explorer search result data shared with callers of this public
 * contract.
 */
typedef struct UmiUiExplorerSearchResult {
    size_t node_index;
    int score;
} UmiUiExplorerSearchResult;

/**
 * Represent the ui explorer search results data shared with callers of this public
 * contract.
 */
typedef struct UmiUiExplorerSearchResults {
    UmiUiExplorerSearchResult items[UMI_UI_EXPLORER_MAX_RESULTS];
    size_t count;
} UmiUiExplorerSearchResults;

/**
 * Represent the ui explorer model data shared with callers of this public contract.
 */
typedef struct UmiUiExplorerModel UmiUiExplorerModel;

/**
 * Initialise ui explorer model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_explorer_model_create(UmiUiExplorerModel **out_model);
/**
 * Release or reset state held by ui explorer model so the same storage can be reused
 * safely.
 */
void umi_ui_explorer_model_destroy(UmiUiExplorerModel *model);
/**
 * Provide the ui explorer model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_explorer_model_upsert(UmiUiExplorerModel *model,
                                       const UmiUiExplorerNode *node);
/**
 * Remove ui explorer model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_explorer_model_remove(UmiUiExplorerModel *model,
                                       const char *node_id);
/**
 * Find ui explorer model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_explorer_model_find(const UmiUiExplorerModel *model,
                                     const char *node_id,
                                     UmiUiExplorerNode *out_node);
/**
 * Find ui explorer model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_explorer_model_at(const UmiUiExplorerModel *model,
                                   size_t index,
                                   UmiUiExplorerNode *out_node);
/**
 * Provide the ui explorer model select operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_explorer_model_select(UmiUiExplorerModel *model,
                                       const char *node_id);
/**
 * Provide the ui explorer model set expanded operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_explorer_model_set_expanded(UmiUiExplorerModel *model,
                                             const char *node_id,
                                             int expanded);
/**
 * Provide the ui explorer search operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_explorer_search(const UmiUiExplorerModel *model,
                                 const char *query,
                                 UmiUiExplorerSearchResults *out_results);
/**
 * Return the number of records represented by ui explorer model without changing their
 * state.
 */
size_t umi_ui_explorer_model_count(const UmiUiExplorerModel *model);
/**
 * Provide the ui explorer model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_explorer_model_revision(const UmiUiExplorerModel *model);

#ifdef __cplusplus
}
#endif
#endif
