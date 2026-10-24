/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Draft regressions exercise user-visible editing transitions independently
 * of widgets and storage. Successful completions below model a storage reply;
 * the separate storage test exercises actual atomic document writes. */
#include "umicom/data/record_draft.h"
#include "umicom/base/sha256.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)

static UmiRecordDraftAttribution attribution(void)
{
    return (UmiRecordDraftAttribution){.source = "fictional-import", .actor = "test-author",
        .reason = "Review example", .observed_at_nanoseconds = 20U, .recorded_at_nanoseconds = 30U};
}

static int fixture(UmiRecordDocument *document)
{
    memset(document, 0, sizeof *document);
    document->record = (UmiRecordRevision){.revision = 7U, .previous_revision = 6U,
        .amendment = {.entity_id = "example-a", .source = "fixture", .actor = "test-author",
            .reason = "Baseline", .recorded_at_nanoseconds = 10U}};
    document->length = 4U;
    memcpy(document->content, "base", 4U);
    OK(UmiSha256Buffer(document->content, document->length, document->record.amendment.content_sha256));
    return 0;
}

static int content_is(UmiRecordDraft *draft, const void *expected, size_t length)
{
    unsigned char bytes[UMI_RECORD_DOCUMENT_MAX_BYTES];
    size_t actual = 99U;
    OK(umi_record_draft_copy_content(draft, bytes, sizeof bytes, &actual));
    CHECK(actual == length);
    CHECK(length == 0U || memcmp(bytes, expected, length) == 0);
    return 0;
}

static int state_is(UmiRecordDraft *draft, bool ready, bool dirty, bool saving, uint64_t revision)
{
    UmiRecordDraftState state;
    OK(umi_record_draft_state(draft, &state));
    CHECK(state.ready == ready && state.dirty == dirty && state.saving == saving);
    CHECK(state.expected_revision == revision);
    CHECK(state.is_new == (ready && revision == 0U));
    CHECK((state.pending_request_id != 0U) == saving);
    return 0;
}

static int exercise(const char *name)
{
    UmiRecordDraft *draft = NULL;
    UmiRecordDocument document;
    UmiRecordSaveRequest request;
    UmiRecordDraftAttribution author = attribution();
    OK(umi_record_draft_create(&draft));
    CHECK(fixture(&document) == 0);
    if (strcmp(name, "new") == 0) {
        CHECK(state_is(draft, false, false, false, 0U) == 0);
        CHECK(umi_record_draft_begin(draft, "") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_draft_begin(draft, "bad\nentity") == UMI_STATUS_INVALID_ARGUMENT);
        char entity[65]; memset(entity, 'a', sizeof entity); entity[64] = '\0';
        CHECK(umi_record_draft_begin(draft, entity) == UMI_STATUS_INVALID_ARGUMENT);
        entity[63] = '\0'; OK(umi_record_draft_begin(draft, entity));
        CHECK(state_is(draft, true, true, false, 0U) == 0);
        size_t length = 99U; OK(umi_record_draft_copy_content(draft, NULL, 0U, &length)); CHECK(length == 0U);
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(request.length == 0U && request.proposed.revision == 1U);
        CHECK(strcmp(request.proposed.amendment.entity_id, entity) == 0);
        CHECK(strcmp(request.proposed.amendment.content_sha256,
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") == 0);
        OK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &request.proposed));
        CHECK(state_is(draft, true, false, false, 1U) == 0);
    } else if (strcmp(name, "open-integrity") == 0) {
        document.content[0] = 'X';
        CHECK(umi_record_draft_open(draft, &document) == UMI_STATUS_PARSE_ERROR);
        CHECK(state_is(draft, false, false, false, 0U) == 0);
        document.content[0] = 'b';
        document.record.revision = 8U;
        CHECK(umi_record_draft_open(draft, &document) == UMI_STATUS_INVALID_ARGUMENT);
        document.record.revision = 7U;
        document.length = UMI_RECORD_DOCUMENT_MAX_BYTES + 1U;
        CHECK(umi_record_draft_open(draft, &document) == UMI_STATUS_CAPACITY_EXCEEDED);
        document.length = 4U;
        OK(umi_record_draft_open(draft, &document));
        document.content[0] = 'X'; /* Open retained an independent baseline. */
        CHECK(content_is(draft, "base", 4U) == 0);
        CHECK(state_is(draft, true, false, false, 7U) == 0);
    } else if (strcmp(name, "dirty-guard") == 0) {
        OK(umi_record_draft_begin(draft, "new-entity"));
        CHECK(umi_record_draft_open(draft, &document) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_record_draft_begin(draft, "other") == UMI_STATUS_INVALID_STATE);
        OK(umi_record_draft_discard(draft));
        OK(umi_record_draft_open(draft, &document));
        OK(umi_record_draft_set_content(draft, "local", 5U));
        CHECK(umi_record_draft_open(draft, &document) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_record_draft_begin(draft, "other") == UMI_STATUS_INVALID_STATE);
        CHECK(content_is(draft, "local", 5U) == 0);
        CHECK(state_is(draft, true, true, false, 7U) == 0);
    } else if (strcmp(name, "discard") == 0) {
        CHECK(umi_record_draft_discard(draft) == UMI_STATUS_INVALID_STATE);
        OK(umi_record_draft_open(draft, &document));
        OK(umi_record_draft_set_content(draft, "local", 5U));
        OK(umi_record_draft_discard(draft));
        CHECK(content_is(draft, "base", 4U) == 0);
        CHECK(state_is(draft, true, false, false, 7U) == 0);
        OK(umi_record_draft_begin(draft, "new"));
        OK(umi_record_draft_set_content(draft, "discard me", 10U));
        OK(umi_record_draft_discard(draft));
        CHECK(state_is(draft, false, false, false, 0U) == 0);
        UmiRecordDraftState state; OK(umi_record_draft_state(draft, &state));
        CHECK(state.length == 0U && state.entity_id[0] == '\0');
    } else if (strcmp(name, "binary") == 0) {
        unsigned char bytes[UMI_RECORD_DOCUMENT_MAX_BYTES];
        for (size_t i = 0U; i < sizeof bytes; ++i) bytes[i] = (unsigned char)(i % 256U);
        OK(umi_record_draft_begin(draft, "bytes"));
        OK(umi_record_draft_set_content(draft, bytes, sizeof bytes));
        CHECK(content_is(draft, bytes, sizeof bytes) == 0);
        CHECK(umi_record_draft_set_content(draft, bytes, sizeof bytes + 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_record_draft_set_content(draft, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(content_is(draft, bytes, sizeof bytes) == 0);
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(request.length == sizeof bytes && memcmp(request.content, bytes, sizeof bytes) == 0);
        CHECK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_CANCELLED, NULL) == UMI_STATUS_CANCELLED);
        OK(umi_record_draft_set_content(draft, NULL, 0U));
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        for (size_t i = 0U; i < sizeof request.content; ++i) CHECK(request.content[i] == 0U);
    } else if (strcmp(name, "invalid-attribution") == 0) {
        OK(umi_record_draft_begin(draft, "example"));
        memset(&request, 0xa5, sizeof request);
        unsigned char before[sizeof request]; memcpy(before, &request, sizeof request);
        for (unsigned int i = 0U; i < 5U; ++i) {
            author = attribution();
            if (i == 0U) author.actor[0] = '\0';
            if (i == 1U) strcpy(author.reason, "invalid\treason");
            if (i == 2U) memset(author.source, 's', sizeof author.source);
            if (i == 3U) author.recorded_at_nanoseconds = 0U;
            if (i == 4U) memset(author.actor, 'a', sizeof author.actor);
            CHECK(umi_record_draft_prepare_save(draft, &author, &request) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(before, &request, sizeof request) == 0);
            CHECK(state_is(draft, true, true, false, 0U) == 0);
        }
    } else if (strcmp(name, "save-success") == 0) {
        OK(umi_record_draft_open(draft, &document));
        CHECK(umi_record_draft_prepare_save(draft, &author, &request) == UMI_STATUS_INVALID_STATE);
        OK(umi_record_draft_set_content(draft, "amended", 7U));
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(request.proposed.previous_revision == 7U && request.proposed.revision == 8U);
        CHECK(strcmp(request.proposed.amendment.actor, author.actor) == 0);
        CHECK(strcmp(request.proposed.amendment.reason, author.reason) == 0);
        CHECK(request.proposed.amendment.observed_at_nanoseconds == 20U);
        OK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &request.proposed));
        CHECK(state_is(draft, true, false, false, 8U) == 0);
        OK(umi_record_draft_set_content(draft, "later", 5U));
        OK(umi_record_draft_discard(draft));
        CHECK(content_is(draft, "amended", 7U) == 0);
    } else if (strcmp(name, "save-failure") == 0) {
        OK(umi_record_draft_open(draft, &document));
        OK(umi_record_draft_set_content(draft, "local", 5U));
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        uint64_t first = request.request_id;
        CHECK(umi_record_draft_finish_save(draft, first, UMI_STATUS_IO_ERROR, NULL) == UMI_STATUS_IO_ERROR);
        CHECK(state_is(draft, true, true, false, 7U) == 0);
        CHECK(content_is(draft, "local", 5U) == 0);
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(request.request_id != first && request.proposed.previous_revision == 7U);
        CHECK(umi_record_draft_finish_save(draft, first, UMI_STATUS_CANCELLED, NULL) == UMI_STATUS_INVALID_STATE);
        CHECK(state_is(draft, true, true, true, 7U) == 0);
    } else if (strcmp(name, "edit-during-save") == 0 || strcmp(name, "edit-away-back") == 0) {
        OK(umi_record_draft_open(draft, &document));
        OK(umi_record_draft_set_content(draft, "captured", 8U));
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        OK(umi_record_draft_set_content(draft, "new typing", 10U));
        bool back = strcmp(name, "edit-away-back") == 0;
        if (back) OK(umi_record_draft_set_content(draft, "base", 4U));
        /* The old baseline is clean, but it is different from the pending write. */
        CHECK(state_is(draft, true, !back, true, 7U) == 0);
        CHECK(request.length == 8U && memcmp(request.content, "captured", 8U) == 0);
        OK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &request.proposed));
        CHECK(state_is(draft, true, true, false, 8U) == 0);
        CHECK(content_is(draft, back ? "base" : "new typing", back ? 4U : 10U) == 0);
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(request.proposed.previous_revision == 8U);
        CHECK(request.length == (back ? 4U : 10U));
    } else if (strcmp(name, "completions") == 0) {
        OK(umi_record_draft_begin(draft, "new"));
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(umi_record_draft_finish_save(draft, request.request_id + 1U, UMI_STATUS_OK, &request.proposed) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiRecordRevision wrong = request.proposed;
        strcpy(wrong.amendment.entity_id, "other");
        CHECK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &wrong) == UMI_STATUS_INVALID_STATE);
        wrong = request.proposed; wrong.amendment.content_sha256[0] = wrong.amendment.content_sha256[0] == 'a' ? 'b' : 'a';
        CHECK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &wrong) == UMI_STATUS_INVALID_STATE);
        wrong = request.proposed; wrong.revision = 0U;
        CHECK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &wrong) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(state_is(draft, true, true, true, 0U) == 0);
        OK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &request.proposed));
        CHECK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &request.proposed) == UMI_STATUS_INVALID_STATE);
        CHECK(state_is(draft, true, false, false, 1U) == 0);
    } else if (strcmp(name, "pending-guard") == 0) {
        OK(umi_record_draft_begin(draft, "new"));
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        UmiRecordSaveRequest extra; memset(&extra, 0xa5, sizeof extra);
        unsigned char before[sizeof extra]; memcpy(before, &extra, sizeof extra);
        CHECK(umi_record_draft_prepare_save(draft, &author, &extra) == UMI_STATUS_BUSY);
        CHECK(memcmp(before, &extra, sizeof extra) == 0);
        CHECK(umi_record_draft_begin(draft, "other") == UMI_STATUS_BUSY);
        CHECK(umi_record_draft_open(draft, &document) == UMI_STATUS_BUSY);
        CHECK(umi_record_draft_discard(draft) == UMI_STATUS_BUSY);
        CHECK(state_is(draft, true, true, true, 0U) == 0);
    } else if (strcmp(name, "token-lifetime") == 0) {
        OK(umi_record_draft_begin(draft, "first"));
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        uint64_t old = request.request_id;
        CHECK(umi_record_draft_finish_save(draft, old, UMI_STATUS_CANCELLED, NULL) == UMI_STATUS_CANCELLED);
        OK(umi_record_draft_discard(draft));
        OK(umi_record_draft_begin(draft, "second"));
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(request.request_id != old);
        CHECK(umi_record_draft_finish_save(draft, old, UMI_STATUS_IO_ERROR, NULL) == UMI_STATUS_INVALID_STATE);
        CHECK(state_is(draft, true, true, true, 0U) == 0);
    } else if (strcmp(name, "overflow") == 0) {
        document.record.revision = UINT64_MAX;
        document.record.previous_revision = UINT64_MAX - 1U;
        OK(umi_record_draft_open(draft, &document));
        OK(umi_record_draft_set_content(draft, "new", 3U));
        memset(&request, 0xa5, sizeof request);
        unsigned char before[sizeof request]; memcpy(before, &request, sizeof request);
        CHECK(umi_record_draft_prepare_save(draft, &author, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, &request, sizeof request) == 0);
        CHECK(state_is(draft, true, true, false, UINT64_MAX) == 0);
    } else if (strcmp(name, "argument-atomicity") == 0) {
        OK(umi_record_draft_open(draft, &document));
        unsigned char bytes[8]; memset(bytes, 0xa5, sizeof bytes);
        size_t length = 99U;
        CHECK(umi_record_draft_copy_content(draft, bytes, 3U, &length) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(length == 99U);
        for (size_t i = 0U; i < sizeof bytes; ++i) CHECK(bytes[i] == 0xa5U);
        CHECK(umi_record_draft_copy_content(draft, NULL, sizeof bytes, &length) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(length == 99U);
        CHECK(umi_record_draft_copy_content(draft, bytes, sizeof bytes, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_draft_set_content(NULL, bytes, sizeof bytes) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_draft_create(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_draft_begin(draft, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_draft_open(draft, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_draft_state(draft, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_draft_finish_save(NULL, 1U, UMI_STATUS_OK, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(state_is(draft, true, false, false, 7U) == 0);
        CHECK(content_is(draft, "base", 4U) == 0);
    } else if (strcmp(name, "independent") == 0) {
        UmiRecordDraft *other = NULL; OK(umi_record_draft_create(&other));
        OK(umi_record_draft_open(draft, &document));
        OK(umi_record_draft_open(other, &document));
        OK(umi_record_draft_set_content(draft, "one", 3U));
        OK(umi_record_draft_set_content(other, "two", 3U));
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(content_is(other, "two", 3U) == 0);
        CHECK(state_is(other, true, true, false, 7U) == 0);
        umi_record_draft_destroy(other);
    } else {
        fprintf(stderr, "Unknown draft regression: %s\n", name);
        umi_record_draft_destroy(draft);
        return 1;
    }
    umi_record_draft_destroy(draft);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    return exercise(argv[1]);
}
