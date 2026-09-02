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
UmiStatus umi_codeguard_duplicate_set_add(UmiCodeGuardDuplicateSet *set, const char *path);
/**
 * Provide the codeguard duplicate emit operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_duplicate_emit(const UmiCodeGuardDuplicateSet *set, UmiCodeGuardResult *result);
/**
 * Return the number of records represented by codeguard duplicate set without changing
 * their state.
 */
size_t umi_codeguard_duplicate_set_count(const UmiCodeGuardDuplicateSet *set);
#ifdef __cplusplus
}
#endif
#endif
