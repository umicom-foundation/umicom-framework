/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Named query history reuses atomic record documents and reviewed opens. */
#ifndef UMICOM_DATA_STRUCTURED_FORM_SEARCH_PRESET_H
#define UMICOM_DATA_STRUCTURED_FORM_SEARCH_PRESET_H
#include "umicom/application/experience/structured_form_search_preset.h"
#include "umicom/data/record_document.h"
#include "umicom/data/structured_form_query.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FORM_SEARCH_PRESET_PREFIX "form.search."
typedef struct UmiStructuredFormSearchPresetRecord {
  UmiRecordRevision record;
  UmiStructuredFormSearchPreset preset;
} UmiStructuredFormSearchPresetRecord;
/* Preset entities use form.search. followed by 1..40 ASCII letters, digits,
 * underscore or hyphen. Their history stays separate from searched answers.
 * Save uses an expected record revision: zero creates, a reviewed revision
 * amends. A stale write never replaces a newer definition. Existing history
 * is retained. Presets and attribution are ordinary unencrypted local data,
 * not credentials or permission to access the query's namespace.
 * Load revision zero for latest, or an explicit earlier revision. The original
 * schema is required. All outputs are unchanged on failure. */
UmiStatus umi_structured_form_search_preset_save(UmiDataServer *server,
                                                 const UmiRecordAmendment *metadata,
                                                 const UmiStructuredFormSchema *schema,
                                                 const UmiStructuredFormSearchPreset *preset,
                                                 uint64_t expected_revision,
                                                 UmiRecordRevision *out);
UmiStatus umi_structured_form_search_preset_load(UmiDataServer *server, const char *entity_id,
                                                 uint64_t revision,
                                                 const UmiStructuredFormSchema *schema,
                                                 UmiStructuredFormSearchPresetRecord *out);
UmiStatus umi_structured_form_search_preset_open_reviewed(UmiDataServer *server,
                                                          const UmiRecordRevision *reviewed,
                                                          const UmiStructuredFormSchema *schema,
                                                          UmiStructuredFormSearchPresetRecord *out);
/* Build a fresh first-page query; never restore a stored cursor or stale result.
 * The caller still authorizes the selected database and preset namespace. */
UmiStatus umi_structured_form_query_from_preset(const UmiStructuredFormSchema *schema,
                                                const UmiStructuredFormSearchPreset *preset,
                                                size_t limit, UmiStructuredFormQuery *out);
#ifdef __cplusplus
}
#endif
#endif
