/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_recovery_budget.c
 * PURPOSE: Refuse automatic recovery admission without pruning or replacing earlier snapshots.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/document/recovery_storage.h"
#include "umicom/platform/rooted_files.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1],
               *cases[] = {"admit",           "exact-bytes",        "short-bytes",   "record-limit",
                           "existing-bytes",  "oversized-existing", "invalid-count", "invalid-budget",
                           "invalid-maximum", "cancelled"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    char root[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    UmiDocumentRecoveryInfo info = {0};
    strcpy(info.key, "0123456789abcdef0123456789abcdef");
    strcpy(info.display_name, "draft.c");
    info.text_bytes = 6U;
    UmiDocumentRecoveryDraft *draft = NULL;
    CHECK(UmiDocumentRecoveryDraftCreate(&info, "source", 6U, NULL, &draft) == UMI_STATUS_OK);
    unsigned char *encoded = NULL;
    size_t bytes = 0U;
    CHECK(UmiDocumentRecoveryDraftEncode(draft, &encoded, &bytes) == UMI_STATUS_OK);
    UmiDocumentRecoveryBytesFree(encoded);
    uint64_t budget = (uint64_t)bytes + 100U;
    size_t records = 2U;
    UmiStatus expected = UMI_STATUS_OK;
    int existing = 0;
    if (strcmp(mode, "exact-bytes") == 0)
        budget = (uint64_t)bytes;
    if (strcmp(mode, "short-bytes") == 0)
    {
        budget = (uint64_t)bytes - 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "record-limit") == 0 || strcmp(mode, "existing-bytes") == 0 ||
        strcmp(mode, "oversized-existing") == 0)
    {
        CHECK(UmiRootedFileWrite(root, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.draft", "earlier", 7U) ==
              UMI_STATUS_OK);
        existing = 1;
    }
    if (strcmp(mode, "record-limit") == 0)
    {
        records = 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "existing-bytes") == 0)
    {
        budget = (uint64_t)bytes + 6U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "oversized-existing") == 0)
    {
        budget = 6U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "invalid-count") == 0)
    {
        records = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-budget") == 0)
    {
        budget = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-maximum") == 0)
    {
        records = UMI_DOCUMENT_RECOVERY_CATALOGUE_LIMIT + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiCancellationToken *cancel = NULL;
    if (strcmp(mode, "cancelled") == 0)
    {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    CHECK(UmiDocumentRecoverySaveWithinBudget(root, draft, records, budget, cancel) == expected);
    UmiDocumentRecoveryCatalogue *catalogue = NULL;
    CHECK(UmiDocumentRecoveryList(root, NULL, &catalogue) == UMI_STATUS_OK &&
          UmiDocumentRecoveryCatalogueCount(catalogue) ==
              (size_t)existing + (expected == UMI_STATUS_OK ? 1U : 0U));
    if (existing)
    {
        unsigned char *earlier = NULL;
        size_t count = 0U;
        CHECK(UmiRootedFileRead(root, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.draft", 16U, &earlier, &count) ==
                  UMI_STATUS_OK &&
              count == 7U && memcmp(earlier, "earlier", 7U) == 0);
        UmiRootedFileFree(earlier);
    }
    UmiDocumentRecoveryCatalogueDestroy(catalogue);
    UmiDocumentRecoveryDraftDestroy(draft);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
