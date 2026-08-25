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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiDevRepositoryIndexState {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevRepositoryIndexState;

void umi_ai_dev_repository_index_state_init(UmiAiDevRepositoryIndexState *value);
UmiStatus umi_ai_dev_repository_index_state_record(UmiAiDevRepositoryIndexState *value, int success);
void umi_ai_dev_repository_index_state_set_pending(UmiAiDevRepositoryIndexState *value, uint32_t pending);
uint32_t umi_ai_dev_repository_index_state_health_score(const UmiAiDevRepositoryIndexState *value);
int umi_ai_dev_repository_index_state_ready(const UmiAiDevRepositoryIndexState *value);

#ifdef __cplusplus
}
#endif

#endif
