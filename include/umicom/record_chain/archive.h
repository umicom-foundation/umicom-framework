/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/record_chain/archive.h
 * PURPOSE: Exchange exact-size canonical record-chain values without struct dumps.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RECORD_CHAIN_ARCHIVE_H
#define UMICOM_RECORD_CHAIN_ARCHIVE_H
#include "umicom/record_chain/chain.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RECORD_CHAIN_CHECKPOINT_BYTES 80U
#define UMI_RECORD_CHAIN_RECORD_BYTES 156U
/** Fixed schemas use explicit big-endian integers and byte arrays. Encoding
 * needs at least the stated capacity and writes exactly the stated size;
 * decoding requires exactly that size, rejecting trailing data. Outputs are
 * unchanged on failure and must not overlap inputs. No allocator or file I/O.
 * Decoding checks shape and receipt digest, not payload or trusted provenance.
 * A decoded checkpoint is untrusted until compared with an independent anchor. */
UmiStatus umi_record_chain_checkpoint_encode(const UmiRecordChainCheckpoint *,
    void *bytes, size_t capacity);
UmiStatus umi_record_chain_checkpoint_decode(const void *bytes, size_t size,
    UmiRecordChainCheckpoint *out);
UmiStatus umi_record_chain_record_encode(const UmiRecordChainRecord *,
    void *bytes, size_t capacity);
UmiStatus umi_record_chain_record_decode(const void *bytes, size_t size,
    UmiRecordChainRecord *out);
#ifdef __cplusplus
}
#endif
#endif
