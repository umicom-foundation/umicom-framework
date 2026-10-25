/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_bundle_codec.c
 *
 * PURPOSE:
 *   Corrupt or ambiguous transfer framing must not become an accepted archive.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Corrupt or ambiguous transfer framing must not become an accepted archive. */
#include "umicom/base/sha256.h"
#include "umicom/data/record_bundle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static void number(unsigned char *p, uint32_t n) {
  for (size_t i = 0; i < 4; ++i)
    p[i] = (unsigned char)(n >> (8 * (3 - i)));
}
static uint32_t read_number(const unsigned char *p) {
  uint32_t n = 0;
  for (size_t i = 0; i < 4; ++i)
    n = (n << 8) | p[i];
  return n;
}
static UmiStatus resign(unsigned char *bytes, size_t length) {
  char digest[UMI_SHA256_HEX_CAPACITY];
  UmiStatus status = UmiSha256Buffer(bytes, length - 64, digest);
  if (status == UMI_STATUS_OK)
    memcpy(bytes + length - 64, digest, 64);
  return status;
}
int main(int argc, char **argv) {
  const char *cases[] = {
      "roundtrip",     "binary",   "empty",       "canonical",      "truncated",   "digest",
      "magic",         "reserved", "zero-count",  "count-overflow", "text-length", "content-length",
      "embedded-null", "trailing", "body-digest", "foreign-entity", "output",      "index"};
  if (argc != 2)
    return 2;
  bool selected = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      selected = true;
  if (!selected)
    return 2;
  int result = 0;
  const char *name = argv[1];
  UmiDataServer *store = NULL;
  UmiRecordBundle *bundle = NULL, *decoded = NULL;
  unsigned char *bytes = NULL, *again = NULL;
  size_t length = 0, again_length = 0;
  UmiRecordDocument *document = malloc(sizeof(*document));
  CHECK(document);
  OK(umi_data_server_create_memory(&store));
  UmiRecordAmendment metadata = {.entity_id = "bundle.record",
                                 .source = "Synthetic source",
                                 .actor = "Example author",
                                 .reason = "Transfer fixture",
                                 .recorded_at_nanoseconds = 1};
  unsigned char content[] = {0, 1, 127, 255, 0};
  UmiRecordRevision saved;
  size_t content_length = strcmp(name, "empty") == 0 ? 0 : sizeof content;
  OK(umi_record_document_append(store, &metadata, content, content_length, 0, &saved));
  metadata.recorded_at_nanoseconds = 2;
  OK(umi_record_document_append(store, &metadata, "second", 6, 1, &saved));
  OK(umi_record_bundle_capture(store, "bundle.record", &bundle));
  CHECK(umi_record_bundle_count(bundle) == 2);
  OK(umi_record_bundle_encode(bundle, &bytes, &length));
  if (strcmp(name, "index") == 0) {
    document->length = 123;
    CHECK(umi_record_bundle_at(bundle, 2, document) == UMI_STATUS_NOT_FOUND &&
          document->length == 123);
    goto cleanup;
  }
  if (strcmp(name, "truncated") == 0) {
    for (size_t i = 0; i < length; ++i) {
      CHECK(umi_record_bundle_decode(bytes, i, &decoded) != UMI_STATUS_OK && decoded == NULL);
    }
    goto cleanup;
  }
  if (strcmp(name, "digest") == 0)
    bytes[length - 1] ^= 1;
  if (strcmp(name, "magic") == 0) {
    bytes[0] = 'X';
    OK(resign(bytes, length));
  }
  if (strcmp(name, "reserved") == 0) {
    bytes[12] = 1;
    OK(resign(bytes, length));
  }
  if (strcmp(name, "zero-count") == 0) {
    number(bytes + 8, 0);
    OK(resign(bytes, length));
  }
  if (strcmp(name, "count-overflow") == 0) {
    number(bytes + 8, UMI_RECORD_BUNDLE_MAX_REVISIONS + 1);
    OK(resign(bytes, length));
  }
  if (strcmp(name, "text-length") == 0) {
    number(bytes + 16, UINT32_MAX);
    OK(resign(bytes, length));
  }
  if (strcmp(name, "content-length") == 0) {
    number(bytes + 20, UINT32_MAX);
    OK(resign(bytes, length));
  }
  if (strcmp(name, "embedded-null") == 0) {
    bytes[24] = 0;
    OK(resign(bytes, length));
  }
  if (strcmp(name, "body-digest") == 0) {
    bytes[24 + read_number(bytes + 16)] ^= 1;
    OK(resign(bytes, length));
  }
  if (strcmp(name, "foreign-entity") == 0) {
    size_t second = 24 + read_number(bytes + 16) + read_number(bytes + 20);
    bytes[second + 8 + 6] = 'x';
    OK(resign(bytes, length));
  }
  if (strcmp(name, "trailing") == 0) {
    bytes[length - 64] = 42;
    ++length;
    OK(resign(bytes, length));
  }
  if (strcmp(name, "output") == 0) {
    bytes[length - 1] ^= 1;
    decoded = bundle;
    CHECK(umi_record_bundle_decode(bytes, length, &decoded) != UMI_STATUS_OK && decoded == bundle);
    decoded = NULL;
    goto cleanup;
  }
  if (strcmp(name, "roundtrip") != 0 && strcmp(name, "binary") != 0 && strcmp(name, "empty") != 0 &&
      strcmp(name, "canonical") != 0) {
    CHECK(umi_record_bundle_decode(bytes, length, &decoded) != UMI_STATUS_OK && decoded == NULL);
    goto cleanup;
  }
  OK(umi_record_bundle_decode(bytes, length, &decoded));
  OK(umi_record_bundle_at(decoded, 0, document));
  CHECK(document->record.revision == 1 && document->length == content_length &&
        memcmp(document->content, content, content_length) == 0);
  OK(umi_record_bundle_at(decoded, 1, document));
  CHECK(document->length == 6 && memcmp(document->content, "second", 6) == 0);
  if (strcmp(name, "canonical") == 0) {
    OK(umi_record_bundle_encode(decoded, &again, &again_length));
    CHECK(again_length == length && memcmp(bytes, again, length) == 0);
  }
  if (strcmp(name, "binary") == 0) {
    memset(bytes, 0, length);
    OK(umi_record_bundle_at(decoded, 0, document));
    CHECK(document->length == sizeof content &&
          memcmp(document->content, content, sizeof content) == 0);
  }
cleanup:
  umi_record_bundle_bytes_free(again);
  umi_record_bundle_bytes_free(bytes);
  umi_record_bundle_destroy(decoded);
  umi_record_bundle_destroy(bundle);
  umi_data_server_destroy(store);
  free(document);
  return result;
}
