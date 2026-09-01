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

typedef struct UmiLanguageDiagnosticRegistry UmiLanguageDiagnosticRegistry;

UmiStatus umi_language_diagnostic_registry_create(UmiLanguageDiagnosticRegistry **out_registry);
void umi_language_diagnostic_registry_destroy(UmiLanguageDiagnosticRegistry *registry);
UmiStatus umi_language_diagnostic_registry_upsert(UmiLanguageDiagnosticRegistry *registry, const UmiLanguageDiagnosticSnapshot *item);
UmiStatus umi_language_diagnostic_registry_remove(UmiLanguageDiagnosticRegistry *registry, const char *id);
UmiStatus umi_language_diagnostic_registry_find(const UmiLanguageDiagnosticRegistry *registry, const char *id, UmiLanguageDiagnosticSnapshot *out_item);
UmiStatus umi_language_diagnostic_registry_at(const UmiLanguageDiagnosticRegistry *registry, size_t index, UmiLanguageDiagnosticSnapshot *out_item);
size_t umi_language_diagnostic_registry_count(const UmiLanguageDiagnosticRegistry *registry);
uint64_t umi_language_diagnostic_registry_revision(const UmiLanguageDiagnosticRegistry *registry);
void umi_language_diagnostic_registry_clear(UmiLanguageDiagnosticRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
