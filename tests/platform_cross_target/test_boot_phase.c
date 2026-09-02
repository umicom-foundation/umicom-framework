/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_boot_phase.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the boot phase cross-target capability.
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

#include "umicom/platform/cross_target/boot_phase.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){CHECK(umi_ct_boot_phase_transition_valid(UMI_CT_BOOT_MEMORY,UMI_CT_BOOT_INTERRUPTS));CHECK(!umi_ct_boot_phase_transition_valid(UMI_CT_BOOT_MEMORY,UMI_CT_BOOT_USERSPACE));CHECK(umi_ct_boot_phase_text(UMI_CT_BOOT_READY)[0]=='r');return 0;}
