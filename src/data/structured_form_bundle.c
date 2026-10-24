/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * The review owns captured bytes so selecting another file cannot change an
 * approved restore. Shared readers preserve the meaning of historical versions. */
#include "umicom/data/structured_form_bundle.h"
#include <stdlib.h>
#include <string.h>
struct UmiStructuredFormBundleReview {
  UmiRecordBundle *bundle;
  const UmiStructuredFormCatalogue *catalogue;
  UmiStructuredFormBundleSummary summary;
};
static size_t bounded_length(const char *text, size_t bound) {
  if (text == NULL)
    return bound;
  size_t n = 0;
  while (n < bound && text[n] != '\0')
    ++n;
  return n;
}
static int namespace_valid(const char *text, size_t n) {
  if (n < 2 || n >= sizeof(((UmiRecordAmendment *)0)->entity_id) || text[n - 1] != '.')
    return 0;
  for (size_t i = 0; i < n; ++i) {
    unsigned char c = (unsigned char)text[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' ||
          c == '_' || c == '-'))
      return 0;
  }
  return 1;
}
UmiStatus umi_structured_form_bundle_review(const UmiRecordBundle *bundle,
                                            const UmiStructuredFormCatalogue *catalogue,
                                            const char *expected_form_id,
                                            const char *entity_namespace,
                                            UmiStructuredFormBundleKeyCheck key_check,
                                            void *context, UmiStructuredFormBundleReview **out) {
  size_t prefix_length = bounded_length(entity_namespace, 64);
  size_t id_length = bounded_length(expected_form_id, UMI_APPLICATION_EXPERIENCE_ID_CAPACITY);
  if (bundle == NULL || catalogue == NULL || out == NULL ||
      !namespace_valid(entity_namespace, prefix_length) || id_length == 0 ||
      id_length == UMI_APPLICATION_EXPERIENCE_ID_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  unsigned char *bytes = NULL;
  size_t length = 0;
  UmiStatus status = umi_record_bundle_encode(bundle, &bytes, &length);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormBundleReview *review = calloc(1, sizeof(*review));
  UmiRecordDocument *document = malloc(sizeof(*document));
  UmiStructuredFormHistorical *decoded = malloc(sizeof(*decoded));
  if (review == NULL || document == NULL || decoded == NULL)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status = umi_record_bundle_decode(bytes, length, &review->bundle);
  if (status == UMI_STATUS_OK) {
    review->catalogue = catalogue;
    review->summary.revision_count = umi_record_bundle_count(review->bundle);
    memcpy(review->summary.bundle_sha256, bytes + length - 64, 64);
    memcpy(review->summary.form_id, expected_form_id, id_length + 1);
    for (size_t i = 0; i < review->summary.revision_count && status == UMI_STATUS_OK; ++i) {
      status = umi_record_bundle_at(review->bundle, i, document);
      if (status == UMI_STATUS_OK &&
          (strncmp(document->record.amendment.entity_id, entity_namespace, prefix_length) != 0 ||
           document->record.amendment.entity_id[prefix_length] == '\0'))
        status = UMI_STATUS_PERMISSION_DENIED;
      if (status == UMI_STATUS_OK)
        status = umi_structured_form_reader_decode(catalogue, document, decoded);
      if (status == UMI_STATUS_OK && strcmp(decoded->resolved.form.form_id, expected_form_id) != 0)
        status = UMI_STATUS_INVALID_STATE;
      if (status == UMI_STATUS_OK) {
        if (i == 0) {
          review->summary.first = document->record;
          review->summary.first_schema_version = decoded->resolved.form.schema_version;
        }
        review->summary.latest = document->record;
        review->summary.latest_schema_version = decoded->resolved.form.schema_version;
        review->summary.content_bytes += document->length;
      }
    }
    if (status == UMI_STATUS_OK && key_check != NULL &&
        !key_check(review->summary.latest.amendment.entity_id, context))
      status = UMI_STATUS_PERMISSION_DENIED;
  }
  free(decoded);
  free(document);
  umi_record_bundle_bytes_free(bytes);
  if (status != UMI_STATUS_OK) {
    umi_structured_form_bundle_review_destroy(review);
    return status;
  }
  *out = review;
  return UMI_STATUS_OK;
}
void umi_structured_form_bundle_review_destroy(UmiStructuredFormBundleReview *review) {
  if (review == NULL)
    return;
  umi_record_bundle_destroy(review->bundle);
  free(review);
}
UmiStatus umi_structured_form_bundle_summary(const UmiStructuredFormBundleReview *review,
                                             UmiStructuredFormBundleSummary *out) {
  if (review == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = review->summary;
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_bundle_inspect(const UmiStructuredFormBundleReview *review,
                                             size_t index, UmiStructuredFormHistorical *out) {
  if (review == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordDocument *document = malloc(sizeof(*document));
  if (document == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = umi_record_bundle_at(review->bundle, index, document);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_reader_decode(review->catalogue, document, out);
  free(document);
  return status;
}
UmiStatus umi_structured_form_bundle_restore(UmiDataServer *server,
                                             const UmiStructuredFormBundleReview *review,
                                             const char *reviewed_sha256, UmiRecordRevision *out) {
  if (server == NULL || review == NULL || out == NULL || bounded_length(reviewed_sha256, 65) != 64)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (memcmp(reviewed_sha256, review->summary.bundle_sha256, 65) != 0)
    return UMI_STATUS_INVALID_STATE;
  return umi_record_bundle_restore_new(server, review->bundle, out);
}

UmiStatus umi_structured_form_bundle_review_clone(const UmiStructuredFormBundleReview *review,
                                                  UmiStructuredFormBundleReview **out) {
  if (review == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStructuredFormBundleReview *copy = calloc(1, sizeof(*copy));
  if (copy == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  copy->catalogue = review->catalogue;
  copy->summary = review->summary;
  unsigned char *bytes = NULL;
  size_t length = 0;
  UmiStatus status = umi_record_bundle_encode(review->bundle, &bytes, &length);
  if (status == UMI_STATUS_OK)
    status = umi_record_bundle_decode(bytes, length, &copy->bundle);
  umi_record_bundle_bytes_free(bytes);
  if (status != UMI_STATUS_OK) {
    umi_structured_form_bundle_review_destroy(copy);
    return status;
  }
  *out = copy;
  return UMI_STATUS_OK;
}

UmiStatus umi_structured_form_bundle_restore_receipted(UmiDataServer *server,
                                                       const UmiStructuredFormBundleReview *review,
                                                       const char *reviewed_sha256,
                                                       const UmiRecordTransferRequest *request,
                                                       UmiRecordTransferReceipt *out) {
  if (!server || !review || !request || !out || bounded_length(reviewed_sha256, 65) != 64)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (memcmp(reviewed_sha256, review->summary.bundle_sha256, 65) != 0)
    return UMI_STATUS_INVALID_STATE;
  return umi_record_transfer_restore(server, review->bundle, reviewed_sha256, request, out);
}
