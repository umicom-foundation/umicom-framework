/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_portability_matrix.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the portability matrix cross-target capability.
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

#include "umicom/platform/cross_target/portability_matrix.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtPortabilityMatrix m;umi_ct_portability_matrix_init(&m);CHECK(umi_ct_portability_matrix_set(&m,UMI_CT_OS_UMICOM,UMI_CT_PORT_THREADS,UMI_CT_SUPPORT_NATIVE)==UMI_STATUS_OK);CHECK(umi_ct_portability_matrix_get(&m,UMI_CT_OS_UMICOM,UMI_CT_PORT_THREADS)==UMI_CT_SUPPORT_NATIVE);return 0;}
