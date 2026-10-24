/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Resolve stored query definitions only against trusted immutable schemas. */
#ifndef UMICOM_STRUCTURED_FORM_SEARCH_RESOLVER_H
#define UMICOM_STRUCTURED_FORM_SEARCH_RESOLVER_H
#include "umicom/application/experience/structured_form_catalogue.h"
#include "umicom/application/experience/structured_form_search_preset.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormResolvedSearch {
  const UmiStructuredFormSchema *schema;
  UmiStructuredFormSearchPreset preset;
} UmiStructuredFormResolvedSearch;
/* Read a bounded identity hint, select the exact catalogue entry, then decode
 * and validate the entire definition. The hint alone never authorizes a read.
 * Unknown versions are refused; no migration, newest-version fallback or I/O.
 * The result borrows the selected schema for the lifetime of its use.
 * Output is unchanged on failure, and may overlap input storage. */
UmiStatus umi_structured_form_search_resolve(const UmiStructuredFormCatalogue *catalogue,
                                             const void *bytes, size_t length,
                                             UmiStructuredFormResolvedSearch *out);
#ifdef __cplusplus
}
#endif
#endif
