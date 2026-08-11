/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/security.h
 *
 * PURPOSE:
 *   Bridge web requests to product-neutral permission checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_SECURITY_H
#define UMICOM_WEB_SECURITY_H
#include "umicom/web/request.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef int (*UmiWebAuthoriseFn)(const char *principal,const char *permission,void *user_data);
typedef struct UmiWebSecurity { UmiWebAuthoriseFn authorise; void *user_data; } UmiWebSecurity;
UmiStatus umi_web_security_init(UmiWebSecurity *security,UmiWebAuthoriseFn authorise,void *user_data);
UmiStatus umi_web_security_require(const UmiWebSecurity *security,const char *principal,const char *permission);
#ifdef __cplusplus
}
#endif
#endif
