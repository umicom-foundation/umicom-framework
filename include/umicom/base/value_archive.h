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
/* Conditional typed decoding adds a checked restore for caller-owned values:
 * - Keep an independent copy of the encoded fields reviewed before starting an
 *   import. Pass that expected snapshot with the destination to the typed
 *   archive_decode_if_unchanged function. Passing the destination itself as
 *   expected is allowed, but cannot detect changes since an earlier review.
 * - The function validates both snapshots and compares their full canonical
 *   encodings. A mismatch returns INVALID_STATE without changing the destination.
 *   It ignores padding and unused string tails, just as encoding does.
 * - The archive must have the matching schema, valid fields and a valid checksum.
 *   Any error leaves all destination bytes unchanged. OUT_OF_MEMORY is possible:
 *   comparison uses at most twice the encoded size, then decoding uses one
 *   heap-allocated record. The old encode/decode APIs retain their contracts.
 * - The caller serializes access to all input and output storage for the whole
 *   call. This is not a mutex, a historical change detector or a database commit.
 *   A value that changes away and back has the same encoded state. Owners that
 *   need monotonic history must also use their revision-checked publication API.
 * - Decoded IDs and revisions come from the archive. Restore into a reviewed
 *   draft, then use the relevant registry/owner API to publish live state. No
 *   network request, trade, provider connection or other command is executed.
 */

/* Typed archive_decode_with_workspace operations give SDK callers explicit
 * control of the candidate record storage:
 * - Allocate two separate, correctly aligned objects of the declared record
 *   type: a scratch workspace and the destination. A reusable heap allocation
 *   lets a small-stack thread decode without an automatic whole-record copy.
 * - Supply the same readable byte span accepted by the established decoder.
 *   Input, scratch and destination may not overlap, including partial overlaps.
 *   The supported flat-address hosts reject overlap with INVALID_ARGUMENT.
 * - Scratch is temporary and may be overwritten even on failure. Never pass
 *   a live owner, shared draft, or object containing owned resources as scratch.
 *   The typed contract defines an inline value with no external ownership.
 * - All failures preserve every destination byte. A successful call copies
 *   validated fields into independent destination storage; scratch can then be
 *   reused. The caller remains responsible for serializing all accesses.
 * - No allocation, I/O or live publication is introduced by this API. It uses
 *   the existing format, schema, reader and validator, with no migration. The
 *   original decoder and conditional restore interfaces remain unchanged.
 */
#ifdef __cplusplus
}
#endif
#endif
