/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/hierarchy.c
 *
 * PURPOSE:
 *   Build deterministic hierarchy rows and aggregate descendant state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/hierarchy.h"

#include <string.h>

#include "umicom/test_platform/history.h"

static size_t find_node(const UmiTestPlatformHierarchyNode *nodes,
                        size_t count,
                        const char *id)
{
    size_t index;
    for (index = 0U; index < count; ++index) {
        if (strcmp(nodes[index].id, id) == 0) return index;
    }
    return SIZE_MAX;
}

static int node_is_test(const UmiTestPlatformHierarchyNode *node)
{
    return node != NULL &&
           (strcmp(node->kind, "test") == 0 ||
            strcmp(node->kind, "case") == 0);
}

static size_t node_depth(const UmiTestPlatformHierarchyNode *nodes,
                         size_t count,
                         size_t start)
{
    size_t depth = 0U;
    size_t current = start;
    size_t guard = 0U;
    while (nodes[current].parent_id[0] != '\0' && guard++ < count) {
        size_t parent = find_node(nodes, count, nodes[current].parent_id);
        if (parent == SIZE_MAX || parent == current) break;
        depth += 1U;
        current = parent;
    }
    return depth;
}

UmiStatus umi_test_platform_hierarchy_build(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    UmiTestPlatformHierarchyNode *nodes,
    size_t capacity,
    size_t *out_count)
{
    UmiTestPlatformItemSnapshot item;
    size_t count;
    size_t index;
    if (items == NULL || nodes == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_count = 0U;
    count = umi_test_platform_item_registry_count(items);
    if (count > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(nodes, 0, capacity * sizeof(*nodes));
    for (index = 0U; index < count; ++index) {
        if (umi_test_platform_item_registry_at(items, index, &item) !=
            UMI_STATUS_OK) {
            return UMI_STATUS_INTERNAL_ERROR;
        }
        (void)strncpy(nodes[index].id, item.id,
                      sizeof(nodes[index].id) - 1U);
        (void)strncpy(nodes[index].parent_id, item.parent_id,
                      sizeof(nodes[index].parent_id) - 1U);
        (void)strncpy(nodes[index].name, item.name,
                      sizeof(nodes[index].name) - 1U);
        (void)strncpy(nodes[index].kind, item.kind,
                      sizeof(nodes[index].kind) - 1U);
        nodes[index].revision = item.revision;
    }
    for (index = 0U; index < count; ++index) {
        size_t candidate;
        nodes[index].depth = node_depth(nodes, count, index);
        for (candidate = 0U; candidate < count; ++candidate) {
            if (strcmp(nodes[candidate].parent_id, nodes[index].id) == 0) {
                nodes[index].child_count += 1U;
            }
        }
    }
    for (index = 0U; index < count; ++index) {
        UmiTestPlatformResultSnapshot latest;
        size_t current;
        size_t guard = 0U;
        int failed = 0;
        if (!node_is_test(&nodes[index])) continue;
        if (results != NULL &&
            umi_test_platform_history_latest(results, nodes[index].id,
                                              &latest) == UMI_STATUS_OK) {
            failed = latest.outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED;
        }
        current = index;
        for (;;) {
            UmiTestPlatformItemSnapshot current_item;
            nodes[current].descendant_test_count += 1U;
            if (umi_test_platform_item_registry_find(items, nodes[index].id,
                                                     &current_item) ==
                    UMI_STATUS_OK && current_item.enabled) {
                nodes[current].enabled_test_count += 1U;
            }
            if (failed) nodes[current].failed_test_count += 1U;
            if (nodes[current].parent_id[0] == '\0' || guard++ >= count) break;
            current = find_node(nodes, count, nodes[current].parent_id);
            if (current == SIZE_MAX) break;
        }
    }
    *out_count = count;
    return UMI_STATUS_OK;
}
