/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bounded portable filter snapshots bind operands to their schema meaning. */
#ifndef UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_FILTER_SNAPSHOT_H
#define UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_FILTER_SNAPSHOT_H
#include "umicom/application/experience/structured_form_filter_text.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES 8192U
/* Encodes form ID/version and ordered conditions, including each referenced
 * field type, text format and unit. No pointer, struct padding, locale, record
 * contents or continuation state is serialized. Numeric text is canonical.
 * Decode requires the original schema; it never migrates a query or converts
 * units. Version must change for any other schema-semantic change.
 * Both operations stage outputs, allow input/output aliasing and leave output
 * and output length unchanged on failure. Unused output capacity is untouched.
 * Framing detects invalid data; use record documents for integrity checking.
 * A valid snapshot neither authenticates its author nor authorizes a search. */
UmiStatus umi_structured_form_filter_snapshot_encode(const UmiStructuredFormSchema *schema,
                                                     const UmiStructuredFormFilter *filter,
                                                     void *out, size_t capacity,
                                                     size_t *out_length);
UmiStatus umi_structured_form_filter_snapshot_decode(const UmiStructuredFormSchema *schema,
                                                     const void *bytes, size_t length,
                                                     UmiStructuredFormFilter *out);
#ifdef __cplusplus
}
#endif
#endif
