/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_socket_semantics.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the socket semantics cross-target capability.
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

#include "umicom/platform/cross_target/socket_semantics.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtSocketSemantics s={true,true,false,true,true,false};CHECK(umi_ct_socket_semantics_support(&s,true,false,true)==UMI_CT_SUPPORT_NATIVE);CHECK(umi_ct_socket_semantics_support(&s,true,true,true)==UMI_CT_SUPPORT_DEGRADED);return 0;}
