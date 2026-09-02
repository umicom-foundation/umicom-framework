/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_architecture_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for CPU architecture, ABI, pointer-width and endianness descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/architecture_descriptor.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrArchitectureDescriptor value; umi_dr_architecture_descriptor_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"x86_64") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.abi,sizeof(value.abi),"gnu") == UMI_STATUS_OK); CHECK(umi_dr_architecture_descriptor_valid(&value)); CHECK(umi_dr_architecture_descriptor_fingerprint(&value) != 0U);
    return 0;
}
