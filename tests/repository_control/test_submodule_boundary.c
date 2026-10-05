/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_submodule_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository submodule boundary semantics.
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
#include "umicom/repository/submodule.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/repository/submodule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRepositorySubmoduleTransferEqual(const UmiRepositorySubmodule *a, const UmiRepositorySubmodule *b)
{
    return strcmp(a->name, b->name) == 0 &&
        strcmp(a->path, b->path) == 0 &&
        strcmp(a->url, b->url) == 0 &&
        strcmp(a->branch, b->branch) == 0 &&
        a->required == b->required;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRepositorySubmoduleTransferTails(UmiRepositorySubmodule *value)
{
    (void)value;
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->path) + 1U;
        memset(value->path + used, 0xa5, sizeof(value->path) - used);
    }
    {
        size_t used = strlen(value->url) + 1U;
        memset(value->url + used, 0xa5, sizeof(value->url) - used);
    }
    {
        size_t used = strlen(value->branch) + 1U;
        memset(value->branch + used, 0xa5, sizeof(value->branch) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRepositorySubmoduleTransferMalformed(const UmiRepositorySubmodule *sample)
{
    (void)sample;
    {
        UmiRepositorySubmodule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_repository_submodule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_repository_submodule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRepositorySubmodule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.path, 'x', sizeof(invalid.path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_repository_submodule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_repository_submodule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRepositorySubmodule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.url, 'x', sizeof(invalid.url));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_repository_submodule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_repository_submodule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated url was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRepositorySubmodule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.branch, 'x', sizeof(invalid.branch));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_repository_submodule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_repository_submodule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated branch was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRepositorySubmoduleTransferCases, UmiRepositorySubmodule,
    umi_repository_submodule_archive_encode, umi_repository_submodule_archive_decode,
    UmiRepositorySubmoduleTransferEqual, UmiRepositorySubmoduleTransferTails, UmiRepositorySubmoduleTransferMalformed)

int main(void)
{
    UmiRepositorySubmodule s;
    assert(umi_repository_submodule_init(&s, "studio", "applications/studio", "", "", 0) == UMI_STATUS_OK);
    if (UmiRepositorySubmoduleTransferCases(&s) != 0) return 1;

    assert(s.branch[0] == '\0');
    assert(!s.required);
    return 0;
}
