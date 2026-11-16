/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/ollama_model_info.h
 *
 * PURPOSE:
 *   Own copied per-model detail and capability declarations without activating a model or inferring runtime support.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_OLLAMA_MODEL_INFO_H
#define UMICOM_AI_OLLAMA_MODEL_INFO_H
#include "umicom/ai/model_discovery.h"
#include "umicom/base/sha256.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_OLLAMA_MODEL_INFO_MAX_BYTES (4U * 1024U * 1024U)
#define UMI_OLLAMA_MODEL_CAPABILITY_LIMIT 64U
#define UMI_OLLAMA_MODEL_CAPABILITY_CAPACITY 129U
typedef enum UmiOllamaModelInfoKnown {
    UMI_OLLAMA_MODEL_PARAMETER_COUNT=1U,
    UMI_OLLAMA_MODEL_CONTEXT_LENGTH=2U,
    UMI_OLLAMA_MODEL_EMBEDDING_LENGTH=4U
} UmiOllamaModelInfoKnown;
typedef struct UmiOllamaModelInfoSummary {
    char requested_model[UMI_AI_DISCOVERY_ID_CAPACITY];
    char parent_model[UMI_AI_DISCOVERY_ID_CAPACITY];
    char format[64],family[128],parameter_size[64],quantization[64];
    char architecture[128],modified_at[64];
    uint64_t parameter_count,context_length,embedding_length;
    uint32_t known;
    bool capabilities_observed;
    size_t capability_count,response_bytes;
    unsigned char response_sha256[UMI_SHA256_BYTES];
} UmiOllamaModelInfoSummary;
typedef struct UmiOllamaModelInfo UmiOllamaModelInfo;
/* Decode a complete host-supplied /api/show object. requested_model is a
 * nonempty UTF-8 caller label, <=512 bytes, not a response authentication claim.
 * Keep the reviewed connection and revision separately; do not merge providers
 * by this label. No HTTP, store, vault, model activation or inference occurs.
 *
 * Details, model_info and capabilities are optional. Missing numeric fields
 * remain unknown; present numbers require integer spelling in 0..INT64_MAX.
 * Context/embedding length use the exact general.architecture prefix; never
 * guess it from the family label. An empty object observes no known metadata.
 * Missing capabilities differs from a present empty array. Copy up to 64
 * nonempty unique UTF-8 names, each <=128 bytes, in declared order. Preserve
 * unknown names and case; do not infer permissions or operational capability.
 *
 * Validate unknown JSON fields and all decoded object-key uniqueness. Do not
 * retain templates, system prompts, license text, tensor lists or raw JSON.
 * A nonempty string error member returns INVALID_STATE; wrong/empty error
 * values return PARSE_ERROR. Bounds: 4 MiB, 65536 nodes, depth 32.
 *
 * Initialize *out=NULL. Every failure preserves it. Copy successful metadata
 * and hash exact input bytes, then clear temporary JSON ownership. Caller input
 * and token remain immutable/alive until return. Cancellation is cooperative;
 * allocation, copying and hashing are not interruptible. */
UmiStatus UmiOllamaModelInfoCreate(const char *requested_model,const void *bytes,
    size_t length,const UmiCancellationToken *cancel,UmiOllamaModelInfo **out);
void UmiOllamaModelInfoDestroy(UmiOllamaModelInfo *info);
/* Copy outputs only on success. Immutable reads may overlap; destruction waits
 * for all readers. Inputs/outputs must not overlap the opaque owner's storage. */
UmiStatus UmiOllamaModelInfoInspect(const UmiOllamaModelInfo *info,UmiOllamaModelInfoSummary *out);
UmiStatus UmiOllamaModelInfoCapabilityAt(const UmiOllamaModelInfo *info,size_t index,
    char *out,size_t capacity);
/* Missing list returns NOT_FOUND and preserves out_declared. A known list
 * returns OK with true/false for exact case-sensitive membership. This is a
 * declaration lookup, not a capability test or a provider action permission. */
UmiStatus UmiOllamaModelInfoDeclares(const UmiOllamaModelInfo *info,
    const char *capability,bool *out_declared);
#ifdef __cplusplus
}
#endif
#endif
