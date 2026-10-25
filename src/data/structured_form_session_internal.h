/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/structured_form_session_internal.h
 *
 * PURPOSE:
 *   Private ownership shared by typed sessions and reviewed conflict adoption.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Private ownership shared by typed sessions and reviewed conflict adoption. */
#ifndef UMICOM_STRUCTURED_FORM_SESSION_INTERNAL_H
#define UMICOM_STRUCTURED_FORM_SESSION_INTERNAL_H
#include "umicom/data/structured_form_session.h"
struct UmiStructuredFormSession {
  const UmiStructuredFormSchema *schema;
  UmiRecordDraft *draft;
  uint64_t generation;
  UmiStructuredForm current, baseline, pending;
};
#endif
