/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_sysroot_requirement.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the sysroot requirement cross-target capability.
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

#include "umicom/platform/cross_target/sysroot_requirement.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

#include "umicom/platform/cross_target/target_triple.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtSysrootRequirement r={0};UmiCtSysrootEvidence e={0};CHECK(umi_ct_target_triple_parse("riscv64-umicom-umicom-native",&r.target)==UMI_STATUS_OK);e.target=r.target;CHECK(umi_ct_copy(r.sysroot_id,sizeof(r.sysroot_id),"umos-sdk")==UMI_STATUS_OK);CHECK(umi_ct_copy(e.sysroot_id,sizeof(e.sysroot_id),"umos-sdk")==UMI_STATUS_OK);r.require_headers=r.require_libraries=r.require_crt=true;e.headers=e.libraries=e.crt=true;CHECK(umi_ct_sysroot_requirement_satisfied(&r,&e));e.crt=false;CHECK(!umi_ct_sysroot_requirement_satisfied(&r,&e));return 0;}
