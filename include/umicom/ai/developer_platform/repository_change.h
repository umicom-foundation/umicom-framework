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

typedef struct UmiAiDevRepositoryChange {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevRepositoryChange;

void umi_ai_dev_repository_change_init(UmiAiDevRepositoryChange *value);
UmiStatus umi_ai_dev_repository_change_record(UmiAiDevRepositoryChange *value, int success);
void umi_ai_dev_repository_change_set_pending(UmiAiDevRepositoryChange *value, uint32_t pending);
uint32_t umi_ai_dev_repository_change_health_score(const UmiAiDevRepositoryChange *value);
int umi_ai_dev_repository_change_ready(const UmiAiDevRepositoryChange *value);

#ifdef __cplusplus
}
#endif

#endif
