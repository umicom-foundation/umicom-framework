/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_model_catalogue_admission.c
 *
 * PURPOSE:
 *   Verify complete model identities, revision admission and unchanged state on rejected catalogue updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/developer_platform/model_catalogue.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/* Compare failed publications byte for byte. The baseline uses memcpy because
 * structure assignment need not preserve padding bytes on every compiler. */
static int unchanged(const UmiAiDevModelCatalogue *a, const UmiAiDevModelCatalogue *b)
{
    return memcmp(a, b, sizeof(*a)) == 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiAiDevModelCatalogue c, before;
    umi_ai_dev_model_catalogue_init(&c);
    char longest[UMI_AI_DEV_ID_CAPACITY];
    memset(longest, 'a', sizeof(longest));
    longest[sizeof(longest) - 1U] = '\0';
    char overlong[UMI_AI_DEV_ID_CAPACITY + 2U];
    memset(overlong, 'a', sizeof(overlong));
    overlong[sizeof(overlong) - 1U] = '\0';

    if (strcmp(mode, "nulls") == 0) {
        umi_ai_dev_model_catalogue_init(NULL);
        CHECK(UmiAiDevModelCatalogueValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_dev_model_catalogue_upsert(NULL, "x", 0, 0) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDevModelCatalogueUpsertIfRevision(NULL, "x", 0, 0, 0) == UMI_STATUS_INVALID_ARGUMENT);
        memcpy(&before, &c, sizeof(c));
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, NULL, 0, 0) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(unchanged(&c, &before));
        CHECK(umi_ai_dev_model_catalogue_find(NULL, "x") == NULL);
        CHECK(umi_ai_dev_model_catalogue_find(&c, NULL) == NULL);
        CHECK(umi_ai_dev_model_catalogue_enabled_count(NULL) == 0);
    } else if (strcmp(mode, "lifecycle") == 0) {
        CHECK(UmiAiDevModelCatalogueValidate(&c) == UMI_STATUS_OK);
        CHECK(c.count == 0 && c.revision == 0);
        CHECK(umi_ai_dev_model_catalogue_find(&c, "x") == NULL);
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "x", 3, 4) == UMI_STATUS_OK);
        CHECK(c.count == 1 && c.revision == 1);
        CHECK(umi_ai_dev_model_catalogue_find(&c, "x") == &c.entries[0]);
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "x", 7, 8) == UMI_STATUS_OK);
        CHECK(c.count == 1 && c.revision == 2 && c.entries[0].priority == 7 && c.entries[0].flags == 8);
    } else if (strcmp(mode, "max-id") == 0) {
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, longest, 0, 0) == UMI_STATUS_OK);
        CHECK(strcmp(c.entries[0].id, longest) == 0);
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, longest, 1, 2) == UMI_STATUS_OK);
        CHECK(c.count == 1 && c.revision == 2);
    } else if (strcmp(mode, "overlong") == 0 || strcmp(mode, "shared-prefix") == 0 ||
               strcmp(mode, "nonterminated") == 0 || strcmp(mode, "empty-id") == 0) {
        if (strcmp(mode, "shared-prefix") == 0)
            CHECK(umi_ai_dev_model_catalogue_upsert(&c, longest, 4, 5) == UMI_STATUS_OK);
        if (strcmp(mode, "nonterminated") == 0) memset(overlong, 'x', sizeof(overlong));
        const char *id = strcmp(mode, "empty-id") == 0 ? "" : overlong;
        memcpy(&before, &c, sizeof(c));
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, id, 8, 9) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(unchanged(&c, &before));
        CHECK(umi_ai_dev_model_catalogue_find(&c, id) == NULL);
        if (strcmp(mode, "shared-prefix") == 0) {
            overlong[UMI_AI_DEV_ID_CAPACITY] = 'z';
            CHECK(umi_ai_dev_model_catalogue_upsert(&c, overlong, 8, 9) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(unchanged(&c, &before));
        }
    } else if (strcmp(mode, "case-sensitive") == 0) {
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "Model", 1, 0) == UMI_STATUS_OK);
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "model", 2, 0) == UMI_STATUS_OK);
        CHECK(c.count == 2 && umi_ai_dev_model_catalogue_find(&c, "MODEL") == NULL);
    } else if (strcmp(mode, "extension-flags") == 0) {
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "x", UINT32_MAX, UINT64_MAX) == UMI_STATUS_OK);
        CHECK(c.entries[0].flags == UINT64_MAX && c.entries[0].priority == UINT32_MAX);
        CHECK(UmiAiDevModelCatalogueValidate(&c) == UMI_STATUS_OK);
    } else if (strcmp(mode, "alias-existing") == 0) {
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "local/model", 0, 0) == UMI_STATUS_OK);
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, c.entries[0].id, 1, 2) == UMI_STATUS_OK);
        CHECK(c.count == 1 && strcmp(c.entries[0].id, "local/model") == 0);
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, c.entries[0].id + 6, 2, 3) == UMI_STATUS_OK);
        CHECK(c.count == 2 && strcmp(c.entries[1].id, "model") == 0);
    } else if (strcmp(mode, "alias-new-slot") == 0) {
        memcpy(c.entries[0].id, "model", 6);
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, c.entries[0].id, 3, 4) == UMI_STATUS_OK);
        CHECK(c.count == 1 && strcmp(c.entries[0].id, "model") == 0);
    } else if (strcmp(mode, "capacity") == 0 || strcmp(mode, "full-update") == 0) {
        for (size_t i = 0; i < UMI_AI_DEV_SMALL_CAPACITY; ++i) {
            char id[32];
            int n = snprintf(id, sizeof(id), "model-%zu", i);
            CHECK(n > 0 && (size_t)n < sizeof(id));
            CHECK(umi_ai_dev_model_catalogue_upsert(&c, id, 0, 0) == UMI_STATUS_OK);
        }
        memcpy(&before, &c, sizeof(c));
        if (strcmp(mode, "capacity") == 0) {
            CHECK(umi_ai_dev_model_catalogue_upsert(&c, "extra", 0, 0) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(unchanged(&c, &before));
        } else {
            CHECK(umi_ai_dev_model_catalogue_upsert(&c, "model-0", 9, 7) == UMI_STATUS_OK);
            CHECK(c.count == UMI_AI_DEV_SMALL_CAPACITY && c.revision == before.revision + 1);
            CHECK(c.entries[0].priority == 9 && c.entries[0].flags == 7);
        }
    } else if (strcmp(mode, "revision") == 0) {
        CHECK(UmiAiDevModelCatalogueUpsertIfRevision(&c, "x", 0, 0, 0) == UMI_STATUS_OK);
        CHECK(UmiAiDevModelCatalogueUpsertIfRevision(&c, "y", 0, 0, 1) == UMI_STATUS_OK);
        CHECK(UmiAiDevModelCatalogueUpsertIfRevision(&c, "x", 2, 3, 2) == UMI_STATUS_OK);
        CHECK(c.count == 2 && c.revision == 3 && c.entries[0].priority == 2);
    } else if (strcmp(mode, "stale-insert") == 0 || strcmp(mode, "stale-update") == 0) {
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "x", 0, 0) == UMI_STATUS_OK);
        memcpy(&before, &c, sizeof(c));
        const char *id = strcmp(mode, "stale-insert") == 0 ? "y" : "x";
        CHECK(UmiAiDevModelCatalogueUpsertIfRevision(&c, id, 9, 7, 0) == UMI_STATUS_BUSY);
        CHECK(unchanged(&c, &before));
    } else if (strcmp(mode, "saturation-insert") == 0 || strcmp(mode, "saturation-update") == 0) {
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "x", 0, 0) == UMI_STATUS_OK);
        c.revision = UINT64_MAX;
        memcpy(&before, &c, sizeof(c));
        const char *id = strcmp(mode, "saturation-insert") == 0 ? "y" : "x";
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, id, 0, 0) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(unchanged(&c, &before));
    } else if (strncmp(mode, "corrupt-", 8) == 0) {
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "x", 0, 0) == UMI_STATUS_OK);
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "y", 0, 0) == UMI_STATUS_OK);
        if (strcmp(mode, "corrupt-count") == 0) c.count = SIZE_MAX;
        else if (strcmp(mode, "corrupt-id") == 0) memset(c.entries[0].id, 'x', sizeof(c.entries[0].id));
        else if (strcmp(mode, "corrupt-empty") == 0) c.entries[0].id[0] = '\0';
        else if (strcmp(mode, "corrupt-duplicate") == 0) memcpy(c.entries[1].id, "x", 2);
        else if (strcmp(mode, "corrupt-revision") == 0) c.revision = 1;
        else CHECK(0);
        memcpy(&before, &c, sizeof(c));
        CHECK(UmiAiDevModelCatalogueValidate(&c) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "z", 0, 0) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_ai_dev_model_catalogue_find(&c, "x") == NULL);
        CHECK(umi_ai_dev_model_catalogue_enabled_count(&c) == 0);
        CHECK(unchanged(&c, &before));
    } else if (strcmp(mode, "enabled") == 0) {
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "x", 0, 0) == UMI_STATUS_OK);
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "y", 0, 0) == UMI_STATUS_OK);
        c.entries[0].enabled = 0;
        c.entries[1].enabled = -7;
        CHECK(umi_ai_dev_model_catalogue_enabled_count(&c) == 1);
        CHECK(umi_ai_dev_model_catalogue_upsert(&c, "x", 0, 0) == UMI_STATUS_OK);
        CHECK(umi_ai_dev_model_catalogue_enabled_count(&c) == 2);
    } else CHECK(0);
    return 0;
}
