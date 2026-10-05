/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_riscv_platform.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the riscv platform cross-target capability.
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

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/platform/cross_target/riscv_platform.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/riscv_platform.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtRiscvPlatformTransferEqual(const UmiCtRiscvPlatform *a, const UmiCtRiscvPlatform *b)
{
    return strcmp(a->platform_id, b->platform_id) == 0 &&
        a->machine == b->machine &&
        a->memory_bytes == b->memory_bytes &&
        a->cpu_count == b->cpu_count &&
        a->plic == b->plic &&
        a->clint == b->clint &&
        a->pci == b->pci &&
        a->virtio == b->virtio;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtRiscvPlatformTransferTails(UmiCtRiscvPlatform *value)
{
    (void)value;
    {
        size_t used = strlen(value->platform_id) + 1U;
        memset(value->platform_id + used, 0xa5, sizeof(value->platform_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtRiscvPlatformTransferMalformed(const UmiCtRiscvPlatform *sample)
{
    (void)sample;
    {
        UmiCtRiscvPlatform invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.platform_id, 'x', sizeof(invalid.platform_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_riscv_platform_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_riscv_platform_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated platform_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtRiscvPlatformTransferCases, UmiCtRiscvPlatform,
    umi_ct_riscv_platform_archive_encode, umi_ct_riscv_platform_archive_decode,
    UmiCtRiscvPlatformTransferEqual, UmiCtRiscvPlatformTransferTails, UmiCtRiscvPlatformTransferMalformed)

int main(void){UmiCtRiscvPlatform p={"qemu.rv64",UMI_CT_RISCV_MACHINE_QEMU_VIRT,UINT64_C(512)*1024U*1024U,4U,true,true,true,true};CHECK(umi_ct_riscv_platform_validate(&p)==UMI_STATUS_OK);
    if (UmiCtRiscvPlatformTransferCases(&p) != 0) return 1;
p.clint=false;CHECK(umi_ct_riscv_platform_validate(&p)==UMI_STATUS_INVALID_STATE);return 0;}
