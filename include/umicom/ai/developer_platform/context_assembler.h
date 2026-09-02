/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_assembler.h
 *
 * PURPOSE:
 *   Track context assembly progress and completeness before inference.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_ASSEMBLER_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_ASSEMBLER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev context assembler data shared with callers of this public contract.
 */
typedef struct UmiAiDevContextAssembler { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevContextAssembler;
/**
 * Initialise ai dev context assembler from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_context_assembler_init(UmiAiDevContextAssembler *collection);
/**
 * Add ai dev context assembler only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_dev_context_assembler_add(UmiAiDevContextAssembler *collection, const char *id);
/**
 * Remove ai dev context assembler while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ai_dev_context_assembler_remove(UmiAiDevContextAssembler *collection, const char *id);
/**
 * Provide the ai dev context assembler contains operation used by this module and its
 * client applications.
 */
int umi_ai_dev_context_assembler_contains(const UmiAiDevContextAssembler *collection, const char *id);
/**
 * Return the number of records represented by ai dev context assembler without changing
 * their state.
 */
size_t umi_ai_dev_context_assembler_count(const UmiAiDevContextAssembler *collection);

#ifdef __cplusplus
}
#endif

#endif
