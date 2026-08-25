/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_hardware_bus.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the hardware bus cross-target capability.
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

#include "umicom/platform/cross_target/hardware_bus.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtHardwareBus b={"virtio",UMI_CT_BUS_VIRTIO,true,true,true,64U};CHECK(umi_ct_hardware_bus_validate(&b)==UMI_STATUS_OK);b.address_bits=24U;CHECK(umi_ct_hardware_bus_validate(&b)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
