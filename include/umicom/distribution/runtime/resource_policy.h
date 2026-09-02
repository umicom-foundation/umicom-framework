/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/resource_policy.h
 *
 * PURPOSE:
 *   resource selection rules for locale, scale and optional fallbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the dr resource policy data shared with callers of this public contract.
 */
typedef struct UmiDrResourcePolicy { char preferred_locale[32]; uint32_t preferred_scale; bool allow_locale_fallback; bool allow_scale_fallback; } UmiDrResourcePolicy;
/**
 * Initialise dr resource policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_resource_policy_init(UmiDrResourcePolicy *policy); bool umi_dr_resource_policy_accepts(const UmiDrResourcePolicy *policy,const char *locale,uint32_t scale);

#ifdef __cplusplus
}
#endif
#endif
