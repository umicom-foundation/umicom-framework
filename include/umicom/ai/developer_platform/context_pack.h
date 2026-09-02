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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the ai dev context pack data shared with callers of this public contract.
 */
typedef struct UmiAiDevContextPack { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevContextPack;
/**
 * Initialise ai dev context pack from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_context_pack_init(UmiAiDevContextPack *collection);
/**
 * Add ai dev context pack only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ai_dev_context_pack_add(UmiAiDevContextPack *collection, const char *id);
/**
 * Remove ai dev context pack while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ai_dev_context_pack_remove(UmiAiDevContextPack *collection, const char *id);
/**
 * Provide the ai dev context pack contains operation used by this module and its client
 * applications.
 */
int umi_ai_dev_context_pack_contains(const UmiAiDevContextPack *collection, const char *id);
/**
 * Return the number of records represented by ai dev context pack without changing their
 * state.
 */
size_t umi_ai_dev_context_pack_count(const UmiAiDevContextPack *collection);

#ifdef __cplusplus
}
#endif

#endif
