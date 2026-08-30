/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_syscall_catalogue.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the syscall catalogue cross-target capability.
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

#include "umicom/platform/cross_target/syscall_catalogue.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtSyscallCatalogue c;UmiCtSyscallDescriptor d={7U,"clock.read",0U,1U};umi_ct_syscall_catalogue_init(&c);CHECK(umi_ct_syscall_catalogue_add(&c,&d)==UMI_STATUS_OK);CHECK(umi_ct_syscall_catalogue_add(&c,&d)==UMI_STATUS_ALREADY_EXISTS);CHECK(umi_ct_syscall_catalogue_find_number(&c,7U)!=NULL);return 0;}
