/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/reconnect_policy.h
 *
 * PURPOSE:
 *   Produce bounded exponential reconnect delays without embedding networking ownership in the UI layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RECONNECT_POLICY_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RECONNECT_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
typedef struct UmiNativeWebReconnectPolicy { uint32_t attempt_limit; uint64_t base_delay_ms; uint64_t max_delay_ms; } UmiNativeWebReconnectPolicy;
/* Compute bounded exponential reconnect delay; return zero when attempts are exhausted. */
uint64_t umi_native_web_reconnect_delay(const UmiNativeWebReconnectPolicy *policy, uint32_t attempt);

#ifdef __cplusplus
}
#endif
#endif
