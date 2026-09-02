/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/initialized.c
 *
 * PURPOSE:
 *   Implement LSP initialized lifecycle message.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/initialized.h"
/*
 * Provide the language runtime request initialized operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_initialized(UmiLanguageRuntimeServer*s){return s?umi_language_runtime_server_send_notification(s,"initialized",NULL):UMI_STATUS_INVALID_ARGUMENT;}
