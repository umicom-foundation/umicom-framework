/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_index_status_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module index_status.
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
#include "umicom/repository/index_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/repository/index_status.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRepositoryIndexStatusTransferEqual(const UmiRepositoryIndexStatus *a, const UmiRepositoryIndexStatus *b)
{
    return a->staged_paths == b->staged_paths &&
        a->staged_gitlinks == b->staged_gitlinks &&
        a->conflicted_paths == b->conflicted_paths;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRepositoryIndexStatusTransferTails(UmiRepositoryIndexStatus *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRepositoryIndexStatusTransferMalformed(const UmiRepositoryIndexStatus *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRepositoryIndexStatusTransferCases, UmiRepositoryIndexStatus,
    umi_repository_index_status_archive_encode, umi_repository_index_status_archive_decode,
    UmiRepositoryIndexStatusTransferEqual, UmiRepositoryIndexStatusTransferTails, UmiRepositoryIndexStatusTransferMalformed)

int main(void){ UmiRepositoryIndexStatus s; umi_repository_index_status_init(&s); s.staged_paths=2U; s.staged_gitlinks=1U; assert(umi_repository_index_status_validate(&s)==UMI_STATUS_OK);
    if (UmiRepositoryIndexStatusTransferCases(&s) != 0) return 1;
 return 0; }
