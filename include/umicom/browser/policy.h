/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/policy.h
 *
 * PURPOSE:
 *   Validate which URL schemes an embedded browser may navigate to.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Browser contracts describe navigation and messages without exposing WebKit, Edge, GTK or Qt objects.
 */

#ifndef UMICOM_BROWSER_POLICY_H
#define UMICOM_BROWSER_POLICY_H
#include "umicom/browser/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBrowserPolicy { int allow_http; int allow_https; int allow_file; int allow_javascript_messages; } UmiBrowserPolicy;
UmiBrowserPolicy umi_browser_policy_default(void);
UmiStatus umi_browser_policy_check(const UmiBrowserPolicy *policy,const char *url);
#ifdef __cplusplus
}
#endif
#endif
