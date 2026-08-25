/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_target_operating_system.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the target operating system cross-target capability.
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

#include "umicom/platform/cross_target/target_operating_system.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtOperatingSystem os;CHECK(umi_ct_operating_system_parse("umos",&os)==UMI_STATUS_OK);CHECK(os==UMI_CT_OS_UMICOM);CHECK(umi_ct_operating_system_is_unix_like(os));CHECK(umi_ct_operating_system_parse("plan9",&os)==UMI_STATUS_NOT_FOUND);return 0;}
