/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_runtime_catalogue_revision.c
 *
 * PURPOSE:
 *   Verify transactional revision updates and bounded refusal of corrupted private state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "ai/runtime_catalogue_fixture.h"
/* Include the real implementation only to inject unreachable public-state faults
 * and revision saturation. Production callers still receive an opaque owner. */
#include "../src/ai/runtime_catalogue.c"

static void invalid_owner(UmiAiRuntimeCatalogue *catalogue)
{
    UmiAiRuntimeDescriptor value = catalogue_fixture("runtime.local"), out, sentinel;
    UmiAiRuntimeCatalogue *before = malloc(sizeof(*before));
    assert(before != NULL); *before = *catalogue;
    memset(&sentinel, 0x5a, sizeof(sentinel)); out = sentinel;
    assert(UmiAiRuntimeCatalogueValidate(catalogue) == UMI_STATUS_INVALID_STATE);
    assert(umi_ai_runtime_catalogue_count(catalogue) == 0U);
    assert(umi_ai_runtime_catalogue_provider_count(catalogue) == 0U);
    assert(umi_ai_runtime_catalogue_find(catalogue, "runtime.local", &out) == UMI_STATUS_INVALID_STATE);
    assert(umi_ai_runtime_catalogue_at(catalogue, 0U, &out) == UMI_STATUS_INVALID_STATE);
    assert(umi_ai_runtime_catalogue_select(catalogue, 0U, 0U, 0, &out) == UMI_STATUS_INVALID_STATE);
    assert(umi_ai_runtime_catalogue_upsert(catalogue, &value) == UMI_STATUS_INVALID_STATE);
    assert(umi_ai_runtime_catalogue_set_health(catalogue, value.runtime_id, &value.health) == UMI_STATUS_INVALID_STATE);
    assert(memcmp(&out, &sentinel, sizeof(out)) == 0);
    assert(memcmp(catalogue, before, sizeof(*before)) == 0); free(before);
}

int main(int argc, char **argv)
{
    UmiAiRuntimeCatalogue *catalogue = NULL, *before;
    UmiAiRuntimeDescriptor value = catalogue_fixture("runtime.local"), observed, latest;
    UmiAiHealth health = {0};
    assert(argc == 2);
    assert(umi_ai_runtime_catalogue_create(&catalogue) == UMI_STATUS_OK);
    assert(UmiAiRuntimeCatalogueUpsertIfRevision(catalogue, &value, 0U) == UMI_STATUS_OK);
    assert(umi_ai_runtime_catalogue_find(catalogue, value.runtime_id, &observed) == UMI_STATUS_OK);
    before = malloc(sizeof(*before)); assert(before != NULL); *before = *catalogue;
    strcpy(health.provider_id, value.provider_id); health.available = 1; health.checked_at_ns = 12U;
    if (strcmp(argv[1], "conditional") == 0) {
        assert(UmiAiRuntimeCatalogueUpsertIfRevision(catalogue, &value, 0U) == UMI_STATUS_BUSY);
        assert(UmiAiRuntimeCatalogueUpsertIfRevision(catalogue, &value, observed.revision + 1U) == UMI_STATUS_BUSY);
        assert(memcmp(catalogue, before, sizeof(*before)) == 0);
        strcpy(value.model_id, "model.replaced");
        assert(UmiAiRuntimeCatalogueUpsertIfRevision(catalogue, &value, observed.revision) == UMI_STATUS_OK);
        assert(umi_ai_runtime_catalogue_find(catalogue, value.runtime_id, &latest) == UMI_STATUS_OK);
        assert(latest.revision > observed.revision); *before = *catalogue;
        assert(UmiAiRuntimeCatalogueSetHealthIfRevision(catalogue, value.runtime_id, &health, observed.revision) == UMI_STATUS_BUSY);
        assert(memcmp(catalogue, before, sizeof(*before)) == 0);
/* The conditional scenario also checks unrelated record changes and competing observations. The former implementation is retained for owner review. */
#if 0
        assert(UmiAiRuntimeCatalogueSetHealthIfRevision(catalogue, value.runtime_id, &health, latest.revision) == UMI_STATUS_OK);
#endif
        /* Updating another record changes the catalogue's global revision but
         * must not invalidate this runtime's captured revision. */
        UmiAiRuntimeDescriptor other = catalogue_fixture("runtime.unrelated");
        assert(umi_ai_runtime_catalogue_upsert(catalogue, &other) == UMI_STATUS_OK);
        assert(UmiAiRuntimeCatalogueSetHealthIfRevision(catalogue, value.runtime_id, &health, latest.revision) == UMI_STATUS_OK);
        /* Two observations captured from one revision cannot both publish. */
        assert(UmiAiRuntimeCatalogueSetHealthIfRevision(catalogue, value.runtime_id, &health, latest.revision) == UMI_STATUS_BUSY);
        assert(umi_ai_runtime_catalogue_select(catalogue, UMI_AI_RUNTIME_CHAT, 8192U, 1, &latest) == UMI_STATUS_OK);
        assert(latest.health.checked_at_ns == 12U && strcmp(latest.model_id, "model.replaced") == 0);
        strcpy(value.runtime_id, "missing");
        assert(UmiAiRuntimeCatalogueUpsertIfRevision(catalogue, &value, observed.revision) == UMI_STATUS_BUSY);
        assert(UmiAiRuntimeCatalogueSetHealthIfRevision(catalogue, "missing", &health, observed.revision) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(argv[1], "provider-replaced") == 0) {
        strcpy(value.provider_id, "provider.replaced");
        assert(umi_ai_runtime_catalogue_upsert(catalogue, &value) == UMI_STATUS_OK); *before = *catalogue;
        assert(UmiAiRuntimeCatalogueSetHealthIfRevision(catalogue, value.runtime_id, &health, observed.revision) == UMI_STATUS_BUSY);
        assert(umi_ai_runtime_catalogue_set_health(catalogue, value.runtime_id, &health) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(catalogue, before, sizeof(*before)) == 0);
    } else if (strcmp(argv[1], "health-input") == 0) {
        assert(UmiAiRuntimeCatalogueSetHealthIfRevision(catalogue, value.runtime_id, &health, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        memset(health.message, 'x', sizeof(health.message));
        assert(umi_ai_runtime_catalogue_set_health(catalogue, value.runtime_id, &health) == UMI_STATUS_INVALID_ARGUMENT);
        memset(&health, 0, sizeof(health)); memset(health.provider_id, 'x', sizeof(health.provider_id));
        assert(umi_ai_runtime_catalogue_set_health(catalogue, value.runtime_id, &health) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(catalogue, before, sizeof(*before)) == 0);
        memset(&health, 0, sizeof(health)); health.available = 1;
        assert(umi_ai_runtime_catalogue_set_health(catalogue, value.runtime_id, &health) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "saturation") == 0) {
        catalogue->revision = UINT64_MAX; *before = *catalogue;
        assert(UmiAiRuntimeCatalogueValidate(catalogue) == UMI_STATUS_OK);
        assert(umi_ai_runtime_catalogue_upsert(catalogue, &value) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(UmiAiRuntimeCatalogueUpsertIfRevision(catalogue, &value, observed.revision) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(umi_ai_runtime_catalogue_set_health(catalogue, value.runtime_id, &health) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(UmiAiRuntimeCatalogueSetHealthIfRevision(catalogue, value.runtime_id, &health, observed.revision) == UMI_STATUS_CAPACITY_EXCEEDED);
        strcpy(value.runtime_id, "new");
        assert(umi_ai_runtime_catalogue_upsert(catalogue, &value) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(catalogue, before, sizeof(*before)) == 0);
    } else if (strcmp(argv[1], "capacity") == 0) {
        for (size_t i = 1U; i < UMI_AI_RUNTIME_CATALOGUE_CAPACITY; ++i) {
            (void)snprintf(value.runtime_id, sizeof(value.runtime_id), "runtime.%zu", i);
            assert(umi_ai_runtime_catalogue_upsert(catalogue, &value) == UMI_STATUS_OK);
        }
        *before = *catalogue; strcpy(value.runtime_id, "overflow");
        assert(umi_ai_runtime_catalogue_upsert(catalogue, &value) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(catalogue, before, sizeof(*before)) == 0);
        strcpy(value.runtime_id, "runtime.local");
        assert(UmiAiRuntimeCatalogueUpsertIfRevision(catalogue, &value, observed.revision) == UMI_STATUS_OK);
        assert(umi_ai_runtime_catalogue_count(catalogue) == UMI_AI_RUNTIME_CATALOGUE_CAPACITY);
        assert(umi_ai_runtime_catalogue_provider_count(catalogue) == 1U);
    } else {
        if (strcmp(argv[1], "count") == 0) catalogue->count = SIZE_MAX;
        else if (strcmp(argv[1], "zero-revision") == 0) catalogue->revision = 0U;
        else if (strcmp(argv[1], "record-revision") == 0) catalogue->items[0].revision = catalogue->revision + 1U;
        else if (strcmp(argv[1], "zero-record-revision") == 0) catalogue->items[0].revision = 0U;
        else if (strcmp(argv[1], "stored-text") == 0) memset(catalogue->items[0].runtime_id, 'x', sizeof(value.runtime_id));
        else if (strcmp(argv[1], "duplicate") == 0) { catalogue->items[1] = catalogue->items[0]; catalogue->count = 2U; }
        else { assert(!"unknown revision case"); }
        invalid_owner(catalogue);
    }
    free(before); umi_ai_runtime_catalogue_destroy(catalogue); return 0;
}
