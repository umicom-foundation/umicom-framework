/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_boot_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the boot plan cross-target capability.
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

#include "umicom/platform/cross_target/boot_plan.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtBootPlan p;UmiCtBootService a={"user",UMI_CT_BOOT_USERSPACE,false,100U},b={"mem",UMI_CT_BOOT_MEMORY,true,100U};umi_ct_boot_plan_init(&p);CHECK(umi_ct_boot_plan_add(&p,&a)==UMI_STATUS_OK);CHECK(umi_ct_boot_plan_add(&p,&b)==UMI_STATUS_OK);CHECK(umi_ct_boot_plan_sort(&p)==UMI_STATUS_OK);CHECK(p.services[0].phase==UMI_CT_BOOT_MEMORY);return 0;}
