/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_conformance_case.c
 *
 * PURPOSE:
 *   Focused regression coverage for deterministic conformance-case inputs used by automated frontend tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/conformance_case.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcConformanceCase c; CHECK(umi_fc_conformance_case_make("focus",3U,1U,true,&c)==UMI_STATUS_OK); CHECK(umi_fc_conformance_case_missing(&c)==2U);
    return 0;
}
