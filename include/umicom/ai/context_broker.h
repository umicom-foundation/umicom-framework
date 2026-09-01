/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/context_broker.h
 *
 * PURPOSE:
 *   Select project and workspace context deterministically within explicit
 *   token, source-count, privacy and provider-sharing budgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The broker records provenance and token estimates, not private editor or Git
 * objects.  Applications can therefore explain exactly which sources would be
 * supplied while their adapters retain ownership of the actual source text.
 */
#ifndef UMICOM_AI_CONTEXT_BROKER_H
#define UMICOM_AI_CONTEXT_BROKER_H

#include "umicom/ai/privacy_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CONTEXT_SOURCE_CAPACITY 128U
#define UMI_AI_CONTEXT_PLAN_SOURCE_MAX 32U

typedef enum UmiAiContextSourceKind {
    UMI_AI_CONTEXT_WORKSPACE = 1,
    UMI_AI_CONTEXT_PROJECT = 2,
    UMI_AI_CONTEXT_DOCUMENT = 3,
    UMI_AI_CONTEXT_SELECTION = 4,
    UMI_AI_CONTEXT_DIAGNOSTICS = 5,
    UMI_AI_CONTEXT_SOURCE_CONTROL = 6,
    UMI_AI_CONTEXT_RETRIEVAL = 7
} UmiAiContextSourceKind;

typedef struct UmiAiContextSource {
    char source_id[UMI_AI_ID_CAPACITY];
    char label[UMI_AI_SMALL_TEXT_CAPACITY];
    char uri[UMI_AI_TEXT_CAPACITY];
    UmiAiContextSourceKind kind;
    UmiAiDataClassification classification;
    uint32_t estimated_tokens;
    uint32_t priority;
    int mandatory;
    int enabled;
    uint64_t revision;
} UmiAiContextSource;

typedef struct UmiAiContextPlan {
    UmiAiContextSource sources[UMI_AI_CONTEXT_PLAN_SOURCE_MAX];
    size_t source_count;
    size_t dropped_count;
    uint32_t context_limit;
    uint32_t reserved_output_tokens;
    uint32_t selected_tokens;
    uint32_t remaining_tokens;
    UmiAiDataClassification highest_classification;
    int truncated;
    uint64_t revision;
} UmiAiContextPlan;

typedef struct UmiAiContextBroker UmiAiContextBroker;

UmiStatus umi_ai_context_broker_create(UmiAiContextBroker **out_broker);
void umi_ai_context_broker_destroy(UmiAiContextBroker *broker);
UmiStatus umi_ai_context_broker_upsert(
    UmiAiContextBroker *broker,
    const UmiAiContextSource *source);
UmiStatus umi_ai_context_broker_remove(
    UmiAiContextBroker *broker,
    const char *source_id);
void umi_ai_context_broker_clear(UmiAiContextBroker *broker);
UmiStatus umi_ai_context_broker_at(
    const UmiAiContextBroker *broker,
    size_t index,
    UmiAiContextSource *out_source);
size_t umi_ai_context_broker_count(const UmiAiContextBroker *broker);
UmiStatus umi_ai_context_broker_plan(
    const UmiAiContextBroker *broker,
    uint32_t context_limit,
    uint32_t reserved_output_tokens,
    size_t maximum_sources,
    UmiAiProviderKind provider_kind,
    const UmiAiPrivacyPolicy *privacy,
    int sensitive_approved,
    UmiAiContextPlan *out_plan);
const char *umi_ai_context_source_kind_text(UmiAiContextSourceKind kind);

#ifdef __cplusplus
}
#endif

#endif
