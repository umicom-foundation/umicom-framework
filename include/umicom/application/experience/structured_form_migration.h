/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/structured_form_migration.h
 * PURPOSE: Transfer compatible answers to a newer explicitly selected schema.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_MIGRATION_H
#define UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_MIGRATION_H
#include "umicom/application/experience/structured_form.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Same form ID, strictly newer semantic schema version, stable field IDs.
 * Removed answered fields, changed declared units/types/formats and invalid
 * retained answers refuse the whole migration. New fields use checked defaults.
 * Source and output remain unchanged on failure; aliasing source/output is
 * supported. Success creates a dirty next revision for explicit review/save.
 * No arbitrary conversion, renaming or dropping of original answers occurs. */
UmiStatus umi_structured_form_migrate(const UmiStructuredFormSchema *source_schema,
                                      const UmiStructuredForm *source,
                                      const UmiStructuredFormSchema *target_schema,
                                      UmiStructuredForm *out);
#ifdef __cplusplus
}
#endif
#endif
