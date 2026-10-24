/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/structured_form_snapshot.h
 * PURPOSE: Preserve semantic form answers in a bounded, versioned binary snapshot.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_SNAPSHOT_H
#define UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_SNAPSHOT_H
#include "umicom/application/experience/structured_form.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FORM_SNAPSHOT_MAX_BYTES 131072U
/* A bounded untrusted header hint used only to select a trusted schema. Success
 * does not validate the remaining body, prove authenticity or permit rendering.
 * Resolve the original schema and perform a full decode before using answers. */
typedef struct UmiStructuredFormSnapshotIdentity {
  char form_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  uint64_t schema_version, revision, saved_revision;
  size_t field_count;
} UmiStructuredFormSnapshotIdentity;
UmiStatus umi_structured_form_snapshot_identity(const void *bytes, size_t length,
    UmiStructuredFormSnapshotIdentity *out);
/* Codec version 1 preserves schema ID/version, revision/saved revision, field
 * IDs, states, typed values, units and reasons. Only active value members are
 * stored, never C structure padding. No encryption, authentication or checksum
 * is provided; use the record service for atomic storage and content hashing.
 * Decimal values require the supported IEEE binary64 representation; other
 * representations return NOT_IMPLEMENTED. Locale never affects the bytes.
 * Both buffer and out_length remain unchanged on failure. */
UmiStatus umi_structured_form_snapshot_encode(const UmiStructuredFormSchema *schema,
                                              const UmiStructuredForm *form, void *out,
                                              size_t capacity, size_t *out_length);
/* Exact framing: unknown versions, trailing data, malformed strings or values,
 * mismatched field order/IDs and incompatible semantic versions are refused.
 * The caller supplies the original schema; migration is a separate operation.
 * Output is unchanged on failure. Input may alias output. */
UmiStatus umi_structured_form_snapshot_decode(const UmiStructuredFormSchema *schema,
                                              const void *bytes, size_t length,
                                              UmiStructuredForm *out);
#ifdef __cplusplus
}
#endif
#endif
