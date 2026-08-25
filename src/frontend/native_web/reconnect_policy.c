/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/reconnect_policy.c
 *
 * PURPOSE:
 *   Produce bounded exponential reconnect delays without embedding networking ownership in the UI layer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/reconnect_policy.h"

uint64_t umi_native_web_reconnect_delay(const UmiNativeWebReconnectPolicy *policy,uint32_t attempt){uint64_t delay;uint32_t i;if(policy==NULL||attempt>=policy->attempt_limit)return 0U;delay=policy->base_delay_ms;for(i=0U;i<attempt&&delay<policy->max_delay_ms;++i){if(delay>policy->max_delay_ms/2U){delay=policy->max_delay_ms;break;}delay*=2U;}return delay>policy->max_delay_ms?policy->max_delay_ms:delay;}

