/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_media_storyboard_file_contract.c
 *
 * PURPOSE:
 *   Inject file-boundary outcomes into the real storyboard wrapper without pretending to qualify native I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/creative_workspace/media_storyboard.h"
#include "umicom/platform/atomic_file.h"
#include "umicom/platform/input_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
/* The included production wrapper has an absolute_path parameter. Keep the
 * fixture identity distinct without changing any path, I/O result or counter.
 * Retain the previous declaration for engineering review. */
#if 0
#ifdef _WIN32
static const char absolute_path[] = "C:/umicom-storyboard-contract/document.json";
#else
static const char absolute_path[] = "/umicom-storyboard-contract/document.json";
#endif
#endif
#ifdef _WIN32
static const char fixture_document_path[] = "C:/umicom-storyboard-contract/document.json";
#else
static const char fixture_document_path[] = "/umicom-storyboard-contract/document.json";
#endif
static const char good_json[] =
    "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[]}";
static const char *read_payload = good_json;
static UmiStatus read_status = UMI_STATUS_OK, write_status = UMI_STATUS_OK;
static UmiCancellationToken *token;
static unsigned writes, reads, freed;
static int cancel_during_write, cancel_after_read;
static UmiStatus FakeAtomicWrite(const char *path, const void *bytes, size_t size)
{
    ++writes;
    CHECK(strcmp(path, fixture_document_path) == 0);
    UmiMediaStoryboardDraft restored = {0};
    CHECK(UmiMediaStoryboardDraftImport(bytes, size, NULL, &restored) == UMI_STATUS_OK);
    CHECK(restored.count == 1U && strcmp(restored.scenes[0].title, "Opening") == 0);
    if (cancel_during_write) umi_cancellation_token_request(token);
    return write_status;
}
static UmiStatus FakeInputRead(const char *path, size_t maximum_bytes,
    unsigned char **out_bytes, size_t *out_size)
{
    ++reads;
    CHECK(strcmp(path, fixture_document_path) == 0);
    CHECK(maximum_bytes == UMI_MEDIA_STORYBOARD_DOCUMENT_BYTES);
    *out_bytes = NULL;
    *out_size = 0U;
    if (read_status != UMI_STATUS_OK) return read_status;
    size_t size = strlen(read_payload);
    *out_bytes = malloc(size + 1U);
    CHECK(*out_bytes != NULL);
    memcpy(*out_bytes, read_payload, size + 1U);
    *out_size = size;
    if (cancel_after_read) umi_cancellation_token_request(token);
    return UMI_STATUS_OK;
}
static void FakeInputFree(void *bytes)
{
    if (bytes != NULL) ++freed;
    free(bytes);
}
/* Compile the production file wrapper with only its native I/O calls replaced.
 * The real model, JSON parser and document encoder remain linked from Framework.
 * Normal native I/O and crash behavior require separate owner qualification. */
#define UmiMediaStoryboardDraftSave TestStoryboardSave
#define UmiMediaStoryboardDraftLoad TestStoryboardLoad
#define umi_atomic_file_write FakeAtomicWrite
#define UmiInputFileRead FakeInputRead
#define UmiInputFileFree FakeInputFree
#include "../../src/creative_workspace/media_storyboard_file.c"
#undef UmiMediaStoryboardDraftSave
#undef UmiMediaStoryboardDraftLoad
#undef umi_atomic_file_write
#undef UmiInputFileRead
#undef UmiInputFileFree

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    const char *name = argv[1];
    const char *path = fixture_document_path;
    UmiStatus expected = UMI_STATUS_OK;
    UmiMediaStoryboardDraft draft = {0}, before;
    unsigned expected_calls = 1U;
    int saving = strncmp(name, "save-", 5U) == 0;
    if (saving) {
        draft.count = 1U;
        strcpy(draft.scenes[0].title, "Opening");
        strcpy(draft.scenes[0].story, "Show the morning.");
        draft.scenes[0].duration_seconds = 10U;
        if (strcmp(name, "save-failure") == 0) write_status = expected = UMI_STATUS_IO_ERROR;
        else if (strcmp(name, "save-cancel-before") == 0) {
            umi_cancellation_token_request(token);
            expected = UMI_STATUS_CANCELLED; expected_calls = 0U;
        } else if (strcmp(name, "save-cancel-during") == 0) cancel_during_write = 1;
        else if (strcmp(name, "save-failure-cancel-during") == 0) {
            cancel_during_write = 1; write_status = expected = UMI_STATUS_IO_ERROR;
        } else if (strcmp(name, "save-invalid-draft") == 0) {
            draft.scenes[0].duration_seconds = 0U;
            expected = UMI_STATUS_INVALID_ARGUMENT; expected_calls = 0U;
        } else if (strcmp(name, "save-relative-path") == 0) {
            path = "relative.json"; expected = UMI_STATUS_INVALID_ARGUMENT; expected_calls = 0U;
        } else CHECK(strcmp(name, "save-success") == 0);
        memcpy(&before, &draft, sizeof(before));
        CHECK(TestStoryboardSave(&draft, path, token) == expected);
        CHECK(writes == expected_calls && reads == 0U);
        CHECK(memcmp(&draft, &before, sizeof(draft)) == 0);
    } else {
        memset(&draft, 0x5a, sizeof(draft));
        memcpy(&before, &draft, sizeof(before));
        if (strcmp(name, "load-failure") == 0) read_status = expected = UMI_STATUS_IO_ERROR;
        else if (strcmp(name, "load-corrupt") == 0) read_payload = "{";
        else if (strcmp(name, "load-cancel-before") == 0) {
            umi_cancellation_token_request(token);
            expected = UMI_STATUS_CANCELLED; expected_calls = 0U;
        } else if (strcmp(name, "load-cancel-after-read") == 0) {
            cancel_after_read = 1; expected = UMI_STATUS_CANCELLED;
        } else if (strcmp(name, "load-relative-path") == 0) {
            path = "relative.json"; expected = UMI_STATUS_INVALID_ARGUMENT; expected_calls = 0U;
        } else if (strcmp(name, "load-null-output") == 0) {
            expected = UMI_STATUS_INVALID_ARGUMENT; expected_calls = 0U;
        } else if (strcmp(name, "load-empty") == 0) {
            read_payload = ""; expected = UMI_STATUS_INVALID_ARGUMENT;
        } else CHECK(strcmp(name, "load-success") == 0);
        UmiStatus status = TestStoryboardLoad(path, token,
            strcmp(name, "load-null-output") == 0 ? NULL : &draft);
        if (strcmp(name, "load-corrupt") == 0) CHECK(status != UMI_STATUS_OK);
        else CHECK(status == expected);
        CHECK(reads == expected_calls && writes == 0U);
        CHECK(freed == (read_status == UMI_STATUS_OK ? expected_calls : 0U));
        if (status != UMI_STATUS_OK) CHECK(memcmp(&draft, &before, sizeof(draft)) == 0);
        else {
            UmiMediaStoryboardDraft blank = {0};
            CHECK(memcmp(&draft, &blank, sizeof(draft)) == 0);
        }
    }
    umi_cancellation_token_destroy(token);
    return 0;
}
