/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_launcher_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for launcher executable, arguments and working-directory contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/launcher_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrLauncherManifest value; umi_dr_launcher_manifest_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"default")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.executable,sizeof(value.executable),"bin/app")==UMI_STATUS_OK); CHECK(umi_dr_launcher_manifest_valid(&value)); CHECK(umi_dr_launcher_manifest_fingerprint(&value) != 0U);
    return 0;
}
