/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_conversation_import.c
 *
 * PURPOSE:
 *   Verify conditional saved-session adoption, separate revision identities and unchanged state after bounded file or conflict refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "ai/conversation_archive_fixture.h"
#include "umicom/platform/filesystem.h"
/* Reach the service counter only for exhaustion fixtures. The archive and file
 * paths are exercised through their real public APIs. */
#include "../src/ai/authorengine_service.c"

static UmiAiAuthorEngineServiceSnapshot service_snapshot(UmiAiAuthorEngineService *service)
{
    UmiAiAuthorEngineServiceSnapshot snapshot;
    assert(umi_ai_authorengine_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
    return snapshot;
}

int main(int argc, char **argv)
{
    static const char *const modes[] = {
        "insert", "replace", "conflict", "stale", "missing-replace", "invalid-record",
        "null-record", "null-report", "null-service", "source-zero", "source-max",
        "policy", "capacity", "last-service", "service-max", "service-zero",
        "read-invalid-path", "read-zero-budget", "read-large-budget",
        "file-insert", "file-replace", "file-conflict", "file-malformed", "file-limit"
    };
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *service = NULL;
    UmiAiConversationRecord *current = archive_record_fixture("session.current");
    UmiAiConversationRecord *candidate = archive_record_fixture("session.loaded");
    UmiAiConversationRecord *candidate_before = malloc(sizeof *candidate_before);
    UmiAiConversationRecord *stored = malloc(sizeof *stored), *records_before;
    UmiAiConversationImportReport report, report_before, initial;
    UmiAiAuthorEngineServiceSnapshot before, after;
    UmiSnapshotCapture archive_before, archive_after;
    UmiAiPolicy policy_before;
    UmiAiPrivacyPolicy privacy_before;
    UmiStatus status;
    uint64_t expected_revision = 0U;
    int known = 0, file_case, succeeded;
    assert(argc == 3 && candidate_before != NULL && stored != NULL);
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(argv[1], modes[i]) == 0) known = 1;
    assert(known);
#define CASE(name) (strcmp(argv[1], name) == 0)
    file_case = strncmp(argv[1], "file-", 5U) == 0;
    succeeded = CASE("insert") || CASE("replace") || CASE("source-zero") ||
        CASE("source-max") || CASE("policy") || CASE("last-service") ||
        CASE("file-insert") || CASE("file-replace");
    umi_ai_runtime_init(&runtime);
    strcpy(config.executable, "uaengine"); strcpy(config.workspace, ".");
    strcpy(config.provider, "provider.local");
    assert(umi_ai_authorengine_service_create(&runtime, &config, &service) == UMI_STATUS_OK);
    /* No matching provider/model is registered: importing metadata must not load
     * a model or require an available endpoint. */
    assert(UmiAiAuthorEngineImportSession(service, current, 0U, &initial) == UMI_STATUS_OK);
    assert(umi_ai_conversation_archive_find(service->conversations, "session.current", current) == UMI_STATUS_OK);
    candidate->revision = 1234U;
    strcpy(candidate->title, "Recovered conversation");
    if (CASE("replace") || CASE("stale") || CASE("conflict") ||
        CASE("file-replace") || CASE("file-conflict")) {
        strcpy(candidate->session.session_id, "session.current");
        strcpy(candidate->conversation.conversation_id, "session.current");
        if (CASE("replace") || CASE("file-replace")) expected_revision = current->revision;
        if (CASE("stale")) expected_revision = current->revision + 1U;
    }
    if (CASE("missing-replace")) expected_revision = current->revision;
    if (CASE("invalid-record")) memset(candidate->title, 'x', sizeof candidate->title);
    if (CASE("source-zero")) candidate->revision = 0U;
    if (CASE("source-max")) candidate->revision = UINT64_MAX;
    if (CASE("policy")) {
        candidate->remote_approved = 1; candidate->persistence_approved = 1;
        candidate->classification = UMI_AI_DATA_CONFIDENTIAL;
    }
    if (CASE("capacity")) {
        UmiAiConversationRecord *extra = archive_record_fixture("session.fill");
        for (size_t i = 1U; i < UMI_AI_CONVERSATION_ARCHIVE_CAPACITY; ++i) {
            (void)snprintf(extra->session.session_id, sizeof extra->session.session_id, "session.fill.%zu", i);
            assert(umi_ai_conversation_archive_upsert(service->conversations, extra) == UMI_STATUS_OK);
        }
        free(extra);
    }
    if (CASE("last-service")) service->revision = UINT64_MAX - 1U;
    if (CASE("service-max")) service->revision = UINT64_MAX;
    if (CASE("service-zero")) service->revision = 0U;
    before = service_snapshot(service);
    assert(UmiAiConversationArchiveCapture(service->conversations, &archive_before) == UMI_STATUS_OK);
    records_before = malloc(archive_before.count * sizeof(*records_before));
    assert(records_before != NULL);
    for (size_t i = 0U; i < archive_before.count; ++i)
        assert(umi_ai_conversation_archive_at(service->conversations, i, &records_before[i]) == UMI_STATUS_OK);
    memcpy(candidate_before, candidate, sizeof *candidate);
    memcpy(&policy_before, &runtime.policy, sizeof policy_before);
    memcpy(&privacy_before, &service->privacy_policy, sizeof privacy_before);
    memset(&report, 0xA5, sizeof report); memcpy(&report_before, &report, sizeof report);
    if (file_case) {
        if (CASE("file-malformed"))
            assert(umi_fs_write_text(argv[2], "format=umicom-ai-session-v1\nsession.id=partial\n") == UMI_STATUS_OK);
        else
            assert(umi_ai_conversation_archive_save_record(candidate, &privacy_before, argv[2]) == UMI_STATUS_OK);
        status = UmiAiAuthorEngineLoadSessionBounded(service, argv[2],
            CASE("file-limit") ? 1U : UMI_AI_CONVERSATION_FILE_MAX_BYTES, expected_revision, &report);
    } else if (CASE("read-invalid-path") || CASE("read-zero-budget") || CASE("read-large-budget")) {
        size_t limit = CASE("read-zero-budget") ? 0U :
            CASE("read-large-budget") ? (size_t)UMI_AI_CONVERSATION_FILE_MAX_BYTES + 1U :
                UMI_AI_CONVERSATION_FILE_MAX_BYTES;
        status = UmiAiAuthorEngineLoadSessionBounded(service,
            CASE("read-invalid-path") ? "relative.session" : argv[2], limit, 0U, &report);
    } else {
        status = UmiAiAuthorEngineImportSession(CASE("null-service") ? NULL : service,
            CASE("null-record") ? NULL : candidate, expected_revision, CASE("null-report") ? NULL : &report);
    }
    assert(memcmp(candidate_before, candidate, sizeof *candidate) == 0);
    assert(memcmp(&policy_before, &runtime.policy, sizeof policy_before) == 0);
    assert(memcmp(&privacy_before, &service->privacy_policy, sizeof privacy_before) == 0);
    after = service_snapshot(service);
    assert(UmiAiConversationArchiveCapture(service->conversations, &archive_after) == UMI_STATUS_OK);
    if (succeeded) {
        assert(status == UMI_STATUS_OK);
        assert(report.source_record_revision == candidate->revision);
        assert(report.stored_record_revision == archive_before.revision + 1U);
        assert(report.service_revision == before.revision + 1U);
        assert(report.replaced == (expected_revision != 0U));
        assert(strcmp(report.session_id, candidate->session.session_id) == 0);
        assert(strcmp(after.active_session_id, report.session_id) == 0 &&
            after.revision == report.service_revision);
        assert(archive_after.revision == report.stored_record_revision &&
            archive_after.count == archive_before.count + (expected_revision == 0U ? 1U : 0U));
        assert(umi_ai_conversation_archive_find(service->conversations, report.session_id, stored) == UMI_STATUS_OK);
        assert(stored->revision == report.stored_record_revision &&
            strcmp(stored->title, "Recovered conversation") == 0);
        assert(stored->conversation.message_count == candidate->conversation.message_count &&
            strcmp(stored->conversation.messages[0].text, candidate->conversation.messages[0].text) == 0);
        if (CASE("policy")) assert(stored->classification == UMI_AI_DATA_CONFIDENTIAL);
        if (CASE("last-service")) {
            assert(report.service_revision == UINT64_MAX);
            memcpy(&report_before, &report, sizeof report);
            assert(UmiAiAuthorEngineImportSession(service, candidate, stored->revision, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
            assert(memcmp(&report_before, &report, sizeof report) == 0);
            assert(service_snapshot(service).revision == UINT64_MAX);
        }
    } else {
        UmiStatus expected = CASE("conflict") || CASE("stale") || CASE("missing-replace") ||
            CASE("file-conflict") ? UMI_STATUS_BUSY :
            CASE("capacity") || CASE("service-max") || CASE("file-limit") ? UMI_STATUS_CAPACITY_EXCEEDED :
            CASE("service-zero") ? UMI_STATUS_INVALID_STATE :
            CASE("file-malformed") ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_INVALID_ARGUMENT;
        assert(status == expected);
        assert(memcmp(&report_before, &report, sizeof report) == 0);
        assert(memcmp(&before, &after, sizeof before) == 0);
        assert(archive_after.count == archive_before.count && archive_after.revision == archive_before.revision);
        for (size_t i = 0U; i < archive_before.count; ++i) {
            assert(umi_ai_conversation_archive_at(service->conversations, i, stored) == UMI_STATUS_OK);
            assert(memcmp(stored, &records_before[i], sizeof *stored) == 0);
        }
        if (CASE("service-max")) {
            assert(UmiAiAuthorEngineLoadSessionBounded(service, argv[2],
                UMI_AI_CONVERSATION_FILE_MAX_BYTES, 0U, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
            assert(memcmp(&report_before, &report, sizeof report) == 0);
        }
    }
    if (file_case) assert(umi_fs_remove_tree(argv[2]) == UMI_STATUS_OK);
    free(records_before); free(current); free(candidate); free(candidate_before); free(stored);
    umi_ai_authorengine_service_destroy(service);
    umi_ai_runtime_destroy(&runtime);
    return 0;
#undef CASE
}
