/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/duplicate.h
 *
 * PURPOSE:
 *   Collect source fingerprints and report byte-identical or normalised
 *   duplicate files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_DUPLICATE_H
#define UMICOM_CODEGUARD_DUPLICATE_H
#include <stddef.h>
#include "umicom/platform/cancellation.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/codeguard/result.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the code guard duplicate file data shared with callers of this public
 * contract.
 */
typedef struct UmiCodeGuardDuplicateFile {
    char path[1024];
    uint64_t raw_hash;
    uint64_t normalized_hash;
    size_t size;
} UmiCodeGuardDuplicateFile;
/**
 * Represent the code guard duplicate set data shared with callers of this public contract.
 */
typedef struct UmiCodeGuardDuplicateSet UmiCodeGuardDuplicateSet;
/**
 * Initialise codeguard duplicate set from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_codeguard_duplicate_set_create(UmiCodeGuardDuplicateSet **out_set);
/**
 * Release or reset state held by codeguard duplicate set so the same storage can be reused
 * safely.
 */
void umi_codeguard_duplicate_set_destroy(UmiCodeGuardDuplicateSet *set);
/**
 * Add codeguard duplicate set only after its inputs and available capacity have been
 * checked.
 */
/* Own the full path (up to 1023 bytes) and fingerprints on success.
 * Failure leaves prior entries unchanged. Capacity arithmetic is checked. */
UmiStatus umi_codeguard_duplicate_set_add(UmiCodeGuardDuplicateSet *set, const char *path);
/**
 * Provide the codeguard duplicate emit operation used by this module and its client
 * applications.
 */
/* Emit fingerprint-match candidates in pair order; empty files are omitted.
 * A raw hash plus size match is not a byte comparison. Normalization can merge
 * distinct token sequences, and hashes can collide. Review current source.
 * Stop at the first insertion error; earlier findings remain appended. Use
 * the retained scan service when partial attempts must not replace a report. */
UmiStatus umi_codeguard_duplicate_emit(const UmiCodeGuardDuplicateSet *set, UmiCodeGuardResult *result);
/**
 * Return the number of records represented by codeguard duplicate set without changing
 * their state.
 */
size_t umi_codeguard_duplicate_set_count(const UmiCodeGuardDuplicateSet *set);

/* Native bounded admission used by profile-driven scans. Candidate identity
 * and collection count remain unchanged on failure. Uses the same regular
 * final-leaf, byte-budget and cancellation contract as hash_file_bounded. */
UmiStatus umi_codeguard_duplicate_set_add_bounded(UmiCodeGuardDuplicateSet *set,
    const char *path, uint64_t maximum_bytes, const UmiCancellationToken *cancellation);
#ifdef __cplusplus
}
#endif
#endif
