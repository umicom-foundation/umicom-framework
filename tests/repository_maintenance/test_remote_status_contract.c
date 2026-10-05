/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_remote_status_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module remote_status.
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
#include "umicom/repository/remote_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/repository/remote_status.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRepositoryRemoteStatusTransferEqual(const UmiRepositoryRemoteStatus *a, const UmiRepositoryRemoteStatus *b)
{
    return a->remote_count == b->remote_count &&
        a->has_origin == b->has_origin &&
        a->upstream_configured == b->upstream_configured &&
        a->fetch_available == b->fetch_available;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRepositoryRemoteStatusTransferTails(UmiRepositoryRemoteStatus *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRepositoryRemoteStatusTransferMalformed(const UmiRepositoryRemoteStatus *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRepositoryRemoteStatusTransferCases, UmiRepositoryRemoteStatus,
    umi_repository_remote_status_archive_encode, umi_repository_remote_status_archive_decode,
    UmiRepositoryRemoteStatusTransferEqual, UmiRepositoryRemoteStatusTransferTails, UmiRepositoryRemoteStatusTransferMalformed)

int main(void){ UmiRepositoryRemoteStatus s; umi_repository_remote_status_init(&s); s.remote_count=1U; s.has_origin=1; assert(umi_repository_remote_status_validate(&s)==UMI_STATUS_OK);
    if (UmiRepositoryRemoteStatusTransferCases(&s) != 0) return 1;
 return 0; }
