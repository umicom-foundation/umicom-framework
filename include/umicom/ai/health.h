/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/health.h
 *
 * PURPOSE:
 *   Record the current operational health of an AI provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Provider health lets Studio distinguish model errors from application errors and present a clear operational status.
 */

#ifndef INCLUDE_UMICOM_AI_HEALTH_H
#define INCLUDE_UMICOM_AI_HEALTH_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiHealth {
    char provider_id[UMI_AI_ID_CAPACITY];
    int available;
    uint64_t checked_at_ns;
    char message[UMI_AI_SMALL_TEXT_CAPACITY];
} UmiAiHealth;

UmiStatus umi_ai_health_init(UmiAiHealth *value);

#ifdef __cplusplus
}
#endif

#endif
