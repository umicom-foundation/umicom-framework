/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_patch_transactions.c
 *
 * PURPOSE:
 *   Verify patch transaction outcomes, callback ownership and restoration evidence with an in-memory adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/coding_patch.h"
#include "support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct TransactionMemory {
    UmiAiCodingPatch *patch;
    UmiAiCodingPatchPolicy policy;
    UmiAiCodingFileAdapter adapter;
    char text[3][32];
    int present[3];
    size_t reads, mutations;
    size_t fail_mutation, fail_compensation, fail_read;
    const char *guard;
    int guard_done;
    int guard_phase;
    UmiAiCodingPatch *outer;
} TransactionMemory;
static const char *const paths[3] = {"one.c", "two.c", "three.c"};

static size_t PathIndex(const char *path)
{
    for (size_t i = 0U; i < 3U; ++i) if (strcmp(path, paths[i]) == 0) return i;
    assert(0); return 0U;
}
static void RunGuard(TransactionMemory *memory, int phase);
static UmiStatus ReadFile(void *data, const char *path, char *out, size_t capacity, size_t *length)
{
    TransactionMemory *memory = data;
    ++memory->reads;
    RunGuard(memory, 0);
    if (memory->fail_read == memory->reads) return UMI_STATUS_IO_ERROR;
    size_t index = PathIndex(path);
    if (!memory->present[index]) return UMI_STATUS_NOT_FOUND;
    size_t size = strlen(memory->text[index]);
    assert(size < capacity); memcpy(out, memory->text[index], size + 1U); *length = size;
    return UMI_STATUS_OK;
}
static UmiStatus WriteFile(void *data, const char *path, const char *text, size_t length)
{
    TransactionMemory *memory = data;
    ++memory->mutations;
    RunGuard(memory, memory->mutations > memory->fail_mutation && memory->fail_mutation != 0U ? 2 : 1);
    if (memory->fail_compensation == memory->mutations) return UMI_STATUS_PERMISSION_DENIED;
    size_t index = PathIndex(path);
    assert(length < sizeof(memory->text[index]));
    memcpy(memory->text[index], text, length); memory->text[index][length] = 0; memory->present[index] = 1;
    /* Simulate a primary error reported after a partial or complete write. */
    return memory->fail_mutation == memory->mutations ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static UmiStatus RemoveFile(void *data, const char *path)
{
    TransactionMemory *memory = data;
    ++memory->mutations;
    RunGuard(memory, memory->mutations > memory->fail_mutation && memory->fail_mutation != 0U ? 2 : 1);
    if (memory->fail_compensation == memory->mutations) return UMI_STATUS_PERMISSION_DENIED;
    size_t index = PathIndex(path); memory->present[index] = 0; memory->text[index][0] = 0;
    return memory->fail_mutation == memory->mutations ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static void MakeMemory(TransactionMemory *memory, UmiAiCodingPatchOperation operation)
{
    memset(memory, 0, sizeof(*memory));
    memory->patch = malloc(sizeof(*memory->patch)); assert(memory->patch != NULL);
    memory->policy = umi_ai_coding_patch_policy_default(); memory->policy.allow_delete = 1;
    memory->adapter = (UmiAiCodingFileAdapter){sizeof(memory->adapter), UMI_AI_CODING_ABI_VERSION, ReadFile, WriteFile, RemoveFile, memory};
    assert(umi_ai_coding_patch_init(memory->patch, "patch", "request", "Three changes", "Exercise owned callbacks") == UMI_STATUS_OK);
    for (size_t i = 0U; i < 3U; ++i) {
        const char *before = operation == UMI_AI_CODING_PATCH_CREATE ? "" : "before\n";
        const char *after = operation == UMI_AI_CODING_PATCH_DELETE ? "" : "after\n";
        memory->present[i] = operation != UMI_AI_CODING_PATCH_CREATE;
        strcpy(memory->text[i], before);
        assert(umi_ai_coding_patch_add_file(memory->patch, paths[i], operation, before, after) == UMI_STATUS_OK);
    }
    assert(umi_ai_coding_patch_approve(memory->patch, "reviewer") == UMI_STATUS_OK);
}
static UmiStatus Execute(TransactionMemory *memory, int reverting, UmiAiCodingPatchTransactionReport *report)
{
    return reverting ? UmiAiCodingPatchRevertWithReport(memory->patch, &memory->adapter, report) :
        UmiAiCodingPatchApplyWithReport(memory->patch, &memory->policy, &memory->adapter, report);
}
static void RunGuard(TransactionMemory *memory, int phase)
{
    if (memory->guard == NULL || memory->guard_done || phase != memory->guard_phase) return;
    memory->guard_done = 1;
    UmiAiCodingPatch *patch = memory->outer != NULL ? memory->outer : memory->patch;
    assert(UmiAiCodingPatchCheckIdle(patch) == UMI_STATUS_BUSY);
    assert(UmiAiCodingPatchCheckIntegrity(patch) == UMI_STATUS_OK);
    UmiAiCodingPatch *before = malloc(sizeof(*before)); assert(before != NULL); memcpy(before, patch, sizeof(*before));
    UmiAiCodingPatchTransactionReport report, sentinel;
    memset(&report, 0x5a, sizeof(report)); memcpy(&sentinel, &report, sizeof(report));
    const char *action = memory->guard;
    UmiStatus status = UMI_STATUS_OK;
    if (strcmp(action, "init") == 0) status = umi_ai_coding_patch_init(patch, "replacement", "request", "Changed", "Callback");
    else if (strcmp(action, "add") == 0) status = umi_ai_coding_patch_add_file(patch, "four.c", UMI_AI_CODING_PATCH_CREATE, "", "new");
    else if (strcmp(action, "approve") == 0) status = umi_ai_coding_patch_approve(patch, "other");
    else if (strcmp(action, "reject") == 0) status = umi_ai_coding_patch_reject(patch);
    else if (strcmp(action, "apply") == 0) status = umi_ai_coding_patch_apply(patch, &memory->policy, &memory->adapter);
    else if (strcmp(action, "revert") == 0) status = umi_ai_coding_patch_revert(patch, &memory->adapter);
    else if (strcmp(action, "apply-report") == 0) status = UmiAiCodingPatchApplyWithReport(patch, &memory->policy, &memory->adapter, &report);
    else if (strcmp(action, "revert-report") == 0) status = UmiAiCodingPatchRevertWithReport(patch, &memory->adapter, &report);
    else if (strcmp(action, "nested") == 0) {
        TransactionMemory nested; MakeMemory(&nested, UMI_AI_CODING_PATCH_MODIFY);
        nested.outer = patch; nested.guard = "reject";
        assert(Execute(&nested, 0, &report) == UMI_STATUS_OK && nested.guard_done);
        assert(UmiAiCodingPatchCheckIdle(nested.patch) == UMI_STATUS_OK);
        free(nested.patch);
    } else { assert(strcmp(action, "adapter") == 0); memory->adapter.read = NULL; memory->adapter.write = NULL; memory->adapter.remove = NULL; }
    if (strcmp(action, "nested") != 0 && strcmp(action, "adapter") != 0) {
        assert(status == UMI_STATUS_BUSY); assert(memcmp(&report, &sentinel, sizeof(report)) == 0);
    }
    assert(memcmp(before, patch, sizeof(*patch)) == 0); free(before);
}
static void SetReverting(TransactionMemory *memory, int reverting)
{
    if (reverting) assert(umi_ai_coding_patch_apply(memory->patch, &memory->policy, &memory->adapter) == UMI_STATUS_OK);
    memory->reads = memory->mutations = 0U;
}
static void CheckTransaction(const char *mode, const char *variant)
{
    int reverting = strcmp(mode, "revert") == 0;
    assert(reverting || strcmp(mode, "apply") == 0);
    UmiAiCodingPatchOperation operation = strncmp(variant, "create-", 7U) == 0 ? UMI_AI_CODING_PATCH_CREATE :
        strncmp(variant, "delete-", 7U) == 0 ? UMI_AI_CODING_PATCH_DELETE : UMI_AI_CODING_PATCH_MODIFY;
    const char *scenario = strchr(variant, '-'); assert(scenario != NULL); ++scenario;
    TransactionMemory memory; MakeMemory(&memory, operation); SetReverting(&memory, reverting);
    UmiAiCodingPatchState prior = memory.patch->state;
    uint64_t revision = memory.patch->revision;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(scenario, "success") != 0) {
        memory.fail_mutation = 2U; expected = UMI_STATUS_IO_ERROR;
        if (strcmp(scenario, "compensation") == 0) memory.fail_compensation = 3U;
        else if (strcmp(scenario, "readback") == 0) memory.fail_read = 4U;
        else if (strcmp(scenario, "conflict") == 0) {
            memory.fail_mutation = 0U; memory.present[0] = !memory.present[0]; expected = UMI_STATUS_INVALID_STATE;
            /* Absent modified/deleted files report NOT_FOUND through the adapter. */
            if (!memory.present[0]) expected = UMI_STATUS_NOT_FOUND;
        } else assert(strcmp(scenario, "restored") == 0);
    }
    UmiAiCodingPatchTransactionReport report;
    assert(Execute(&memory, reverting, &report) == expected);
    assert(report.admitted == 1 && report.primary_status == expected && report.revision_before == revision);
    assert(UmiAiCodingPatchCheckIdle(memory.patch) == UMI_STATUS_OK);
    if (strcmp(scenario, "success") == 0) {
        assert(report.verified_files == 3U && report.mutation_attempts == 3U && report.mutations_completed == 3U);
        assert(report.failed_file_index == SIZE_MAX && report.compensation_attempts == 0U && report.restoration_checks == 0U);
        assert(memory.patch->state == (reverting ? UMI_AI_CODING_PATCH_REVERTED : UMI_AI_CODING_PATCH_APPLIED));
        assert(report.revision_after == revision + 1U);
    } else if (strcmp(scenario, "conflict") == 0) {
        assert(report.verified_files == 0U && report.failed_file_index == 0U && report.mutation_attempts == 0U);
        assert(report.compensation_attempts == 0U && report.restoration_checks == 0U && report.revision_after == revision);
        assert(memory.patch->state == prior);
    } else {
        assert(report.verified_files == 3U && report.failed_file_index == 1U);
        assert(report.mutation_attempts == 2U && report.mutations_completed == 1U);
        assert(report.compensation_attempts == 2U && report.restoration_checks == 2U);
        assert(report.revision_after == revision + 1U && memory.patch->state == prior);
        if (strcmp(scenario, "compensation") == 0) {
            assert(report.compensation_failures == 1U && report.restoration_failures == 1U);
            assert(report.compensation_status == UMI_STATUS_PERMISSION_DENIED);
        } else if (strcmp(scenario, "readback") == 0) {
            assert(report.compensation_failures == 0U && report.restoration_failures == 1U && report.compensation_status == UMI_STATUS_IO_ERROR);
        } else assert(report.compensation_failures == 0U && report.restoration_failures == 0U && report.compensation_status == UMI_STATUS_OK);
    }
    assert(memory.patch->revision == report.revision_after); free(memory.patch);
}
static void CheckGuard(const char *mode, const char *action)
{
    int reverting = strcmp(mode, "revert") == 0;
    TransactionMemory memory; MakeMemory(&memory, UMI_AI_CODING_PATCH_MODIFY); SetReverting(&memory, reverting);
    memory.guard = action;
    if (strcmp(action, "write-add") == 0) { memory.guard = "add"; memory.guard_phase = 1; }
    if (strcmp(action, "compensation-init") == 0) { memory.guard = "init"; memory.guard_phase = 2; memory.fail_mutation = 2U; }
    UmiAiCodingPatchTransactionReport report;
    assert(Execute(&memory, reverting, &report) == (memory.fail_mutation != 0U ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK));
    assert(memory.guard_done && UmiAiCodingPatchCheckIdle(memory.patch) == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_init(memory.patch, "new", "new", "New", "Guard released") == UMI_STATUS_OK);
    free(memory.patch);
}
static void CheckAdmission(const char *mode, const char *name)
{
    int reverting = strcmp(mode, "revert") == 0;
    TransactionMemory memory; MakeMemory(&memory, UMI_AI_CODING_PATCH_MODIFY); SetReverting(&memory, reverting);
    UmiAiCodingPatchTransactionReport report, sentinel;
    memset(&report, 0x5a, sizeof(report)); memcpy(&sentinel, &report, sizeof(report));
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(name, "exhausted") == 0) { memory.patch->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "adapter") == 0) memory.adapter.structure_size = 0U;
    else assert(strcmp(name, "null-report") == 0);
    UmiAiCodingPatch *before = malloc(sizeof(*before)); assert(before != NULL); memcpy(before, memory.patch, sizeof(*before));
    assert(Execute(&memory, reverting, strcmp(name, "null-report") == 0 ? NULL : &report) == expected);
    assert(memcmp(&report, &sentinel, sizeof(report)) == 0 && memcmp(before, memory.patch, sizeof(*before)) == 0);
    assert(memory.reads == 0U && memory.mutations == 0U && UmiAiCodingPatchCheckIdle(memory.patch) == UMI_STATUS_OK);
    free(before); free(memory.patch);
}
int main(int argc, char **argv)
{
    assert(argc == 4);
    assert(umi_ai_coding_patch_approve(NULL, "reviewer") == UMI_STATUS_INVALID_STATE);
    assert(umi_ai_coding_patch_reject(NULL) == UMI_STATUS_INVALID_STATE);
    assert(UmiAiCodingPatchCheckIdle(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    if (strcmp(argv[1], "transaction") == 0) CheckTransaction(argv[2], argv[3]);
    else if (strcmp(argv[1], "guard") == 0) CheckGuard(argv[2], argv[3]);
    else { assert(strcmp(argv[1], "admission") == 0); CheckAdmission(argv[2], argv[3]); }
    return 0;
}
