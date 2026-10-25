/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/structured_form_catalogue.h
 *
 * PURPOSE:
 *   Trusted form versions for historical reads and explicit upgrade selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Trusted form versions for historical reads and explicit upgrade selection. */
#ifndef UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_CATALOGUE_H
#define UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_CATALOGUE_H
#include "umicom/application/experience/structured_form_snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FORM_CATALOGUE_CAPACITY 64U
typedef struct UmiStructuredFormCatalogue UmiStructuredFormCatalogue;
typedef struct UmiStructuredFormResolved {
  const UmiStructuredFormSchema *schema;
  UmiStructuredForm form;
} UmiStructuredFormResolved;
/* The catalogue copies pointers, sorts by ID/version and refuses duplicate
 * identities. Every schema, descriptor, rule and referenced string is borrowed
 * immutable data that must outlive the catalogue AND any resolved form using it.
 * The input pointer array need only survive creation. Empty catalogues are valid.
 * A completed catalogue is immutable and may be shared by read-only workers. */
UmiStatus umi_structured_form_catalogue_create(const UmiStructuredFormSchema *const *schemas,
                                               size_t count, UmiStructuredFormCatalogue **out);
void umi_structured_form_catalogue_destroy(UmiStructuredFormCatalogue *catalogue);
size_t umi_structured_form_catalogue_count(const UmiStructuredFormCatalogue *catalogue);
/* Exact lookup never substitutes the newest schema. All output arguments remain
 * unchanged on failure. at() lists deterministic ID/version order. */
UmiStatus umi_structured_form_catalogue_find(const UmiStructuredFormCatalogue *catalogue,
                                             const char *form_id, uint64_t version,
                                             const UmiStructuredFormSchema **out);
UmiStatus umi_structured_form_catalogue_at(const UmiStructuredFormCatalogue *catalogue,
                                           size_t index, const UmiStructuredFormSchema **out);
/* latest() selects a potential upgrade destination; it does not migrate data. */
UmiStatus umi_structured_form_catalogue_latest(const UmiStructuredFormCatalogue *catalogue,
                                               const char *form_id,
                                               const UmiStructuredFormSchema **out);
/* Uses only the bounded identity hint to select a trusted entry, then fully
 * validates framing, values, field order, units and version before publishing.
 * No dynamic schema loading, file lookup, network I/O or fallback occurs.
 * Allocate the large result on the heap. Input may alias the result storage. */
UmiStatus umi_structured_form_catalogue_decode(const UmiStructuredFormCatalogue *catalogue,
                                               const void *bytes, size_t length,
                                               UmiStructuredFormResolved *out);
#ifdef __cplusplus
}
#endif
#endif
