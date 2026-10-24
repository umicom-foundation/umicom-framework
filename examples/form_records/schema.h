/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A fictional equipment register shared by portable and native SDK examples. */
#ifndef UMICOM_FORM_RECORD_EXAMPLE_SCHEMA_H
#define UMICOM_FORM_RECORD_EXAMPLE_SCHEMA_H
#include "umicom/application/experience/structured_form.h"
static const UmiApplicationExperienceField equipment_fields[] = {
    {.field_id = "asset.label",
     .label = "Equipment label",
     .help_text = "Use fictional equipment only.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
     .required = 1,
     .enforce_length_range = 1,
     .minimum_length = 1,
     .maximum_length = 80},
    {.field_id = "asset.location",
     .label = "Location",
     .help_text = "A fictional shelf or room.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
     .required = 1,
     .enforce_length_range = 1,
     .minimum_length = 1,
     .maximum_length = 120}};
static const UmiStructuredFormField equipment_rules[] = {
    {"asset.label", "", UMI_FORM_TEXT_PLAIN, 0, 0},
    {"asset.location", "", UMI_FORM_TEXT_PLAIN, 1, 0}};
static const UmiApplicationExperienceFormSchema equipment_base = {
    sizeof(UmiApplicationExperienceFormSchema),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "example.equipment",
    "Fictional equipment register",
    "Inspect and reconcile local records.",
    "equipment.save",
    equipment_fields,
    2};
static const UmiStructuredFormSchema equipment_schema = {&equipment_base, 1, equipment_rules, 2};
#endif
