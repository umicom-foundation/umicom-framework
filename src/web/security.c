/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/security.c
 *
 * PURPOSE:
 *   Implement a permission-check bridge for web endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/security.h"
/*
 * Initialise web security from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_web_security_init(UmiWebSecurity *security,UmiWebAuthoriseFn authorise,void *user_data){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(security==NULL)return UMI_STATUS_INVALID_ARGUMENT;security->authorise=authorise;security->user_data=user_data;return UMI_STATUS_OK;}
/*
 * Provide the web security require operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_security_require(const UmiWebSecurity *security,const char *principal,const char *permission){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(security==NULL||permission==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(security->authorise==NULL)return UMI_STATUS_PERMISSION_DENIED;return security->authorise(principal!=NULL?principal:"",permission,security->user_data)?UMI_STATUS_OK:UMI_STATUS_PERMISSION_DENIED;}
