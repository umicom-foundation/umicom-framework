/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/policy.c
 *
 * PURPOSE:
 *   Validate which URL schemes an embedded browser may navigate to.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The embedded-browser layer stays provider neutral. A future WebKit, Edge or Qt adapter can implement the same function table.
 */

#include "umicom/browser/policy.h"
#include <string.h>
UmiBrowserPolicy umi_browser_policy_default(void){UmiBrowserPolicy p={0,1,0,1};return p;}
UmiStatus umi_browser_policy_check(const UmiBrowserPolicy *p,const char *url){if(p==NULL||url==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(strncmp(url,"https://",8U)==0)return p->allow_https?UMI_STATUS_OK:UMI_STATUS_PERMISSION_DENIED;if(strncmp(url,"http://",7U)==0)return p->allow_http?UMI_STATUS_OK:UMI_STATUS_PERMISSION_DENIED;if(strncmp(url,"file://",7U)==0)return p->allow_file?UMI_STATUS_OK:UMI_STATUS_PERMISSION_DENIED;return UMI_STATUS_PERMISSION_DENIED;}
