/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_os_service_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the os service descriptor cross-target capability.
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

#include "umicom/platform/cross_target/os_service_descriptor.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtOsServiceDescriptor s;CHECK(umi_ct_os_service_descriptor_init(&s,"network.core",UMI_CT_OS_SERVICE_NETWORK,true)==UMI_STATUS_OK);CHECK(umi_ct_os_service_descriptor_add_dependency(&s,"kernel.scheduler")==UMI_STATUS_OK);CHECK(umi_ct_os_service_descriptor_add_dependency(&s,"kernel.scheduler")==UMI_STATUS_ALREADY_EXISTS);return 0;}
