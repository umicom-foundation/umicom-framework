/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/healthcare/weight_log.c
 *
 * PURPOSE:
 *   Validate canonical gram observations through the existing structured form service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/weight_log.h"
#include <string.h>

/* Only domain semantics live here. Framework form services still own revision
 * admission, missing-answer policy, date parsing and serialization. Integer
 * grams avoid floating-point rounding at client/storage boundaries. */
static const UmiApplicationExperienceField fields[] = {
    {.field_id="weight.observed_on", .label="Observation date",
     .help_text="Enter the calendar date of the observation.",
     .kind=UMI_APPLICATION_EXPERIENCE_FIELD_DATE,
     .value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT, .required=1,
     .enforce_length_range=1, .minimum_length=10U, .maximum_length=10U},
    {.field_id="weight.mass_grams", .label="Mass in grams",
     .help_text="Canonical whole grams; no health classification is applied.",
     .kind=UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER,
     .value_type=UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER, .required=1,
     .enforce_number_range=1, .minimum_number=1.0, .maximum_number=1000000000.0},
    {.field_id="weight.note", .label="Note",
     .help_text="Optional personal note; do not copy it into diagnostic logs.",
     .kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
     .value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
     .enforce_length_range=1, .minimum_length=0U, .maximum_length=255U}
};
static const UmiStructuredFormField rules[] = {
    {"weight.observed_on", "", UMI_FORM_TEXT_DATE, 0, 0},
    {"weight.mass_grams", "mass.g", UMI_FORM_TEXT_PLAIN, 0, 0},
    {"weight.note", "", UMI_FORM_TEXT_PLAIN, 0, 0}
};
static const UmiApplicationExperienceFormSchema base = {
    .struct_size=sizeof(UmiApplicationExperienceFormSchema),
    .api_version=UMI_APPLICATION_EXPERIENCE_API_VERSION,
    .form_id="wellness.weight-log", .title="Weight log",
    .description="Record an observation without prescribing a health target.",
    .submit_command_id="wellness.weight-log.save",
    .fields=fields, .field_count=3U
};
static const UmiStructuredFormSchema schema = {&base, 1U, rules, 3U};

const UmiStructuredFormSchema *umi_weight_log_schema(void) { return &schema; }

UmiStatus umi_weight_log_parse_kilograms(const char *text, size_t length,
                                       uint32_t *out_grams)
{
    if (text == NULL || out_grams == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length == 0U || length > 11U) return UMI_STATUS_PARSE_ERROR;
    size_t position=0U, whole_digits=0U, fractional_digits=0U;
    uint64_t whole=0U, fraction=0U;
    while (position < length && text[position] >= '0' && text[position] <= '9') {
        if (++whole_digits > 7U) return UMI_STATUS_PARSE_ERROR;
        whole=whole*10U+(uint64_t)(text[position]-'0');
        ++position;
    }
    if (whole_digits == 0U) return UMI_STATUS_PARSE_ERROR;
    if (position < length) {
        if (text[position++] != '.') return UMI_STATUS_PARSE_ERROR;
        while (position < length && text[position] >= '0' && text[position] <= '9') {
            if (++fractional_digits > 3U) return UMI_STATUS_PARSE_ERROR;
            fraction=fraction*10U+(uint64_t)(text[position]-'0');
            ++position;
        }
        if (fractional_digits == 0U) return UMI_STATUS_PARSE_ERROR;
    }
    if (position != length) return UMI_STATUS_PARSE_ERROR;
    while (fractional_digits++ < 3U) fraction*=10U;
    const uint64_t grams=whole*1000U+fraction;
    if (grams == 0U || grams > UMI_WEIGHT_LOG_MAX_GRAMS)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_grams=(uint32_t)grams;
    return UMI_STATUS_OK;
}

UmiStatus umi_weight_log_read(const UmiStructuredForm *form, UmiWeightLogEntry *out)
{
    if (form == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiApplicationExperienceValidationReport report;
    const UmiStatus status=umi_structured_form_validate(&schema, form, 1, &report);
    if (status != UMI_STATUS_OK) return status;
    if (report.error_count != 0U) return UMI_STATUS_INVALID_STATE;
    UmiWeightLogEntry candidate={0};
    memcpy(candidate.observed_on, form->answers[0].value.text_value, 11U);
    candidate.mass_grams=(uint32_t)form->answers[1].value.integer_value;
    if (form->answers[2].state == UMI_FORM_KNOWN)
        memcpy(candidate.note, form->answers[2].value.text_value,
               strlen(form->answers[2].value.text_value)+1U);
    *out=candidate;
    return UMI_STATUS_OK;
}
