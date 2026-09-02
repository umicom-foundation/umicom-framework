/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/crash_registry.c
 *
 * PURPOSE:
 *   Implement the crash registry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Crash evidence registry | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/observability/crash_registry.h"
#include <string.h>

/*
 * Find operations crash registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiOperationsCrashEvidence *umi_operations_crash_registry_find(const UmiOperationsCrashRegistry *registry,const char *crash_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || crash_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[index].crash_id,crash_id) == 0) return &registry->items[index];
    return NULL;
}

/*
 * Add operations crash registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_operations_crash_registry_add(UmiOperationsCrashRegistry *registry,const UmiOperationsCrashEvidence *evidence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || evidence == NULL || evidence->crash_id[0] == '\0' || evidence->application[0] == '\0' || evidence->reason[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_operations_crash_registry_find(registry,evidence->crash_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count == UMI_OPERATIONS_MAX_CRASHES) {
        (void)memmove(&registry->items[0],&registry->items[1],(registry->count - 1U) * sizeof(registry->items[0]));
        registry->count -= 1U;
    }
    registry->items[registry->count++] = *evidence;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find operations crash registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiOperationsCrashEvidence *umi_operations_crash_registry_at(const UmiOperationsCrashRegistry *registry,size_t index)
{
    return registry == NULL || index >= registry->count ? NULL : &registry->items[index];
}

/*
 * Provide the operations crash registry unrecovered operation used by this module and its
 * client applications.
 */
size_t umi_operations_crash_registry_unrecovered(const UmiOperationsCrashRegistry *registry)
{
    size_t count = 0U;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!registry->items[index].recovered) count += 1U;
    return count;
}

/*
 * Provide the operations crash registry mark recovered operation used by this module and
 * its client applications.
 */
UmiStatus umi_operations_crash_registry_mark_recovered(UmiOperationsCrashRegistry *registry,const char *crash_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || crash_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[index].crash_id,crash_id) == 0) { registry->items[index].recovered = true; registry->revision += 1U; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
