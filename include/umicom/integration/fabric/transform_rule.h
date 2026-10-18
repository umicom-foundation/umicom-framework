/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/transform_rule.h
 *
 * PURPOSE:
 *   Describe a deterministic field/content transform without embedding a scripting runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_TRANSFORM_RULE_H
#define UMICOM_INTEGRATION_FABRIC_TRANSFORM_RULE_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric transform rule data shared with callers of this public contract.
 */
typedef struct UmiFabricTransformRule {
    char rule_id[UMI_FABRIC_ID_CAPACITY];
    char source_path[UMI_FABRIC_TEXT_CAPACITY];
    char target_path[UMI_FABRIC_TEXT_CAPACITY];
    char operation[UMI_FABRIC_ID_CAPACITY];
    bool required;
} UmiFabricTransformRule;

/**
 * Initialise fabric transform rule from caller-provided values so later operations receive
 * a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_fabric_transform_rule_init(UmiFabricTransformRule *item, const char *rule_id, const char *source_path, const char *target_path, const char *operation, bool required);
/**
 * Check that fabric transform rule satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_transform_rule_validate(const UmiFabricTransformRule *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fabric_transform_rule_archive_encode(const UmiFabricTransformRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fabric_transform_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiFabricTransformRule *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiFabricTransformRule object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_fabric_transform_rule_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiFabricTransformRule *workspace, UmiFabricTransformRule *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_fabric_transform_rule_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFabricTransformRule *expected, UmiFabricTransformRule *value);


#ifdef __cplusplus
}
#endif
#endif
