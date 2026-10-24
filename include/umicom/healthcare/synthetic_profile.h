/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Versioned fictional administrative profiles for native and browser exercises. */
#ifndef UMICOM_HEALTHCARE_SYNTHETIC_PROFILE_H
#define UMICOM_HEALTHCARE_SYNTHETIC_PROFILE_H
#include "umicom/application/experience/structured_form.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Process-lifetime immutable schema. These are synthetic administrative answers,
 * not a patient registry, clinical record, identity proof or consent service.
 * No address, phone number, treatment or funding decision is collected. */
const UmiStructuredFormSchema *umi_healthcare_synthetic_profile_schema(void);
/* Accept exactly SYN- followed by six decimal digits, including leading zeros.
 * This spelling matches synthetic exercises; matching text alone does not link
 * two independent stores or establish that a person exists in a registry. */
UmiStatus umi_healthcare_synthetic_profile_key_validate(const char *key);
#ifdef __cplusplus
}
#endif
#endif
