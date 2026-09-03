/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/providers/c_language_assistance.h
 *
 * PURPOSE:
 *   Publish an offline C completion and quick-documentation provider. The
 *   provider gives useful help before an external language server is ready and
 *   remains available to every Framework-based editor.
 *
 * OWNERSHIP:
 *   Catalogue records are immutable Framework data. Registries and
 *   documentation stores remain owned by their callers.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_PROVIDERS_C_LANGUAGE_ASSISTANCE_H
#define UMICOM_EDITOR_PROVIDERS_C_LANGUAGE_ASSISTANCE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/completion_orchestration.h"
#include "umicom/editor/completion_provider.h"
#include "umicom/editor/quick_documentation.h"
#include "umicom/editor/signature_help_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_C_LANGUAGE_ASSISTANCE_API_VERSION 1U
#define UMI_EDITOR_C_COMPLETION_PROVIDER_ID "editor.completion.c.builtin"
#define UMI_EDITOR_C23_COMPLETION_PROVIDER_ID "editor.completion.c23.builtin"

/** Opaque owner that keeps the related C assistance services together. */
typedef struct UmiEditorCLanguageAssistance UmiEditorCLanguageAssistance;

/** Copy health and counts without exposing the owner's mutable services. */
typedef struct UmiEditorCLanguageAssistanceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorCompletionProviderRegistrySnapshot completion_providers;
    UmiEditorQuickDocumentationSnapshot documentation;
    UmiEditorSignatureHelpSnapshot signatures;
    int available;
} UmiEditorCLanguageAssistanceSnapshot;

/**
 * Describe one built-in C symbol, snippet, type, keyword or header without
 * exposing the provider's private storage.
 */
typedef struct UmiEditorCLanguageAssistanceEntry {
    const char *id;
    const char *label;
    const char *detail;
    const char *insert_text;
    const char *plain_insert_text;
    const char *kind;
    const char *documentation;
    const char *source_uri;
    const char *safety_topic;
    int snippet;
    int header;
} UmiEditorCLanguageAssistanceEntry;

/**
 * Create one complete offline C assistance owner. It registers the `c` and
 * `c23` providers and seeds documentation and signatures atomically.
 */
UmiStatus umi_editor_c_language_assistance_create(
    UmiEditorCLanguageAssistance **out_assistance);

/** Release every service owned by a C assistance instance. */
void umi_editor_c_language_assistance_destroy(
    UmiEditorCLanguageAssistance *assistance);

/** Copy provider, documentation and signature health for UI or diagnostics. */
UmiStatus umi_editor_c_language_assistance_snapshot(
    const UmiEditorCLanguageAssistance *assistance,
    UmiEditorCLanguageAssistanceSnapshot *out_snapshot);

/** Borrow the provider registry so extensions can add compatible providers. */
UmiEditorCompletionProviderRegistry *
umi_editor_c_language_assistance_providers(
    UmiEditorCLanguageAssistance *assistance);

/** Borrow the coordinator used for context-aware completion requests. */
UmiEditorCompletionOrchestration *
umi_editor_c_language_assistance_completion(
    UmiEditorCLanguageAssistance *assistance);

/** Borrow the trusted quick-documentation model. */
UmiEditorQuickDocumentation *
umi_editor_c_language_assistance_documentation(
    UmiEditorCLanguageAssistance *assistance);

/** Borrow the offline callable-signature model. */
UmiEditorSignatureHelpModel *
umi_editor_c_language_assistance_signatures(
    UmiEditorCLanguageAssistance *assistance);

/** Return the number of immutable records in the offline C help catalogue. */
size_t umi_editor_c_language_assistance_count(void);

/** Find one catalogue record by position without transferring ownership. */
const UmiEditorCLanguageAssistanceEntry *
umi_editor_c_language_assistance_at(size_t index);

/** Find one catalogue record by its stable identifier. */
const UmiEditorCLanguageAssistanceEntry *
umi_editor_c_language_assistance_find(const char *entry_id);

/**
 * Register the built-in provider for both `c` and `c23` document identifiers.
 * Re-registering updates the same provider records instead of adding copies.
 */
UmiStatus umi_editor_c_language_assistance_register(
    UmiEditorCompletionProviderRegistry *registry);

/**
 * Add the C catalogue to a caller-owned quick-documentation model so hover,
 * completion-detail and documentation panels can show the same trusted text.
 */
UmiStatus umi_editor_c_language_assistance_populate_documentation(
    UmiEditorQuickDocumentation *documentation);

/**
 * Add callable C library records to a caller-owned signature-help model. The
 * editor can show this data after an opening parenthesis without contacting a
 * network service or transferring ownership of the model.
 */
UmiStatus umi_editor_c_language_assistance_populate_signatures(
    UmiEditorSignatureHelpModel *signatures);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_PROVIDERS_C_LANGUAGE_ASSISTANCE_H */
