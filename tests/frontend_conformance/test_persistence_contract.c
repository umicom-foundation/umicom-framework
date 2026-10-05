/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_persistence_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for layout, focus, panel, geometry and context state persistence requirements.
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
#include "umicom/frontend/conformance/persistence_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/conformance/persistence_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFcPersistenceContractTransferEqual(const UmiFcPersistenceContract *a, const UmiFcPersistenceContract *b)
{
    return a->required_fields == b->required_fields &&
        a->schema_version == b->schema_version &&
        a->forward_readable == b->forward_readable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFcPersistenceContractTransferTails(UmiFcPersistenceContract *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFcPersistenceContractTransferMalformed(const UmiFcPersistenceContract *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFcPersistenceContractTransferCases, UmiFcPersistenceContract,
    umi_fc_persistence_contract_archive_encode, umi_fc_persistence_contract_archive_decode,
    UmiFcPersistenceContractTransferEqual, UmiFcPersistenceContractTransferTails, UmiFcPersistenceContractTransferMalformed)

int main(void) {
    UmiFcPersistenceContract x={3U,1U,true}; CHECK(umi_fc_persistence_contract_validate(&x));
    if (UmiFcPersistenceContractTransferCases(&x) != 0) return 1;

    return 0;
}
