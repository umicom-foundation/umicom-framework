/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/runtime_catalogue.h
 *
 * PURPOSE:
 *   Describe and select local, remote, in-process and AuthorEngine-backed AI
 *   runtimes without exposing vendor SDK or process implementation details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A provider is the callable boundary, a model is the AI capability, and a
 * runtime record explains where that combination is hosted.  Keeping these
 * facts together lets Studio show one honest catalogue before making a call.
 */
#ifndef UMICOM_AI_RUNTIME_CATALOGUE_H
#define UMICOM_AI_RUNTIME_CATALOGUE_H

#include "umicom/ai/model.h"
#include "umicom/ai/health.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_RUNTIME_CATALOGUE_CAPACITY 64U

/**
 * List the named ai runtime transport values accepted by this public contract.
 */
typedef enum UmiAiRuntimeTransport {
    UMI_AI_RUNTIME_IN_PROCESS = 1,
    UMI_AI_RUNTIME_PROCESS = 2,
    UMI_AI_RUNTIME_HTTP = 3,
    UMI_AI_RUNTIME_LIBRARY = 4
} UmiAiRuntimeTransport;

/**
 * List the named ai runtime capability values accepted by this public contract.
 */
typedef enum UmiAiRuntimeCapability {
    UMI_AI_RUNTIME_CHAT = 1U << 0,
    UMI_AI_RUNTIME_COMPLETION = 1U << 1,
    UMI_AI_RUNTIME_EMBEDDINGS = 1U << 2,
    UMI_AI_RUNTIME_TOOLS = 1U << 3,
    UMI_AI_RUNTIME_STREAMING = 1U << 4
} UmiAiRuntimeCapability;

/**
 * Represent the ai runtime descriptor data shared with callers of this public contract.
 */
typedef struct UmiAiRuntimeDescriptor {
    char runtime_id[UMI_AI_ID_CAPACITY];
    char provider_id[UMI_AI_ID_CAPACITY];
    char display_name[UMI_AI_SMALL_TEXT_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    char endpoint[UMI_AI_TEXT_CAPACITY];
    UmiAiProviderKind provider_kind;
    UmiAiRuntimeTransport transport;
    uint32_t capabilities;
    uint32_t context_tokens;
    uint32_t max_output_tokens;
    int configured;
    int local;
    UmiAiHealth health;
    uint64_t revision;
} UmiAiRuntimeDescriptor;

/**
 * Represent the ai runtime catalogue data shared with callers of this public contract.
 */
typedef struct UmiAiRuntimeCatalogue UmiAiRuntimeCatalogue;

/**
 * Initialise ai runtime catalogue from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ai_runtime_catalogue_create(
    UmiAiRuntimeCatalogue **out_catalogue);
/**
 * Release or reset state held by ai runtime catalogue so the same storage can be reused
 * safely.
 */
void umi_ai_runtime_catalogue_destroy(UmiAiRuntimeCatalogue *catalogue);
/**
 * Provide the ai runtime catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_runtime_catalogue_upsert(
    UmiAiRuntimeCatalogue *catalogue,
    const UmiAiRuntimeDescriptor *descriptor);
/**
 * Find ai runtime catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_runtime_catalogue_find(
    const UmiAiRuntimeCatalogue *catalogue,
    const char *runtime_id,
    UmiAiRuntimeDescriptor *out_descriptor);
/**
 * Find ai runtime catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_runtime_catalogue_at(
    const UmiAiRuntimeCatalogue *catalogue,
    size_t index,
    UmiAiRuntimeDescriptor *out_descriptor);
/**
 * Provide the ai runtime catalogue set health operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_runtime_catalogue_set_health(
    UmiAiRuntimeCatalogue *catalogue,
    const char *runtime_id,
    const UmiAiHealth *health);
/**
 * Provide the ai runtime catalogue select operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_runtime_catalogue_select(
    const UmiAiRuntimeCatalogue *catalogue,
    uint32_t required_capabilities,
    uint32_t required_context_tokens,
    int local_only,
    UmiAiRuntimeDescriptor *out_descriptor);
/**
 * Return the number of records represented by ai runtime catalogue without changing their
 * state.
 */
size_t umi_ai_runtime_catalogue_count(
    const UmiAiRuntimeCatalogue *catalogue);
/**
 * Return the number of records represented by ai runtime catalogue provider without
 * changing their state.
 */
size_t umi_ai_runtime_catalogue_provider_count(
    const UmiAiRuntimeCatalogue *catalogue);
/**
 * Provide the ai runtime transport text operation used by this module and its client
 * applications.
 */
const char *umi_ai_runtime_transport_text(UmiAiRuntimeTransport transport);

#ifdef __cplusplus
}
#endif

#endif
