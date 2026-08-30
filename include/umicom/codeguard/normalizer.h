/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/normalizer.h
 *
 * PURPOSE:
 *   Hash raw and normalised source so duplicate files can be detected
 *   deterministically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_NORMALIZER_H
#define UMICOM_CODEGUARD_NORMALIZER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
uint64_t umi_codeguard_hash_bytes(const void *data, size_t size);
UmiStatus umi_codeguard_hash_file(const char *path, uint64_t *out_raw_hash,
                                  uint64_t *out_normalized_hash, size_t *out_size);
#ifdef __cplusplus
}
#endif
#endif
