/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/theme_token.h
 *
 * PURPOSE:
 *   semantic theme token representation independent of GTK CSS, Qt stylesheets or browser CSS.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_THEME_TOKEN_H
#define UMICOM_FRONTEND_CONFORMANCE_THEME_TOKEN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc theme token data shared with callers of this public contract.
 */
typedef struct UmiFcThemeToken { char id[UMI_FC_ID_CAPACITY]; double r,g,b,a; bool required; } UmiFcThemeToken;
/**
 * Provide the fc theme token make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_theme_token_make(const char *id,double r,double g,double b,double a,bool required,UmiFcThemeToken *out_token);
/**
 * Provide the fc theme token distance operation used by this module and its client
 * applications.
 */
double umi_fc_theme_token_distance(const UmiFcThemeToken *a,const UmiFcThemeToken *b);

#ifdef __cplusplus
}
#endif
#endif
