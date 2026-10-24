/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Transfer fixtures stay below the CTest working directory and retain evidence. */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/data/record_bundle_files.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/rooted_files.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  const char *cases[] = {"roundtrip", "exclusive", "unicode",        "relative",
                         "traversal", "missing",   "missing-parent", "directory",
                         "malformed", "oversized", "null-bundle",    "symlink"};
  bool known = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(name, cases[i]) == 0)
      known = true;
  CHECK(known);
  char root[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY];
  FixtureDirectory(root);
  const char *leaf = strcmp(name, "unicode") == 0 ? "caf\xc3\xa9.umibundle" : "history.umibundle";
  FixturePath(file, root, leaf);
  UmiDataServer *store = NULL;
  UmiRecordBundle *bundle = NULL, *read = NULL;
  CHECK(umi_data_server_create_memory(&store) == UMI_STATUS_OK);
  UmiRecordAmendment author = {.entity_id = "file.record",
                               .source = "Synthetic transfer",
                               .actor = "Example author",
                               .reason = "File fixture",
                               .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  CHECK(umi_record_document_append(store, &author, "content", 7, 0, &saved) == UMI_STATUS_OK);
  CHECK(umi_record_bundle_capture(store, "file.record", &bundle) == UMI_STATUS_OK);
  if (strcmp(name, "relative") == 0) {
    CHECK(umi_record_bundle_write_new("relative.umibundle", bundle) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (strcmp(name, "traversal") == 0) {
    CHECK(umi_record_bundle_read(root, "../history.umibundle", &read) != UMI_STATUS_OK &&
          read == NULL);
  } else if (strcmp(name, "missing") == 0) {
    CHECK(umi_record_bundle_read(root, leaf, &read) == UMI_STATUS_NOT_FOUND && read == NULL);
  } else if (strcmp(name, "missing-parent") == 0) {
    char absent[UMI_PATH_CAPACITY];
    FixturePath(absent, root, "absent");
    FixturePath(file, absent, leaf);
    CHECK(umi_record_bundle_write_new(file, bundle) != UMI_STATUS_OK);
  } else if (strcmp(name, "null-bundle") == 0) {
    CHECK(umi_record_bundle_write_new(file, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    int exists = 1;
    CHECK(UmiRootedFileExists(root, leaf, &exists) == UMI_STATUS_OK && !exists);
  } else if (strcmp(name, "directory") == 0) {
    CHECK(umi_fs_make_directories(file) == UMI_STATUS_OK);
    CHECK(umi_record_bundle_write_new(file, bundle) != UMI_STATUS_OK);
    CHECK(umi_record_bundle_read(root, leaf, &read) != UMI_STATUS_OK && read == NULL);
  } else if (strcmp(name, "malformed") == 0 || strcmp(name, "oversized") == 0) {
    size_t length = strcmp(name, "oversized") == 0 ? UMI_RECORD_BUNDLE_MAX_BYTES + 1U : 32U;
    unsigned char *bytes = calloc(length, 1);
    CHECK(bytes);
    UmiOutputFile *output = NULL;
    CHECK(UmiOutputFileCreate(file, &output) == UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(output, bytes, length) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(output) == UMI_STATUS_OK);
    UmiOutputFileDestroy(output);
    free(bytes);
    CHECK(umi_record_bundle_read(root, leaf, &read) != UMI_STATUS_OK && read == NULL);
  } else {
    CHECK(umi_record_bundle_write_new(file, bundle) == UMI_STATUS_OK);
    if (strcmp(name, "exclusive") == 0)
      CHECK(umi_record_bundle_write_new(file, bundle) == UMI_STATUS_ALREADY_EXISTS);
    if (strcmp(name, "symlink") == 0) {
      char alias[UMI_PATH_CAPACITY];
      FixturePath(alias, root, "alias.umibundle");
#ifdef _WIN32
      if (!FixtureSymbolicLink(alias, file, 0)) {
        umi_record_bundle_destroy(bundle);
        umi_data_server_destroy(store);
        return 77;
      }
#else
      CHECK(symlink(file, alias) == 0);
#endif
      CHECK(umi_record_bundle_read(root, "alias.umibundle", &read) != UMI_STATUS_OK &&
            read == NULL);
      CHECK(umi_record_bundle_write_new(alias, bundle) == UMI_STATUS_ALREADY_EXISTS);
    }
    CHECK(umi_record_bundle_read(root, leaf, &read) == UMI_STATUS_OK);
    CHECK(umi_record_bundle_count(read) == 1);
    UmiRecordDocument *document = malloc(sizeof(*document));
    CHECK(document);
    CHECK(umi_record_bundle_at(read, 0, document) == UMI_STATUS_OK);
    CHECK(document->length == 7 && memcmp(document->content, "content", 7) == 0);
    free(document);
  }
  umi_record_bundle_destroy(read);
  umi_record_bundle_destroy(bundle);
  umi_data_server_destroy(store);
  return 0;
}
