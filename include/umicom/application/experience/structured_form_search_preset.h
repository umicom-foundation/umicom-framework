/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Named saved queries retain explicit namespace and typed answer conditions. */
#ifndef UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_SEARCH_PRESET_H
#define UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_SEARCH_PRESET_H
#include "umicom/application/experience/structured_form_filter_snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FORM_SEARCH_PRESET_MAX_BYTES 9216U
typedef struct UmiStructuredFormSearchPreset {
  char title[160], entity_prefix[64], metadata_text[160];
  UmiStructuredFormFilter filter;
} UmiStructuredFormSearchPreset;
/* Nonempty title has no ASCII control bytes. Namespace is nonempty, ends in
 * a dot and contains ASCII letters, digits, dot, underscore or hyphen only.
 * All fixed strings terminate. UTF-8 rendering is a host responsibility.
 * Namespace and metadata text are query scope, never an access grant.
 * Snapshot framing embeds an exact-schema filter snapshot and no result cursor.
 * Inputs may alias outputs; errors leave outputs and lengths unchanged. */
UmiStatus umi_structured_form_search_preset_validate(const UmiStructuredFormSchema *schema,
                                                     const UmiStructuredFormSearchPreset *preset);
UmiStatus umi_structured_form_search_preset_encode(const UmiStructuredFormSchema *schema,
                                                   const UmiStructuredFormSearchPreset *preset,
                                                   void *out, size_t capacity, size_t *out_length);
UmiStatus umi_structured_form_search_preset_decode(const UmiStructuredFormSchema *schema,
                                                   const void *bytes, size_t length,
                                                   UmiStructuredFormSearchPreset *out);
#ifdef __cplusplus
}
#endif
#endif
