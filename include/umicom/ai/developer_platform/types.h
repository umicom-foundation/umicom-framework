/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/types.h
 *
 * PURPOSE:
 *   Define shared AI developer platform constants, states, locality and risk classifications.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_TYPES_H
#define UMICOM_AI_DEVELOPER_PLATFORM_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_DEV_ID_CAPACITY 96U
#define UMI_AI_DEV_TEXT_CAPACITY 256U
#define UMI_AI_DEV_PATH_CAPACITY 512U
#define UMI_AI_DEV_SMALL_CAPACITY 32U
#define UMI_AI_DEV_MEDIUM_CAPACITY 64U

/**
 * List the named ai dev state values accepted by this public contract.
 */
typedef enum UmiAiDevState {
    UMI_AI_DEV_STATE_UNKNOWN = 0,
    UMI_AI_DEV_STATE_READY = 1,
    UMI_AI_DEV_STATE_DEGRADED = 2,
    UMI_AI_DEV_STATE_BLOCKED = 3,
    UMI_AI_DEV_STATE_FAILED = 4
} UmiAiDevState;

/**
 * List the named ai dev locality values accepted by this public contract.
 */
typedef enum UmiAiDevLocality {
    UMI_AI_DEV_LOCALITY_ANY = 0,
    UMI_AI_DEV_LOCALITY_LOCAL = 1,
    UMI_AI_DEV_LOCALITY_REMOTE = 2
} UmiAiDevLocality;

/**
 * List the named ai dev risk values accepted by this public contract.
 */
typedef enum UmiAiDevRisk {
    UMI_AI_DEV_RISK_NONE = 0,
    UMI_AI_DEV_RISK_LOW = 1,
    UMI_AI_DEV_RISK_MEDIUM = 2,
    UMI_AI_DEV_RISK_HIGH = 3,
    UMI_AI_DEV_RISK_CRITICAL = 4
} UmiAiDevRisk;

typedef uint64_t UmiAiDevCapabilityFlags;
enum {
    UMI_AI_DEV_CAP_CHAT = UINT64_C(1) << 0,
    UMI_AI_DEV_CAP_CODE = UINT64_C(1) << 1,
    UMI_AI_DEV_CAP_TOOLS = UINT64_C(1) << 2,
    UMI_AI_DEV_CAP_EMBEDDINGS = UINT64_C(1) << 3,
    UMI_AI_DEV_CAP_REVIEW = UINT64_C(1) << 4,
    UMI_AI_DEV_CAP_INLINE = UINT64_C(1) << 5,
    UMI_AI_DEV_CAP_JSON = UINT64_C(1) << 6,
    UMI_AI_DEV_CAP_LOCAL = UINT64_C(1) << 7
};

/**
 * Provide the ai dev state text operation used by this module and its client applications.
 */
const char *umi_ai_dev_state_text(UmiAiDevState state);
/**
 * Provide the ai dev locality text operation used by this module and its client
 * applications.
 */
const char *umi_ai_dev_locality_text(UmiAiDevLocality locality);
/**
 * Provide the ai dev risk text operation used by this module and its client applications.
 */
const char *umi_ai_dev_risk_text(UmiAiDevRisk risk);
/**
 * Provide the ai dev percent operation used by this module and its client applications.
 */
uint32_t umi_ai_dev_percent(uint64_t value, uint64_t maximum);

#ifdef __cplusplus
}
#endif

#endif
