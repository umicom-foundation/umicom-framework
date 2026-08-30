/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_dedup.h
 *
 * PURPOSE:
 *   Detect duplicate context using deterministic fingerprints.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_DEDUP_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_DEDUP_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

uint64_t umi_ai_dev_context_dedup_bytes(const void *data,size_t size);
uint64_t umi_ai_dev_context_dedup_text(const char *text);
int umi_ai_dev_context_dedup_same(const char *left,const char *right);

#ifdef __cplusplus
}
#endif

#endif
