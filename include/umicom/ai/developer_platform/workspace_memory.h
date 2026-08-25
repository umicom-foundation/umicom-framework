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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiDevWorkspaceMemory { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevWorkspaceMemory;
void umi_ai_dev_workspace_memory_init(UmiAiDevWorkspaceMemory *collection);
UmiStatus umi_ai_dev_workspace_memory_add(UmiAiDevWorkspaceMemory *collection, const char *id);
UmiStatus umi_ai_dev_workspace_memory_remove(UmiAiDevWorkspaceMemory *collection, const char *id);
int umi_ai_dev_workspace_memory_contains(const UmiAiDevWorkspaceMemory *collection, const char *id);
size_t umi_ai_dev_workspace_memory_count(const UmiAiDevWorkspaceMemory *collection);

#ifdef __cplusplus
}
#endif

#endif
