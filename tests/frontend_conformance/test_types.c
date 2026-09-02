/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_types.c
 *
 * PURPOSE:
 *   Focused regression coverage for shared frontend-conformance identifiers, enums, bounded text and deterministic scoring helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/types.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    char buf[32];
    CHECK(umi_fc_copy_text(buf, sizeof(buf), "gtk4") == UMI_STATUS_OK);
    CHECK(umi_fc_hash_text("stable") == umi_fc_hash_text("stable"));
    CHECK(umi_fc_ratio(3U, 4U) > 0.74 && umi_fc_ratio(3U, 4U) < 0.76);
    CHECK(umi_fc_clamp_score(2.0) == 1.0);
    CHECK(umi_fc_frontend_kind_text(UMI_FC_FRONTEND_NATIVE_WEB)[0] == 'n');
    return 0;
}
