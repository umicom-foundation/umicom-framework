/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_os_service_dependency.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the os service dependency cross-target capability.
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

#include "umicom/platform/cross_target/os_service_dependency.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtOsServiceDependency d;CHECK(umi_ct_os_service_dependency_init(&d,"net","kernel",true)==UMI_STATUS_OK);CHECK(!umi_ct_os_service_dependency_self_cycle(&d));CHECK(umi_ct_os_service_dependency_init(&d,"net","net",true)==UMI_STATUS_INVALID_STATE);return 0;}
