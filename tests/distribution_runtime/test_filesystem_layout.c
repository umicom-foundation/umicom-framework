/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_filesystem_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for canonical install-root, bin, lib, share and writable-state layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/filesystem_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrFilesystemLayout value; umi_dr_filesystem_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"portable")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.root,sizeof(value.root),".")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.bin,sizeof(value.bin),"bin")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.lib,sizeof(value.lib),"lib")==UMI_STATUS_OK); CHECK(umi_dr_filesystem_layout_valid(&value)); CHECK(umi_dr_filesystem_layout_fingerprint(&value) != 0U);
    return 0;
}
