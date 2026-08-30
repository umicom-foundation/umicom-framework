/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_cross_target_service.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the cross target service cross-target capability.
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

#include "umicom/platform/cross_target/cross_target_service.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

#include "umicom/platform/cross_target/target_triple.h"
int main(void){UmiCtCrossTargetService s;UmiCtTarget t;UmiCtTargetProfile p;UmiCtCrossTargetSnapshot x={0};umi_ct_cross_target_service_init(&s);CHECK(umi_ct_target_triple_parse("riscv64-umicom-umicom-native",&t)==UMI_STATUS_OK);CHECK(umi_ct_target_profile_init(&p,"umos",&t,"lp64d")==UMI_STATUS_OK);CHECK(umi_ct_cross_target_service_register(&s,&p)==UMI_STATUS_OK);x.target=t;CHECK(umi_ct_copy(x.abi,sizeof(x.abi),"lp64d")==UMI_STATUS_OK);x.cpu_count=4U;x.page_size=4096U;x.fingerprint=1U;x.health.health=UMI_CT_HEALTH_READY;CHECK(umi_ct_cross_target_service_activate(&s,"umos",&x)==UMI_STATUS_OK);CHECK(umi_ct_cross_target_service_active(&s)!=NULL);return 0;}
