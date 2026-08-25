/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_pack.h
 *
 * PURPOSE:
 *   Build a deterministic ordered pack of context identifiers.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_PACK_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_PACK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevContextPack { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevContextPack;
void umi_ai_dev_context_pack_init(UmiAiDevContextPack *collection);
UmiStatus umi_ai_dev_context_pack_add(UmiAiDevContextPack *collection, const char *id);
UmiStatus umi_ai_dev_context_pack_remove(UmiAiDevContextPack *collection, const char *id);
int umi_ai_dev_context_pack_contains(const UmiAiDevContextPack *collection, const char *id);
size_t umi_ai_dev_context_pack_count(const UmiAiDevContextPack *collection);

#ifdef __cplusplus
}
#endif

#endif
