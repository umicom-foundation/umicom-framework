/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_target_catalogue.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the target catalogue cross-target capability.
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

#include "umicom/platform/cross_target/target_catalogue.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtTargetCatalogue c;UmiCtTarget t={0};UmiCtTargetProfile p;umi_ct_target_catalogue_init(&c);CHECK(umi_ct_target_profile_init(&p,"rv64",&t,"lp64")==UMI_STATUS_OK);CHECK(umi_ct_target_catalogue_add(&c,&p)==UMI_STATUS_OK);CHECK(umi_ct_target_catalogue_add(&c,&p)==UMI_STATUS_ALREADY_EXISTS);CHECK(umi_ct_target_catalogue_find(&c,"rv64")!=NULL);return 0;}
