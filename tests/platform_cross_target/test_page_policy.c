/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_page_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the page policy cross-target capability.
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

#include "umicom/platform/cross_target/page_policy.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtPagePolicy p={4096U,2097152U,true,true};CHECK(umi_ct_page_policy_validate(&p)==UMI_STATUS_OK);CHECK(umi_ct_page_align_up(&p,4097U)==8192U);CHECK(umi_ct_page_count(&p,8193U)==3U);return 0;}
