/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_dynamic_library_semantics.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the dynamic library semantics cross-target capability.
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

#include "umicom/platform/cross_target/dynamic_library_semantics.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

#include <string.h>
int main(void){UmiCtDynamicLibrarySemantics s=umi_ct_dynamic_library_semantics_default(UMI_CT_OS_LINUX);char n[64];CHECK(umi_ct_dynamic_library_name(&s,"umicom",n,sizeof(n))==UMI_STATUS_OK);CHECK(strcmp(n,"libumicom.so")==0);return 0;}
