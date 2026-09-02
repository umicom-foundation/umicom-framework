/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/signature.h
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
#ifndef UMICOM_LANGUAGE_SIGNATURE_H
#define UMICOM_LANGUAGE_SIGNATURE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_SIGNATURE_CAPACITY 2048U
#define UMI_LANGUAGE_SIGNATURE_API_VERSION 1U

/**
 * Represent the language signature snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageSignatureSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char label[512];
    char documentation[1024];
    uint32_t active_parameter;
    uint32_t line;
    uint32_t column;
    uint64_t revision;
} UmiLanguageSignatureSnapshot;

/**
 * Represent the language signature registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageSignatureRegistry UmiLanguageSignatureRegistry;

/**
 * Initialise language signature registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_signature_registry_create(UmiLanguageSignatureRegistry **out_registry);
/**
 * Release or reset state held by language signature registry so the same storage can be
 * reused safely.
 */
void umi_language_signature_registry_destroy(UmiLanguageSignatureRegistry *registry);
/**
 * Provide the language signature registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_signature_registry_upsert(UmiLanguageSignatureRegistry *registry, const UmiLanguageSignatureSnapshot *item);
/**
 * Remove language signature registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_signature_registry_remove(UmiLanguageSignatureRegistry *registry, const char *id);
/**
 * Find language signature registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_signature_registry_find(const UmiLanguageSignatureRegistry *registry, const char *id, UmiLanguageSignatureSnapshot *out_item);
/**
 * Find language signature registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_signature_registry_at(const UmiLanguageSignatureRegistry *registry, size_t index, UmiLanguageSignatureSnapshot *out_item);
/**
 * Return the number of records represented by language signature registry without changing
 * their state.
 */
size_t umi_language_signature_registry_count(const UmiLanguageSignatureRegistry *registry);
/**
 * Provide the language signature registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_signature_registry_revision(const UmiLanguageSignatureRegistry *registry);
/**
 * Release or reset state held by language signature registry so the same storage can be
 * reused safely.
 */
void umi_language_signature_registry_clear(UmiLanguageSignatureRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
