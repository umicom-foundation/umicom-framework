/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/record_chain/chain.h
 * PURPOSE: Define portable ordered record commitments and checkpoint verification.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RECORD_CHAIN_CHAIN_H
#define UMICOM_RECORD_CHAIN_CHAIN_H
#include "umicom/base/sha256.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RECORD_CHAIN_ID_BYTES 32U
#define UMI_RECORD_CHAIN_PAYLOAD_LIMIT (16U * 1024U * 1024U)
#define UMI_RECORD_CHAIN_BATCH_LIMIT 4096U

/** A caller-selected, nonzero identity separates independently owned histories.
 * Use one stable identity per source and encoding schema. It is public metadata,
 * not a key. Sequence zero identifies the deterministic empty-chain checkpoint.
 * Keep the expected checkpoint independently of the records being checked. */
typedef struct UmiRecordChainCheckpoint {
    unsigned char chain_id[UMI_RECORD_CHAIN_ID_BYTES];
    uint64_t sequence;
    unsigned char digest[UMI_SHA256_BYTES];
} UmiRecordChainCheckpoint;

/** A receipt commits to exact payload bytes, kind, position and predecessor.
 * The application defines nonzero kinds and canonical payload schemas. This
 * component does not validate business rules, deduplicate transaction IDs,
 * authenticate actors, encrypt data, establish consensus or execute contracts.
 * Hashes of predictable private data can disclose information by guessing. */
typedef struct UmiRecordChainRecord {
    unsigned char chain_id[UMI_RECORD_CHAIN_ID_BYTES];
    uint64_t sequence;
    uint32_t kind;
    uint64_t payload_size;
    unsigned char previous_digest[UMI_SHA256_BYTES];
    unsigned char payload_digest[UMI_SHA256_BYTES];
    unsigned char digest[UMI_SHA256_BYTES];
} UmiRecordChainRecord;

typedef struct UmiRecordChainInput {
    const UmiRecordChainRecord *record;
    const void *payload;
    size_t payload_size;
} UmiRecordChainInput;

/** No allocation, I/O or shared mutable state. Inputs and outputs must not
 * overlap; the checkpoint argument is intentionally updated in place. Every
 * rejected operation leaves all caller outputs and checkpoints unchanged.
 * NULL payload is permitted only with size zero. */
UmiStatus umi_record_chain_start(const unsigned char chain_id[32],
    UmiRecordChainCheckpoint *out);
UmiStatus umi_record_chain_append(UmiRecordChainCheckpoint *checkpoint,
    uint32_t kind, const void *payload, size_t size, UmiRecordChainRecord *out);
/** Recompute both commitments, then advance only when the identity, next
 * sequence and predecessor match. Never treat a decoded receipt as accepted. */
UmiStatus umi_record_chain_accept(UmiRecordChainCheckpoint *checkpoint,
    const UmiRecordChainRecord *record, const void *payload, size_t size);
/** Check the entire batch before publishing a new checkpoint. expected_tip is
 * required: a prefix of a valid chain must not silently qualify as complete.
 * Inputs and payloads remain borrowed and stable for the duration of the call. */
UmiStatus umi_record_chain_verify(UmiRecordChainCheckpoint *checkpoint,
    const UmiRecordChainInput *inputs, size_t count,
    const UmiRecordChainCheckpoint *expected_tip);
bool umi_record_chain_checkpoint_equal(const UmiRecordChainCheckpoint *left,
    const UmiRecordChainCheckpoint *right);
#ifdef __cplusplus
}
#endif
#endif
