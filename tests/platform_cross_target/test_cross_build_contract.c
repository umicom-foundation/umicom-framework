/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_cross_build_contract.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the cross build contract cross-target capability.
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
#include "umicom/platform/cross_target/cross_build_contract.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/cross_build_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtCrossBuildContractTransferEqual(const UmiCtCrossBuildContract *a, const UmiCtCrossBuildContract *b)
{
    return strcmp(a->contract_id, b->contract_id) == 0 &&
        a->target.structure_size == b->target.structure_size &&
        a->target.api_version == b->target.api_version &&
        strcmp(a->target.triple, b->target.triple) == 0 &&
        strcmp(a->target.vendor, b->target.vendor) == 0 &&
        a->target.architecture == b->target.architecture &&
        a->target.operating_system == b->target.operating_system &&
        a->target.environment == b->target.environment &&
        a->target.pointer_bits == b->target.pointer_bits &&
        a->target.endian == b->target.endian &&
        strcmp(a->required_toolchain_family, b->required_toolchain_family) == 0 &&
        strcmp(a->required_abi, b->required_abi) == 0 &&
        a->require_sysroot == b->require_sysroot &&
        a->require_emulator == b->require_emulator &&
        a->require_debugger == b->require_debugger &&
        a->require_assembly == b->require_assembly;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtCrossBuildContractTransferTails(UmiCtCrossBuildContract *value)
{
    (void)value;
    {
        size_t used = strlen(value->contract_id) + 1U;
        memset(value->contract_id + used, 0xa5, sizeof(value->contract_id) - used);
    }
    {
        size_t used = strlen(value->target.triple) + 1U;
        memset(value->target.triple + used, 0xa5, sizeof(value->target.triple) - used);
    }
    {
        size_t used = strlen(value->target.vendor) + 1U;
        memset(value->target.vendor + used, 0xa5, sizeof(value->target.vendor) - used);
    }
    {
        size_t used = strlen(value->required_toolchain_family) + 1U;
        memset(value->required_toolchain_family + used, 0xa5, sizeof(value->required_toolchain_family) - used);
    }
    {
        size_t used = strlen(value->required_abi) + 1U;
        memset(value->required_abi + used, 0xa5, sizeof(value->required_abi) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtCrossBuildContractTransferMalformed(const UmiCtCrossBuildContract *sample)
{
    (void)sample;
    {
        UmiCtCrossBuildContract invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.contract_id, 'x', sizeof(invalid.contract_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_cross_build_contract_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_cross_build_contract_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated contract_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCtCrossBuildContract invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target.triple, 'x', sizeof(invalid.target.triple));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_cross_build_contract_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_cross_build_contract_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target.triple was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCtCrossBuildContract invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target.vendor, 'x', sizeof(invalid.target.vendor));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_cross_build_contract_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_cross_build_contract_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target.vendor was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCtCrossBuildContract invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.required_toolchain_family, 'x', sizeof(invalid.required_toolchain_family));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_cross_build_contract_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_cross_build_contract_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated required_toolchain_family was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCtCrossBuildContract invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.required_abi, 'x', sizeof(invalid.required_abi));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_cross_build_contract_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_cross_build_contract_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated required_abi was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtCrossBuildContractTransferCases, UmiCtCrossBuildContract,
    umi_ct_cross_build_contract_archive_encode, umi_ct_cross_build_contract_archive_decode,
    UmiCtCrossBuildContractTransferEqual, UmiCtCrossBuildContractTransferTails, UmiCtCrossBuildContractTransferMalformed)

int main(void){UmiCtCrossBuildContract c={0};CHECK(umi_ct_copy(c.contract_id,sizeof(c.contract_id),"rv64.build")==UMI_STATUS_OK);c.target.architecture=UMI_CT_ARCH_RISCV64;c.target.operating_system=UMI_CT_OS_UMICOM;CHECK(umi_ct_copy(c.required_toolchain_family,sizeof(c.required_toolchain_family),"gnu")==UMI_STATUS_OK);CHECK(umi_ct_copy(c.required_abi,sizeof(c.required_abi),"lp64d")==UMI_STATUS_OK);c.require_sysroot=true;CHECK(umi_ct_cross_build_contract_validate(&c)==UMI_STATUS_OK);
/* The archive reconstructs local ABI metadata rather than storing host structure sizes. Initialize the transfer fixture while retaining the earlier zero-initialized round-trip call for review. */
#if 0
    if (UmiCtCrossBuildContractTransferCases(&c) != 0) return 1;
#endif
    /* The validator above still covers the legacy zero-initialized input.
     * Archive decoding reconstructs the receiver's local structure size, so
     * exact field round trips use an explicitly initialized current target. */
    c.target.structure_size = (uint32_t)sizeof(c.target);
    c.target.api_version = UMI_CT_API_VERSION;
    if (UmiCtCrossBuildContractTransferCases(&c) != 0) return 1;
return 0;}
