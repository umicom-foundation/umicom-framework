/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/initialized.c
 *
 * PURPOSE:
 *   Implement LSP initialized lifecycle message.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/initialized.h"
UmiStatus umi_language_runtime_request_initialized(UmiLanguageRuntimeServer*s){return s?umi_language_runtime_server_send_notification(s,"initialized",NULL):UMI_STATUS_INVALID_ARGUMENT;}
