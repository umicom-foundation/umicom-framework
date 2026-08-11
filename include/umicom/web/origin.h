/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/origin.h
 *
 * PURPOSE:
 *   Maintain an explicit browser-origin allowlist.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_ORIGIN_H
#define UMICOM_WEB_ORIGIN_H
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebOriginPolicy UmiWebOriginPolicy;
UmiStatus umi_web_origin_policy_create(UmiWebOriginPolicy **out_policy);
void umi_web_origin_policy_destroy(UmiWebOriginPolicy *policy);
UmiStatus umi_web_origin_allow(UmiWebOriginPolicy *policy,const char *origin);
int umi_web_origin_is_allowed(const UmiWebOriginPolicy *policy,const char *origin);
size_t umi_web_origin_count(const UmiWebOriginPolicy *policy);
#ifdef __cplusplus
}
#endif
#endif
