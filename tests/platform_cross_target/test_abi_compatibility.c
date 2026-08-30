/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_abi_compatibility.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the abi compatibility cross-target capability.
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

#include "umicom/platform/cross_target/abi_compatibility.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

#include <string.h>
int main(void){UmiCtAbiDescriptor a={"a",UMI_CT_DATA_LP64,UMI_CT_CALL_RISCV,64U,16U,128U,true},b=a;char r[64];CHECK(umi_ct_abi_compare(&a,&b,r,sizeof(r))==UMI_CT_ABI_COMPATIBLE);b.calling_convention=UMI_CT_CALL_UMICOM;CHECK(umi_ct_abi_compare(&a,&b,r,sizeof(r))==UMI_CT_ABI_BRIDGE_REQUIRED);b.pointer_bits=32U;CHECK(umi_ct_abi_compare(&a,&b,r,sizeof(r))==UMI_CT_ABI_INCOMPATIBLE);return 0;}
