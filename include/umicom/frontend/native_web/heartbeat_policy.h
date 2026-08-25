/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/heartbeat_policy.h
 *
 * PURPOSE:
 *   Evaluate browser transport liveness from monotonic activity timestamps and policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_HEARTBEAT_POLICY_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_HEARTBEAT_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
typedef struct UmiNativeWebHeartbeatPolicy { uint64_t interval_ms; uint64_t timeout_ms; } UmiNativeWebHeartbeatPolicy;
/* Return true when a connection has exceeded its liveness deadline. */
bool umi_native_web_heartbeat_expired(const UmiNativeWebHeartbeatPolicy *policy, uint64_t last_activity_ms, uint64_t now_ms);

#ifdef __cplusplus
}
#endif
#endif
