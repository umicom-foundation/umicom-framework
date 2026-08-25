/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_umicom_os_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the umicom os profile cross-target capability.
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

#include "umicom/platform/cross_target/umicom_os_profile.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtUmicomOsProfile p;CHECK(umi_ct_umicom_os_profile_default(&p)==UMI_STATUS_OK);CHECK(umi_ct_umicom_os_profile_validate(&p)==UMI_STATUS_OK);CHECK(p.target.architecture==UMI_CT_ARCH_RISCV64&&p.smp&&p.userspace);return 0;}
