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

/**
 * Represent the language folding range snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the language folding range registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageFoldingRangeRegistry UmiLanguageFoldingRangeRegistry;

/**
 * Initialise language folding range registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_folding_range_registry_create(UmiLanguageFoldingRangeRegistry **out_registry);
/**
 * Release or reset state held by language folding range registry so the same storage can
 * be reused safely.
 */
void umi_language_folding_range_registry_destroy(UmiLanguageFoldingRangeRegistry *registry);
/**
 * Provide the language folding range registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_folding_range_registry_upsert(UmiLanguageFoldingRangeRegistry *registry, const UmiLanguageFoldingRangeSnapshot *item);
/**
 * Remove language folding range registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_language_folding_range_registry_remove(UmiLanguageFoldingRangeRegistry *registry, const char *id);
/**
 * Find language folding range registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_language_folding_range_registry_find(const UmiLanguageFoldingRangeRegistry *registry, const char *id, UmiLanguageFoldingRangeSnapshot *out_item);
/**
 * Find language folding range registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_language_folding_range_registry_at(const UmiLanguageFoldingRangeRegistry *registry, size_t index, UmiLanguageFoldingRangeSnapshot *out_item);
/**
 * Return the number of records represented by language folding range registry without
 * changing their state.
 */
size_t umi_language_folding_range_registry_count(const UmiLanguageFoldingRangeRegistry *registry);
/**
 * Provide the language folding range registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_language_folding_range_registry_revision(const UmiLanguageFoldingRangeRegistry *registry);
/**
 * Release or reset state held by language folding range registry so the same storage can
 * be reused safely.
 */
void umi_language_folding_range_registry_clear(UmiLanguageFoldingRangeRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
