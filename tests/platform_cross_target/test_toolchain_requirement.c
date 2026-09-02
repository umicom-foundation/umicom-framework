/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_toolchain_requirement.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the toolchain requirement cross-target capability.
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

#include "umicom/platform/cross_target/toolchain_requirement.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtToolchainRequirement r={"gnu",14U,0U,true,true,true,false};UmiCtToolchainEvidence e={"gnu",14U,2U,true,true,true,true};CHECK(umi_ct_toolchain_requirement_satisfied(&r,&e));e.linker=false;CHECK(!umi_ct_toolchain_requirement_satisfied(&r,&e));return 0;}
