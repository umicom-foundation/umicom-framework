/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_types_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module operation_types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/operation_types.h"
int main(void){ assert(strcmp(umi_toolchain_operation_kind_text(UMI_TOOLCHAIN_OPERATION_REPOSITORY_READ), "repository-read") == 0); assert(umi_toolchain_operation_is_repository(UMI_TOOLCHAIN_OPERATION_REPOSITORY_WRITE)); return 0; }
