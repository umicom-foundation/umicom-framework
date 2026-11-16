/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/local_models/sdk_consumer/main.c
 *
 * PURPOSE:
 *   Exercise the installed model catalogue and provider linkage without network or model execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "provider_ownership.h"
#include "umicom/local_models/manager.h"
#include "umicom/local_models/bundle.h"
#include "umicom/local_models/runtime.h"
#include "umicom/ai/authorengine_service.h"
#include "umicom/ai/coding_context.h"
#include "umicom/ai/coding_patch.h"
#include "umicom/ai/coding_assistant.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Exercise the installed descriptor-aware boundary with a deliberately offline
 * observation. The callback checks routing; it does not assert endpoint readiness. */
static UmiStatus sdk_runtime_probe(
    void *context, const UmiAiRuntimeDescriptor *observed, UmiAiHealth *health)
{
    const UmiAiRuntimeDescriptor *expected = context;
    if (strcmp(observed->runtime_id, expected->runtime_id) != 0 ||
        strcmp(observed->model_id, expected->model_id) != 0 ||
        strcmp(observed->endpoint, expected->endpoint) != 0)
        return UMI_STATUS_INVALID_STATE;
    health->available = 0;
    strcpy(health->message, "Offline SDK consumer; no endpoint probed");
    return UMI_STATUS_OK;
}

static UmiStatus sdk_runtime_health(UmiAiRuntimeDescriptor *description)
{
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *service = NULL;
    UmiStatus status;
    size_t healthy = SIZE_MAX;
    umi_ai_runtime_init(&runtime);
    strcpy(config.executable, "uaengine");
    strcpy(config.workspace, ".");
    strcpy(config.provider, "umicom.local-chat");
    status = umi_ai_authorengine_service_create(&runtime, &config, &service);
    if (status == UMI_STATUS_OK)
        status = umi_ai_authorengine_service_register_runtime(service, description);
    if (status == UMI_STATUS_OK)
        status = UmiAiAuthorEngineRefreshRuntimeHealth(
            service, 1U, sdk_runtime_probe, description, &healthy);
    if (status == UMI_STATUS_OK && healthy != 0U) status = UMI_STATUS_INVALID_STATE;
    umi_ai_authorengine_service_destroy(service);
    umi_ai_runtime_destroy(&runtime);
    return status;
}

/* An installed consumer can own conversation metadata and reject delayed
 * edits without a cloned Framework tree or any inference/persistence I/O. */
static UmiStatus sdk_conversation_archive(void)
{
    UmiAiConversationArchive *archive = NULL;
    UmiAiConversationRecord *record = calloc(1U, sizeof *record);
    UmiSnapshotCapture initial, current;
    UmiStatus status;
    uint64_t read_revision = 0U;
    if (record == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_ai_conversation_record_init(record, "session.sdk", "umicom.local-chat",
        "sdk-model", "workspace.sdk", "SDK conversation", 1U);
    if (status == UMI_STATUS_OK) {
        static const char serialized[] =
            "format=umicom-ai-session-v1\nsession.id=session.sdk\n"
            "session.provider=umicom.local-chat\nsession.model=sdk-model\n"
            "session.created=1\nrecord.workspace=workspace.sdk\nrecord.title=SDK conversation\n"
            "record.classification=2\nrecord.remote-approved=0\nrecord.persistence-approved=0\n"
            "record.updated=1\nrecord.revision=1\nmessage.count=0\n";
        status = UmiAiConversationRecordParse(serialized, sizeof serialized - 1U, record);
        /* Invalid path admission resolves the bounded-load symbol without I/O.
         * Actual file reading is qualified by the separate archive file test. */
        if (status == UMI_STATUS_OK && UmiAiConversationArchiveLoadBounded(NULL,
                UMI_AI_CONVERSATION_FILE_MAX_BYTES, record) != UMI_STATUS_INVALID_ARGUMENT)
            status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) status = UmiAiConversationRecordValidate(record);
    if (status == UMI_STATUS_OK) status = umi_ai_conversation_archive_create(&archive);
    if (status == UMI_STATUS_OK) status = UmiAiConversationArchiveCapture(archive, &initial);
    if (status == UMI_STATUS_OK) status = UmiAiConversationArchiveUpsertIfRevision(archive, record, 0U);
    if (status == UMI_STATUS_OK)
        status = umi_ai_conversation_archive_find(archive, "session.sdk", record);
    if (status == UMI_STATUS_OK) {
        read_revision = record->revision;
        strcpy(record->title, "An accepted edit");
        status = UmiAiConversationArchiveUpsertIfRevision(archive, record, read_revision);
    }
    if (status == UMI_STATUS_OK &&
        UmiAiConversationArchiveUpsertIfRevision(archive, record, read_revision) != UMI_STATUS_BUSY)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK &&
        UmiAiConversationArchiveRemoveIfRevision(archive, "session.sdk", read_revision) != UMI_STATUS_BUSY)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiAiConversationArchiveCapture(archive, &current);
    if (status == UMI_STATUS_OK && (current.count != 1U || current.revision != initial.revision + 2U))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = umi_ai_conversation_archive_find(archive, "session.sdk", record);
    if (status == UMI_STATUS_OK)
        status = UmiAiConversationArchiveRemoveIfRevision(archive, "session.sdk", record->revision);
    if (status == UMI_STATUS_OK) status = UmiAiConversationArchiveCapture(archive, &current);
    if (status == UMI_STATUS_OK && (current.count != 0U || current.revision != initial.revision + 3U))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiAiConversationArchiveValidate(archive);
    umi_ai_conversation_archive_destroy(archive);
    free(record);
    return status;
}

/* Import metadata through the installed service, keeping historical file
 * identity separate from the revision assigned by the live archive. */
static UmiStatus sdk_session_import(void)
{
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *service = NULL;
    UmiAiConversationRecord *record = calloc(1U, sizeof *record);
    UmiAiConversationImportReport imported, unchanged;
    UmiStatus status;
    if (record == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_ai_runtime_init(&runtime);
    strcpy(config.executable, "uaengine");
    strcpy(config.workspace, ".");
    strcpy(config.provider, "umicom.local-chat");
    status = umi_ai_authorengine_service_create(&runtime, &config, &service);
    if (status == UMI_STATUS_OK)
        status = umi_ai_conversation_record_init(record, "session.import",
            "umicom.local-chat", "sdk-model", "workspace.sdk", "Imported record", 1U);
    if (status == UMI_STATUS_OK) {
        record->revision = 700U; /* Historical file metadata is not a live token. */
        status = UmiAiAuthorEngineImportSession(service, record, 0U, &imported);
    }
    if (status == UMI_STATUS_OK &&
        (imported.source_record_revision != 700U || imported.stored_record_revision == 700U ||
         imported.replaced || strcmp(imported.session_id, "session.import") != 0))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        memcpy(&unchanged, &imported, sizeof imported);
        if (UmiAiAuthorEngineImportSession(service, record, 0U, &imported) != UMI_STATUS_BUSY ||
            memcmp(&unchanged, &imported, sizeof imported) != 0)
            status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK)
        status = UmiAiAuthorEngineImportSession(service, record,
            imported.stored_record_revision, &imported);
    if (status == UMI_STATUS_OK && (!imported.replaced ||
        imported.stored_record_revision != unchanged.stored_record_revision + 1U ||
        imported.service_revision != unchanged.service_revision + 1U))
        status = UMI_STATUS_INVALID_STATE;
    /* Resolve bounded service admission without opening any file. */
    if (status == UMI_STATUS_OK && UmiAiAuthorEngineLoadSessionBounded(service,
        NULL, UMI_AI_CONVERSATION_FILE_MAX_BYTES, 0U, &imported) != UMI_STATUS_INVALID_ARGUMENT)
        status = UMI_STATUS_INVALID_STATE;
    free(record);
    umi_ai_authorengine_service_destroy(service);
    umi_ai_runtime_destroy(&runtime);
    return status;
}

/* Installed applications can prepare exact argv and retain the prior complete
 * plan after refusal. This exercise never invokes AuthorEngine or a shell. */
static UmiStatus sdk_invocation_plan(void)
{
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineInvocation invocation, before;
    UmiStatus status;
    strcpy(config.executable, "uaengine");
    strcpy(config.workspace, "example workspace");
    status = umi_ai_authorengine_plan_publication_invocation(
        &config, UMI_AI_AUTHOR_ENGINE_BUILD, NULL, &invocation);
    if (status != UMI_STATUS_OK) return status;
    memcpy(&before, &invocation, sizeof before);
    if (umi_ai_authorengine_plan_publication_invocation(&config,
        UMI_AI_AUTHOR_ENGINE_EXPORT, "unexpected site", &invocation) != UMI_STATUS_INVALID_ARGUMENT ||
        memcmp(&before, &invocation, sizeof before) != 0)
        return UMI_STATUS_INVALID_STATE;
    strcpy(invocation.arguments[4], "example output/site");
    status = umi_ai_authorengine_plan_publication_invocation(&config,
        UMI_AI_AUTHOR_ENGINE_SERVE_PREVIEW, invocation.arguments[4], &invocation);
    if (status != UMI_STATUS_OK) return status;
    if (invocation.argument_count != 3U ||
        strcmp(invocation.arguments[2], "example output/site") != 0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* Context planning copies provenance and estimates only. A failed mandatory
 * source decision keeps the last complete plan and cannot authorize a request. */
static UmiStatus sdk_context_plan(void)
{
    UmiAiContextBroker *broker = NULL;
    UmiAiContextSource source = {0};
    UmiAiContextPlan *plan = malloc(sizeof *plan), *prior = malloc(sizeof *prior);
    UmiAiPrivacyPolicy policy = umi_ai_privacy_policy_default();
    UmiSnapshotCapture capture;
    UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    if (plan == NULL || prior == NULL) goto cleanup;
    status = UmiAiPrivacyPolicyValidate(&policy);
    if (status == UMI_STATUS_OK) status = umi_ai_context_broker_create(&broker);
    strcpy(source.source_id, "context.public"); strcpy(source.label, "Public description");
    strcpy(source.uri, "workspace://sdk/example"); source.kind = UMI_AI_CONTEXT_DOCUMENT;
    source.classification = UMI_AI_DATA_PUBLIC; source.estimated_tokens = 100U;
    source.enabled = 1; source.mandatory = 1;
    if (status == UMI_STATUS_OK) status = UmiAiContextSourceValidate(&source);
    if (status == UMI_STATUS_OK) status = umi_ai_context_broker_upsert(broker, &source);
    if (status == UMI_STATUS_OK) status = UmiAiContextBrokerCapture(broker, &capture);
    if (status == UMI_STATUS_OK)
        status = umi_ai_context_broker_plan(broker, 200U, 0U, 8U,
            UMI_AI_PROVIDER_REMOTE, &policy, 1, plan);
    if (status == UMI_STATUS_OK && (plan->source_count != 1U ||
        plan->selected_tokens != 100U || plan->revision != capture.revision))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        memcpy(prior, plan, sizeof *prior);
        source.classification = UMI_AI_DATA_RESTRICTED;
        status = umi_ai_context_broker_upsert(broker, &source);
    }
    if (status == UMI_STATUS_OK &&
        (umi_ai_context_broker_plan(broker, 200U, 0U, 8U, UMI_AI_PROVIDER_REMOTE,
            &policy, 1, plan) != UMI_STATUS_PERMISSION_DENIED ||
         memcmp(prior, plan, sizeof *plan) != 0))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiAiContextBrokerClearChecked(broker);
    if (status == UMI_STATUS_OK) status = UmiAiContextBrokerCapture(broker, &capture);
    if (status == UMI_STATUS_OK && capture.count != 0U) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK &&
        umi_ai_context_broker_plan(broker, 200U, 0U, 8U, (UmiAiProviderKind)0,
            &policy, 1, plan) != UMI_STATUS_INVALID_ARGUMENT)
        status = UMI_STATUS_INVALID_STATE;
cleanup:
    umi_ai_context_broker_destroy(broker);
    free(plan); free(prior);
    return status;
}

/* Installed consumers can invalidate an old preview without contacting the
 * model. The observation records metadata owners, not execution permission. */
static UmiStatus sdk_context_observation(const UmiAiRuntimeDescriptor *descriptor)
{
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *service = NULL;
    UmiAiContextPlan *plan = malloc(sizeof *plan);
    UmiAiContextPlanObservation observation;
    UmiAiPrivacyPolicy privacy = umi_ai_privacy_policy_default();
    UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    umi_ai_runtime_init(&runtime);
    if (plan == NULL) goto cleanup;
    strcpy(config.executable, "uaengine"); strcpy(config.workspace, ".");
    strcpy(config.provider, "umicom.local-chat");
    status = umi_ai_authorengine_service_create(&runtime, &config, &service);
    if (status == UMI_STATUS_OK)
        status = umi_ai_authorengine_service_register_runtime(service, descriptor);
    if (status == UMI_STATUS_OK)
        status = UmiAiAuthorEnginePlanContextObserved(service, descriptor->runtime_id, 1, plan, &observation);
    if (status == UMI_STATUS_OK)
        status = UmiAiAuthorEngineCheckContextObservation(service, &observation);
    if (status == UMI_STATUS_OK)
        status = umi_ai_authorengine_service_set_policy(service, &runtime.policy, &privacy, 4096U, 512U);
    if (status == UMI_STATUS_OK &&
        UmiAiAuthorEngineCheckContextObservation(service, &observation) != UMI_STATUS_BUSY)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = UmiAiAuthorEnginePlanContextObserved(service, observation.runtime_id, 1, plan, &observation);
    if (status == UMI_STATUS_OK)
        status = UmiAiAuthorEngineCheckContextObservation(service, &observation);
cleanup:
    umi_ai_authorengine_service_destroy(service); umi_ai_runtime_destroy(&runtime);
    free(plan);
    return status;
}

/* Repository context remains metadata-only in this installed exercise. */
static UmiStatus sdk_coding_context(void)
{
    UmiAiCodingContextIndex *owner = NULL;
    UmiAiCodingContextFile source = {0};
    UmiAiCodingContextPlan *plan = malloc(sizeof *plan);
    UmiAiPrivacyPolicy privacy = umi_ai_privacy_policy_default();
    UmiSnapshotCapture capture;
    UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    if (plan == NULL) goto cleanup;
    strcpy(source.path, "src/example.c"); strcpy(source.language_id, "c23");
    source.classification = UMI_AI_DATA_PUBLIC; source.estimated_tokens = 32U;
    source.priority = 1U; source.active = -1; source.enabled = 1;
    status = UmiAiCodingContextFileValidate(&source);
    if (status == UMI_STATUS_OK) status = umi_ai_coding_context_index_create(&owner);
    if (status == UMI_STATUS_OK) status = umi_ai_coding_context_upsert(owner, &source);
    if (status == UMI_STATUS_OK) status = UmiAiCodingContextCapture(owner, &capture);
    if (status == UMI_STATUS_OK)
        status = umi_ai_coding_context_plan(owner, 0U, 8U, UMI_AI_PROVIDER_LOCAL, &privacy, 1, plan);
    if (status == UMI_STATUS_OK && (plan->file_count != 0U || plan->dropped_count != 1U || !plan->truncated))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = umi_ai_coding_context_plan(owner, 32U, 8U, UMI_AI_PROVIDER_LOCAL, &privacy, 1, plan);
    if (status == UMI_STATUS_OK && (plan->file_count != 1U || plan->remaining_tokens != 0U || plan->revision != capture.revision))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiAiCodingContextClearChecked(owner);
    if (status == UMI_STATUS_OK) status = UmiAiCodingContextIndexValidate(owner);
cleanup:
    umi_ai_coding_context_index_destroy(owner); free(plan); return status;
}

/* Resolve installed patch admission APIs without touching a real file. */
static UmiStatus sdk_coding_patch(void)
{
    UmiAiCodingPatch *patch = malloc(sizeof *patch);
    UmiAiCodingPatchPolicy policy = umi_ai_coding_patch_policy_default();
    UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    if (patch == NULL) return status;
    status = UmiAiCodingPatchPolicyValidate(&policy);
    if (status == UMI_STATUS_OK)
        status = umi_ai_coding_patch_init(patch, "sdk.patch", "sdk.request",
            "Explain a reviewed change", "Offline SDK metadata exercise");
    if (status == UMI_STATUS_OK) status = UmiAiCodingPatchCheckIntegrity(patch);
    if (status == UMI_STATUS_OK)
        status = umi_ai_coding_patch_add_file(patch, "src/example.c",
            UMI_AI_CODING_PATCH_MODIFY, "before\n", "after\n");
    if (status == UMI_STATUS_OK) status = umi_ai_coding_patch_validate(patch, &policy);
    if (status == UMI_STATUS_OK) status = umi_ai_coding_patch_approve(patch, "sdk reviewer");
    if (status == UMI_STATUS_OK) status = UmiAiCodingPatchCheckIntegrity(patch);
    if (status == UMI_STATUS_OK) status = umi_ai_coding_patch_reject(patch);
    if (status == UMI_STATUS_OK && patch->state != UMI_AI_CODING_PATCH_REJECTED)
        status = UMI_STATUS_INVALID_STATE;
    free(patch); return status;
}


/* Plan a coding request through installed services without invoking a model. */
/* Exercise checked lifetime ownership from the installed SDK; the offline task has no active patch callbacks. The former implementation is retained for owner review. */
#if 0
static UmiStatus sdk_coding_task(const UmiAiRuntimeDescriptor *descriptor)
{
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *engine = NULL;
    UmiAiCodingAssistantService *assistant = NULL;
    UmiAiCodingTaskPlan *plan = malloc(sizeof *plan);
    UmiAiCodingRequest request;
    UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    if (plan == NULL) return status;
    umi_ai_runtime_init(&runtime);
    strcpy(config.executable, "uaengine"); strcpy(config.workspace, ".");
    strcpy(config.provider, "sdk.local");
    status = umi_ai_authorengine_service_create(&runtime, &config, &engine);
    if (status == UMI_STATUS_OK) status = umi_ai_authorengine_service_register_runtime(engine, descriptor);
    if (status == UMI_STATUS_OK) status = umi_ai_coding_assistant_create(engine, &assistant);
    umi_ai_coding_request_init(&request, UMI_AI_CODING_TASK_EXPLAIN);
    strcpy(request.request_id, "sdk.request"); strcpy(request.session_id, "sdk.session");
    memcpy(request.runtime_id, descriptor->runtime_id, sizeof request.runtime_id);
    strcpy(request.workspace_root, "."); strcpy(request.instruction, "Explain the offline SDK example");
    if (status == UMI_STATUS_OK)
        status = umi_ai_coding_assistant_prepare(assistant, &request, descriptor->provider_kind, plan);
    if (status == UMI_STATUS_OK && (!plan->ready || plan->revision == 0U)) status = UMI_STATUS_INVALID_STATE;
    umi_ai_coding_assistant_destroy(assistant);
    umi_ai_authorengine_service_destroy(engine); free(plan); return status;
}

#endif
static UmiStatus sdk_coding_task(const UmiAiRuntimeDescriptor *descriptor)
{
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *engine = NULL;
    UmiAiCodingAssistantService *assistant = NULL;
    UmiAiCodingTaskPlan *plan = malloc(sizeof *plan);
    UmiAiCodingRequest request;
    UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    if (plan == NULL) return status;
    umi_ai_runtime_init(&runtime);
    strcpy(config.executable, "uaengine"); strcpy(config.workspace, ".");
    strcpy(config.provider, "sdk.local");
    status = umi_ai_authorengine_service_create(&runtime, &config, &engine);
    if (status == UMI_STATUS_OK) status = umi_ai_authorengine_service_register_runtime(engine, descriptor);
    if (status == UMI_STATUS_OK) status = umi_ai_coding_assistant_create(engine, &assistant);
    umi_ai_coding_request_init(&request, UMI_AI_CODING_TASK_EXPLAIN);
    strcpy(request.request_id, "sdk.request"); strcpy(request.session_id, "sdk.session");
    memcpy(request.runtime_id, descriptor->runtime_id, sizeof request.runtime_id);
    strcpy(request.workspace_root, "."); strcpy(request.instruction, "Explain the offline SDK example");
    if (status == UMI_STATUS_OK)
        status = umi_ai_coding_assistant_prepare(assistant, &request, descriptor->provider_kind, plan);
    if (status == UMI_STATUS_OK && (!plan->ready || plan->revision == 0U)) status = UMI_STATUS_INVALID_STATE;
    /* No file adapter is invoked in this offline exercise, so destruction
     * cannot be reentrant; resolve and check the installed lifetime API. */
    UmiStatus destruction = UmiAiCodingAssistantDestroyChecked(assistant);
    if (status == UMI_STATUS_OK) status = destruction;
    umi_ai_authorengine_service_destroy(engine); free(plan); return status;
}



/* Resolve bundle planning from the installed library using synthetic metadata.
 * A selected component set is not a downloaded or runnable model pipeline. */
static UmiStatus sdk_model_bundle(void)
{
    UmiModelBundlePlan *plan=malloc(sizeof(*plan));
    if(plan==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiModelBundleFile files[2]={0};
    strcpy(files[0].filename,"weights/model.safetensors");
    strcpy(files[1].filename,"config.json");
    for(size_t i=0U;i<2U;++i)
        strcpy(files[i].sha256,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    files[0].bytes=10U;files[0].format=UMI_MODEL_ARTIFACT_SAFETENSORS;
    files[1].bytes=2U;files[1].format=UMI_MODEL_ARTIFACT_JSON;
    UmiStatus status=UmiModelBundlePlanCreate("umicom-fixtures/synthetic",
        "0123456789abcdef0123456789abcdef01234567",files,2U,plan);
    if(status==UMI_STATUS_OK) status=UmiModelBundlePlanValidate(plan);
    if(status==UMI_STATUS_OK && (plan->count!=2U || plan->total_bytes!=12U))
        status=UMI_STATUS_INVALID_STATE;
    char url[UMI_MODEL_URL_CAPACITY];
    if(status==UMI_STATUS_OK) status=UmiModelBundleUrlAt(plan,0U,url,sizeof(url));
    UmiModelSource source;
    if(status==UMI_STATUS_OK &&
        UmiModelBundleGgufSourceAt(plan,0U,&source)!=UMI_STATUS_NOT_IMPLEMENTED)
        status=UMI_STATUS_INVALID_STATE;
    free(plan);
    return status;
}

/* Resolve companion selection through the installed SDK using only synthetic
 * metadata. Filename completeness does not assert tensor or backend validity. */
static UmiStatus sdk_gguf_selection(void)
{
    UmiModelBundlePlan *plan=malloc(sizeof(*plan));
    if(plan==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiModelBundleFile files[3]={0};
    strcpy(files[0].filename,"weights/model-00002-of-00002.gguf");
    strcpy(files[1].filename,"config.json");
    strcpy(files[2].filename,"weights/model-00001-of-00002.gguf");
    for(size_t i=0U;i<3U;++i)
        strcpy(files[i].sha256,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    files[0].bytes=24U;files[0].format=UMI_MODEL_ARTIFACT_GGUF;
    files[1].bytes=2U;files[1].format=UMI_MODEL_ARTIFACT_JSON;
    files[2].bytes=25U;files[2].format=UMI_MODEL_ARTIFACT_GGUF;
    UmiStatus status=UmiModelBundlePlanCreate("umicom-fixtures/synthetic",
        "0123456789abcdef0123456789abcdef01234567",files,3U,plan);
    if(status==UMI_STATUS_OK) status=UmiModelBundleSelectGguf(plan,0U,plan);
    if(status==UMI_STATUS_OK && (plan->count!=2U || plan->total_bytes!=49U ||
        strcmp(plan->files[0].filename,"weights/model-00001-of-00002.gguf")!=0))
        status=UMI_STATUS_INVALID_STATE;
    free(plan);
    return status;
}

int main(void)
{
    const UmiModelCatalogueEntry *entry = UmiModelCatalogueAt(0U);
    if (entry == NULL || UmiModelSourceValidate(&entry->source) != UMI_STATUS_OK) return 1;
    /* An invalid port exercises the exported factory without HTTP traffic.
     * An explicitly offline SDK reports UNAVAILABLE instead of a mock. */
    UmiAiProvider provider = {0};
    UmiStatus status = UmiModelLocalProviderCreate(0U, 100U, NULL, &provider);
    UmiStatus expected = UmiAiWorkspaceLocalProviderAvailable()
        ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_UNAVAILABLE;
    if (status != expected || provider.instance != NULL) return 1;
    /* Resolve both health symbols from the installed package while preserving
     * a completely offline consumer: consent refusal precedes transport, and
     * invalid endpoint admission is the same in HTTP-enabled/offline builds. */
    UmiAiLocalHealth observed = {503U, false};
    if (UmiModelHealthProbe(8080U, 1000U, false, NULL, &observed) != UMI_STATUS_PERMISSION_DENIED ||
        observed.http_status != 503U || observed.endpoint_reports_ready) return 1;
    if (UmiAiWorkspaceLocalHealthRead(0U, 1000U, NULL, &observed) != UMI_STATUS_INVALID_ARGUMENT ||
        observed.http_status != 503U || observed.endpoint_reports_ready) return 1;
    /* Import readiness symbols from the installed SDK without network I/O.
     * Denied consent must preserve the caller's previous report. */
    UmiModelReadinessPlan waitPlan;
    uint32_t nominal = 0U;
    if (UmiModelReadinessPlanCreate(8080U, 3U, 50U, 100U, &waitPlan) != UMI_STATUS_OK ||
        UmiModelReadinessBudget(&waitPlan, &nominal) != UMI_STATUS_OK || nominal != 400U) return 1;
    UmiModelReadinessReport report = {0};
    report.attempts_completed = 23U;
    unsigned char before[sizeof report]; memcpy(before, &report, sizeof report);
    if (UmiModelReadinessWait(&waitPlan, false, NULL, &report) != UMI_STATUS_PERMISSION_DENIED ||
        memcmp(before, &report, sizeof report) != 0) return 1;
    /* The installed package must export the model-to-runtime bridge and
     * conditional catalogue APIs without a clone or source-tree dependency. */
#ifdef _WIN32
    const char *server = "C:/umicom-example/llama-server.exe";
    const char *model = "C:/umicom-example/model.gguf";
#else
    const char *server = "/umicom-example/llama-server";
    const char *model = "/umicom-example/model.gguf";
#endif
    UmiModelRunPlan run;
    UmiAiRuntimeDescriptor description, selected;
    UmiAiRuntimeCatalogue *catalogue = NULL;
    if (UmiModelRunPlanCreate(server, model, entry->source.sha256, "sdk-model",
        18080U, 2048U, 4U, &run) != UMI_STATUS_OK) return 1;
    if (UmiModelRuntimeDescriptorCreate(&run, "runtime.sdk", "SDK model", 256U,
        &description) != UMI_STATUS_OK) return 1;
    if (description.health.available || description.revision != 0U) return 1;
    /* Resolve the installed checked-generation and callback-lifetime exports
     * before the existing catalogue exercises. The provider is entirely inert. */
    if (UmiModelSdkExerciseOwnership() != UMI_STATUS_OK) return 1;
    if (sdk_model_bundle() != UMI_STATUS_OK) return 1;
    if (sdk_gguf_selection() != UMI_STATUS_OK) return 1;
    if (sdk_runtime_health(&description) != UMI_STATUS_OK) return 1;
    if (sdk_context_observation(&description) != UMI_STATUS_OK) return 1;
    if (sdk_conversation_archive() != UMI_STATUS_OK) return 1;
    if (sdk_session_import() != UMI_STATUS_OK) return 1;
    if (sdk_invocation_plan() != UMI_STATUS_OK) return 1;
    if (sdk_context_plan() != UMI_STATUS_OK) return 1;
    if (sdk_coding_context() != UMI_STATUS_OK) return 1;
    if (sdk_coding_patch() != UMI_STATUS_OK) return 1;
    if (sdk_coding_task(&description) != UMI_STATUS_OK) return 1;
    if (umi_ai_runtime_catalogue_create(&catalogue) != UMI_STATUS_OK) return 1;
    status = UmiAiRuntimeCatalogueUpsertIfRevision(catalogue, &description, 0U);
    if (status == UMI_STATUS_OK)
        status = umi_ai_runtime_catalogue_select(catalogue, UMI_AI_RUNTIME_CHAT, 1U, 1, &selected);
    umi_ai_runtime_catalogue_destroy(catalogue);
    if (status != UMI_STATUS_NOT_FOUND) return 1;
    /* Resolve explicit console ownership through an inert catalogue command.
     * Empty compatibility cleanup is separately idempotent and owns no child. */
    UmiModelSession *console = NULL;
    char *arguments[] = {"catalogue"};
    if (UmiModelsCommandWithSession(1, arguments, &console) != 0 || console != NULL) return 1;
    if (UmiModelsCommandHasRetainedSession() || UmiModelsCommandRelease() != UMI_STATUS_OK) return 1;
    (void)printf("Described %s at %s; unavailable until separately observed.\n",
        description.model_id, description.endpoint);
    (void)printf("%s; native downloads %s; local chat %s\n", entry->id,
        UmiModelDownloadAvailable() ? "available" : "unavailable",
        UmiAiWorkspaceLocalProviderAvailable() ? "available" : "unavailable");
/* Report consumer output failure after releasing the local metadata owner. The former implementation is retained for owner review. */
#if 0
    return 0;
#endif
    return fflush(stdout) == 0 && !ferror(stdout) ? 0 : 1;
}
