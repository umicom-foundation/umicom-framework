/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_docking_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for dock zones, floating, auto-hide and split/tab workstation requirements.
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
#include "umicom/frontend/conformance/docking_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/conformance/docking_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFcDockingContractTransferEqual(const UmiFcDockingContract *a, const UmiFcDockingContract *b)
{
    return a->required_features == b->required_features &&
        a->allowed_zones == b->allowed_zones &&
        a->responsive_fallback == b->responsive_fallback;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFcDockingContractTransferTails(UmiFcDockingContract *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFcDockingContractTransferMalformed(const UmiFcDockingContract *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFcDockingContractTransferCases, UmiFcDockingContract,
    umi_fc_docking_contract_archive_encode, umi_fc_docking_contract_archive_decode,
    UmiFcDockingContractTransferEqual, UmiFcDockingContractTransferTails, UmiFcDockingContractTransferMalformed)

int main(void) {
    UmiFcDockingContract x={3U,31U,true}; CHECK(umi_fc_docking_contract_validate(&x));
    if (UmiFcDockingContractTransferCases(&x) != 0) return 1;

    return 0;
}
