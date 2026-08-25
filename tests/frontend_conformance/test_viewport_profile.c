/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_viewport_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for viewport aspect, pixel density and input-mode classification for frontend adaptation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/viewport_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcViewportProfile p={1920,1080,1.0,UMI_FC_INPUT_MOUSE_KEYBOARD}; CHECK(umi_fc_viewport_profile_aspect(&p)>1.7); CHECK(!umi_fc_viewport_profile_is_portrait(&p));
    return 0;
}
