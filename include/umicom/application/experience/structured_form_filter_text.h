/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/structured_form_filter_text.h
 *
 * PURPOSE:
 *   One textual filter boundary for native and web clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * One textual filter boundary for native and web clients. */
#ifndef UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_FILTER_TEXT_H
#define UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_FILTER_TEXT_H
#include "umicom/application/experience/structured_form_filter.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFormFilterTextTerm {
  char field_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  UmiFormFilterOperator operation;
  UmiFormAnswerState state;
  char value[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
} UmiFormFilterTextTerm;
/* Value operators require KNOWN. STATE requires empty value and uses state.
 * Parse numeric values through the shared locale-independent ASCII codec.
 * The selected schema supplies the exact unit; clients must display that unit
 * beside the input, not label the same text as a converted quantity. Numeric
 * thresholds retain the filter service's outside-field-range semantics.
 * All fixed strings must terminate. Outputs remain unchanged on failure and
 * may alias input storage. Formatting emits only meaningful active values. */
UmiStatus umi_structured_form_filter_term_parse(const UmiStructuredFormSchema *schema,
                                                const UmiFormFilterTextTerm *text,
                                                UmiFormFilterTerm *out);
UmiStatus umi_structured_form_filter_term_format(const UmiStructuredFormSchema *schema,
                                                 const UmiFormFilterTerm *term,
                                                 UmiFormFilterTextTerm *out);
#ifdef __cplusplus
}
#endif
#endif
