/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/language_bridge.c
 *
 * PURPOSE:
 *   Implement language-intelligence cross-navigation using existing registries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/language_bridge.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the ide language symbol target operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_language_symbol_target(
    UmiLanguageService *language,
    const char *symbol_id,
    UmiIdeNavigationTarget *out_target)
{
    UmiLanguageSymbolSnapshot symbol;
    UmiLanguageSymbolRegistry *registry;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (language == NULL || symbol_id == NULL ||
        symbol_id[0] == '\0' || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_language_service_symbol(language);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_language_symbol_registry_find(
        registry,
        symbol_id,
        &symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_target, 0, sizeof(*out_target));
    out_target->domain = UMI_IDE_DOMAIN_LANGUAGE;
    out_target->reason = UMI_IDE_NAVIGATION_SYMBOL;

    (void)snprintf(
        out_target->subject_id,
        sizeof(out_target->subject_id),
        "%s",
        symbol.id);
    (void)snprintf(
        out_target->label,
        sizeof(out_target->label),
        "%.180s | %.60s",
        symbol.name,
        symbol.kind);

    umi_ide_location_init(&out_target->location);

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (symbol.document_id[0] != '\0') {
        status = umi_ide_location_set_uri(
            &out_target->location,
            symbol.document_id,
            symbol.line,
            symbol.column);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        out_target->location.end_line = symbol.end_line;
        out_target->location.end_column = symbol.end_column;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ide language diagnostic target operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_language_diagnostic_target(
    UmiLanguageService *language,
    size_t diagnostic_index,
    UmiIdeNavigationTarget *out_target)
{
    UmiLanguageDiagnosticSnapshot diagnostic;
    UmiLanguageDiagnosticRegistry *registry;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (language == NULL || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_language_service_diagnostic(language);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_language_diagnostic_registry_at(
        registry,
        diagnostic_index,
        &diagnostic);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_target, 0, sizeof(*out_target));
    out_target->domain = UMI_IDE_DOMAIN_LANGUAGE;
    out_target->reason = UMI_IDE_NAVIGATION_PROBLEM;

    (void)snprintf(
        out_target->subject_id,
        sizeof(out_target->subject_id),
        "%s",
        diagnostic.id);
    (void)snprintf(
        out_target->label,
        sizeof(out_target->label),
        "%.240s",
        diagnostic.message);

    umi_ide_location_init(&out_target->location);

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (diagnostic.document_id[0] != '\0') {
        status = umi_ide_location_set_uri(
            &out_target->location,
            diagnostic.document_id,
            diagnostic.line,
            diagnostic.column);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        out_target->location.end_line = diagnostic.end_line;
        out_target->location.end_column = diagnostic.end_column;
    }

    return UMI_STATUS_OK;
}
