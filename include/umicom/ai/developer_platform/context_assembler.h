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

typedef struct UmiAiDevContextAssembler { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevContextAssembler;
void umi_ai_dev_context_assembler_init(UmiAiDevContextAssembler *collection);
UmiStatus umi_ai_dev_context_assembler_add(UmiAiDevContextAssembler *collection, const char *id);
UmiStatus umi_ai_dev_context_assembler_remove(UmiAiDevContextAssembler *collection, const char *id);
int umi_ai_dev_context_assembler_contains(const UmiAiDevContextAssembler *collection, const char *id);
size_t umi_ai_dev_context_assembler_count(const UmiAiDevContextAssembler *collection);

#ifdef __cplusplus
}
#endif

#endif
