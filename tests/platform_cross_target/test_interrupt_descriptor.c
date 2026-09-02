/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_interrupt_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the interrupt descriptor cross-target capability.
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

#include "umicom/platform/cross_target/interrupt_descriptor.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtInterruptDescriptor d={10U,2U,UMI_CT_IRQ_LEVEL,false,1U};CHECK(umi_ct_interrupt_descriptor_validate(&d,255U)==UMI_STATUS_OK);d.priority=0U;CHECK(umi_ct_interrupt_descriptor_validate(&d,255U)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
