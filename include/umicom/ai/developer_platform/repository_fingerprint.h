/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/repository_fingerprint.h
 *
 * PURPOSE:
 *   Produce deterministic non-cryptographic identity evidence for indexed content.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_FINGERPRINT_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_FINGERPRINT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

uint64_t umi_ai_dev_repository_fingerprint_bytes(const void *data,size_t size);
uint64_t umi_ai_dev_repository_fingerprint_text(const char *text);
int umi_ai_dev_repository_fingerprint_same(const char *left,const char *right);

#ifdef __cplusplus
}
#endif

#endif
