/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reviewed-baseline adoption preserves local text and rejects a different
 * entity, stale comparison or rewritten same-number revision. */
#include "umicom/data/record_draft.h"
#include "umicom/base/sha256.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static int make_document(UmiRecordDocument *document, uint64_t revision, const char *content)
{
    memset(document, 0, sizeof *document);
    document->record = (UmiRecordRevision){.revision = revision, .previous_revision = revision - 1U,
        .amendment = {.entity_id = "example", .source = "fixture", .actor = "reviewer",
            .reason = "Synthetic revision", .recorded_at_nanoseconds = 100U}};
    document->length = strlen(content); memcpy(document->content, content, document->length);
    OK(UmiSha256Buffer(document->content, document->length, document->record.amendment.content_sha256));
    return 0;
}
static int exercise(const char *name)
{
    UmiRecordDraft *draft = NULL;
    UmiRecordDocument old, latest;
    UmiRecordDraftState state;
    UmiRecordSaveRequest request;
    UmiRecordDraftAttribution author = {.source = "fixture", .actor = "reviewer", .reason = "Reviewed comparison", .recorded_at_nanoseconds = 200U};
    CHECK(make_document(&old, 1U, "base") == 0);
    CHECK(make_document(&latest, 2U, "external") == 0);
    OK(umi_record_draft_create(&draft));
    OK(umi_record_draft_open(draft, &old));
    OK(umi_record_draft_set_content(draft, "local", 5U));
    if (strcmp(name, "preserve") == 0) {
        OK(umi_record_draft_adopt_reviewed(draft, &latest, 1U));
        OK(umi_record_draft_state(draft, &state)); CHECK(state.dirty && state.expected_revision == 2U);
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(request.length == 5U && memcmp(request.content, "local", 5U) == 0 && request.proposed.previous_revision == 2U);
    } else if (strcmp(name, "changed-baseline") == 0) {
        OK(umi_record_draft_adopt_reviewed(draft, &latest, 1U));
        CHECK(umi_record_draft_adopt_reviewed(draft, &old, 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_record_draft_adopt_reviewed(draft, &old, 2U) == UMI_STATUS_INVALID_STATE);
        OK(umi_record_draft_state(draft, &state)); CHECK(state.expected_revision == 2U && state.dirty);
    } else if (strcmp(name, "identity") == 0) {
        strcpy(latest.record.amendment.entity_id, "another");
        CHECK(umi_record_draft_adopt_reviewed(draft, &latest, 1U) == UMI_STATUS_INVALID_STATE);
        OK(umi_record_draft_state(draft, &state)); CHECK(state.expected_revision == 1U && state.dirty);
    } else if (strcmp(name, "same-revision") == 0) {
        OK(umi_record_draft_adopt_reviewed(draft, &old, 1U));
        CHECK(make_document(&latest, 1U, "rewritten") == 0);
        CHECK(umi_record_draft_adopt_reviewed(draft, &latest, 1U) == UMI_STATUS_INVALID_STATE);
        latest = old; strcpy(latest.record.amendment.actor, "changed actor");
        CHECK(umi_record_draft_adopt_reviewed(draft, &latest, 1U) == UMI_STATUS_INVALID_STATE);
        OK(umi_record_draft_state(draft, &state)); CHECK(state.expected_revision == 1U);
    } else if (strcmp(name, "pending") == 0) {
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(umi_record_draft_adopt_reviewed(draft, &latest, 1U) == UMI_STATUS_BUSY);
        OK(umi_record_draft_state(draft, &state)); CHECK(state.saving && state.expected_revision == 1U);
    } else if (strcmp(name, "invalid") == 0) {
        latest.content[0] = 'X';
        CHECK(umi_record_draft_adopt_reviewed(draft, &latest, 1U) == UMI_STATUS_PARSE_ERROR);
        CHECK(umi_record_draft_adopt_reviewed(draft, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        latest.length = UMI_RECORD_DOCUMENT_MAX_BYTES + 1U;
        CHECK(umi_record_draft_adopt_reviewed(draft, &latest, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        OK(umi_record_draft_state(draft, &state)); CHECK(state.dirty && state.expected_revision == 1U);
    } else if (strcmp(name, "new-collision") == 0) {
        OK(umi_record_draft_discard(draft));
        OK(umi_record_draft_begin(draft, "example"));
        OK(umi_record_draft_set_content(draft, "local", 5U));
        OK(umi_record_draft_adopt_reviewed(draft, &latest, 0U));
        OK(umi_record_draft_state(draft, &state)); CHECK(!state.is_new && state.expected_revision == 2U && state.dirty);
    } else if (strcmp(name, "matches-latest") == 0) {
        OK(umi_record_draft_set_content(draft, "external", 8U));
        OK(umi_record_draft_adopt_reviewed(draft, &latest, 1U));
        OK(umi_record_draft_state(draft, &state)); CHECK(!state.dirty && state.expected_revision == 2U);
        CHECK(umi_record_draft_prepare_save(draft, &author, &request) == UMI_STATUS_INVALID_STATE);
    } else return 2;
    char content[32]; size_t length = 0U;
    OK(umi_record_draft_copy_content(draft, content, sizeof content, &length));
    CHECK(length == (strcmp(name, "matches-latest") == 0 ? 8U : 5U));
    CHECK(memcmp(content, strcmp(name, "matches-latest") == 0 ? "external" : "local", length) == 0);
    umi_record_draft_destroy(draft);
    return 0;
}
int main(int argc, char **argv) { return argc == 2 ? exercise(argv[1]) : 2; }
