/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/requests.h
 *
 * PURPOSE:
 *   Aggregate built-in LSP request/notification builders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUESTS_H
#define UMICOM_LANGUAGE_RUNTIME_REQUESTS_H
#include "umicom/language_runtime/requests/completion.h"
#include "umicom/language_runtime/requests/hover.h"
#include "umicom/language_runtime/requests/signature_help.h"
#include "umicom/language_runtime/requests/definition.h"
#include "umicom/language_runtime/requests/document_symbols.h"
#include "umicom/language_runtime/requests/semantic_tokens.h"
#include "umicom/language_runtime/requests/folding_ranges.h"
#include "umicom/language_runtime/requests/workspace_symbols.h"
#include "umicom/language_runtime/requests/references.h"
#include "umicom/language_runtime/requests/initialize.h"
#include "umicom/language_runtime/requests/initialized.h"
#include "umicom/language_runtime/requests/shutdown.h"
#include "umicom/language_runtime/requests/exit.h"
#include "umicom/language_runtime/requests/did_open.h"
#include "umicom/language_runtime/requests/did_change.h"
#include "umicom/language_runtime/requests/did_save.h"
#include "umicom/language_runtime/requests/did_close.h"
#include "umicom/language_runtime/requests/code_action.h"
#include "umicom/language_runtime/requests/formatting.h"
#include "umicom/language_runtime/requests/rename.h"
#include "umicom/language_runtime/requests/inlay_hints.h"
#endif
