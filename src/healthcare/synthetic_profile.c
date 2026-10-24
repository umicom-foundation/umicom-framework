/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * One immutable schema keeps native/web answers and stored versions aligned.
 * Missing answers and fictional contact permission remain explicit states. */
#include "umicom/healthcare/synthetic_profile.h"
#include <string.h>
static const UmiApplicationExperienceFieldOption contact_methods[] = {
    {"none", "No preferred method", 0},
    {"email", "Email", 0},
    {"telephone", "Telephone", 0},
    {"post", "Post", 0}};
static const UmiApplicationExperienceField fields[] = {
    {.field_id = "profile.label",
     .label = "Fictional display name",
     .help_text = "Use invented data only.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
     .required = 1,
     .enforce_length_range = 1,
     .minimum_length = 1,
     .maximum_length = 120},
    {.field_id = "profile.birth-date",
     .label = "Fictional date of birth",
     .help_text = "Calendar date in YYYY-MM-DD form; no age or clinical decision is calculated.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT},
    {.field_id = "profile.contact-method",
     .label = "Preferred contact method",
     .help_text = "A preference for this synthetic exercise; no contact is made.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_CHOICE,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
     .options = contact_methods,
     .option_count = sizeof(contact_methods) / sizeof(contact_methods[0])},
    {.field_id = "profile.contact-permission",
     .label = "Fictional contact permission",
     .help_text = "Choose an explicit true or false answer. This is not legally verified consent.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_BOOLEAN,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN,
     .required = 1},
    {.field_id = "profile.source",
     .label = "Source description",
     .help_text = "Describe the fictional source of these answers.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
     .required = 1,
     .enforce_length_range = 1,
     .minimum_length = 1,
     .maximum_length = 160},
    {.field_id = "profile.note",
     .label = "Administrative note",
     .help_text = "Fictional administrative context, without diagnoses, prescriptions or "
                  "identifying details.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_MULTILINE,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
     .enforce_length_range = 1,
     .maximum_length = 400}};
static const UmiStructuredFormField rules[] = {
    {"profile.label", "", UMI_FORM_TEXT_PLAIN, 0, 0},
    {"profile.birth-date", "", UMI_FORM_TEXT_DATE, 1, 0},
    {"profile.contact-method", "", UMI_FORM_TEXT_PLAIN, 1, 1},
    {"profile.contact-permission", "", UMI_FORM_TEXT_PLAIN, 0, 0},
    {"profile.source", "", UMI_FORM_TEXT_PLAIN, 0, 0},
    {"profile.note", "", UMI_FORM_TEXT_PLAIN, 1, 1}};
static const UmiApplicationExperienceFormSchema base = {
    sizeof(base),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "healthcare.synthetic-profile",
    "Synthetic administrative profile",
    "Fictional data only. This workspace does not provide clinical care.",
    "healthcare.profile.save",
    fields,
    sizeof(fields) / sizeof(fields[0])};
static const UmiStructuredFormSchema schema = {&base, 1, rules, sizeof(rules) / sizeof(rules[0])};
const UmiStructuredFormSchema *umi_healthcare_synthetic_profile_schema(void) { return &schema; }
UmiStatus umi_healthcare_synthetic_profile_key_validate(const char *key) {
  if (key == NULL || strlen(key) != 10 || memcmp(key, "SYN-", 4) != 0)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 4; i < 10; ++i)
    if (key[i] < '0' || key[i] > '9')
      return UMI_STATUS_INVALID_ARGUMENT;
  return UMI_STATUS_OK;
}
