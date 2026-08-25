/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_riscv_privilege.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the riscv privilege cross-target capability.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/riscv_privilege.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtPrivilegeMask m=UMI_CT_PRIV_USER|UMI_CT_PRIV_SUPERVISOR|UMI_CT_PRIV_MACHINE;CHECK(umi_ct_riscv_privilege_validate_os(m,false)==UMI_STATUS_OK);CHECK(umi_ct_riscv_privilege_validate_os(m,true)==UMI_STATUS_UNAVAILABLE);return 0;}
