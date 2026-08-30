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

typedef struct UmiDeveloperProjectTree {
    UmiDeveloperProjectTreeNode
        nodes[UMI_DEVELOPER_PROJECT_TREE_CAPACITY];
    size_t node_count;
    uint64_t source_revision;
    uint64_t revision;
} UmiDeveloperProjectTree;

void umi_developer_project_tree_init(UmiDeveloperProjectTree *tree);

UmiStatus umi_developer_project_tree_build(
    UmiDeveloperProjectTree *tree,
    const UmiFileIndex *file_index,
    const UmiDeveloperProjectLanguageRegistry *languages);

UmiStatus umi_developer_project_tree_at(
    const UmiDeveloperProjectTree *tree,
    size_t index,
    UmiDeveloperProjectTreeNode *out_node);

#ifdef __cplusplus
}
#endif

#endif
