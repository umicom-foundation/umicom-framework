/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/remote_provider.h
 *
 * PURPOSE:
 *   Configure a remote AI provider endpoint without embedding provider-specific secrets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Remote provider configuration stores a secret reference rather than an API key, keeping credential resolution inside the secrets service.
 */

#ifndef INCLUDE_UMICOM_AI_REMOTE_PROVIDER_H
#define INCLUDE_UMICOM_AI_REMOTE_PROVIDER_H

#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai remote provider config data shared with callers of this public
 * contract.
 */
typedef struct UmiAiRemoteProviderConfig {
    char provider_id[UMI_AI_ID_CAPACITY];
    char endpoint[UMI_AI_TEXT_CAPACITY];
    char secret_reference[UMI_AI_ID_CAPACITY];
} UmiAiRemoteProviderConfig;

/**
 * Check that ai remote provider satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ai_remote_provider_validate(const UmiAiRemoteProviderConfig *config);

#ifdef __cplusplus
}
#endif

#endif
