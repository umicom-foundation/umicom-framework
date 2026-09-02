/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_bundle_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for bundle manifest counts, total bytes and aggregate content fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrBundleManifest m; UmiDrBundleFile f; umi_dr_bundle_manifest_init(&m); umi_dr_bundle_file_init(&f); CHECK(umi_dr_copy_text(f.id,sizeof(f.id),"x")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(f.path,sizeof(f.path),"bin/x")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(f.digest,sizeof(f.digest),"d")==UMI_STATUS_OK); f.size_bytes=100U; CHECK(umi_dr_bundle_manifest_add(&m,&f)==UMI_STATUS_OK); CHECK(m.total_bytes==100U && m.file_count==1U);
    return 0;
}
