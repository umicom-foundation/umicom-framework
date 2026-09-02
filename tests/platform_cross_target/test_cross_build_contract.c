/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_cross_build_contract.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the cross build contract cross-target capability.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/cross_build_contract.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtCrossBuildContract c={0};CHECK(umi_ct_copy(c.contract_id,sizeof(c.contract_id),"rv64.build")==UMI_STATUS_OK);c.target.architecture=UMI_CT_ARCH_RISCV64;c.target.operating_system=UMI_CT_OS_UMICOM;CHECK(umi_ct_copy(c.required_toolchain_family,sizeof(c.required_toolchain_family),"gnu")==UMI_STATUS_OK);CHECK(umi_ct_copy(c.required_abi,sizeof(c.required_abi),"lp64d")==UMI_STATUS_OK);c.require_sysroot=true;CHECK(umi_ct_cross_build_contract_validate(&c)==UMI_STATUS_OK);return 0;}
