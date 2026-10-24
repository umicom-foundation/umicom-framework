/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Application-owned version definitions stay stable while workers read history. */
#include "umicom/application/experience/structured_form_catalogue.h"
#include <stdlib.h>
#include <string.h>
struct UmiStructuredFormCatalogue {
  const UmiStructuredFormSchema *schemas[UMI_FORM_CATALOGUE_CAPACITY];
  size_t count;
};
static int compare(const void *a, const void *b) {
  const UmiStructuredFormSchema *left = *(const UmiStructuredFormSchema *const *)a;
  const UmiStructuredFormSchema *right = *(const UmiStructuredFormSchema *const *)b;
  int order = strcmp(left->base->form_id, right->base->form_id);
  if (order != 0)
    return order;
  return left->version < right->version ? -1 : (left->version > right->version ? 1 : 0);
}
UmiStatus umi_structured_form_catalogue_create(const UmiStructuredFormSchema *const *schemas,
                                               size_t count, UmiStructuredFormCatalogue **out) {
  if (out == NULL || (schemas == NULL && count != 0))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (count > UMI_FORM_CATALOGUE_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiStructuredFormCatalogue *catalogue = calloc(1, sizeof(*catalogue));
  if (catalogue == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  for (size_t i = 0; i < count; ++i) {
    UmiStatus status = umi_structured_form_schema_validate(schemas[i]);
    if (status != UMI_STATUS_OK) {
      free(catalogue);
      return status;
    }
    catalogue->schemas[i] = schemas[i];
  }
  catalogue->count = count;
  qsort(catalogue->schemas, count, sizeof(catalogue->schemas[0]), compare);
  for (size_t i = 1; i < count; ++i)
    if (compare(&catalogue->schemas[i - 1], &catalogue->schemas[i]) == 0) {
      free(catalogue);
      return UMI_STATUS_ALREADY_EXISTS;
    }
  *out = catalogue;
  return UMI_STATUS_OK;
}
void umi_structured_form_catalogue_destroy(UmiStructuredFormCatalogue *catalogue) {
  free(catalogue);
}
size_t umi_structured_form_catalogue_count(const UmiStructuredFormCatalogue *catalogue) {
  return catalogue == NULL ? 0 : catalogue->count;
}
UmiStatus umi_structured_form_catalogue_at(const UmiStructuredFormCatalogue *catalogue,
                                           size_t index, const UmiStructuredFormSchema **out) {
  if (catalogue == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (index >= catalogue->count)
    return UMI_STATUS_NOT_FOUND;
  *out = catalogue->schemas[index];
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_catalogue_find(const UmiStructuredFormCatalogue *catalogue,
                                             const char *form_id, uint64_t version,
                                             const UmiStructuredFormSchema **out) {
  if (catalogue == NULL || out == NULL || version == 0 ||
      !umi_application_experience_identifier_valid(form_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0; i < catalogue->count; ++i) {
    const UmiStructuredFormSchema *s = catalogue->schemas[i];
    if (s->version == version && strcmp(s->base->form_id, form_id) == 0) {
      *out = s;
      return UMI_STATUS_OK;
    }
  }
  return UMI_STATUS_NOT_FOUND;
}
UmiStatus umi_structured_form_catalogue_latest(const UmiStructuredFormCatalogue *catalogue,
                                               const char *form_id,
                                               const UmiStructuredFormSchema **out) {
  if (catalogue == NULL || out == NULL || !umi_application_experience_identifier_valid(form_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = catalogue->count; i > 0; --i) {
    const UmiStructuredFormSchema *s = catalogue->schemas[i - 1];
    if (strcmp(s->base->form_id, form_id) == 0) {
      *out = s;
      return UMI_STATUS_OK;
    }
  }
  return UMI_STATUS_NOT_FOUND;
}
UmiStatus umi_structured_form_catalogue_decode(const UmiStructuredFormCatalogue *catalogue,
                                               const void *bytes, size_t length,
                                               UmiStructuredFormResolved *out) {
  if (catalogue == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStructuredFormSnapshotIdentity identity;
  UmiStatus status = umi_structured_form_snapshot_identity(bytes, length, &identity);
  if (status != UMI_STATUS_OK)
    return status;
  const UmiStructuredFormSchema *schema = NULL;
  status = umi_structured_form_catalogue_find(catalogue, identity.form_id, identity.schema_version,
                                              &schema);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormResolved *candidate = malloc(sizeof(*candidate));
  if (candidate == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_structured_form_snapshot_decode(schema, bytes, length, &candidate->form);
  if (status == UMI_STATUS_OK) {
    candidate->schema = schema;
    *out = *candidate;
  }
  free(candidate);
  return status;
}
