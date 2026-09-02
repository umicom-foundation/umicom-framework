/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/policy.c
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
 * The embedded-browser layer stays provider neutral. A future WebKit, Edge or Qt adapter can implement the same function table.
 */

#include "umicom/browser/policy.h"
#include <string.h>
/*
 * Provide the browser policy default operation used by this module and its client
 * applications.
 */
UmiBrowserPolicy umi_browser_policy_default(void){UmiBrowserPolicy p={0,1,0,1};return p;}
/*
 * Provide the browser policy check operation used by this module and its client
 * applications.
 */
UmiStatus umi_browser_policy_check(const UmiBrowserPolicy *p,const char *url){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||url==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strncmp(url,"https://",8U)==0)return p->allow_https?UMI_STATUS_OK:UMI_STATUS_PERMISSION_DENIED;if(strncmp(url,"http://",7U)==0)return p->allow_http?UMI_STATUS_OK:UMI_STATUS_PERMISSION_DENIED;if(strncmp(url,"file://",7U)==0)return p->allow_file?UMI_STATUS_OK:UMI_STATUS_PERMISSION_DENIED;return UMI_STATUS_PERMISSION_DENIED;}
