/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/heartbeat_policy.c
 *
 * PURPOSE:
 *   Evaluate browser transport liveness from monotonic activity timestamps and policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/heartbeat_policy.h"

bool umi_native_web_heartbeat_expired(const UmiNativeWebHeartbeatPolicy *policy,uint64_t last_activity_ms,uint64_t now_ms){if(policy==NULL||now_ms<last_activity_ms)return true;return now_ms-last_activity_ms>policy->timeout_ms;}

