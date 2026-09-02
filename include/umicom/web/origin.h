/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/origin.h
 *
 * PURPOSE:
 *   Maintain an explicit browser-origin allowlist.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_ORIGIN_H
#define UMICOM_WEB_ORIGIN_H
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web origin policy data shared with callers of this public contract.
 */
typedef struct UmiWebOriginPolicy UmiWebOriginPolicy;
/**
 * Initialise web origin policy from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_web_origin_policy_create(UmiWebOriginPolicy **out_policy);
/**
 * Release or reset state held by web origin policy so the same storage can be reused
 * safely.
 */
void umi_web_origin_policy_destroy(UmiWebOriginPolicy *policy);
/**
 * Provide the web origin allow operation used by this module and its client applications.
 */
UmiStatus umi_web_origin_allow(UmiWebOriginPolicy *policy,const char *origin);
/**
 * Provide the web origin is allowed operation used by this module and its client
 * applications.
 */
int umi_web_origin_is_allowed(const UmiWebOriginPolicy *policy,const char *origin);
/**
 * Return the number of records represented by web origin without changing their state.
 */
size_t umi_web_origin_count(const UmiWebOriginPolicy *policy);
#ifdef __cplusplus
}
#endif
#endif
