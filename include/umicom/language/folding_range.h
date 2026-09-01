/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/folding_range.h
 *
 * PURPOSE:
 *   Define a provider-neutral language-intelligence record that can be backed by LSP, native analysers or future Umicom language engines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_LANGUAGE_FOLDING_RANGE_H
#define UMICOM_LANGUAGE_FOLDING_RANGE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_FOLDING_RANGE_CAPACITY 2048U
#define UMI_LANGUAGE_FOLDING_RANGE_API_VERSION 1U

typedef struct UmiLanguageFoldingRangeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char kind[64];
    uint32_t start_line;
    uint32_t end_line;
    int collapsed;
    uint64_t revision;
} UmiLanguageFoldingRangeSnapshot;

typedef struct UmiLanguageFoldingRangeRegistry UmiLanguageFoldingRangeRegistry;

UmiStatus umi_language_folding_range_registry_create(UmiLanguageFoldingRangeRegistry **out_registry);
void umi_language_folding_range_registry_destroy(UmiLanguageFoldingRangeRegistry *registry);
UmiStatus umi_language_folding_range_registry_upsert(UmiLanguageFoldingRangeRegistry *registry, const UmiLanguageFoldingRangeSnapshot *item);
UmiStatus umi_language_folding_range_registry_remove(UmiLanguageFoldingRangeRegistry *registry, const char *id);
UmiStatus umi_language_folding_range_registry_find(const UmiLanguageFoldingRangeRegistry *registry, const char *id, UmiLanguageFoldingRangeSnapshot *out_item);
UmiStatus umi_language_folding_range_registry_at(const UmiLanguageFoldingRangeRegistry *registry, size_t index, UmiLanguageFoldingRangeSnapshot *out_item);
size_t umi_language_folding_range_registry_count(const UmiLanguageFoldingRangeRegistry *registry);
uint64_t umi_language_folding_range_registry_revision(const UmiLanguageFoldingRangeRegistry *registry);
void umi_language_folding_range_registry_clear(UmiLanguageFoldingRangeRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
