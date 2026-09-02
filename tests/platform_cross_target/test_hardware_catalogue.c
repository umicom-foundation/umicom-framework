/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_hardware_catalogue.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the hardware catalogue cross-target capability.
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

#include "umicom/platform/cross_target/hardware_catalogue.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtHardwareCatalogue c;UmiCtHardwareDevice a={"a","x",UMI_CT_DEVICE_OTHER,0x1000U,0x100U,1U,true},b={"b","x",UMI_CT_DEVICE_OTHER,0x1080U,0x100U,2U,true};umi_ct_hardware_catalogue_init(&c);CHECK(umi_ct_hardware_catalogue_add(&c,&a)==UMI_STATUS_OK);CHECK(umi_ct_hardware_catalogue_add(&c,&b)==UMI_STATUS_BUSY);CHECK(umi_ct_hardware_catalogue_find(&c,"a")!=NULL);return 0;}
