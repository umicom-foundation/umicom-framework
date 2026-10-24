/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/structured_form_text.h
 * PURPOSE: Share editable answer text between native and browser clients.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_TEXT_H
#define UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_TEXT_H
#include "umicom/application/experience/structured_form.h"
#ifdef __cplusplus
extern "C" {
#endif
/* This is an owned text boundary, not a second form model. All arrays must be
 * terminated. Known answers have no reason; missing answers have no value/unit.
 * Frontends may keep inactive drafts separately but must not submit them here. */
typedef struct UmiFormTextAnswer {
  UmiFormAnswerState state;
  char value[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
  char unit_id[UMI_FORM_UNIT_CAPACITY];
  char reason[UMI_FORM_REASON_CAPACITY];
} UmiFormTextAnswer;
UmiStatus umi_structured_form_answer_parse(const UmiStructuredFormSchema *schema, size_t index,
                                           const UmiFormTextAnswer *text, UmiFormAnswer *out);
UmiStatus umi_structured_form_answer_format(const UmiStructuredFormSchema *schema, size_t index,
                                            const UmiFormAnswer *answer, UmiFormTextAnswer *out);
#ifdef __cplusplus
}
#endif
#endif
