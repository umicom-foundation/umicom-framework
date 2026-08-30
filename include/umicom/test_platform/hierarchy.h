/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/hierarchy.h
 *
 * PURPOSE:
 *   Materialise the discovered parent/child test hierarchy with aggregate
 *   enabled, failed and descendant counts for reusable explorer frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_HIERARCHY_H
#define UMICOM_TEST_PLATFORM_HIERARCHY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/item.h"
#include "umicom/test_platform/result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestPlatformHierarchyNode {
    char id[128];
    char parent_id[128];
    char name[256];
    char kind[64];
    size_t depth;
    size_t child_count;
    size_t descendant_test_count;
    size_t enabled_test_count;
    size_t failed_test_count;
    uint64_t revision;
} UmiTestPlatformHierarchyNode;

/* Preserve registry order while calculating depth and descendant aggregates. */
UmiStatus umi_test_platform_hierarchy_build(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    UmiTestPlatformHierarchyNode *nodes,
    size_t capacity,
    size_t *out_count
);

#ifdef __cplusplus
}
#endif
#endif
