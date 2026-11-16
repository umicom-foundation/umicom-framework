/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/model_formats/gguf.h
 *
 * PURPOSE:
 *   Observe bounded GGUF role metadata and complete file identity without loading model weights.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MODEL_FORMATS_GGUF_H
#define UMICOM_MODEL_FORMATS_GGUF_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/sha256.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_GGUF_MAX_METADATA_BYTES UINT64_C(67108864)
#define UMI_GGUF_MAX_METADATA_ENTRIES UINT64_C(65536)
#define UMI_GGUF_MAX_VALUES UINT64_C(4194304)
#define UMI_GGUF_MAX_ARRAY_DEPTH 8U
#define UMI_GGUF_MAX_FILE_BYTES UINT64_C(1099511627776)
typedef enum UmiGgufRole {
    UMI_GGUF_ROLE_UNKNOWN=0,
    UMI_GGUF_ROLE_MODEL_CANDIDATE=1,
    UMI_GGUF_ROLE_AUXILIARY=2,
    UMI_GGUF_ROLE_VOCABULARY_ONLY=3
} UmiGgufRole;
typedef struct UmiGgufMetadata {
    uint32_t version;
    uint64_t tensors,entries,metadata_bytes;
    bool has_architecture,has_type,has_adapter_type;
    char architecture[64],type[64],adapter_type[64];
    UmiGgufRole role;
} UmiGgufMetadata;
typedef struct UmiGgufFileIdentity {
    uint64_t bytes;
    unsigned char sha256[UMI_SHA256_BYTES];
} UmiGgufFileIdentity;
typedef struct UmiGgufFileInspection {
    UmiGgufFileIdentity file;
    UmiGgufMetadata metadata;
} UmiGgufFileInspection;

/* Inspect a readable prefix containing the complete little-endian v2/v3 header
 * and metadata. Ignore trailing tensor descriptors/data. No heap allocation.
 * Retain only general.architecture, general.type and adapter.type, each as a
 * nonempty printable ASCII identifier of at most 63 bytes. Reject repeated or
 * non-string selected keys. All keys are nonempty printable ASCII <=511 bytes.
 * Other string values are skipped as raw bytes, not UTF-8-validated or stored.
 * Scalar types 0..12 and nested homogeneous arrays are supported; bool values
 * must be 0 or 1. Limits include all parsed bytes and values, not tensor data.
 * Failure/cancellation preserves every output byte. Inputs and token are
 * borrowed for the call; output may overlap prefix storage after completion.
 *
 * Auxiliary means architecture controlvector/clip, type adapter or a declared
 * adapter.type. Zero tensors otherwise means vocabulary-only. A nonempty
 * architecture with absent/model type is only a MODEL_CANDIDATE; unknown type
 * remains UNKNOWN. Missing role metadata is not inferred from a filename.
 * Neither role nor syntactic metadata proves tensor completeness, text-model
 * suitability, backend support, available memory, trust or readiness. */
UmiStatus UmiGgufMetadataInspect(const unsigned char *prefix,size_t prefix_bytes,
    const UmiCancellationToken *cancel,UmiGgufMetadata *out_metadata);

/* Read and hash the entire regular file through UmiInputFileVisit. Parse its
 * metadata incrementally without retaining large strings, arrays or weights.
 * Reuse that reader's absolute UTF-8 path, trusted-parent, no-final-link and
 * before/after metadata checks. Blocking work belongs on a worker thread.
 * Optional expected identity is copied before reading, can borrow output.file,
 * and must name 24..MAX_FILE_BYTES. A larger file is refused before callbacks;
 * a shorter file or digest mismatch returns INVALID_STATE. Publish only after
 * read/close, cancellation, complete metadata and expected identity checks.
 * The file is not frozen after inspection; recheck before later use. No launch,
 * download, tensor validation, universal compatibility or signature claim. */
UmiStatus UmiGgufFileInspect(const char *path,const UmiGgufFileIdentity *expected,
    const UmiCancellationToken *cancel,UmiGgufFileInspection *out_inspection);
#ifdef __cplusplus
}
#endif
#endif
