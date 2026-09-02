/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_library_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for shared/private runtime library placement policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/library_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrLibraryLayout value; umi_dr_library_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"libs")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.private_dir,sizeof(value.private_dir),"lib")==UMI_STATUS_OK); value.search_relative=true; CHECK(umi_dr_library_layout_valid(&value)); CHECK(umi_dr_library_layout_fingerprint(&value) != 0U);
    return 0;
}
