/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_theme_token.c
 *
 * PURPOSE:
 *   Focused regression coverage for semantic theme token representation independent of GTK CSS, Qt stylesheets or browser CSS.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/theme_token.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcThemeToken a,b; CHECK(umi_fc_theme_token_make("surface",0.1,0.2,0.3,1.0,true,&a)==UMI_STATUS_OK); CHECK(umi_fc_theme_token_make("surface",0.1,0.2,0.3,1.0,true,&b)==UMI_STATUS_OK); CHECK(umi_fc_theme_token_distance(&a,&b)==0.0);
    return 0;
}
