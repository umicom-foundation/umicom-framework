/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/completion_provider.h
 *
 * PURPOSE:
 *   Define provider-neutral completion requests, provider capabilities and a
 *   registry that can host native, indexed, Language Server and AI providers.
 *
 * THREADING:
 *   Registry mutation is coordinated by the owning controller. Provider
 *   callbacks may perform asynchronous work outside the registry, but emitted
 *   candidates are copied synchronously by the supplied sink.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_COMPLETION_PROVIDER_H
#define UMICOM_EDITOR_COMPLETION_PROVIDER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/completion_candidate.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION 1U
#define UMI_EDITOR_COMPLETION_PROVIDER_LABEL_CAPACITY 256U
#define UMI_EDITOR_COMPLETION_LANGUAGE_ID_CAPACITY 64U
#define UMI_EDITOR_COMPLETION_TRIGGER_CHARACTERS_CAPACITY 64U
#define UMI_EDITOR_COMPLETION_DOCUMENT_ID_CAPACITY 128U
#define UMI_EDITOR_COMPLETION_URI_CAPACITY 1024U
#define UMI_EDITOR_COMPLETION_PREFIX_CAPACITY 512U
#define UMI_EDITOR_COMPLETION_LINE_CONTEXT_CAPACITY 2048U

typedef enum UmiEditorCompletionTriggerKind {
    UMI_EDITOR_COMPLETION_TRIGGER_INVOKED = 1,
    UMI_EDITOR_COMPLETION_TRIGGER_CHARACTER = 2,
    UMI_EDITOR_COMPLETION_TRIGGER_INCOMPLETE_REFRESH = 3,
    UMI_EDITOR_COMPLETION_TRIGGER_AUTOMATIC = 4
} UmiEditorCompletionTriggerKind;

typedef int (*UmiEditorCompletionCancellationProbe)(void *user_data);

typedef struct UmiEditorCompletionRequest {
    uint32_t struct_size;
    uint32_t api_version;
    char document_id[UMI_EDITOR_COMPLETION_DOCUMENT_ID_CAPACITY];
    char document_uri[UMI_EDITOR_COMPLETION_URI_CAPACITY];
    char language_id[UMI_EDITOR_COMPLETION_LANGUAGE_ID_CAPACITY];
    char prefix[UMI_EDITOR_COMPLETION_PREFIX_CAPACITY];
    char line_prefix[UMI_EDITOR_COMPLETION_LINE_CONTEXT_CAPACITY];
    char line_suffix[UMI_EDITOR_COMPLETION_LINE_CONTEXT_CAPACITY];
    char trigger_character[8];
    UmiEditorCompletionTriggerKind trigger_kind;
    size_t maximum_candidates;
    uint64_t cursor_offset;
    uint64_t line;
    uint64_t column;
    uint64_t document_revision;
    uint64_t request_id;
    int allow_snippets;
    int allow_deprecated;
    int allow_ai;
    int allow_remote;
    int trusted_workspace;
    UmiEditorCompletionCancellationProbe cancellation_probe;
    void *cancellation_user_data;
} UmiEditorCompletionRequest;

typedef struct UmiEditorCompletionProviderDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_COMPLETION_PROVIDER_ID_CAPACITY];
    char label[UMI_EDITOR_COMPLETION_PROVIDER_LABEL_CAPACITY];
    char language_id[UMI_EDITOR_COMPLETION_LANGUAGE_ID_CAPACITY];
    char trigger_characters[UMI_EDITOR_COMPLETION_TRIGGER_CHARACTERS_CAPACITY];
    int32_t priority;
    UmiEditorCompletionSource source;
    int supports_manual_invocation;
    int supports_trigger_characters;
    int supports_automatic_invocation;
    int supports_incomplete_refresh;
    int supports_resolution;
    int supports_commit_characters;
    int supports_snippets;
    int requires_network;
    int requires_workspace_trust;
    int enabled;
} UmiEditorCompletionProviderDescriptor;

typedef struct UmiEditorCompletionProviderResponse {
    uint32_t struct_size;
    uint32_t api_version;
    size_t emitted_candidate_count;
    uint64_t provider_revision;
    int incomplete;
    int cacheable;
} UmiEditorCompletionProviderResponse;

typedef UmiStatus (*UmiEditorCompletionCandidateSink)(
    const UmiEditorCompletionCandidate *candidate,
    void *user_data);
typedef UmiStatus (*UmiEditorCompletionProvideFunction)(
    const UmiEditorCompletionRequest *request,
    UmiEditorCompletionCandidateSink sink,
    void *sink_user_data,
    UmiEditorCompletionProviderResponse *out_response,
    void *provider_user_data);
typedef UmiStatus (*UmiEditorCompletionResolveFunction)(
    const UmiEditorCompletionRequest *request,
    UmiEditorCompletionCandidate *in_out_candidate,
    void *provider_user_data);
typedef UmiStatus (*UmiEditorCompletionCancelFunction)(
    uint64_t request_id,
    void *provider_user_data);

typedef struct UmiEditorCompletionProviderCallbacks {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorCompletionProvideFunction provide;
    UmiEditorCompletionResolveFunction resolve;
    UmiEditorCompletionCancelFunction cancel;
} UmiEditorCompletionProviderCallbacks;

typedef struct UmiEditorCompletionProviderRegistrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t provider_count;
    size_t enabled_provider_count;
    size_t ai_provider_count;
    size_t remote_provider_count;
    size_t resolving_provider_count;
    size_t language_count;
    uint64_t revision;
} UmiEditorCompletionProviderRegistrySnapshot;

typedef struct UmiEditorCompletionProviderRegistry
    UmiEditorCompletionProviderRegistry;

UmiEditorCompletionRequest umi_editor_completion_request_default(
    const char *document_id,
    const char *language_id,
    uint64_t request_id);
UmiStatus umi_editor_completion_request_validate(
    const UmiEditorCompletionRequest *request);
int umi_editor_completion_request_is_cancelled(
    const UmiEditorCompletionRequest *request);

UmiStatus umi_editor_completion_provider_registry_create(
    UmiEditorCompletionProviderRegistry **out_registry);
void umi_editor_completion_provider_registry_destroy(
    UmiEditorCompletionProviderRegistry *registry);
UmiStatus umi_editor_completion_provider_registry_clear(
    UmiEditorCompletionProviderRegistry *registry);
UmiStatus umi_editor_completion_provider_registry_register(
    UmiEditorCompletionProviderRegistry *registry,
    const UmiEditorCompletionProviderDescriptor *descriptor,
    const UmiEditorCompletionProviderCallbacks *callbacks,
    void *provider_user_data);
UmiStatus umi_editor_completion_provider_registry_unregister(
    UmiEditorCompletionProviderRegistry *registry,
    const char *provider_id);
UmiStatus umi_editor_completion_provider_registry_find(
    const UmiEditorCompletionProviderRegistry *registry,
    const char *provider_id,
    UmiEditorCompletionProviderDescriptor *out_descriptor);
UmiStatus umi_editor_completion_provider_registry_at(
    const UmiEditorCompletionProviderRegistry *registry,
    size_t position,
    UmiEditorCompletionProviderDescriptor *out_descriptor);
UmiStatus umi_editor_completion_provider_registry_snapshot(
    const UmiEditorCompletionProviderRegistry *registry,
    UmiEditorCompletionProviderRegistrySnapshot *out_snapshot);
int umi_editor_completion_provider_supports_request(
    const UmiEditorCompletionProviderDescriptor *descriptor,
    const UmiEditorCompletionRequest *request);
UmiStatus umi_editor_completion_provider_registry_invoke(
    const UmiEditorCompletionProviderRegistry *registry,
    const char *provider_id,
    const UmiEditorCompletionRequest *request,
    UmiEditorCompletionCandidateSink sink,
    void *sink_user_data,
    UmiEditorCompletionProviderResponse *out_response);
UmiStatus umi_editor_completion_provider_registry_resolve(
    const UmiEditorCompletionProviderRegistry *registry,
    const char *provider_id,
    const UmiEditorCompletionRequest *request,
    UmiEditorCompletionCandidate *in_out_candidate);
UmiStatus umi_editor_completion_provider_registry_cancel(
    const UmiEditorCompletionProviderRegistry *registry,
    const char *provider_id,
    uint64_t request_id);
size_t umi_editor_completion_provider_registry_count(
    const UmiEditorCompletionProviderRegistry *registry);
uint64_t umi_editor_completion_provider_registry_revision(
    const UmiEditorCompletionProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_COMPLETION_PROVIDER_H */
