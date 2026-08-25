/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_capability_matrix.c
 *
 * PURPOSE:
 *   Focused regression coverage for cross-frontend capability matrix used to compare GTK4, Qt6, Native Web and headless renderers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/capability_matrix.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcCapabilityMatrix m; UmiFcFrontendKind fs[2]={UMI_FC_FRONTEND_GTK4,UMI_FC_FRONTEND_QT6}; umi_fc_capability_matrix_init(&m); CHECK(umi_fc_capability_matrix_set(&m,fs[0],7U)==UMI_STATUS_OK); CHECK(umi_fc_capability_matrix_set(&m,fs[1],3U)==UMI_STATUS_OK); CHECK(umi_fc_capability_matrix_common(&m,fs,2U)==3U);
    return 0;
}
