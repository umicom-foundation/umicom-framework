/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/structured_form_filter.h
 *
 * PURPOSE:
 *   Typed filters preserve unit and missing-answer semantics across frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Typed filters preserve unit and missing-answer semantics across frontends. */
#ifndef UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_FILTER_H
#define UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_FILTER_H
#include "umicom/application/experience/structured_form.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FORM_FILTER_CAPACITY 8U
typedef enum UmiFormFilterOperator {
  UMI_FORM_FILTER_STATE = 0,
  UMI_FORM_FILTER_EQUAL,
  UMI_FORM_FILTER_CONTAINS,
  UMI_FORM_FILTER_LESS,
  UMI_FORM_FILTER_LESS_EQUAL,
  UMI_FORM_FILTER_GREATER,
  UMI_FORM_FILTER_GREATER_EQUAL
} UmiFormFilterOperator;
typedef struct UmiFormFilterTerm {
  char field_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  UmiFormFilterOperator operation;
  UmiFormAnswer operand;
} UmiFormFilterTerm;
typedef struct UmiStructuredFormFilter {
  size_t count;
  UmiFormFilterTerm terms[UMI_FORM_FILTER_CAPACITY];
} UmiStructuredFormFilter;
/* All active terms are ANDed in order; zero terms match a valid draft.
 * STATE uses operand.state only; value must be NONE and unit/reason empty.
 * Other terms require KNOWN, the exact field type/unit and no missing reason.
 * EQUAL uses validated field values. Numeric comparisons accept finite typed
 * thresholds outside the field's allowed range; no coercion or subtraction is
 * used. CONTAINS is a case-sensitive byte substring for plain text/multiline
 * fields only, not choice labels, dates or timestamps. An empty needle matches
 * any known text. Missing answers do not match value predicates.
 * Filters are not access control. Schemas remain borrowed immutable data. */
UmiStatus umi_structured_form_filter_validate(const UmiStructuredFormSchema *schema,
                                              const UmiStructuredFormFilter *filter);
/* Validate the entire draft before evaluating every term, even when an earlier
 * term does not match. Output is unchanged on invalid filter or form. */
UmiStatus umi_structured_form_filter_match(const UmiStructuredFormSchema *schema,
                                           const UmiStructuredForm *form,
                                           const UmiStructuredFormFilter *filter, bool *out);
/* Deterministic digest of semantic schema identity and ordered filter operands.
 * It binds a paging query; it is not a signature or a schema authenticity check.
 * Inactive array slots and value members are excluded; output unchanged on error. */
UmiStatus umi_structured_form_filter_digest(const UmiStructuredFormSchema *schema,
                                            const UmiStructuredFormFilter *filter, char out[65]);
#ifdef __cplusplus
}
#endif
#endif
