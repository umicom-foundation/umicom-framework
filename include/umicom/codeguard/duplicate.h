/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/duplicate.h
 *
 * PURPOSE:
 *   Collect source fingerprints and report byte-identical or normalised
 *   duplicate files.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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
typedef struct UmiCodeGuardDuplicateFile {
    char path[1024];
    uint64_t raw_hash;
    uint64_t normalized_hash;
    size_t size;
} UmiCodeGuardDuplicateFile;
typedef struct UmiCodeGuardDuplicateSet UmiCodeGuardDuplicateSet;
UmiStatus umi_codeguard_duplicate_set_create(UmiCodeGuardDuplicateSet **out_set);
void umi_codeguard_duplicate_set_destroy(UmiCodeGuardDuplicateSet *set);
UmiStatus umi_codeguard_duplicate_set_add(UmiCodeGuardDuplicateSet *set, const char *path);
UmiStatus umi_codeguard_duplicate_emit(const UmiCodeGuardDuplicateSet *set, UmiCodeGuardResult *result);
size_t umi_codeguard_duplicate_set_count(const UmiCodeGuardDuplicateSet *set);
#ifdef __cplusplus
}
#endif
#endif
