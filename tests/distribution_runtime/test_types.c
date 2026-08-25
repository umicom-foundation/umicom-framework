/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_types.c
 *
 * PURPOSE:
 *   Focused regression coverage for shared cross-platform distribution vocabulary, stable identifiers, version comparison and deterministic fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/types.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    char text[32];
    CHECK(umi_dr_copy_text(text, sizeof(text), "windows") == UMI_STATUS_OK);
    CHECK(umi_dr_hash_text("same") == umi_dr_hash_text("same"));
    CHECK(umi_dr_version_compare((UmiDrVersion){1,2,0}, (UmiDrVersion){1,1,9}) > 0);
    CHECK(umi_dr_version_at_least((UmiDrVersion){2,0,0}, (UmiDrVersion){1,9,9}));
    CHECK(umi_dr_platform_text(UMI_DR_PLATFORM_LINUX)[0] == 'l');
    return 0;
}
