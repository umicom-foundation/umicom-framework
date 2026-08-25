/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_render_fingerprint.c
 *
 * PURPOSE:
 *   Focused regression coverage for stable render fingerprints independent of toolkit-specific object addresses.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/render_fingerprint.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    uint64_t a=umi_fc_render_fingerprint_begin("editor"); uint64_t b=umi_fc_render_fingerprint_add(a,"panel","main",1); CHECK(a!=0U&&b!=a); CHECK(b==umi_fc_render_fingerprint_add(a,"panel","main",1));
    return 0;
}
