/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/language.h
 *
 * PURPOSE:
 *   Aggregate the complete provider-neutral Umicom language-intelligence API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef FRAMEWORK_INCLUDE_UMICOM_LANGUAGE_LANGUAGE_H
#define FRAMEWORK_INCLUDE_UMICOM_LANGUAGE_LANGUAGE_H

#include "umicom/language/definition.h"
#include "umicom/language/provider.h"
#include "umicom/language/document.h"
#include "umicom/language/symbol.h"
#include "umicom/language/completion.h"
#include "umicom/language/hover.h"
#include "umicom/language/signature.h"
#include "umicom/language/diagnostic.h"
#include "umicom/language/code_action.h"
#include "umicom/language/formatting.h"
#include "umicom/language/reference.h"
#include "umicom/language/rename.h"
#include "umicom/language/semantic_token.h"
#include "umicom/language/inlay_hint.h"
#include "umicom/language/folding_range.h"
#include "umicom/language/server_profile.h"
#include "umicom/language/client_session.h"
#include "umicom/language/request_ledger.h"
#include "umicom/language/builtins.h"
#include "umicom/language/profile.h"
#include "umicom/language/problem_report.h"
#include "umicom/language/navigation_history.h"
#include "umicom/language/service.h"

#endif
