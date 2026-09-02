/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/workspace_memory.h
 *
 * PURPOSE:
 *   Maintain bounded durable workspace-memory identities owned by Framework.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_WORKSPACE_MEMORY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_WORKSPACE_MEMORY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev workspace memory data shared with callers of this public contract.
 */
typedef struct UmiAiDevWorkspaceMemory { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevWorkspaceMemory;
/**
 * Initialise ai dev workspace memory from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_workspace_memory_init(UmiAiDevWorkspaceMemory *collection);
/**
 * Add ai dev workspace memory only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_dev_workspace_memory_add(UmiAiDevWorkspaceMemory *collection, const char *id);
/**
 * Remove ai dev workspace memory while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ai_dev_workspace_memory_remove(UmiAiDevWorkspaceMemory *collection, const char *id);
/**
 * Provide the ai dev workspace memory contains operation used by this module and its
 * client applications.
 */
int umi_ai_dev_workspace_memory_contains(const UmiAiDevWorkspaceMemory *collection, const char *id);
/**
 * Return the number of records represented by ai dev workspace memory without changing
 * their state.
 */
size_t umi_ai_dev_workspace_memory_count(const UmiAiDevWorkspaceMemory *collection);

#ifdef __cplusplus
}
#endif

#endif
