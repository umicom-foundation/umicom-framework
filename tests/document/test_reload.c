/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_reload.c
 * PURPOSE: Exercise saved-file reload, explicit consent and rejection safety.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <process.h>
#define PROCESS_ID _getpid
#else
#include <unistd.h>
#define PROCESS_ID getpid
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define ORIGINAL "Umicom Notes: original\n"
#define INCOMING "Umicom Notes: saved elsewhere\n"
#define DRAFT "Umicom Notes: unsaved draft\n"

typedef struct Fixture {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
    UmiDocumentReloadPlan *plan;
    UmiDocumentProvider provider;
    UmiDocumentId id;
    char view[UMI_UI_ID_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char *providerText;
    size_t providerLength;
    int useProvider;
    UmiStatus readStatus;
    UmiStatus statStatus;
    unsigned reads, writes;
    int mutateOnRead;
    UmiStatus hookStatus;
    int createdRoot;
} Fixture;

#ifdef UMI_TEST_ALLOCATION_FAILURE
static long allocationsBeforeFailure = -1;
static int ShouldFail(void) {
    if (allocationsBeforeFailure < 0) return 0;
    if (allocationsBeforeFailure-- == 0) { allocationsBeforeFailure = -1; return 1; }
    return 0;
}
void *__real_malloc(size_t size);
void *__real_calloc(size_t count, size_t size);
void *__real_realloc(void *p, size_t size);
void *__wrap_malloc(size_t size) { return ShouldFail() ? NULL : __real_malloc(size); }
void *__wrap_calloc(size_t n, size_t size) { return ShouldFail() ? NULL : __real_calloc(n, size); }
void *__wrap_realloc(void *p, size_t size) { return ShouldFail() ? NULL : __real_realloc(p, size); }
#endif
#ifdef UMI_TEST_STAT_FAILURE
static int statError;
int __real_stat(const char *path, struct stat *info);
int __wrap_stat(const char *path, struct stat *info) {
    if (statError != 0) { errno = statError; return -1; }
    return __real_stat(path, info);
}
#endif

static UmiStatus EditStatus(Fixture *f, const char *text)
{
    UmiUiDocumentViewSnapshot view;
    UmiStatus status = umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->view, &view);
    if (status != UMI_STATUS_OK) return status;
    view.dirty = 1;
    return UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(f->workbench), &view, text, strlen(text));
}

static UmiStatus MemoryRead(void *context, const char *resource, unsigned char **out, size_t *outLength)
{
    Fixture *f = context;
    (void)resource;
    ++f->reads;
    if (f->readStatus != UMI_STATUS_OK) return f->readStatus;
    unsigned char *copy = malloc(f->providerLength + 1U);
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, f->providerText, f->providerLength + 1U);
    *out = copy; *outLength = f->providerLength;
    if (f->mutateOnRead) {
        int operation = f->mutateOnRead; f->mutateOnRead = 0;
        if (operation == 2) f->hookStatus = UmiDocumentCoordinatorClose(f->documents, f->id, 1);
        else if (operation == 3) f->hookStatus = umi_document_store_replace_text(f->store, f->id, "late store edit\n", 16U);
        else f->hookStatus = EditStatus(f, "late provider callback edit\n");
    }
    return UMI_STATUS_OK;
}
static UmiStatus MemoryStat(void *context, const char *resource, UmiDocumentFileInfo *out)
{
    Fixture *f = context;
    if (f->statStatus != UMI_STATUS_OK) return f->statStatus;
    *out = (UmiDocumentFileInfo){0};
    (void)snprintf(out->path, sizeof out->path, "%s", resource);
    out->exists = 1; out->regular_file = 1; out->readable = 1; out->writable = 1;
    out->byte_count = f->providerLength;
    return UMI_STATUS_OK;
}
static UmiStatus MemoryWrite(void *context, const char *resource, const void *bytes, size_t length, int atomic)
{
    Fixture *f = context;
    (void)resource; (void)atomic;
    char *copy = malloc(length + 1U);
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (length != 0U) memcpy(copy, bytes, length);
    copy[length] = '\0';
    free(f->providerText); f->providerText = copy; f->providerLength = length; ++f->writes;
    return UMI_STATUS_OK;
}
static void MemoryRelease(void *context, void *bytes) { (void)context; free(bytes); }

static int Start(Fixture *f, const char *name, int memory)
{
    char temp[UMI_PATH_CAPACITY], leaf[160];
    CHECK(umi_fs_temp_directory(temp, sizeof temp) == UMI_STATUS_OK);
    CHECK(snprintf(leaf, sizeof leaf, "umicom-reload-%ld-%s", (long)PROCESS_ID(), name) > 0);
    CHECK(umi_fs_join(f->root, sizeof f->root, temp, leaf) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(f->root));
    CHECK(umi_fs_make_directories(f->root) == UMI_STATUS_OK); f->createdRoot = 1;
    CHECK(umi_fs_join(f->path, sizeof f->path, f->root, "notes.c") == UMI_STATUS_OK);
    CHECK(umi_command_registry_create(&f->commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.reload", f->commands, &f->workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f->store) == UMI_STATUS_OK);
    f->provider = umi_document_local_provider(); f->useProvider = memory;
    if (memory) {
        f->provider = (UmiDocumentProvider){ .struct_size = sizeof(UmiDocumentProvider),
            .abi_version = UMI_DOCUMENT_PROVIDER_ABI_VERSION, .provider_id = "test.memory", .scheme = "file",
            .flags = UMI_DOCUMENT_PROVIDER_READ | UMI_DOCUMENT_PROVIDER_STAT | UMI_DOCUMENT_PROVIDER_WRITE | UMI_DOCUMENT_PROVIDER_ATOMIC_WRITE,
            .instance = f, .read = MemoryRead, .write = MemoryWrite, .stat = MemoryStat, .release_bytes = MemoryRelease };
        CHECK(MemoryWrite(f, f->path, ORIGINAL, strlen(ORIGINAL), 0) == UMI_STATUS_OK); f->writes = 0U;
    } else CHECK(umi_fs_write_text(f->path, ORIGINAL) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(f->store, f->workbench, &f->provider, &f->documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_open(f->documents, f->path, f->view, sizeof f->view) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot s;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &s) == UMI_STATUS_OK); f->id = s.document_id;
    return 0;
}
static void Stop(Fixture *f)
{
    UmiDocumentReloadPlanDestroy(f->plan);
    umi_document_coordinator_destroy(f->documents);
    umi_document_store_destroy(f->store);
    umi_ui_workbench_destroy(f->workbench);
    umi_command_registry_destroy(f->commands);
    free(f->providerText);
    if (f->createdRoot) (void)umi_fs_remove_tree(f->root);
}
static int ExternalBytes(Fixture *f, const void *bytes, size_t length)
{
    CHECK((f->useProvider ? MemoryWrite(f, f->path, bytes, length, 0)
        : umi_fs_write_bytes(f->path, bytes, length)) == UMI_STATUS_OK);
    return 0;
}
static int External(Fixture *f, const char *text) { return ExternalBytes(f, text, strlen(text)); }
static int IsText(Fixture *f, const char *text)
{
    char *copy = NULL; size_t length = 0U;
    CHECK(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(f->workbench), f->view, &copy, &length) == UMI_STATUS_OK);
    int equal = length == strlen(text) && memcmp(copy, text, length) == 0;
    UmiUiDocumentViewModelFreeText(copy); CHECK(equal); return 0;
}
static int IsSavedText(Fixture *f, const char *text)
{
    char *copy = NULL; size_t length = 0U;
    CHECK(umi_document_store_copy_text(f->store, f->id, &copy, &length) == UMI_STATUS_OK);
    int equal = length == strlen(text) && memcmp(copy, text, length) == 0;
    umi_document_store_free_text(copy); CHECK(equal); return 0;
}
static int Snapshot(Fixture *f, UmiDocumentWorkingCopySnapshot *out)
{
    for (size_t i = 0U; i < umi_document_coordinator_count(f->documents); ++i) {
        CHECK(umi_document_coordinator_at(f->documents, i, out) == UMI_STATUS_OK);
        if (out->document_id == f->id) return 0;
    }
    return 1;
}
static int Prepare(Fixture *f) { CHECK(UmiDocumentCoordinatorPrepareReload(f->documents, f->id, &f->plan) == UMI_STATUS_OK); return 0; }
static int Apply(Fixture *f, int consent) { CHECK(UmiDocumentCoordinatorApplyReload(f->documents, f->plan, consent) == UMI_STATUS_OK); return 0; }

static int Run(Fixture *f, const char *name)
{
    UmiDocumentWorkingCopySnapshot before, after;
    UmiDocumentSnapshot stored;
    UmiUiDocumentViewSnapshot view;
    UmiDocumentReloadSummary summary;
    int changed = -1;
    CHECK(Snapshot(f, &before) == 0);
    if (strncmp(name, "review-", 7U) == 0) {
        const char *previous = NULL, *incoming = NULL;
        size_t previousLength = 0U, incomingLength = 0U;
        CHECK(EditStatus(f, DRAFT) == UMI_STATUS_OK);
        CHECK(External(f, INCOMING) == 0);
        CHECK(Prepare(f) == 0);
        CHECK(UmiDocumentReloadPlanTexts(f->plan, &previous, &previousLength, &incoming, &incomingLength) == UMI_STATUS_OK);
        CHECK(previousLength == strlen(DRAFT) && incomingLength == strlen(INCOMING));
        CHECK(strcmp(previous, DRAFT) == 0 && strcmp(incoming, INCOMING) == 0);
        CHECK(IsText(f, DRAFT) == 0 && IsSavedText(f, ORIGINAL) == 0);
        if (strcmp(name, "review-invalid") == 0) {
            CHECK(UmiDocumentReloadPlanTexts(NULL, &previous, &previousLength, &incoming, &incomingLength) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(!previous && !incoming && !previousLength && !incomingLength);
            CHECK(UmiDocumentReloadPlanTexts(f->plan, NULL, &previousLength, &incoming, &incomingLength) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDocumentReloadPlanTexts(f->plan, &previous, NULL, &incoming, &incomingLength) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDocumentReloadPlanTexts(f->plan, &previous, &previousLength, NULL, &incomingLength) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDocumentReloadPlanTexts(f->plan, &previous, &previousLength, &incoming, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        } else if (strcmp(name, "review-consumed") == 0) {
            CHECK(Apply(f, 1) == 0);
            CHECK(UmiDocumentReloadPlanTexts(f->plan, &previous, &previousLength, &incoming, &incomingLength) == UMI_STATUS_INVALID_STATE);
            CHECK(!previous && !incoming && !previousLength && !incomingLength);
        } else if (strcmp(name, "review-stale") == 0) {
            CHECK(External(f, "changed again\n") == 0);
            CHECK(EditStatus(f, "newer draft\n") == UMI_STATUS_OK);
            CHECK(UmiDocumentReloadPlanTexts(f->plan, &previous, &previousLength, &incoming, &incomingLength) == UMI_STATUS_OK);
            CHECK(strcmp(previous, DRAFT) == 0 && strcmp(incoming, INCOMING) == 0);
            CHECK(UmiDocumentCoordinatorApplyReload(f->documents, f->plan, 1) == UMI_STATUS_INVALID_STATE);
            CHECK(IsText(f, "newer draft\n") == 0);
        } else if (strcmp(name, "review-owner-closed") == 0) {
            umi_document_coordinator_destroy(f->documents); f->documents = NULL;
            CHECK(UmiDocumentReloadPlanTexts(f->plan, &previous, &previousLength, &incoming, &incomingLength) == UMI_STATUS_OK);
            CHECK(strcmp(previous, DRAFT) == 0 && strcmp(incoming, INCOMING) == 0);
        }
        return 0;
    }
    if (strcmp(name, "invalid") == 0) {
        CHECK(UmiDocumentCoordinatorPrepareReload(NULL, 1U, &f->plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f->plan == NULL);
        CHECK(UmiDocumentCoordinatorPrepareReload(f->documents, 0U, &f->plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorApplyReload(f->documents, NULL, 0) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorApplyReload(f->documents, NULL, 2) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorCheckExternalChange(NULL, 1U, &changed) == UMI_STATUS_INVALID_ARGUMENT && changed == 0);
        CHECK(umi_document_coordinator_new(f->documents, "untitled.c", NULL, 0U) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &after) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorPrepareReload(f->documents, after.document_id, &f->plan) == UMI_STATUS_INVALID_STATE);
        UmiDocumentReloadPlanDestroy(NULL);
        return IsText(f, ORIGINAL);
    }
    if (strcmp(name, "check-missing") == 0) {
        CHECK(remove(f->path) == 0);
        CHECK(UmiDocumentCoordinatorCheckExternalChange(f->documents, f->id, &changed) == UMI_STATUS_NOT_FOUND && changed == 1);
        CHECK(Snapshot(f, &after) == 0 && after.conflict == UMI_DOCUMENT_CONFLICT_DELETED_EXTERNALLY);
        CHECK(umi_document_coordinator_save_active(f->documents) == UMI_STATUS_INVALID_STATE);
        CHECK(!umi_fs_exists(f->path)); return IsText(f, ORIGINAL);
    }
    if (strcmp(name, "check-restored") == 0) {
        CHECK(External(f, INCOMING) == 0);
        CHECK(umi_document_coordinator_check_external_change(f->documents, &changed) == UMI_STATUS_OK && changed == 1);
        CHECK(External(f, ORIGINAL) == 0);
        CHECK(umi_document_coordinator_check_external_change(f->documents, &changed) == UMI_STATUS_OK && changed == 0);
        CHECK(Snapshot(f, &after) == 0 && after.conflict == UMI_DOCUMENT_CONFLICT_NONE);
        CHECK(umi_document_store_snapshot(f->store, f->id, &stored) == UMI_STATUS_OK && !stored.external_change);
        return IsText(f, ORIGINAL);
    }
    if (strcmp(name, "reentry-check-close") == 0) {
        f->mutateOnRead = 2;
        CHECK(umi_document_coordinator_check_external_change(f->documents, &changed) == UMI_STATUS_NOT_FOUND);
        CHECK(f->hookStatus == UMI_STATUS_OK && changed == 0);
        CHECK(umi_document_coordinator_count(f->documents) == 0U);
        return 0;
    }
    if (strcmp(name, "reentry-check-store") == 0) {
        f->mutateOnRead = 3;
        CHECK(umi_document_coordinator_check_external_change(f->documents, &changed) == UMI_STATUS_INVALID_STATE);
        CHECK(f->hookStatus == UMI_STATUS_OK && changed == 0);
        return IsSavedText(f, "late store edit\n");
    }
    if (strcmp(name, "reentry-check-draft") == 0) {
        f->mutateOnRead = 1;
        CHECK(umi_document_coordinator_check_external_change(f->documents, &changed) == UMI_STATUS_INVALID_STATE);
        CHECK(f->hookStatus == UMI_STATUS_OK && changed == 0);
        return IsText(f, "late provider callback edit\n");
    }
    if (strcmp(name, "provider-check") == 0) {
        CHECK(!umi_fs_exists(f->path));
        CHECK(umi_document_coordinator_check_external_change(f->documents, &changed) == UMI_STATUS_OK && changed == 0);
        CHECK(EditStatus(f, DRAFT) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_save_active(f->documents) == UMI_STATUS_OK);
        CHECK(f->writes == 1U && strcmp(f->providerText, DRAFT) == 0);
        return IsText(f, DRAFT);
    }
    if (strcmp(name, "provider-failure") == 0) {
        f->statStatus = UMI_STATUS_PERMISSION_DENIED;
        CHECK(umi_document_coordinator_check_external_change(f->documents, &changed) == UMI_STATUS_PERMISSION_DENIED && changed == 0);
        CHECK(Snapshot(f, &after) == 0 && after.conflict == UMI_DOCUMENT_CONFLICT_NONE);
        f->statStatus = UMI_STATUS_OK; f->readStatus = UMI_STATUS_IO_ERROR;
        CHECK(UmiDocumentCoordinatorPrepareReload(f->documents, f->id, &f->plan) == UMI_STATUS_IO_ERROR);
        CHECK(f->plan == NULL); return IsText(f, ORIGINAL);
    }
    if (strcmp(name, "fingerprint-limits") == 0) {
        UmiDocumentFingerprint fp = {.content_hash=55U, .valid=1}; unsigned calls = f->reads;
        CHECK(UmiDocumentFingerprintRead(&f->provider, f->path, 1U, &fp) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(f->reads == calls && fp.content_hash == 55U);
        f->provider.flags &= ~((uint32_t)UMI_DOCUMENT_PROVIDER_STAT);
        CHECK(UmiDocumentFingerprintRead(&f->provider, f->path, 1U, &fp) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(f->reads == calls + 1U && fp.content_hash == 55U);
        CHECK(UmiDocumentFingerprintRead(&f->provider, f->path, 1024U, &fp) == UMI_STATUS_OK);
        CHECK(fp.byte_count == strlen(ORIGINAL)); return 0;
    }
    if (strcmp(name, "cas") == 0) {
        CHECK(umi_document_store_snapshot(f->store, f->id, &stored) == UMI_STATUS_OK);
        CHECK(UmiDocumentStoreReplaceLoaded(f->store, f->id, stored.revision + 1U, stored.saved_revision,
            stored.path, INCOMING, strlen(INCOMING)) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentStoreReplaceLoaded(f->store, f->id, stored.revision, stored.saved_revision,
            stored.path, "x", SIZE_MAX) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(IsSavedText(f, ORIGINAL) == 0);
        CHECK(UmiDocumentStoreReplaceLoaded(f->store, f->id, stored.revision, stored.saved_revision,
            stored.path, INCOMING, strlen(INCOMING)) == UMI_STATUS_OK);
        CHECK(IsSavedText(f, INCOMING) == 0);
        char *disk = NULL; CHECK(umi_fs_read_text(f->path, &disk, NULL) == UMI_STATUS_OK);
        int same = strcmp(disk, ORIGINAL) == 0; umi_fs_free_text(disk); CHECK(same); return 0;
    }
#ifdef UMI_TEST_STAT_FAILURE
    if (strcmp(name, "stat-errors") == 0) {
        UmiDocumentFileInfo info;
        statError = EACCES; CHECK(umi_document_file_info(f->path, &info) == UMI_STATUS_PERMISSION_DENIED);
        statError = EIO; CHECK(umi_document_file_info(f->path, &info) == UMI_STATUS_IO_ERROR);
        statError = ENOENT; CHECK(umi_document_file_info(f->path, &info) == UMI_STATUS_NOT_FOUND);
        statError = 0; return 0;
    }
#endif
    const int dirty = strstr(name, "dirty") != NULL || strcmp(name, "undo-redo") == 0 || strcmp(name, "target-tab") == 0;
    if (dirty) CHECK(EditStatus(f, DRAFT) == UMI_STATUS_OK);
    CHECK(External(f, INCOMING) == 0);
    if (strcmp(name, "binary") == 0 || strcmp(name, "invalid-encoding") == 0) {
        const unsigned char data[] = { 'U', 'm', 0, 'x' };
        const unsigned char broken[] = { 0xc3 };
        CHECK(ExternalBytes(f, strcmp(name,"binary") == 0 ? data : broken,
            strcmp(name,"binary") == 0 ? sizeof data : sizeof broken) == 0);
        CHECK(UmiDocumentCoordinatorPrepareReload(f->documents, f->id, &f->plan) != UMI_STATUS_OK);
        CHECK(f->plan == NULL); return IsText(f, ORIGINAL);
    }
    if (strcmp(name, "oversized") == 0) {
        FILE *file = fopen(f->path, "wb"); CHECK(file != NULL);
        CHECK(fseek(file, (long)UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES, SEEK_SET) == 0);
        CHECK(fputc('x', file) != EOF); CHECK(fclose(file) == 0);
        CHECK(UmiDocumentCoordinatorPrepareReload(f->documents, f->id, &f->plan) == UMI_STATUS_CAPACITY_EXCEEDED);
        return IsText(f, ORIGINAL);
    }
    if (strcmp(name, "large") == 0 || strcmp(name, "aggregate-budget") == 0) {
        size_t size = strcmp(name, "large") == 0 ? 131072U : UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES;
        char *large = malloc(size + 1U); CHECK(large != NULL); memset(large, 'n', size); large[size] = '\0';
        CHECK(External(f, large) == 0); free(large);
    }
    if (strcmp(name, "utf16") == 0) {
        const unsigned char data[] = {0xff,0xfe,'U',0,'m',0,'i',0,'c',0,'o',0,'m',0,'\r',0,'\n',0};
        CHECK(ExternalBytes(f, data, sizeof data) == 0);
    }
    if (strcmp(name, "crlf") == 0) CHECK(External(f, "Umicom\r\nNotes\r\n") == 0);
    if (strcmp(name, "empty") == 0) CHECK(External(f, "") == 0);
    if (strcmp(name, "same-text") == 0) CHECK(External(f, ORIGINAL) == 0);
    if (strcmp(name, "utf8-caret") == 0) {
        CHECK(External(f, "\xc2\xa3x") == 0);
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->view, &view) == UMI_STATUS_OK);
        view.cursor_offset = 1U; view.selection_length = 5U;
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench), &view) == UMI_STATUS_OK);
    }
    if (strstr(name, "read-only") != NULL) {
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->view, &view) == UMI_STATUS_OK);
        view.read_only = 1; CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench), &view) == UMI_STATUS_OK);
    }
    if (strcmp(name, "reentry-prepare") == 0) {
        f->mutateOnRead = 1;
        CHECK(UmiDocumentCoordinatorPrepareReload(f->documents, f->id, &f->plan) == UMI_STATUS_INVALID_STATE);
        CHECK(f->hookStatus == UMI_STATUS_OK); return IsText(f, "late provider callback edit\n");
    }
    if (strncmp(name, "command-", 8) == 0) {
        char message[512]; CHECK(umi_document_commands_register(f->commands, f->documents) == UMI_STATUS_OK);
        CHECK(umi_command_registry_execute(f->commands, UMI_DOCUMENT_COMMAND_REVERT, "force", message, sizeof message) == UMI_STATUS_INVALID_ARGUMENT);
        UmiStatus status = umi_command_registry_execute(f->commands, UMI_DOCUMENT_COMMAND_REVERT, "", message, sizeof message);
        CHECK(status == (dirty ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK));
        return IsText(f, dirty ? DRAFT : INCOMING);
    }
    CHECK(Prepare(f) == 0);
    CHECK(UmiDocumentReloadPlanSummary(f->plan, &summary) == UMI_STATUS_OK);
    CHECK(summary.has_unsaved_changes == dirty);
    CHECK(IsText(f, dirty ? DRAFT : ORIGINAL) == 0);
    if (strcmp(name, "cancel") == 0) { UmiDocumentReloadPlanDestroy(f->plan); f->plan = NULL; return IsText(f, ORIGINAL); }
    if (strcmp(name, "dirty-refused") == 0) {
        CHECK(UmiDocumentCoordinatorApplyReload(f->documents, f->plan, 0) == UMI_STATUS_INVALID_STATE);
        CHECK(IsSavedText(f, ORIGINAL) == 0); return IsText(f, DRAFT);
    }
    if (strcmp(name, "dirty-read-only") == 0) {
        CHECK(UmiDocumentCoordinatorApplyReload(f->documents, f->plan, 1) == UMI_STATUS_PERMISSION_DENIED);
        return IsText(f, DRAFT);
    }
    if (strcmp(name, "draft-stale") == 0) CHECK(EditStatus(f, "newer draft\n") == UMI_STATUS_OK);
    if (strcmp(name, "store-stale") == 0) CHECK(umi_document_store_replace_text(f->store, f->id, "newer store", 11U) == UMI_STATUS_OK);
    if (strcmp(name, "save-stale") == 0) { CHECK(EditStatus(f, "saved after review\n") == UMI_STATUS_OK); CHECK(External(f, ORIGINAL) == 0); CHECK(umi_document_coordinator_save_active(f->documents) == UMI_STATUS_OK); }
    if (strcmp(name, "save-as-stale") == 0) {
        char path[UMI_PATH_CAPACITY]; CHECK(umi_fs_join(path, sizeof path, f->root, "copy.c") == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_save_active_as(f->documents, path) == UMI_STATUS_OK);
    }
    if (strcmp(name, "file-stale") == 0) CHECK(External(f, "changed again\n") == 0);
    if (strcmp(name, "reentry-apply") == 0) f->mutateOnRead = 1;
    if (strstr(name, "-stale") != NULL || strcmp(name, "reentry-apply") == 0) {
        CHECK(UmiDocumentCoordinatorApplyReload(f->documents, f->plan, 1) == UMI_STATUS_INVALID_STATE);
        return IsText(f, strcmp(name,"draft-stale") == 0 ? "newer draft\n" :
            strcmp(name,"save-stale") == 0 ? "saved after review\n" :
            strcmp(name,"reentry-apply") == 0 ? "late provider callback edit\n" : ORIGINAL);
    }
    if (strcmp(name, "closed") == 0) {
        CHECK(UmiDocumentCoordinatorClose(f->documents, f->id, 1) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorApplyReload(f->documents, f->plan, 1) == UMI_STATUS_NOT_FOUND); return 0;
    }
    if (strcmp(name, "destroy-plan") == 0) {
        umi_document_coordinator_destroy(f->documents); f->documents = NULL;
        UmiDocumentReloadPlanDestroy(f->plan); f->plan = NULL; return 0;
    }
    if (strcmp(name, "aggregate-budget") == 0) {
        size_t size = 7U * 1024U * 1024U; char *large = malloc(size + 1U); CHECK(large != NULL);
        memset(large, 'x', size); large[size] = '\0';
        for (unsigned i = 0U; i < 8U; ++i) {
            UmiUiDocumentViewSnapshot other = {0};
            (void)snprintf(other.view_id, sizeof other.view_id, "budget.%u", i);
            strcpy(other.document_id, other.view_id); strcpy(other.title, "memory-budget.c");
            CHECK(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(f->workbench), &other, large, size) == UMI_STATUS_OK);
        }
        free(large);
        CHECK(UmiDocumentCoordinatorApplyReload(f->documents, f->plan, 0) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(IsSavedText(f, ORIGINAL) == 0); return IsText(f, ORIGINAL);
    }
    char otherView[UMI_UI_ID_CAPACITY] = {0};
    if (strcmp(name, "target-tab") == 0) {
        CHECK(umi_document_coordinator_new(f->documents, "other.c", otherView, sizeof otherView) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), otherView, &view) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(f->workbench), &view, "other unsaved\n", 14U) == UMI_STATUS_OK);
    }
    CHECK(Apply(f, dirty) == 0);
    CHECK(Snapshot(f, &after) == 0 && !after.dirty && after.conflict == UMI_DOCUMENT_CONFLICT_NONE);
    if (strcmp(name, "target-tab") == 0) {
        UmiDocumentWorkingCopySnapshot active;
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK);
        CHECK(strcmp(active.view_id, otherView) == 0 && active.dirty);
        CHECK(umi_ui_workbench_activate_document(f->workbench, f->view) == UMI_STATUS_OK);
    }
    if (strcmp(name, "utf8-caret") == 0) {
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->view, &view) == UMI_STATUS_OK);
        CHECK(view.cursor_offset == 0U && view.selection_length == 0U); return IsText(f, "\xc2\xa3x");
    }
    if (strcmp(name, "utf16") == 0 || strcmp(name,"crlf") == 0) {
        CHECK(after.line_ending == UMI_DOCUMENT_LINE_ENDING_CRLF);
        CHECK(IsText(f, strcmp(name,"utf16") == 0 ? "Umicom\n" : "Umicom\nNotes\n") == 0);
        CHECK(EditStatus(f, "Umicom edited\n") == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_save_active(f->documents) == UMI_STATUS_OK);
        unsigned char *bytes = NULL; size_t size = 0U;
        CHECK(umi_fs_read_bytes(f->path, &bytes, &size) == UMI_STATUS_OK);
        int correct = strcmp(name,"utf16") == 0 ? size > 2U && bytes[0]==0xff && bytes[1]==0xfe : size > 2U && bytes[size-2U]=='\r' && bytes[size-1U]=='\n';
        umi_fs_free_bytes(bytes); CHECK(correct); return 0;
    }
    if (strcmp(name, "large") == 0) {
        CHECK(after.text_length == 131072U);
        CHECK(umi_document_coordinator_undo(f->documents) == UMI_STATUS_OK); return IsText(f, ORIGINAL);
    }
    if (strcmp(name, "empty") == 0) return IsText(f, "");
    if (strcmp(name, "same-text") == 0) { CHECK(after.undo_count == before.undo_count); return IsText(f, ORIGINAL); }
    CHECK(IsText(f, INCOMING) == 0);
    if (strcmp(name, "undo-redo") == 0 || strcmp(name, "dirty-approved") == 0) {
        CHECK(umi_document_coordinator_undo(f->documents) == UMI_STATUS_OK); CHECK(IsText(f, DRAFT) == 0);
        CHECK(umi_document_coordinator_redo(f->documents) == UMI_STATUS_OK); CHECK(IsText(f, INCOMING) == 0);
    }
    if (strcmp(name, "used") == 0) CHECK(UmiDocumentCoordinatorApplyReload(f->documents, f->plan, 0) == UMI_STATUS_INVALID_STATE);
    return 0;
}

#ifdef UMI_TEST_ALLOCATION_FAILURE
static int AllocationCampaign(const char *mode)
{
    unsigned rejected = 0U, succeeded = 0U;
    for (long point = 0; point < 16; ++point) {
        Fixture f = {0}; char name[80]; (void)snprintf(name, sizeof name, "%s-%ld", mode, point);
        CHECK(Start(&f, name, 0) == 0);
        char *large = malloc(65537U); CHECK(large != NULL); memset(large, 'x', 65536U); large[65536U]='\0';
        CHECK(External(&f, large) == 0); free(large);
        UmiStatus status;
        if (strcmp(mode,"allocation-prepare") == 0) {
            allocationsBeforeFailure = point;
            status = UmiDocumentCoordinatorPrepareReload(f.documents, f.id, &f.plan);
        } else {
            CHECK(Prepare(&f) == 0); allocationsBeforeFailure = point;
            status = UmiDocumentCoordinatorApplyReload(f.documents, f.plan, 0);
        }
        allocationsBeforeFailure = -1;
        if (status == UMI_STATUS_OK) ++succeeded;
        else {
            CHECK(status == UMI_STATUS_OUT_OF_MEMORY); ++rejected;
            CHECK(IsText(&f, ORIGINAL) == 0 && IsSavedText(&f, ORIGINAL) == 0);
            UmiDocumentWorkingCopySnapshot s; CHECK(Snapshot(&f, &s) == 0 && s.undo_count == 0U && !s.dirty);
        }
        Stop(&f);
    }
    CHECK(rejected != 0U && succeeded != 0U);
    printf("%s: %u injected rejections, %u successful calls\n", mode, rejected, succeeded);
    return 0;
}
#endif
int main(int argc, char **argv)
{
    CHECK(argc == 2);
#ifdef UMI_TEST_ALLOCATION_FAILURE
    if (strncmp(argv[1], "allocation-", 11U) == 0) return AllocationCampaign(argv[1]);
#endif
    Fixture f = {0};
    int memory = strncmp(argv[1], "provider-", 9U) == 0 || strncmp(argv[1],"reentry-",8U) == 0 || strcmp(argv[1],"fingerprint-limits")==0;
    int result = Start(&f, argv[1], memory);
    if (result == 0) result = Run(&f, argv[1]);
    Stop(&f);
    if (result == 0) printf("%s: passed\n", argv[1]);
    return result;
}
