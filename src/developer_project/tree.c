/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/tree.c
 *
 * PURPOSE:
 *   Build a file-oriented Project Explorer model from canonical file-index data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/tree.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Initialise developer project tree from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_project_tree_init(UmiDeveloperProjectTree *tree)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree == NULL) return;
    (void)memset(tree, 0, sizeof(*tree));
    tree->revision = 1U;
}

/*
 * Provide the developer project tree build operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_project_tree_build(
    UmiDeveloperProjectTree *tree,
    const UmiFileIndex *file_index,
    const UmiDeveloperProjectLanguageRegistry *languages)
{
    UmiFileIndexStats stats;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree == NULL || file_index == NULL || languages == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_developer_project_tree_init(tree);
    stats = umi_file_index_stats(file_index);

    /* Apply this branch only when its contract condition is satisfied. */
    if (stats.files > UMI_DEVELOPER_PROJECT_TREE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < stats.files; ++index) {
        UmiFileIndexEntry entry;
        UmiDeveloperProjectTreeNode *node;
        const UmiDeveloperProjectLanguagePack *language;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_file_index_at(
                file_index, index, &entry) != UMI_STATUS_OK) {
            continue;
        }

        node = &tree->nodes[tree->node_count++];
        (void)memset(node, 0, sizeof(*node));
        copy_text(node->node_id, sizeof(node->node_id), entry.relative_path);
        copy_text(node->label, sizeof(node->label), entry.name);
        copy_text(node->path, sizeof(node->path), entry.path);

        language = umi_developer_project_language_registry_for_extension(
            languages,
            entry.extension);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (language != NULL) {
            copy_text(
                node->language_id,
                sizeof(node->language_id),
                language->language_id);
        }

        node->revision = 1U;
    }

    tree->source_revision = stats.revision;
    tree->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find developer project tree while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_developer_project_tree_at(
    const UmiDeveloperProjectTree *tree,
    size_t index,
    UmiDeveloperProjectTreeNode *out_node)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= tree->node_count) return UMI_STATUS_NOT_FOUND;

    *out_node = tree->nodes[index];
    return UMI_STATUS_OK;
}
