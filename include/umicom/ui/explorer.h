/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/explorer.h
 *
 * PURPOSE:
 *   Define a bounded toolkit-neutral tree model for project, workspace and resource explorers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef enum UmiUiExplorerNodeKind {
    UMI_UI_EXPLORER_WORKSPACE = 1,
    UMI_UI_EXPLORER_FOLDER = 2,
    UMI_UI_EXPLORER_FILE = 3,
    UMI_UI_EXPLORER_VIRTUAL = 4
} UmiUiExplorerNodeKind;

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

typedef struct UmiUiExplorerSearchResult {
    size_t node_index;
    int score;
} UmiUiExplorerSearchResult;

typedef struct UmiUiExplorerSearchResults {
    UmiUiExplorerSearchResult items[UMI_UI_EXPLORER_MAX_RESULTS];
    size_t count;
} UmiUiExplorerSearchResults;

typedef struct UmiUiExplorerModel UmiUiExplorerModel;

UmiStatus umi_ui_explorer_model_create(UmiUiExplorerModel **out_model);
void umi_ui_explorer_model_destroy(UmiUiExplorerModel *model);
UmiStatus umi_ui_explorer_model_upsert(UmiUiExplorerModel *model,
                                       const UmiUiExplorerNode *node);
UmiStatus umi_ui_explorer_model_remove(UmiUiExplorerModel *model,
                                       const char *node_id);
UmiStatus umi_ui_explorer_model_find(const UmiUiExplorerModel *model,
                                     const char *node_id,
                                     UmiUiExplorerNode *out_node);
UmiStatus umi_ui_explorer_model_at(const UmiUiExplorerModel *model,
                                   size_t index,
                                   UmiUiExplorerNode *out_node);
UmiStatus umi_ui_explorer_model_select(UmiUiExplorerModel *model,
                                       const char *node_id);
UmiStatus umi_ui_explorer_model_set_expanded(UmiUiExplorerModel *model,
                                             const char *node_id,
                                             int expanded);
UmiStatus umi_ui_explorer_search(const UmiUiExplorerModel *model,
                                 const char *query,
                                 UmiUiExplorerSearchResults *out_results);
size_t umi_ui_explorer_model_count(const UmiUiExplorerModel *model);
uint64_t umi_ui_explorer_model_revision(const UmiUiExplorerModel *model);

#ifdef __cplusplus
}
#endif
#endif
