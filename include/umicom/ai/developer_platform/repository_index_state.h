/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/repository_index_state.h
 *
 * PURPOSE:
 *   Represent repository index lifecycle and readiness.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_INDEX_STATE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_INDEX_STATE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev repository index state data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevRepositoryIndexState {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevRepositoryIndexState;

/**
 * Initialise ai dev repository index state from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_repository_index_state_init(UmiAiDevRepositoryIndexState *value);
/**
 * Provide the ai dev repository index state record operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_repository_index_state_record(UmiAiDevRepositoryIndexState *value, int success);
/**
 * Provide the ai dev repository index state set pending operation used by this module and
 * its client applications.
 */
void umi_ai_dev_repository_index_state_set_pending(UmiAiDevRepositoryIndexState *value, uint32_t pending);
/**
 * Provide the ai dev repository index state health score operation used by this module and
 * its client applications.
 */
uint32_t umi_ai_dev_repository_index_state_health_score(const UmiAiDevRepositoryIndexState *value);
/**
 * Provide the ai dev repository index state ready operation used by this module and its
 * client applications.
 */
int umi_ai_dev_repository_index_state_ready(const UmiAiDevRepositoryIndexState *value);

#ifdef __cplusplus
}
#endif

#endif
