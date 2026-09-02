/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/policy.h
 *
 * PURPOSE:
 *   Validate which URL schemes an embedded browser may navigate to.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Browser contracts describe navigation and messages without exposing WebKit, Edge, GTK or Qt objects.
 */

#ifndef UMICOM_BROWSER_POLICY_H
#define UMICOM_BROWSER_POLICY_H
#include "umicom/browser/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the browser policy data shared with callers of this public contract.
 */
typedef struct UmiBrowserPolicy { int allow_http; int allow_https; int allow_file; int allow_javascript_messages; } UmiBrowserPolicy;
/**
 * Provide the browser policy default operation used by this module and its client
 * applications.
 */
UmiBrowserPolicy umi_browser_policy_default(void);
/**
 * Provide the browser policy check operation used by this module and its client
 * applications.
 */
UmiStatus umi_browser_policy_check(const UmiBrowserPolicy *policy,const char *url);
#ifdef __cplusplus
}
#endif
#endif
