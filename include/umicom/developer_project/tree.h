/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/tree.h
 *
 * PURPOSE:
 *   Build a toolkit-neutral Project Explorer tree from the Framework file index.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TREE_H
#define UMICOM_DEVELOPER_PROJECT_TREE_H

#include "umicom/developer_project/types.h"
#include "umicom/developer_project/language_registry.h"
#include "umicom/platform/file_index.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer project tree node data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperProjectTreeNode {
    char node_id[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    char parent_id[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    char label[UMI_DEVELOPER_PROJECT_NAME_CAPACITY];
    char path[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    char language_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    int directory;
    int expandable;
    size_t child_count;
    uint64_t revision;
} UmiDeveloperProjectTreeNode;

/**
 * Represent the developer project tree data shared with callers of this public contract.
 */
typedef struct UmiDeveloperProjectTree {
    UmiDeveloperProjectTreeNode
        nodes[UMI_DEVELOPER_PROJECT_TREE_CAPACITY];
    size_t node_count;
    uint64_t source_revision;
    uint64_t revision;
} UmiDeveloperProjectTree;

/**
 * Initialise developer project tree from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_project_tree_init(UmiDeveloperProjectTree *tree);

/**
 * Provide the developer project tree build operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_project_tree_build(
    UmiDeveloperProjectTree *tree,
    const UmiFileIndex *file_index,
    const UmiDeveloperProjectLanguageRegistry *languages);

/**
 * Find developer project tree while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_developer_project_tree_at(
    const UmiDeveloperProjectTree *tree,
    size_t index,
    UmiDeveloperProjectTreeNode *out_node);

#ifdef __cplusplus
}
#endif

#endif
