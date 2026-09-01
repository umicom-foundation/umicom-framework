/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/memory.h
 *
 * PURPOSE:
 *   Keep a bounded in-memory history of Helix evidence identifiers and outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This reference memory is intentionally bounded; durable evolutionary memory can later use the Framework Data Server through the same higher-level workflow.
 */

#ifndef INCLUDE_UMICOM_HELIX_MEMORY_H
#define INCLUDE_UMICOM_HELIX_MEMORY_H

#include <stddef.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_HELIX_MEMORY_CAPACITY 128U
typedef struct UmiHelixMemoryEntry {
    char key[UMI_HELIX_ID_CAPACITY];
    char value[UMI_HELIX_TEXT_CAPACITY];
} UmiHelixMemoryEntry;

typedef struct UmiHelixMemory {
    UmiHelixMemoryEntry entries[UMI_HELIX_MEMORY_CAPACITY];
    size_t count;
} UmiHelixMemory;

void umi_helix_memory_init(UmiHelixMemory *memory);
UmiStatus umi_helix_memory_put(UmiHelixMemory *memory,
                               const char *key,
                               const char *value);
const char *umi_helix_memory_get(const UmiHelixMemory *memory, const char *key);

#ifdef __cplusplus
}
#endif

#endif
