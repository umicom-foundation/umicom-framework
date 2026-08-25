/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_path_semantics.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the path semantics cross-target capability.
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

#include "umicom/platform/cross_target/path_semantics.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtPathSemantics w=umi_ct_path_semantics_default(UMI_CT_OS_WINDOWS),u=umi_ct_path_semantics_default(UMI_CT_OS_UMICOM);CHECK(umi_ct_path_is_absolute(&w,"C:\\umicom"));CHECK(umi_ct_path_is_absolute(&u,"/usr/bin"));CHECK(!umi_ct_path_is_absolute(&u,"tmp/x"));return 0;}
