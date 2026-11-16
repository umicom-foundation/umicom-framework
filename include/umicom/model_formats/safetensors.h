/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/model_formats/safetensors.h
 *
 * PURPOSE:
 *   Inspect bounded Safetensors header and layout metadata without reading or executing tensor payloads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MODEL_FORMATS_SAFETENSORS_H
#define UMICOM_MODEL_FORMATS_SAFETENSORS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SAFETENSORS_MAX_HEADER_BYTES (16U * 1024U * 1024U)
#define UMI_SAFETENSORS_MAX_FILE_BYTES (UINT64_C(1) << 40U)
#define UMI_SAFETENSORS_MAX_TENSORS 65536U
#define UMI_SAFETENSORS_MAX_RANK 64U
#define UMI_SAFETENSORS_MAX_NAME_BYTES 4096U

typedef struct UmiSafetensorsHeaderInfo {
    uint64_t header_bytes, data_bytes, elements;
    size_t tensors, metadata_entries, maximum_rank;
} UmiSafetensorsHeaderInfo;

/* Inspect an eight-byte little-endian length and its complete JSON header.
 * prefix_bytes can include some payload bytes, which are ignored, but cannot
 * exceed file_bytes. The caller supplies the independently observed file size.
 * This function performs no I/O, hashing, payload decoding or model loading.
 *
 * Require a leading '{', complete unique-key JSON and only space padding after
 * the root object. Metadata values must be strings. Tensor records require
 * exactly dtype, shape and data_offsets. Supported byte-aligned dtypes are
 * BOOL, U8/I8, U16/I16, U32/I32, U64/I64, F16/BF16/F32/F64, C64, F8_E5M2,
 * F8_E4M3, F8_E8M0, F8_E4M3FNUZ and F8_E5M2FNUZ. Other types or extra tensor
 * fields return NOT_IMPLEMENTED, not a claim that the format itself is invalid.
 *
 * Check dimensions, overflow, exact byte spans and a contiguous nonoverlapping
 * layout covering all declared payload bytes. Scalars and zero-element tensors
 * are supported, as are empty archives with no payload. Limits above apply;
 * the JSON reader additionally caps nodes at 1,048,576 and containers at 128.
 * Tensor names are bounded decoded UTF-8; decoded NUL text is unsupported.
 *
 * Preserve out_info on every failure. Keep input, output and token alive and
 * nonoverlapping; serialize access. Cancellation is cooperative between parse
 * tokens, object keys and tensor records, not during allocation or sorting.
 * Success establishes header/layout consistency only. Actual byte identity,
 * numerical values, architecture compatibility and readiness remain unverified. */
UmiStatus UmiSafetensorsHeaderInspect(const void *prefix, size_t prefix_bytes,
    uint64_t file_bytes, const UmiCancellationToken *cancel,
    UmiSafetensorsHeaderInfo *out_info);
#ifdef __cplusplus
}
#endif
#endif
