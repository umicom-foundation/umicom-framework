/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_command_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for stable command exposure expectations independent of frontend toolkit.
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
#include "umicom/frontend/conformance/command_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/conformance/command_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFcCommandContractTransferEqual(const UmiFcCommandContract *a, const UmiFcCommandContract *b)
{
    return a->required_commands == b->required_commands &&
        a->command_fingerprint == b->command_fingerprint &&
        a->all_have_stable_ids == b->all_have_stable_ids;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFcCommandContractTransferTails(UmiFcCommandContract *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFcCommandContractTransferMalformed(const UmiFcCommandContract *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFcCommandContractTransferCases, UmiFcCommandContract,
    umi_fc_command_contract_archive_encode, umi_fc_command_contract_archive_decode,
    UmiFcCommandContractTransferEqual, UmiFcCommandContractTransferTails, UmiFcCommandContractTransferMalformed)

int main(void) {
    UmiFcCommandContract x={5U,123U,true}; CHECK(umi_fc_command_contract_validate(&x));
    if (UmiFcCommandContractTransferCases(&x) != 0) return 1;

    return 0;
}
