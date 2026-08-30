/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_riscv_platform.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the riscv platform cross-target capability.
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

#include "umicom/platform/cross_target/riscv_platform.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtRiscvPlatform p={"qemu.rv64",UMI_CT_RISCV_MACHINE_QEMU_VIRT,UINT64_C(512)*1024U*1024U,4U,true,true,true,true};CHECK(umi_ct_riscv_platform_validate(&p)==UMI_STATUS_OK);p.clint=false;CHECK(umi_ct_riscv_platform_validate(&p)==UMI_STATUS_INVALID_STATE);return 0;}
