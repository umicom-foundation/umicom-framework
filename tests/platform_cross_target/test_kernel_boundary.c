/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_kernel_boundary.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the kernel boundary cross-target capability.
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

#include "umicom/platform/cross_target/kernel_boundary.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtKernelBoundary b={"syscall",UMI_CT_DOMAIN_USER,UMI_CT_DOMAIN_KERNEL,true,true,true};CHECK(umi_ct_kernel_boundary_validate(&b)==UMI_STATUS_OK);b.privileged=false;CHECK(umi_ct_kernel_boundary_validate(&b)==UMI_STATUS_INVALID_STATE);return 0;}
