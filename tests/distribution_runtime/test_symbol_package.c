/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_symbol_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for debug symbol package metadata and build-id matching.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/symbol_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrSymbolPackage value; umi_dr_symbol_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"sym")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.build_id,sizeof(value.build_id),"b")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.digest,sizeof(value.digest),"d")==UMI_STATUS_OK); CHECK(umi_dr_symbol_package_valid(&value)); CHECK(umi_dr_symbol_package_fingerprint(&value) != 0U);
    return 0;
}
