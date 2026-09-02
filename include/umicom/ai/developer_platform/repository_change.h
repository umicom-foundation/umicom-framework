/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/repository_change.h
 *
 * PURPOSE:
 *   Track incremental repository change state for index refresh.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_CHANGE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_CHANGE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev repository change data shared with callers of this public contract.
 */
typedef struct UmiAiDevRepositoryChange {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevRepositoryChange;

/**
 * Initialise ai dev repository change from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_repository_change_init(UmiAiDevRepositoryChange *value);
/**
 * Provide the ai dev repository change record operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_repository_change_record(UmiAiDevRepositoryChange *value, int success);
/**
 * Provide the ai dev repository change set pending operation used by this module and its
 * client applications.
 */
void umi_ai_dev_repository_change_set_pending(UmiAiDevRepositoryChange *value, uint32_t pending);
/**
 * Provide the ai dev repository change health score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_repository_change_health_score(const UmiAiDevRepositoryChange *value);
/**
 * Provide the ai dev repository change ready operation used by this module and its client
 * applications.
 */
int umi_ai_dev_repository_change_ready(const UmiAiDevRepositoryChange *value);

#ifdef __cplusplus
}
#endif

#endif
