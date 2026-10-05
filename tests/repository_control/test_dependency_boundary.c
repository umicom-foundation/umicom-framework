/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_dependency_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository dependency boundary semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
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
#include <string.h>
#include "umicom/repository/dependency.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/repository/dependency.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRepositoryDependencyTransferEqual(const UmiRepositoryDependency *a, const UmiRepositoryDependency *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->path, b->path) == 0 &&
        a->required == b->required;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRepositoryDependencyTransferTails(UmiRepositoryDependency *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->path) + 1U;
        memset(value->path + used, 0xa5, sizeof(value->path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRepositoryDependencyTransferMalformed(const UmiRepositoryDependency *sample)
{
    (void)sample;
    {
        UmiRepositoryDependency invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_repository_dependency_validate(&invalid) != UMI_STATUS_OK) ||
            umi_repository_dependency_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRepositoryDependency invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.path, 'x', sizeof(invalid.path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_repository_dependency_validate(&invalid) != UMI_STATUS_OK) ||
            umi_repository_dependency_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRepositoryDependencyTransferCases, UmiRepositoryDependency,
    umi_repository_dependency_archive_encode, umi_repository_dependency_archive_decode,
    UmiRepositoryDependencyTransferEqual, UmiRepositoryDependencyTransferTails, UmiRepositoryDependencyTransferMalformed)

int main(void)
{
    UmiRepositoryDependency d;
    assert(umi_repository_dependency_init(&d, "studio", "applications\\studio", 0) == UMI_STATUS_OK);
    if (UmiRepositoryDependencyTransferCases(&d) != 0) return 1;

    assert(strcmp(d.path, "applications/studio") == 0);
    return 0;
}
