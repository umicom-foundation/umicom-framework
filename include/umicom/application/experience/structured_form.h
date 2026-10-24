/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/structured_form.h
 * PURPOSE: Preserve versioned typed answers, explicit missing states and units.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_H
#define UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_H
#include "umicom/application/experience/form_schema.h"
#include "umicom/application/experience/form_validation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FORM_UNIT_CAPACITY 64U
#define UMI_FORM_REASON_CAPACITY 256U
typedef enum UmiFormAnswerState {
  UMI_FORM_UNANSWERED = 0,
  UMI_FORM_KNOWN = 1,
  UMI_FORM_UNKNOWN = 2,
  UMI_FORM_NOT_APPLICABLE = 3
} UmiFormAnswerState;
typedef enum UmiFormTextFormat {
  UMI_FORM_TEXT_PLAIN = 0,
  UMI_FORM_TEXT_DATE = 1,
  UMI_FORM_TEXT_OFFSET_TIMESTAMP = 2
} UmiFormTextFormat;
typedef struct UmiFormAnswer {
  UmiFormAnswerState state;
  UmiApplicationExperienceValue value;
  char unit_id[UMI_FORM_UNIT_CAPACITY];
  char reason[UMI_FORM_REASON_CAPACITY];
} UmiFormAnswer;
/* Each rule corresponds to the base field at the same index. Unit identifiers
 * are exact, application-defined tokens (for example mass.kg or money.GBP).
 * Validation never converts or silently strips a unit. Empty means unitless. */
typedef struct UmiStructuredFormField {
  const char *field_id;
  const char *unit_id;
  UmiFormTextFormat text_format;
  int allow_unknown;
  int allow_not_applicable;
} UmiStructuredFormField;
/* The immutable base and rules are borrowed. This version identifies content
 * semantics, independently of the existing descriptor ABI api_version. Change
 * it when fields, units or interpretation change; migration is explicit. */
typedef struct UmiStructuredFormSchema {
  const UmiApplicationExperienceFormSchema *base;
  uint64_t version;
  const UmiStructuredFormField *rules;
  size_t rule_count;
} UmiStructuredFormSchema;
/* Owned, bounded answers in schema order. Allocate on the heap when embedded in
 * larger histories. The schema must remain immutable for the editing session. */
typedef struct UmiStructuredForm {
  char form_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  uint64_t schema_version;
  uint64_t revision;
  uint64_t saved_revision;
  size_t count;
  UmiFormAnswer answers[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY];
} UmiStructuredForm;
UmiStatus umi_structured_form_schema_validate(const UmiStructuredFormSchema *schema);
/* Known values reuse existing field constraints. Non-known answers carry NONE
 * and no unit; explicit unknown/not-applicable require a nonempty reason. */
UmiStatus umi_structured_form_answer_validate(const UmiStructuredFormSchema *schema, size_t index,
                                              const UmiFormAnswer *answer, int submitting);
UmiStatus umi_structured_form_init(const UmiStructuredFormSchema *schema, UmiStructuredForm *out);
/* Structural errors preserve report. A valid structure returns OK and reports
 * field problems. Draft validation allows unanswered required fields; submit
 * validation requires an answer, including explicitly permitted missing states. */
UmiStatus umi_structured_form_validate(const UmiStructuredFormSchema *schema,
                                       const UmiStructuredForm *form, int submitting,
                                       UmiApplicationExperienceValidationReport *report);
/* Expected revision binds an edit to the model the frontend actually reviewed.
 * Failed edits and true no-ops leave all state untouched. Read-only is enforced. */
UmiStatus umi_structured_form_set(const UmiStructuredFormSchema *schema, UmiStructuredForm *form,
                                  uint64_t expected_revision, const char *field_id,
                                  const UmiFormAnswer *answer);
/* A delayed save acknowledgement cannot mark a newer edit clean. */
UmiStatus umi_structured_form_mark_saved(UmiStructuredForm *form, uint64_t saved_revision);
int umi_form_answer_equal(const UmiFormAnswer *left, const UmiFormAnswer *right);
const char *umi_form_answer_state_text(UmiFormAnswerState state);
#ifdef __cplusplus
}
#endif
#endif
