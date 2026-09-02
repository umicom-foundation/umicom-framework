/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/memory.c
 *
 * PURPOSE:
 *   Store and retrieve bounded Helix memory entries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Key replacement is explicit, making the reference memory predictable in tests and simple local workflows.
 */

#include "umicom/helix/memory.h"
#include <stddef.h>

#include <stdio.h>
#include <string.h>

/*
 * Initialise helix memory from caller-provided values so later operations receive a known
 * state.
 */
void umi_helix_memory_init(UmiHelixMemory *memory)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (memory != NULL) (void)memset(memory, 0, sizeof(*memory));
}

/* Provide the helix memory put operation used by this module and its client applications. */
UmiStatus umi_helix_memory_put(UmiHelixMemory *memory,
                               const char *key,
                               const char *value)
{
    size_t i;
    int a;
    int b;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (memory == NULL || key == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < memory->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(memory->entries[i].key, key) == 0) {
            b = snprintf(memory->entries[i].value, sizeof(memory->entries[i].value), "%s", value);
            return b >= 0 && (size_t)b < sizeof(memory->entries[i].value)
                ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (memory->count >= UMI_HELIX_MEMORY_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    a = snprintf(memory->entries[memory->count].key,
                 sizeof(memory->entries[memory->count].key), "%s", key);
    b = snprintf(memory->entries[memory->count].value,
                 sizeof(memory->entries[memory->count].value), "%s", value);
    /* Apply this branch only when its contract condition is satisfied. */
    if (a < 0 || b < 0 ||
        (size_t)a >= sizeof(memory->entries[memory->count].key) ||
        (size_t)b >= sizeof(memory->entries[memory->count].value)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++memory->count;
    return UMI_STATUS_OK;
}

/* Provide the helix memory get operation used by this module and its client applications. */
const char *umi_helix_memory_get(const UmiHelixMemory *memory, const char *key)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (memory == NULL || key == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < memory->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(memory->entries[i].key, key) == 0) return memory->entries[i].value;
    }
    return NULL;
}
