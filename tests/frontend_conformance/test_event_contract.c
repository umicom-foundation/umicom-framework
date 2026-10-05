/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_event_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for semantic user-event support requirements independent of native toolkit event classes.
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
#include "umicom/frontend/conformance/event_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/conformance/event_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFcEventContractTransferEqual(const UmiFcEventContract *a, const UmiFcEventContract *b)
{
    return a->required_families == b->required_families &&
        a->ordered == b->ordered &&
        a->cancellable == b->cancellable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFcEventContractTransferTails(UmiFcEventContract *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFcEventContractTransferMalformed(const UmiFcEventContract *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFcEventContractTransferCases, UmiFcEventContract,
    umi_fc_event_contract_archive_encode, umi_fc_event_contract_archive_decode,
    UmiFcEventContractTransferEqual, UmiFcEventContractTransferTails, UmiFcEventContractTransferMalformed)

int main(void) {
    UmiFcEventContract x={3U,true,true}; CHECK(umi_fc_event_contract_validate(&x));
    if (UmiFcEventContractTransferCases(&x) != 0) return 1;

    return 0;
}
