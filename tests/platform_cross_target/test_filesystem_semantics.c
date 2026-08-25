/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_filesystem_semantics.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the filesystem semantics cross-target capability.
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

#include "umicom/platform/cross_target/filesystem_semantics.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtFilesystemSemantics u=umi_ct_filesystem_semantics_default(UMI_CT_OS_UMICOM),b=umi_ct_filesystem_semantics_default(UMI_CT_OS_BARE_METAL);CHECK(umi_ct_filesystem_semantics_score(&u)==6U);CHECK(umi_ct_filesystem_semantics_score(&b)<6U);return 0;}
