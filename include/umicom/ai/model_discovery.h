/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/model_discovery.h
 *
 * PURPOSE:
 *   Own and query bounded model-list observations without inferring runtime readiness or performing network operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_MODEL_DISCOVERY_H
#define UMICOM_AI_MODEL_DISCOVERY_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_DISCOVERY_ID_CAPACITY 513U
#define UMI_AI_DISCOVERY_MAX_MODELS 1024U
#define UMI_AI_DISCOVERY_MAX_BYTES (4U * 1024U * 1024U)
typedef enum UmiAiDiscoveryFormat {
    UMI_AI_DISCOVERY_OPENAI_LIST = 1,
    UMI_AI_DISCOVERY_OLLAMA_AVAILABLE = 2,
    UMI_AI_DISCOVERY_OLLAMA_LOADED = 3
} UmiAiDiscoveryFormat;
typedef enum UmiAiDiscoveryKnown {
    UMI_AI_DISCOVERY_SIZE = 1U,
    UMI_AI_DISCOVERY_VRAM = 2U,
    UMI_AI_DISCOVERY_CONTEXT = 4U
} UmiAiDiscoveryKnown;
typedef struct UmiAiDiscoveredModel {
    char id[UMI_AI_DISCOVERY_ID_CAPACITY];
    char display_name[UMI_AI_DISCOVERY_ID_CAPACITY];
    char digest[129];
    char family[128];
    char format[64];
    char parameter_size[64];
    char quantization[64];
    char modified_at[64];
    char expires_at[64];
    uint64_t size_bytes, vram_bytes, context_tokens;
    uint32_t known;
} UmiAiDiscoveredModel;
typedef struct UmiAiDiscoverySnapshot UmiAiDiscoverySnapshot;
/* Copy and decode one complete JSON response. This is not a network operation.
 * OPENAI_LIST requires object:list and data entries with object:model and id;
 * Ollama formats require models entries with model. Name defaults to model.
 * Optional metadata is checked when present; unknown fields are validated but
 * ignored. All decoded object keys and model IDs must be unique. IDs are case
 * sensitive UTF-8, at most 512 bytes, never normalized or shortened.
 *
 * Fixed bounds: 4 MiB input, 65536 JSON nodes, 32 containers, 1024 models.
 * Numeric metadata uses exact integer spelling in 0..INT64_MAX. Known bits
 * distinguish absent numbers from reported zero. Size meanings remain those
 * of the selected endpoint; an available-model size is not a loaded VRAM size.
 * Unsupported/empty text identifiers or malformed fields return PARSE_ERROR;
 * bounds return CAPACITY_EXCEEDED; duplicate keys/IDs return ALREADY_EXISTS.
 * On every failure *out is NULL. Caller owns a successful snapshot and destroys
 * it once. Cancellation is cooperative; allocations and sorting are not
 * interruptible. Input and token remain borrowed until this call returns.
 *
 * A list is an observation, not authenticated provenance, locality, model
 * activation, permissions or a successful inference. Keep the reviewed endpoint
 * and connection revision alongside the snapshot; do not merge providers by ID.
 */
UmiStatus UmiAiDiscoveryDecode(UmiAiDiscoveryFormat format, const void *bytes,
    size_t length, const UmiCancellationToken *cancel, UmiAiDiscoverySnapshot **out);
void UmiAiDiscoveryDestroy(UmiAiDiscoverySnapshot *snapshot);
/* NULL gives zero. Snapshot format is the caller-selected protocol, not a
 * provider identity extracted from untrusted text. Immutable reads may overlap;
 * destruction must wait until all readers have returned. */
size_t UmiAiDiscoveryCount(const UmiAiDiscoverySnapshot *snapshot);
UmiAiDiscoveryFormat UmiAiDiscoveryFormatOf(const UmiAiDiscoverySnapshot *snapshot);
/* Copy records in source order or by exact decoded ID. No borrowed record
 * escapes the owner. Outputs stay unchanged on failure. Invalid arguments
 * return INVALID_ARGUMENT; absent index/ID returns NOT_FOUND. */
UmiStatus UmiAiDiscoveryAt(const UmiAiDiscoverySnapshot *snapshot, size_t index,
    UmiAiDiscoveredModel *out);
UmiStatus UmiAiDiscoveryFind(const UmiAiDiscoverySnapshot *snapshot, const char *id,
    UmiAiDiscoveredModel *out);

/* Model browsers reuse the immutable discovery owner. Zero-initialized filters
 * select every observed model in source order. Facets compare exact UTF-8
 * strings; search performs substring matching over ID, display name and family,
 * folding ASCII A-Z only. No Unicode normalization or locale collation occurs. */
typedef enum UmiAiDiscoveryOrder {
    UMI_AI_DISCOVERY_ORDER_SOURCE = 0,
    UMI_AI_DISCOVERY_ORDER_ID,
    UMI_AI_DISCOVERY_ORDER_NAME,
    UMI_AI_DISCOVERY_ORDER_SIZE_ASC,
    UMI_AI_DISCOVERY_ORDER_CONTEXT_DESC
} UmiAiDiscoveryOrder;
typedef struct UmiAiDiscoveryFilter {
    char search[257], family[128], format[64], quantization[64];
    uint32_t required_known;
    uint64_t minimum_context_tokens, maximum_size_bytes;
    int size_limit_enabled;
    UmiAiDiscoveryOrder order;
} UmiAiDiscoveryFilter;
/* Copy matching models into a new immutable snapshot with the same observation
 * format. Input and filter are borrowed until return; the result can outlive
 * both. All text arrays must contain bounded, valid UTF-8 strings. Unknown known
 * bits, an invalid order, a non-boolean size_limit_enabled, or a nonzero maximum
 * with the size limit disabled return INVALID_ARGUMENT.
 *
 * Numeric filters require reported metadata; unknown is never treated as zero.
 * A positive minimum_context_tokens requires CONTEXT. An enabled size limit
 * requires SIZE and permits a reported size of zero when maximum is zero.
 * Facets, search and numeric criteria combine with AND. They describe provider
 * observations, not device fit, current availability or verified capability.
 *
 * SOURCE retains input order. Other orders tie-break by complete ID using
 * bytewise UTF-8 comparison. Numeric orders place unknown values last. Name
 * ordering is bytewise, not the ASCII-folded search order. No global locale or
 * mutable sorting state is used. Cancellation is cooperative between records;
 * allocation and bounded sorting are not interruptible.
 *
 * Initialise *out=NULL and keep it separate from the source owner. An occupied
 * output is refused unchanged; other failures keep it NULL. No source mutation,
 * provider merge, I/O, cache persistence or model activation occurs. */
UmiStatus UmiAiDiscoverySelect(const UmiAiDiscoverySnapshot *source,
    const UmiAiDiscoveryFilter *filter, const UmiCancellationToken *cancel,
    UmiAiDiscoverySnapshot **out);

#ifdef __cplusplus
}
#endif
#endif
