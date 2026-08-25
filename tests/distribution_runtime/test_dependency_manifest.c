/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_dependency_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for package dependency declaration with version and optionality constraints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/dependency_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrDependencyManifest value; umi_dr_dependency_manifest_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"dep") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.package_id,sizeof(value.package_id),"framework") == UMI_STATUS_OK); CHECK(umi_dr_dependency_manifest_valid(&value)); CHECK(umi_dr_dependency_manifest_fingerprint(&value) != 0U);
    return 0;
}
