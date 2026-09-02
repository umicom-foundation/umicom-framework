/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/types.c
 *
 * PURPOSE:
 *   Implement stable diagnostic text for IDE integration records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/types.h"

/* Provide the ide domain text operation used by this module and its client applications. */
const char *umi_ide_domain_text(UmiIdeIntegrationDomain domain)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (domain) {
        case UMI_IDE_DOMAIN_EDITOR: return "editor";
        case UMI_IDE_DOMAIN_PROBLEMS: return "problems";
        case UMI_IDE_DOMAIN_TESTS: return "tests";
        case UMI_IDE_DOMAIN_SOURCE_CONTROL: return "source-control";
        case UMI_IDE_DOMAIN_DEBUG: return "debug";
        case UMI_IDE_DOMAIN_LANGUAGE: return "language";
        case UMI_IDE_DOMAIN_AI: return "ai";
        case UMI_IDE_DOMAIN_BUILD: return "build";
        case UMI_IDE_DOMAIN_WORKSPACE: return "workspace";
        default: return "unknown";
    }
}

/*
 * Provide the ide navigation reason text operation used by this module and its client
 * applications.
 */
const char *umi_ide_navigation_reason_text(UmiIdeNavigationReason reason)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (reason) {
        case UMI_IDE_NAVIGATION_DIRECT: return "direct";
        case UMI_IDE_NAVIGATION_PROBLEM: return "problem";
        case UMI_IDE_NAVIGATION_TEST: return "test";
        case UMI_IDE_NAVIGATION_SOURCE_CONTROL: return "source-control";
        case UMI_IDE_NAVIGATION_DEBUG: return "debug";
        case UMI_IDE_NAVIGATION_SYMBOL: return "symbol";
        case UMI_IDE_NAVIGATION_REFERENCE: return "reference";
        case UMI_IDE_NAVIGATION_AI: return "ai";
        default: return "unknown";
    }
}

/*
 * Provide the ide inline state text operation used by this module and its client
 * applications.
 */
const char *umi_ide_inline_state_text(UmiIdeInlineSuggestionState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_IDE_INLINE_IDLE: return "idle";
        case UMI_IDE_INLINE_REQUESTED: return "requested";
        case UMI_IDE_INLINE_READY: return "ready";
        case UMI_IDE_INLINE_ACCEPTED: return "accepted";
        case UMI_IDE_INLINE_REJECTED: return "rejected";
        case UMI_IDE_INLINE_CANCELLED: return "cancelled";
        case UMI_IDE_INLINE_FAILED: return "failed";
        default: return "unknown";
    }
}

/*
 * Provide the ide gate state text operation used by this module and its client
 * applications.
 */
const char *umi_ide_gate_state_text(UmiIdeWorkflowGateState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_IDE_GATE_UNKNOWN: return "unknown";
        case UMI_IDE_GATE_PASS: return "pass";
        case UMI_IDE_GATE_WARN: return "warn";
        case UMI_IDE_GATE_BLOCK: return "block";
        default: return "unknown";
    }
}
