/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/hover.h
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
#ifndef UMICOM_LANGUAGE_HOVER_H
#define UMICOM_LANGUAGE_HOVER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_HOVER_CAPACITY 2048U
#define UMI_LANGUAGE_HOVER_API_VERSION 1U

/**
 * Represent the language hover snapshot data shared with callers of this public contract.
 */
typedef struct UmiLanguageHoverSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char contents[2048];
    uint32_t line;
    uint32_t column;
    uint32_t start_line;
    uint32_t start_column;
    uint32_t end_line;
    uint32_t end_column;
    uint64_t revision;
} UmiLanguageHoverSnapshot;

/**
 * Represent the language hover registry data shared with callers of this public contract.
 */
typedef struct UmiLanguageHoverRegistry UmiLanguageHoverRegistry;

/**
 * Initialise language hover registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_hover_registry_create(UmiLanguageHoverRegistry **out_registry);
/**
 * Release or reset state held by language hover registry so the same storage can be reused
 * safely.
 */
void umi_language_hover_registry_destroy(UmiLanguageHoverRegistry *registry);
/**
 * Provide the language hover registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_hover_registry_upsert(UmiLanguageHoverRegistry *registry, const UmiLanguageHoverSnapshot *item);
/**
 * Remove language hover registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_hover_registry_remove(UmiLanguageHoverRegistry *registry, const char *id);
/**
 * Find language hover registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_hover_registry_find(const UmiLanguageHoverRegistry *registry, const char *id, UmiLanguageHoverSnapshot *out_item);
/**
 * Find language hover registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_hover_registry_at(const UmiLanguageHoverRegistry *registry, size_t index, UmiLanguageHoverSnapshot *out_item);
/**
 * Return the number of records represented by language hover registry without changing
 * their state.
 */
size_t umi_language_hover_registry_count(const UmiLanguageHoverRegistry *registry);
/**
 * Provide the language hover registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_hover_registry_revision(const UmiLanguageHoverRegistry *registry);
/**
 * Release or reset state held by language hover registry so the same storage can be reused
 * safely.
 */
void umi_language_hover_registry_clear(UmiLanguageHoverRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
