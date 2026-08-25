/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/resource_policy.h
 *
 * PURPOSE:
 *   resource selection rules for locale, scale and optional fallbacks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RESOURCE_POLICY_H
#define UMICOM_DISTRIBUTION_RUNTIME_RESOURCE_POLICY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrResourcePolicy { char preferred_locale[32]; uint32_t preferred_scale; bool allow_locale_fallback; bool allow_scale_fallback; } UmiDrResourcePolicy;
void umi_dr_resource_policy_init(UmiDrResourcePolicy *policy); bool umi_dr_resource_policy_accepts(const UmiDrResourcePolicy *policy,const char *locale,uint32_t scale);

#ifdef __cplusplus
}
#endif
#endif
