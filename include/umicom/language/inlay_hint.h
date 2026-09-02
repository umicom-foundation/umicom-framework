/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/inlay_hint.h
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
#ifndef UMICOM_LANGUAGE_INLAY_HINT_H
#define UMICOM_LANGUAGE_INLAY_HINT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INLAY_HINT_CAPACITY 2048U
#define UMI_LANGUAGE_INLAY_HINT_API_VERSION 1U

/**
 * Represent the language inlay hint snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageInlayHintSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char label[512];
    char kind[64];
    uint32_t line;
    uint32_t column;
    int visible;
    uint64_t revision;
} UmiLanguageInlayHintSnapshot;

/**
 * Represent the language inlay hint registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageInlayHintRegistry UmiLanguageInlayHintRegistry;

/**
 * Initialise language inlay hint registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_inlay_hint_registry_create(UmiLanguageInlayHintRegistry **out_registry);
/**
 * Release or reset state held by language inlay hint registry so the same storage can be
 * reused safely.
 */
void umi_language_inlay_hint_registry_destroy(UmiLanguageInlayHintRegistry *registry);
/**
 * Provide the language inlay hint registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_inlay_hint_registry_upsert(UmiLanguageInlayHintRegistry *registry, const UmiLanguageInlayHintSnapshot *item);
/**
 * Remove language inlay hint registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_inlay_hint_registry_remove(UmiLanguageInlayHintRegistry *registry, const char *id);
/**
 * Find language inlay hint registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_inlay_hint_registry_find(const UmiLanguageInlayHintRegistry *registry, const char *id, UmiLanguageInlayHintSnapshot *out_item);
/**
 * Find language inlay hint registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_inlay_hint_registry_at(const UmiLanguageInlayHintRegistry *registry, size_t index, UmiLanguageInlayHintSnapshot *out_item);
/**
 * Return the number of records represented by language inlay hint registry without
 * changing their state.
 */
size_t umi_language_inlay_hint_registry_count(const UmiLanguageInlayHintRegistry *registry);
/**
 * Provide the language inlay hint registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_inlay_hint_registry_revision(const UmiLanguageInlayHintRegistry *registry);
/**
 * Release or reset state held by language inlay hint registry so the same storage can be
 * reused safely.
 */
void umi_language_inlay_hint_registry_clear(UmiLanguageInlayHintRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
