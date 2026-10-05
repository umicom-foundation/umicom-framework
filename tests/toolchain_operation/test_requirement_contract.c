/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_requirement_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module requirement.
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
#include <assert.h>
#include "umicom/toolchain/requirement.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/toolchain/requirement.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiToolchainRequirementTransferEqual(const UmiToolchainRequirement *a, const UmiToolchainRequirement *b)
{
    return a->kind == b->kind &&
        a->required == b->required &&
        a->validate_version == b->validate_version;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiToolchainRequirementTransferTails(UmiToolchainRequirement *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiToolchainRequirementTransferMalformed(const UmiToolchainRequirement *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiToolchainRequirementTransferCases, UmiToolchainRequirement,
    umi_toolchain_requirement_archive_encode, umi_toolchain_requirement_archive_decode,
    UmiToolchainRequirementTransferEqual, UmiToolchainRequirementTransferTails, UmiToolchainRequirementTransferMalformed)

int main(void){ UmiToolchainRequirement r; umi_toolchain_requirement_init(&r, UMI_TOOL_GIT, 1); assert(umi_toolchain_requirement_validate(&r)==UMI_STATUS_OK);
    if (UmiToolchainRequirementTransferCases(&r) != 0) return 1;
 assert(r.required); return 0; }
