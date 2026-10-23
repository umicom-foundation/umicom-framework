/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/record_chain/internal.h
 * PURPOSE: Share canonical field ordering between hashing and portable archives.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RECORD_CHAIN_INTERNAL_H
#define UMICOM_RECORD_CHAIN_INTERNAL_H
#include "umicom/record_chain/archive.h"
#define RECORD_CHAIN_FIELDS_BYTES 116U
void RecordChainWriteNumber(unsigned char *out, uint64_t value, size_t width);
uint64_t RecordChainReadNumber(const unsigned char *bytes, size_t width);
void RecordChainFields(const UmiRecordChainRecord *, unsigned char out[116]);
UmiStatus RecordChainCheckpointValid(const UmiRecordChainCheckpoint *);
UmiStatus RecordChainRecordValid(const UmiRecordChainRecord *);
UmiStatus RecordChainDigest(const UmiRecordChainRecord *, unsigned char out[32]);
#endif
