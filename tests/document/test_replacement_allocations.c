/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_replacement_allocations.c
 * PURPOSE: Inject allocation failures before reviewed edits can publish partial state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"

/* Linux linker wrapping changes exactly one allocation in a chosen call.
 * It is disabled during fixture construction, observation and destruction. */
static long failAfter = -1;
void *__real_malloc(size_t bytes);
void *__real_calloc(size_t count, size_t bytes);
void *__real_realloc(void *memory, size_t bytes);

/* Count allocations without allocating or changing unrelated fixture state. */
static int FailThisAllocation(void)
{
    if (failAfter < 0) return 0;
    return failAfter-- == 0;
}

/* These wrappers preserve normal allocator behavior outside the fault scope. */
void *__wrap_malloc(size_t bytes) { return FailThisAllocation() ? NULL : __real_malloc(bytes); }
/* calloc is included so failure to allocate either plan cannot leak a capture. */
void *__wrap_calloc(size_t count, size_t bytes) { return FailThisAllocation() ? NULL : __real_calloc(count, bytes); }
/* realloc failure must retain the original owned buffer. */
void *__wrap_realloc(void *memory, size_t bytes) { return FailThisAllocation() ? NULL : __real_realloc(memory, bytes); }

/* Snapshot the actual store and view around one injected operation. */
static int Attempt(ReplacementFixture *f, int apply, long allocation, int *outSuccess)
{
    UmiDocumentWorkingCopySnapshot before, after;
    UmiDocumentSnapshot storedBefore, storedAfter;
    CHECK(Draft(f, "note note pending") == UMI_STATUS_OK);
    char *replacement = malloc(65537U); CHECK(replacement != NULL);
    memset(replacement, 'r', 65536U); replacement[65536U] = '\0';
    if (apply) {
        UmiStatus prepared = UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "note", replacement, &f->plan);
        if (prepared != UMI_STATUS_OK) { free(replacement); CHECK(prepared == UMI_STATUS_OK); }
    }
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &before) == UMI_STATUS_OK);
    CHECK(umi_document_store_snapshot(f->store, f->id, &storedBefore) == UMI_STATUS_OK);
    size_t count = 888U;
    failAfter = allocation;
    UmiStatus status = apply ? UmiDocumentCoordinatorApplyReplacement(f->documents, f->plan, &count)
        : UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "note", replacement, &f->plan);
    failAfter = -1; free(replacement);
    CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_OUT_OF_MEMORY);
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &after) == UMI_STATUS_OK);
    CHECK(umi_document_store_snapshot(f->store, f->id, &storedAfter) == UMI_STATUS_OK);
    if (status == UMI_STATUS_OUT_OF_MEMORY || !apply) {
        CHECK(ExpectText(f, "note note pending") == 0);
        CHECK(after.undo_count == before.undo_count && after.redo_count == before.redo_count && after.dirty == before.dirty);
        CHECK(storedAfter.revision == storedBefore.revision && storedAfter.saved_revision == storedBefore.saved_revision && storedAfter.length == storedBefore.length);
    }
    if (status == UMI_STATUS_OUT_OF_MEMORY) {
        CHECK(count == 888U);
        if (!apply) CHECK(f->plan == NULL);
        else CHECK(UmiDocumentCoordinatorCheckReplacement(f->documents, f->plan) == UMI_STATUS_OK);
    } else if (apply) {
        CHECK(count == 2U);
        CHECK(UmiDocumentCoordinatorUndo(f->documents, f->id) == UMI_STATUS_OK);
        CHECK(ExpectText(f, "note note pending") == 0);
    }
    *outSuccess = status == UMI_STATUS_OK;
    return 0;
}

/* Walk every allocation site until an operation completes without a fault. */
int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "prepare") != 0 && strcmp(argv[1], "apply") != 0)) return 2;
    int apply = strcmp(argv[1], "apply") == 0;
    for (long i = 0; i < 128; ++i) {
        ReplacementFixture f = {0}; int succeeded = 0;
        int result = Start(&f, "note note stored");
        if (result == 0) result = Attempt(&f, apply, i, &succeeded);
        failAfter = -1; Stop(&f);
        if (result != 0) return result;
        if (succeeded) { CHECK(i >= 2); return 0; }
    }
    CHECK(!"No successful operation after the bounded allocation walk");
    return 1;
}
