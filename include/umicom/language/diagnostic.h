/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/diagnostic.h
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
#ifndef UMICOM_LANGUAGE_DIAGNOSTIC_H
#define UMICOM_LANGUAGE_DIAGNOSTIC_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_DIAGNOSTIC_CAPACITY 2048U
#define UMI_LANGUAGE_DIAGNOSTIC_API_VERSION 1U

/**
 * Represent the language diagnostic snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageDiagnosticSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char message[1024];
    char source[128];
    char code[128];
    int severity;
    uint32_t line;
    uint32_t column;
    uint32_t end_line;
    uint32_t end_column;
    uint64_t revision;
} UmiLanguageDiagnosticSnapshot;

/**
 * Represent the language diagnostic registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageDiagnosticRegistry UmiLanguageDiagnosticRegistry;

/**
 * Initialise language diagnostic registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_diagnostic_registry_create(UmiLanguageDiagnosticRegistry **out_registry);
/**
 * Release or reset state held by language diagnostic registry so the same storage can be
 * reused safely.
 */
void umi_language_diagnostic_registry_destroy(UmiLanguageDiagnosticRegistry *registry);
/**
 * Provide the language diagnostic registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_diagnostic_registry_upsert(UmiLanguageDiagnosticRegistry *registry, const UmiLanguageDiagnosticSnapshot *item);
/**
 * Remove language diagnostic registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_diagnostic_registry_remove(UmiLanguageDiagnosticRegistry *registry, const char *id);
/**
 * Find language diagnostic registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_diagnostic_registry_find(const UmiLanguageDiagnosticRegistry *registry, const char *id, UmiLanguageDiagnosticSnapshot *out_item);
/**
 * Find language diagnostic registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_diagnostic_registry_at(const UmiLanguageDiagnosticRegistry *registry, size_t index, UmiLanguageDiagnosticSnapshot *out_item);
/**
 * Return the number of records represented by language diagnostic registry without
 * changing their state.
 */
size_t umi_language_diagnostic_registry_count(const UmiLanguageDiagnosticRegistry *registry);
/**
 * Provide the language diagnostic registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_diagnostic_registry_revision(const UmiLanguageDiagnosticRegistry *registry);
/**
 * Release or reset state held by language diagnostic registry so the same storage can be
 * reused safely.
 */
void umi_language_diagnostic_registry_clear(UmiLanguageDiagnosticRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
