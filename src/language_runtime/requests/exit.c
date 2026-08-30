/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/exit.c
 *
 * PURPOSE:
 *   Implement LSP exit lifecycle message.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/exit.h"
UmiStatus umi_language_runtime_request_exit(UmiLanguageRuntimeServer*s){return s?umi_language_runtime_server_send_notification(s,"exit",NULL):UMI_STATUS_INVALID_ARGUMENT;}
