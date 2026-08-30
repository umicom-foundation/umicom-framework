/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_hardware_device.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the hardware device cross-target capability.
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

#include "umicom/platform/cross_target/hardware_device.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtHardwareDevice d={"uart0","ns16550a",UMI_CT_DEVICE_OTHER,UINT64_C(0x10000000),0x100U,10U,true};CHECK(umi_ct_hardware_device_validate(&d)==UMI_STATUS_OK);return 0;}
