/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_context_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for typed context-channel requirements for linked cross-application surfaces.
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
#include "umicom/frontend/conformance/context_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/conformance/context_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFcContextContractTransferEqual(const UmiFcContextContract *a, const UmiFcContextContract *b)
{
    return a->required_types == b->required_types &&
        a->bidirectional == b->bidirectional &&
        a->accessible_label == b->accessible_label;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFcContextContractTransferTails(UmiFcContextContract *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFcContextContractTransferMalformed(const UmiFcContextContract *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFcContextContractTransferCases, UmiFcContextContract,
    umi_fc_context_contract_archive_encode, umi_fc_context_contract_archive_decode,
    UmiFcContextContractTransferEqual, UmiFcContextContractTransferTails, UmiFcContextContractTransferMalformed)

int main(void) {
    UmiFcContextContract x={3U,true,true}; CHECK(umi_fc_context_contract_validate(&x));
    if (UmiFcContextContractTransferCases(&x) != 0) return 1;

    return 0;
}
