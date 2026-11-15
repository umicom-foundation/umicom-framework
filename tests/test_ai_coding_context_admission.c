/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_context_admission.c
 *
 * PURPOSE:
 *   Verify bounded coding context ownership, complete planning and preserved optional-file behavior without reading repository files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/ai/coding_context.h"
/* Direct state access is reserved for corrupt-owner and counter-boundary
 * fixtures. Reads, mutations and plans exercise the real implementation. */
#include "../src/ai/coding_context.c"

static UmiAiCodingContextFile coding_fixture(const char *path)
{
    UmiAiCodingContextFile value = {0};
    (void)snprintf(value.path, sizeof value.path, "%s", path);
    strcpy(value.language_id, "c23"); strcpy(value.summary, "Coding source");
    value.classification = UMI_AI_DATA_PUBLIC; value.estimated_tokens = 32U;
    value.priority = 1U; value.enabled = 1; value.revision = 900U;
    return value;
}
#define REQUIRE(condition) do { if (!(condition)) { \
    fprintf(stderr, "Coding context check failed at line %d\n", __LINE__); \
    result = 1; goto cleanup; } } while (0)
#define CASE(name) (strcmp(mode, name) == 0)
static int check_owner(const char *mode)
{
    UmiAiCodingContextIndex *owner = NULL, *prior_owner = malloc(sizeof *prior_owner);
    UmiAiCodingContextFile item = coding_fixture("src/new.c"), initial = coding_fixture("src/base.c"),
        prior_item, output, prior_output;
    UmiSnapshotCapture capture = {9U, 77U}, prior_capture = capture;
    UmiStatus status, expected = UMI_STATUS_INVALID_ARGUMENT;
    int result = 0, success;
    REQUIRE(prior_owner != NULL);
    status = umi_ai_coding_context_index_create(&owner); REQUIRE(status == UMI_STATUS_OK);
    status = umi_ai_coding_context_upsert(owner, &initial); REQUIRE(status == UMI_STATUS_OK);
    if (CASE("path-bound")) memset(item.path, 'x', sizeof item.path);
    if (CASE("language-bound")) memset(item.language_id, 'x', sizeof item.language_id);
    if (CASE("summary-bound")) memset(item.summary, 'x', sizeof item.summary);
    if (CASE("empty-path")) item.path[0] = '\0';
    if (CASE("optional-empty")) item.language_id[0] = item.summary[0] = '\0';
    if (CASE("zero-tokens")) item.estimated_tokens = 0U;
    if (CASE("zero-priority")) item.priority = 0U;
    if (CASE("class-low")) item.classification = (UmiAiDataClassification)0;
    if (CASE("class-high")) item.classification = (UmiAiDataClassification)99;
    if (CASE("unsafe-paths")) {
        static const char *const paths[] = {"/etc/file", "C:\\file", "../file", "src/../file", "src//file", "src/./file", "src/file:stream"};
        memcpy(prior_owner, owner, sizeof *owner);
        for (size_t i = 0U; i < sizeof paths / sizeof paths[0]; ++i) {
            strcpy(item.path, paths[i]);
            REQUIRE(UmiAiCodingContextFileValidate(&item) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(umi_ai_coding_context_upsert(owner, &item) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(prior_owner, owner, sizeof *owner) == 0);
        }
        goto cleanup;
    }
    if (CASE("capacity") || CASE("replace-full")) {
        for (size_t i = 1U; i < UMI_AI_CODING_CONTEXT_FILE_CAPACITY; ++i) {
            UmiAiCodingContextFile extra = coding_fixture("src/fill.c");
            (void)snprintf(extra.path, sizeof extra.path, "src/fill_%zu.c", i);
            REQUIRE(umi_ai_coding_context_upsert(owner, &extra) == UMI_STATUS_OK);
        }
        if (CASE("replace-full")) strcpy(item.path, initial.path);
    }
    if (strncmp(mode, "revision-max-", 13U) == 0 || CASE("legacy-clear-max")) owner->revision = UINT64_MAX;
    if (CASE("revision-last")) owner->revision = UINT64_MAX - 1U;
    if (CASE("empty-clear")) REQUIRE(UmiAiCodingContextClearChecked(owner) == UMI_STATUS_OK);
    if (CASE("corrupt-count")) owner->count = UMI_AI_CODING_CONTEXT_FILE_CAPACITY + 1U;
    if (CASE("corrupt-revision")) owner->revision = 0U;
    if (CASE("corrupt-record-zero")) owner->files[0].revision = 0U;
    if (CASE("corrupt-record-ahead")) owner->files[0].revision = owner->revision + 1U;
    if (CASE("corrupt-text")) memset(owner->files[0].summary, 'x', sizeof owner->files[0].summary);
    if (CASE("corrupt-duplicate")) { owner->files[1] = owner->files[0]; owner->count = 2U; }
    memcpy(prior_owner, owner, sizeof *owner); memcpy(&prior_item, &item, sizeof item);
    memset(&output, 0xA5, sizeof output); memcpy(&prior_output, &output, sizeof output);
    success = CASE("valid") || CASE("optional-empty") || CASE("replace-full") || CASE("revision-last") || CASE("empty-clear") || CASE("remove-valid");
    if (CASE("legacy-clear-max")) {
        umi_ai_coding_context_clear(owner);
        REQUIRE(memcmp(prior_owner, owner, sizeof *owner) == 0);
        goto cleanup;
    } else if (CASE("revision-max-remove") || CASE("remove-valid"))
        status = umi_ai_coding_context_remove(owner, initial.path);
    else if (CASE("revision-max-clear") || CASE("empty-clear"))
        status = UmiAiCodingContextClearChecked(owner);
    else if (CASE("capture-null")) status = UmiAiCodingContextCapture(owner, NULL);
    else if (CASE("remove-query")) {
        char path[UMI_AI_TEXT_CAPACITY]; memset(path, 'x', sizeof path);
        status = umi_ai_coding_context_remove(owner, path);
    } else if (CASE("missing-remove")) status = umi_ai_coding_context_remove(owner, "src/missing.c");
    else if (CASE("at-missing")) status = umi_ai_coding_context_at(owner, UMI_AI_CODING_CONTEXT_FILE_CAPACITY, &output);
    else status = umi_ai_coding_context_upsert(owner, CASE("null-source") ? NULL : &item);
    REQUIRE(memcmp(&prior_item, &item, sizeof item) == 0);
    if (success) {
        REQUIRE(status == UMI_STATUS_OK);
        REQUIRE(UmiAiCodingContextCapture(owner, &capture) == UMI_STATUS_OK);
        REQUIRE(capture.revision == prior_owner->revision + 1U);
        REQUIRE(capture.count == (CASE("empty-clear") || CASE("remove-valid") ? 0U :
            prior_owner->count + (CASE("replace-full") ? 0U : 1U)));
        if (CASE("remove-valid")) {
            UmiAiCodingContextFile zero = {0};
            /* Compare zeroed storage, including padding, deliberately. */
            memset(&zero, 0, sizeof zero);
            REQUIRE(memcmp(&zero, &owner->files[0], sizeof zero) == 0);
        } else if (!CASE("empty-clear")) {
            size_t slot = CASE("replace-full") ? 0U : owner->count - 1U;
            REQUIRE(umi_ai_coding_context_at(owner, slot, &output) == UMI_STATUS_OK);
            REQUIRE(output.revision == capture.revision && strcmp(output.path, item.path) == 0);
        }
        if (CASE("revision-last")) {
            REQUIRE(capture.revision == UINT64_MAX);
            memcpy(prior_owner, owner, sizeof *owner);
            REQUIRE(UmiAiCodingContextClearChecked(owner) == UMI_STATUS_CAPACITY_EXCEEDED);
            REQUIRE(memcmp(prior_owner, owner, sizeof *owner) == 0);
        }
    } else {
        if (CASE("capacity") || strncmp(mode, "revision-max-", 13U) == 0) expected = UMI_STATUS_CAPACITY_EXCEEDED;
        if (strncmp(mode, "corrupt-", 8U) == 0) expected = UMI_STATUS_INVALID_STATE;
        if (CASE("missing-remove") || CASE("at-missing")) expected = UMI_STATUS_NOT_FOUND;
        REQUIRE(status == expected && memcmp(prior_owner, owner, sizeof *owner) == 0);
        REQUIRE(memcmp(&prior_output, &output, sizeof output) == 0);
        if (expected == UMI_STATUS_INVALID_STATE) {
            REQUIRE(UmiAiCodingContextCapture(owner, &capture) == UMI_STATUS_INVALID_STATE);
            REQUIRE(memcmp(&capture, &prior_capture, sizeof capture) == 0);
            REQUIRE(umi_ai_coding_context_at(owner, 0U, &output) == UMI_STATUS_INVALID_STATE);
            REQUIRE(memcmp(&prior_output, &output, sizeof output) == 0);
            REQUIRE(umi_ai_coding_context_count(owner) == 0U);
            REQUIRE(UmiAiCodingContextClearChecked(owner) == UMI_STATUS_INVALID_STATE);
            REQUIRE(memcmp(prior_owner, owner, sizeof *owner) == 0);
        }
    }
cleanup:
    umi_ai_coding_context_index_destroy(owner); free(prior_owner); return result;
}

static int check_plan(const char *mode)
{
    UmiAiCodingContextIndex *owner = NULL;
    UmiAiCodingContextPlan *plan = malloc(sizeof *plan), *prior_plan = malloc(sizeof *prior_plan);
    UmiAiCodingContextFile a = coding_fixture("src/a.c"), b = coding_fixture("src/b.c"), c = coding_fixture("src/c.c");
    UmiAiPrivacyPolicy privacy = umi_ai_privacy_policy_default();
    UmiAiProviderKind provider = UMI_AI_PROVIDER_LOCAL;
    UmiSnapshotCapture before_capture, after_capture;
    UmiStatus status, expected = UMI_STATUS_OK;
    uint32_t budget = 180U;
    size_t maximum = 8U;
    int result = 0;
    REQUIRE(plan != NULL && prior_plan != NULL);
    REQUIRE(umi_ai_coding_context_index_create(&owner) == UMI_STATUS_OK);
    a.estimated_tokens = 60U; a.priority = 10U;
    b.estimated_tokens = 50U; b.priority = 20U;
    c.estimated_tokens = 30U; c.priority = 20U;
    if (CASE("active-negative")) a.active = -7;
    if (CASE("enabled-negative")) b.enabled = -1;
    if (CASE("zero-budget")) budget = 0U;
    if (CASE("exact-budget")) budget = 140U;
    if (CASE("optional-private")) b.classification = UMI_AI_DATA_RESTRICTED;
    if (CASE("one-slot")) maximum = 1U;
    if (CASE("all-disabled")) a.enabled = b.enabled = c.enabled = 0;
    if (CASE("maximum-tokens")) { budget = UINT32_MAX; a.estimated_tokens = UINT32_MAX; b.enabled = c.enabled = 0; }
    if (CASE("provider-low") || CASE("empty-provider")) provider = (UmiAiProviderKind)0;
    if (CASE("provider-high")) provider = (UmiAiProviderKind)99;
    if (CASE("invalid-policy") || CASE("empty-policy")) privacy.maximum_remote_classification = (UmiAiDataClassification)99;
    if (CASE("zero-files")) maximum = 0U;
    if (CASE("too-many-files")) maximum = UMI_AI_CODING_CONTEXT_PLAN_MAX + 1U;
    if (CASE("provider-low") || CASE("provider-high") || CASE("empty-provider") || CASE("invalid-policy") ||
        CASE("empty-policy") || CASE("null-output") || CASE("null-policy") || CASE("zero-files") || CASE("too-many-files"))
        expected = UMI_STATUS_INVALID_ARGUMENT;
    REQUIRE(umi_ai_coding_context_upsert(owner, &c) == UMI_STATUS_OK);
    REQUIRE(umi_ai_coding_context_upsert(owner, &a) == UMI_STATUS_OK);
    REQUIRE(umi_ai_coding_context_upsert(owner, &b) == UMI_STATUS_OK);
    if (CASE("empty-provider") || CASE("empty-policy")) REQUIRE(UmiAiCodingContextClearChecked(owner) == UMI_STATUS_OK);
    REQUIRE(UmiAiCodingContextCapture(owner, &before_capture) == UMI_STATUS_OK);
    memset(plan, 0xA5, sizeof *plan); memcpy(prior_plan, plan, sizeof *plan);
    status = umi_ai_coding_context_plan(owner, budget, maximum, provider,
        CASE("null-policy") ? NULL : &privacy, 1, CASE("null-output") ? NULL : plan);
    REQUIRE(status == expected);
    REQUIRE(UmiAiCodingContextCapture(owner, &after_capture) == UMI_STATUS_OK);
    REQUIRE(before_capture.revision == after_capture.revision && before_capture.count == after_capture.count);
    if (status != UMI_STATUS_OK) REQUIRE(memcmp(prior_plan, plan, sizeof *plan) == 0);
    else {
        REQUIRE(plan->revision == before_capture.revision && plan->token_budget == budget);
        REQUIRE(plan->selected_tokens <= budget && plan->remaining_tokens == budget - plan->selected_tokens);
        if (CASE("active-negative")) REQUIRE(plan->file_count == 3U && strcmp(plan->files[0].path, a.path) == 0);
        else if (CASE("zero-budget")) REQUIRE(plan->file_count == 0U && plan->dropped_count == 3U && plan->truncated);
        else if (CASE("optional-private")) REQUIRE(plan->file_count == 2U && plan->selected_tokens == 90U && plan->dropped_count == 1U);
        else if (CASE("one-slot")) REQUIRE(plan->file_count == 1U && plan->selected_tokens == 50U && plan->dropped_count == 2U);
        else if (CASE("all-disabled")) REQUIRE(plan->file_count == 0U && plan->dropped_count == 0U && !plan->truncated);
        else if (CASE("maximum-tokens")) REQUIRE(plan->file_count == 1U && plan->selected_tokens == UINT32_MAX && plan->remaining_tokens == 0U);
        else {
            REQUIRE(plan->file_count == 3U && plan->selected_tokens == 140U && plan->dropped_count == 0U);
            REQUIRE(strcmp(plan->files[0].path, b.path) == 0 && strcmp(plan->files[1].path, c.path) == 0 && strcmp(plan->files[2].path, a.path) == 0);
        }
        if (CASE("source-copy")) {
            strcpy(b.summary, "Changed later");
            REQUIRE(umi_ai_coding_context_upsert(owner, &b) == UMI_STATUS_OK);
            REQUIRE(strcmp(plan->files[0].summary, "Coding source") == 0);
        }
    }
cleanup:
    umi_ai_coding_context_index_destroy(owner); free(plan); free(prior_plan); return result;
}
#undef CASE
#undef REQUIRE
int main(int argc, char **argv)
{
    static const char *const owner_modes[] = {"valid",
        "path-bound",
        "language-bound",
        "summary-bound",
        "empty-path",
        "optional-empty",
        "unsafe-paths",
        "zero-tokens",
        "zero-priority",
        "class-low",
        "class-high",
        "null-source",
        "capacity",
        "replace-full",
        "revision-max-upsert",
        "revision-max-remove",
        "revision-max-clear",
        "revision-last",
        "empty-clear",
        "legacy-clear-max",
        "remove-valid",
        "remove-query",
        "missing-remove",
        "at-missing",
        "capture-null",
        "corrupt-count",
        "corrupt-revision",
        "corrupt-record-zero",
        "corrupt-record-ahead",
        "corrupt-duplicate",
        "corrupt-text"};
    static const char *const plan_modes[] = {"ordering",
        "active-negative",
        "enabled-negative",
        "zero-budget",
        "exact-budget",
        "optional-private",
        "one-slot",
        "all-disabled",
        "maximum-tokens",
        "provider-low",
        "provider-high",
        "invalid-policy",
        "empty-provider",
        "empty-policy",
        "null-output",
        "null-policy",
        "zero-files",
        "too-many-files",
        "source-copy"};
    if (argc != 3) return 2;
    if (strcmp(argv[1], "owner") == 0) {
        for (size_t i = 0U; i < sizeof owner_modes / sizeof owner_modes[0]; ++i)
            if (strcmp(argv[2], owner_modes[i]) == 0) return check_owner(argv[2]);
    } else if (strcmp(argv[1], "plan") == 0) {
        for (size_t i = 0U; i < sizeof plan_modes / sizeof plan_modes[0]; ++i)
            if (strcmp(argv[2], plan_modes[i]) == 0) return check_plan(argv[2]);
    }
    return 2;
}
