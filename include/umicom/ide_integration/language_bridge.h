/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/language_bridge.h
 *
 * PURPOSE:
 *   Project provider-neutral language symbols and diagnostics into the common
 *   IDE navigation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_LANGUAGE_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_LANGUAGE_BRIDGE_H
#include "umicom/language/service.h"
#include "umicom/ide_integration/location.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ide language symbol target operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_language_symbol_target(
    UmiLanguageService *language,
    const char *symbol_id,
    UmiIdeNavigationTarget *out_target);

/**
 * Provide the ide language diagnostic target operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_language_diagnostic_target(
    UmiLanguageService *language,
    size_t diagnostic_index,
    UmiIdeNavigationTarget *out_target);

#ifdef __cplusplus
}
#endif
#endif
