/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_target_triple.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the target triple cross-target capability.
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

#include "umicom/platform/cross_target/target_triple.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtTarget t,u;char text[160];CHECK(umi_ct_target_triple_parse("riscv64-umicom-umicom-native",&t)==UMI_STATUS_OK);CHECK(t.architecture==UMI_CT_ARCH_RISCV64);CHECK(t.operating_system==UMI_CT_OS_UMICOM);CHECK(umi_ct_target_triple_format(&t,text,sizeof(text))==UMI_STATUS_OK);CHECK(strstr(text,"riscv64")!=NULL);u=t;CHECK(umi_ct_target_compatible(&t,&u));u.pointer_bits=32U;CHECK(!umi_ct_target_compatible(&t,&u));return 0;}
