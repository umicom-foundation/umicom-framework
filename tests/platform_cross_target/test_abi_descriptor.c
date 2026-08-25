/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_abi_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the abi descriptor cross-target capability.
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

#include "umicom/platform/cross_target/abi_descriptor.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtAbiDescriptor d={"lp64d",UMI_CT_DATA_LP64,UMI_CT_CALL_RISCV,64U,16U,128U,true};CHECK(umi_ct_abi_descriptor_validate(&d)==UMI_STATUS_OK);d.pointer_bits=32U;CHECK(umi_ct_abi_descriptor_validate(&d)==UMI_STATUS_INVALID_STATE);return 0;}
