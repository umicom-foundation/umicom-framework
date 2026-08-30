/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_types.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the types cross-target capability.
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

#include "umicom/platform/cross_target/types.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    char value[16];
    uint64_t a, b;
    CHECK(umi_ct_id_valid("riscv64.umicom"));
    CHECK(!umi_ct_id_valid("bad id"));
    CHECK(umi_ct_copy(value, sizeof(value), "riscv64") == UMI_STATUS_OK);
    CHECK(umi_ct_copy(value, 4U, "riscv64") == UMI_STATUS_CAPACITY_EXCEEDED);
    a = umi_ct_hash_text(0U, "umicom"); b = umi_ct_hash_text(0U, "umicom");
    CHECK(a != 0U && a == b);
    CHECK(umi_ct_support_level_text(UMI_CT_SUPPORT_NATIVE)[0] == 'n');
    return 0;
}
