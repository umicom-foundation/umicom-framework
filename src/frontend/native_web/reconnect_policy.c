/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/reconnect_policy.c
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

#include "umicom/frontend/native_web/reconnect_policy.h"

/*
 * Provide the native web reconnect delay operation used by this module and its client
 * applications.
 */
uint64_t umi_native_web_reconnect_delay(const UmiNativeWebReconnectPolicy *policy,uint32_t attempt){uint64_t delay;uint32_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy==NULL||attempt>=policy->attempt_limit)return 0U;delay=policy->base_delay_ms;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<attempt&&delay<policy->max_delay_ms;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(delay>policy->max_delay_ms/2U){delay=policy->max_delay_ms;break;}delay*=2U;}return delay>policy->max_delay_ms?policy->max_delay_ms:delay;}

