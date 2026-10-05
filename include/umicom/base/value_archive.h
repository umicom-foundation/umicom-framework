/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/value_archive.h
 * PURPOSE: Describe checked, portable value bytes without granting execution authority.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BASE_VALUE_ARCHIVE_H
#define UMICOM_BASE_VALUE_ARCHIVE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/* The limit bounds inspection work as well as collection scratch storage.
 * A product can impose a smaller budget before calling its typed decoder. */
#define UMI_VALUE_ARCHIVE_BYTE_LIMIT (16U * 1024U * 1024U)
#define UMI_VALUE_ARCHIVE_HEADER_SIZE 32U

typedef struct UmiValueArchiveInfo {
    uint64_t schema;
    size_t payload_size;
} UmiValueArchiveInfo;

/** Inspect one complete archive, including its checksum, without decoding a
 * domain value. Refusal leaves out_info unchanged. The input must provide
 * byte_count readable bytes and may not overlap the output. Null arguments
 * are INVALID_ARGUMENT; malformed bytes are PARSE_ERROR; an archive over the
 * byte limit is CAPACITY_EXCEEDED. Inspection allocates nothing.
 *
 * A schema identifies a specific field layout, not a trusted sender. The
 * checksum detects accidental damage; it is neither authentication nor
 * encryption. Use the matching typed decoder and the domain's reviewed
 * publication API before accepting restored values into a live owner.
 * No archive can authorize tool execution, account access or order submission.
 */
UmiStatus umi_value_archive_inspect(const void *bytes, size_t byte_count,
    UmiValueArchiveInfo *out_info);

/* Typed archive APIs use these common ownership rules:
 * - Encode accepts NULL/zero output to measure. A short output reports the
 *   required size but writes no bytes. Other failures preserve both outputs.
 * - Decode constructs a private candidate and validates it before replacing
 *   the destination. Every refusal leaves the destination unchanged.
 * - Text is stored through its first terminator, with no unused array bytes
 *   or struct padding. Decode clears unused text space and rebuilds the local
 *   structure-size field. Other fields, including revision, remain evidence.
 * - Unknown layouts are refused with UNAVAILABLE; there is no implicit
 *   migration. Domain metadata and validation rules still apply.
 * - Integers use explicit byte order; binary64 floating values must be finite.
 *   A host without that representation reports UNAVAILABLE for such a type.
 * - Storage must not overlap input, output, metadata or a live registry.
 *   Serialize access on the owner thread. These calls provide no locking.
 * - Codecs allocate nothing and perform no I/O. Whole-registry restoration
 *   uses bounded temporary storage and the existing atomic replacement path.
 */
#ifdef __cplusplus
}
#endif
#endif
