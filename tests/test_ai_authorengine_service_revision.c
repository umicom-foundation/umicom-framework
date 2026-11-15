/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_authorengine_service_revision.c
 *
 * PURPOSE:
 *   Verify visible partial health updates and refuse service or conversation revision exhaustion before mutation.
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
#include <stdint.h>
#include <string.h>
#include "umicom/ai/authorengine_service.h"
#include "ai/runtime_catalogue_fixture.h"

/* Fault injection changes only the value returned to the service. The real
 * archive remains available for independent before/after comparisons. */
static int override_record_revision;
static uint64_t injected_record_revision;
static UmiStatus read_record_for_service(const UmiAiConversationArchive *archive,
    const char *id, UmiAiConversationRecord *out)
{
    UmiStatus status = umi_ai_conversation_archive_find(archive, id, out);
    if (status == UMI_STATUS_OK && override_record_revision)
        out->revision = injected_record_revision;
    return status;
}
#define umi_ai_conversation_archive_find read_record_for_service
/* Include the implementation to reach the otherwise impractical counter limit.
 * Ordinary setup and all observations still use public contracts. */
#include "../src/ai/authorengine_service.c"
#undef umi_ai_conversation_archive_find

typedef struct RevisionFixture {
    UmiAiAuthorEngineService *service;
    const char *mode;
    unsigned calls;
} RevisionFixture;

static UmiStatus health_probe(void *context, const UmiAiRuntimeDescriptor *descriptor,
    UmiAiHealth *health)
{
    RevisionFixture *fixture = context;
    assert(descriptor != NULL);
    ++fixture->calls;
    if (strcmp(fixture->mode, "first-failure") == 0 ||
        (strcmp(fixture->mode, "partial") == 0 && fixture->calls == 2U))
        return UMI_STATUS_IO_ERROR;
    if (strcmp(fixture->mode, "health-callback-max") == 0) {
        UmiAiPolicy policy = fixture->service->provider_policy;
        UmiAiPrivacyPolicy privacy = fixture->service->privacy_policy;
        assert(umi_ai_authorengine_service_set_policy(fixture->service, &policy,
            &privacy, 16384U, 1024U) == UMI_STATUS_OK);
    }
    health->available = 1;
    strcpy(health->message, "Revision fixture observation");
    return UMI_STATUS_OK;
}

static UmiStatus legacy_health_probe(void *context, UmiAiHealth *health)
{
    RevisionFixture *fixture = context;
    ++fixture->calls;
    health->available = 1;
    if (fixture->calls == 2U) strcpy(health->provider_id, "provider.other");
    return UMI_STATUS_OK;
}

static UmiStatus no_generation(void *context, const UmiAiRequest *request, UmiAiResponse *response)
{
    (void)context; (void)request; (void)response;
    assert(!"revision checks must not invoke generation");
    return UMI_STATUS_NOT_IMPLEMENTED;
}

static UmiAiAuthorEngineServiceSnapshot snapshot(UmiAiAuthorEngineService *service)
{
    UmiAiAuthorEngineServiceSnapshot result;
    assert(umi_ai_authorengine_service_snapshot(service, &result) == UMI_STATUS_OK);
    return result;
}

int main(int argc, char **argv)
{
    static const char *const modes[] = {
        "partial", "complete", "empty", "first-failure", "partial-legacy",
        "health-max", "health-partial-max", "health-callback-max", "policy-max",
        "runtime-max", "session-max", "message-max", "load-max", "zero-revision",
        "message-record-max", "message-record-zero"
    };
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *service = NULL;
    UmiAiAuthorEngineServiceSnapshot before, after;
    UmiAiRuntimeDescriptor first = catalogue_fixture("runtime.first");
    UmiAiRuntimeDescriptor second = catalogue_fixture("runtime.second");
    UmiAiRuntimeDescriptor first_before, first_after, second_before, second_after;
    UmiAiConversationRecord *record_before = calloc(1U, sizeof *record_before);
    UmiAiConversationRecord *record_after = calloc(1U, sizeof *record_after);
    UmiAiMessage message;
    UmiAiProvider provider = {0};
    RevisionFixture fixture = {0};
    UmiStatus status = UMI_STATUS_INVALID_STATE;
    size_t count = SIZE_MAX, index;
    int known = 0, empty, message_case, health_case;
    assert(argc == 2 && record_before != NULL && record_after != NULL);
    for (index = 0U; index < sizeof modes / sizeof modes[0]; ++index)
        if (strcmp(argv[1], modes[index]) == 0) known = 1;
    assert(known);
    fixture.mode = argv[1];
    empty = strcmp(fixture.mode, "empty") == 0;
    message_case = strncmp(fixture.mode, "message-", 8U) == 0;
    health_case = strcmp(fixture.mode, "partial") == 0 ||
        strcmp(fixture.mode, "complete") == 0 || empty ||
        strcmp(fixture.mode, "first-failure") == 0 ||
        strcmp(fixture.mode, "partial-legacy") == 0 ||
        strncmp(fixture.mode, "health-", 7U) == 0 ||
        strcmp(fixture.mode, "zero-revision") == 0;
    umi_ai_runtime_init(&runtime);
    strcpy(config.executable, "uaengine"); strcpy(config.workspace, ".");
    strcpy(config.provider, "provider.local");
    assert(umi_ai_authorengine_service_create(&runtime, &config, &service) == UMI_STATUS_OK);
    fixture.service = service;
    if (!empty) {
        assert(umi_ai_authorengine_service_register_runtime(service, &first) == UMI_STATUS_OK);
        assert(umi_ai_authorengine_service_register_runtime(service, &second) == UMI_STATUS_OK);
        assert(umi_ai_runtime_catalogue_find(service->catalogue, first.runtime_id, &first_before) == UMI_STATUS_OK);
        assert(umi_ai_runtime_catalogue_find(service->catalogue, second.runtime_id, &second_before) == UMI_STATUS_OK);
    }
    if (message_case) {
        assert(umi_ai_authorengine_service_begin_session(service, "session.fixture",
            first.provider_id, first.model_id, "workspace.fixture", "Revision fixture", 1U) == UMI_STATUS_OK);
        assert(umi_ai_conversation_archive_find(service->conversations, "session.fixture", record_before) == UMI_STATUS_OK);
        assert(umi_ai_message_set(&message, UMI_AI_ROLE_USER, "user", "Keep existing conversation") == UMI_STATUS_OK);
    }
    if (strcmp(fixture.mode, "partial-legacy") == 0) {
        provider.structure_size = sizeof provider; provider.abi_version = 1U;
        provider.provider_id = "provider.local"; provider.kind = UMI_AI_PROVIDER_LOCAL;
        provider.instance = &fixture; provider.generate = no_generation; provider.health = legacy_health_probe;
        assert(umi_ai_provider_registry_add(&runtime.providers, &provider) == UMI_STATUS_OK);
    }
    if (strcmp(fixture.mode, "health-partial-max") == 0 ||
        strcmp(fixture.mode, "health-callback-max") == 0)
        service->revision = UINT64_MAX - 1U;
    else if (strcmp(fixture.mode, "zero-revision") == 0)
        service->revision = 0U;
    else if (strstr(fixture.mode, "-max") != NULL &&
             strcmp(fixture.mode, "message-record-max") != 0)
        service->revision = UINT64_MAX;
    if (strcmp(fixture.mode, "message-record-max") == 0 ||
        strcmp(fixture.mode, "message-record-zero") == 0) {
        override_record_revision = 1;
        injected_record_revision = strcmp(fixture.mode, "message-record-max") == 0 ? UINT64_MAX : 0U;
    }
    before = snapshot(service);
    if (health_case) {
        status = strcmp(fixture.mode, "partial-legacy") == 0
            ? umi_ai_authorengine_service_refresh_health(service, 99U, &count)
            : UmiAiAuthorEngineRefreshRuntimeHealth(service, 99U, health_probe, &fixture, &count);
    } else if (strcmp(fixture.mode, "policy-max") == 0) {
        UmiAiPolicy policy = runtime.policy;
        UmiAiPrivacyPolicy privacy = *umi_ai_authorengine_service_privacy_policy(service);
        policy.allow_remote = !policy.allow_remote;
        privacy.persist_sessions = !privacy.persist_sessions;
        status = umi_ai_authorengine_service_set_policy(service, &policy, &privacy, 16384U, 1024U);
        assert(runtime.policy.allow_remote == before.remote_allowed);
        assert(umi_ai_authorengine_service_privacy_policy(service)->persist_sessions == before.persistence_allowed);
    } else if (strcmp(fixture.mode, "runtime-max") == 0) {
        UmiAiRuntimeDescriptor added = catalogue_fixture("runtime.third");
        status = umi_ai_authorengine_service_register_runtime(service, &added);
    } else if (strcmp(fixture.mode, "session-max") == 0) {
        status = umi_ai_authorengine_service_begin_session(service, "session.new",
            first.provider_id, first.model_id, "workspace.fixture", "Must not be added", 99U);
    } else if (message_case) {
        status = umi_ai_authorengine_service_add_message(service, "session.fixture", &message, 99U);
        override_record_revision = 0;
        assert(umi_ai_conversation_archive_find(service->conversations, "session.fixture", record_after) == UMI_STATUS_OK);
        assert(memcmp(record_before, record_after, sizeof *record_before) == 0);
    } else if (strcmp(fixture.mode, "load-max") == 0) {
        /* The empty path would fail loading. Capacity refusal must precede I/O,
         * preserving the output and not adding an archive record. */
        memset(record_before, 0xA5, sizeof *record_before);
        memcpy(record_after, record_before, sizeof *record_before);
        status = umi_ai_authorengine_service_load_session(service, "", record_after);
        assert(memcmp(record_before, record_after, sizeof *record_before) == 0);
    }
    after = snapshot(service);
    if (strcmp(fixture.mode, "partial") == 0 || strcmp(fixture.mode, "partial-legacy") == 0) {
        assert(status == (strcmp(fixture.mode, "partial") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_INVALID_ARGUMENT));
        assert(count == SIZE_MAX && fixture.calls == 2U);
        assert(after.revision == before.revision + 1U && after.healthy_runtimes == 1U);
    } else if (strcmp(fixture.mode, "complete") == 0) {
        assert(status == UMI_STATUS_OK && count == 2U && fixture.calls == 2U);
        assert(after.revision == before.revision + 2U && after.healthy_runtimes == 2U);
    } else if (empty) {
        assert(status == UMI_STATUS_OK && count == 0U && fixture.calls == 0U);
        assert(after.revision == before.revision + 1U && after.runtimes == 0U);
    } else if (strcmp(fixture.mode, "health-partial-max") == 0) {
        assert(status == UMI_STATUS_CAPACITY_EXCEEDED && count == SIZE_MAX && fixture.calls == 1U);
        assert(after.revision == UINT64_MAX && after.healthy_runtimes == 1U);
    } else if (strcmp(fixture.mode, "health-callback-max") == 0) {
        assert(status == UMI_STATUS_CAPACITY_EXCEEDED && count == SIZE_MAX && fixture.calls == 1U);
        assert(after.revision == UINT64_MAX && after.healthy_runtimes == 0U && after.context_limit == 16384U);
    } else {
        UmiStatus expected = strcmp(fixture.mode, "first-failure") == 0 ? UMI_STATUS_IO_ERROR :
            strcmp(fixture.mode, "zero-revision") == 0 || strcmp(fixture.mode, "message-record-zero") == 0
                ? UMI_STATUS_INVALID_STATE : UMI_STATUS_CAPACITY_EXCEEDED;
        assert(status == expected && count == SIZE_MAX);
        assert(memcmp(&before, &after, sizeof before) == 0);
        assert(fixture.calls == (strcmp(fixture.mode, "first-failure") == 0 ? 1U : 0U));
    }
    if (!empty) {
        int published_first = strcmp(fixture.mode, "partial") == 0 ||
            strcmp(fixture.mode, "partial-legacy") == 0 || strcmp(fixture.mode, "complete") == 0 ||
            strcmp(fixture.mode, "health-partial-max") == 0;
        assert(umi_ai_runtime_catalogue_find(service->catalogue, first.runtime_id, &first_after) == UMI_STATUS_OK);
        assert(umi_ai_runtime_catalogue_find(service->catalogue, second.runtime_id, &second_after) == UMI_STATUS_OK);
        if (published_first)
            assert(first_after.health.available && first_after.health.checked_at_ns == 99U &&
                first_after.revision > first_before.revision);
        else assert(memcmp(&first_before, &first_after, sizeof first_before) == 0);
        if (strcmp(fixture.mode, "complete") == 0)
            assert(second_after.health.available && second_after.health.checked_at_ns == 99U &&
                second_after.revision > second_before.revision);
        else assert(memcmp(&second_before, &second_after, sizeof second_before) == 0);
    }
    assert(!service->health_refresh_active);
    umi_ai_authorengine_service_destroy(service);
    umi_ai_runtime_destroy(&runtime);
    free(record_before); free(record_after);
    return 0;
}
