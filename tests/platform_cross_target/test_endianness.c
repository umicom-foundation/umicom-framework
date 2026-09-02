/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_endianness.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the endianness cross-target capability.
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

#include "umicom/platform/cross_target/endianness.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){CHECK(umi_ct_bswap16(UINT16_C(0x1234))==UINT16_C(0x3412));CHECK(umi_ct_bswap32(UINT32_C(0x11223344))==UINT32_C(0x44332211));CHECK(umi_ct_bswap64(UINT64_C(0x0102030405060708))==UINT64_C(0x0807060504030201));return 0;}
