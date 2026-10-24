/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/structured_form_internal.h
 * PURPOSE: Share checked identity boundaries between structured form services.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRUCTURED_FORM_INTERNAL_H
#define UMICOM_STRUCTURED_FORM_INTERNAL_H
#include "umicom/application/experience/structured_form.h"
UmiStatus umi_structured_form_identity_check(const UmiStructuredFormSchema *schema,
                                             const UmiStructuredForm *form);
UmiStatus umi_structured_form_draft_check(const UmiStructuredFormSchema *schema,
                                          const UmiStructuredForm *form);
#endif
