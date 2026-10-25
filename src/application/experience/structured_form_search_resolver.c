/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/structured_form_search_resolver.c
 *
 * PURPOSE:
 *   Catalogue discovery shares the canonical decoder instead of guessing versions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Catalogue discovery shares the canonical decoder instead of guessing versions. */
#include "umicom/application/experience/structured_form_search_resolver.h"
#include "form_wire_internal.h"
#include <stdlib.h>
UmiStatus umi_structured_form_search_resolve(const UmiStructuredFormCatalogue *catalogue,
                                             const void *bytes, size_t length,
                                             UmiStructuredFormResolvedSearch *out) {
  if (!catalogue || !bytes || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (length < 8 || length > UMI_FORM_SEARCH_PRESET_MAX_BYTES || memcmp(bytes, "UMIPST1\n", 8))
    return UMI_STATUS_PARSE_ERROR;
  UmiFormWireReader envelope = {bytes, 8, length, UMI_STATUS_OK};
  char title[160], prefix[64], metadata[160];
  umi_form_wire_take_text(&envelope, title, sizeof title);
  umi_form_wire_take_text(&envelope, prefix, sizeof prefix);
  umi_form_wire_take_text(&envelope, metadata, sizeof metadata);
  size_t count = (size_t)umi_form_wire_take_number(&envelope, 2);
  if (envelope.status != UMI_STATUS_OK)
    return envelope.status;
  if (count != envelope.length - envelope.at || count < 8 ||
      count > UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES ||
      memcmp(envelope.bytes + envelope.at, "UMIFLT1\n", 8))
    return UMI_STATUS_PARSE_ERROR;
  UmiFormWireReader identity = {envelope.bytes + envelope.at, 8, count, UMI_STATUS_OK};
  char form_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  umi_form_wire_take_text(&identity, form_id, sizeof form_id);
  uint64_t version = umi_form_wire_take_number(&identity, 8);
  if (identity.status != UMI_STATUS_OK)
    return identity.status;
  const UmiStructuredFormSchema *schema = NULL;
  UmiStatus status = umi_structured_form_catalogue_find(catalogue, form_id, version, &schema);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormResolvedSearch *candidate = malloc(sizeof *candidate);
  if (!candidate)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_structured_form_search_preset_decode(schema, bytes, length, &candidate->preset);
  if (status == UMI_STATUS_OK) {
    candidate->schema = schema;
    *out = *candidate;
  }
  free(candidate);
  return status;
}
