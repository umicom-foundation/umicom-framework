/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_target_architecture.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the target architecture cross-target capability.
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

#include "umicom/platform/cross_target/target_architecture.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiCtArchitecture a=UMI_CT_ARCH_UNKNOWN; CHECK(umi_ct_architecture_parse("rv64",&a)==UMI_STATUS_OK); CHECK(a==UMI_CT_ARCH_RISCV64); CHECK(umi_ct_architecture_pointer_bits(a)==64U); CHECK(umi_ct_architecture_is_riscv(a)); CHECK(umi_ct_architecture_parse("mips",&a)==UMI_STATUS_NOT_FOUND); return 0; }
