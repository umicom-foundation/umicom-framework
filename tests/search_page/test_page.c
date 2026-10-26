/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/search_page/test_page.c
 *
 * PURPOSE:
 *   Exercise bounded search pages, independent row ownership and stale result rejection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/search_page.h"

int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  const char *known[] = {"normal",       "default",         "align",
                         "last",         "overflow",        "empty",
                         "single",       "maximum",         "limit",
                         "null",         "bounds",          "immutable",
                         "new-request",  "invalid-request", "invalidate",
                         "index-change", "other-session",   "after-owner",
                         "filter",       "cancel-completed", "index-unchanged"};
  int recognised = 0;
  for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i)
    if (strcmp(name, known[i]) == 0)
      recognised = 1;
  CHECK(recognised);
  char root[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY];
  FixtureDirectory(root);
  FixturePath(file, root, "matches.c");
  char text[4096] = {0};
  size_t bytes = 0U;
  size_t matches = strcmp(name, "empty") == 0     ? 0U
                   : strcmp(name, "single") == 0  ? 1U
                   : strcmp(name, "maximum") == 0 ? 1000U
                                                  : 150U;
  for (size_t i = 0U; i < matches; ++i) {
    text[bytes++] = 'x';
    text[bytes++] = '\n';
  }
  text[bytes] = '\0';
  CHECK(umi_fs_write_text(file, text) == UMI_STATUS_OK);
  UmiFileIndexConfig config = umi_file_index_config_default(root);
  UmiFileIndex *index = NULL;
  CHECK(umi_file_index_create(&config, &index) == UMI_STATUS_OK);
  CHECK(umi_file_index_rebuild(index) == UMI_STATUS_OK);
  UmiFileSearchSession *session = NULL;
  CHECK(UmiFileSearchCreate(index, &session) == UMI_STATUS_OK);
  UmiSearchPathFilter filter;
  CHECK(UmiSearchPathFilterInit("*.c", "*.tmp", &filter) == UMI_STATUS_OK);
  CHECK(UmiFileSearchStartFiltered(session, "x", 1, 0U, &filter) == UMI_STATUS_OK);
  CHECK(UmiFileSearchWait(session, 10000U) == UMI_STATUS_OK);
  UmiFileSearchPage *page = NULL;
  size_t offset = 0U, limit = 32U;
  if (strcmp(name, "default") == 0)
    limit = 0U;
  if (strcmp(name, "align") == 0)
    offset = 35U;
  if (strcmp(name, "last") == 0)
    offset = 149U;
  if (strcmp(name, "overflow") == 0)
    offset = SIZE_MAX;
  if (strcmp(name, "maximum") == 0)
    limit = 128U;
  CHECK(UmiFileSearchPageCreate(session, offset, limit, &page) == UMI_STATUS_OK);
  UmiFileSearchPageSummary summary;
  CHECK(UmiFileSearchPageRead(page, &summary) == UMI_STATUS_OK);
  CHECK(summary.search.stats.matches == matches);
  size_t size = limit == 0U ? 64U : limit;
  size_t last = matches == 0U ? 0U : (matches - 1U) / size * size;
  size_t expected = offset > last ? last : offset / size * size;
  CHECK(summary.offset == expected && summary.limit == size);
  CHECK(summary.count == (matches - expected > size ? size : matches - expected));
  CHECK(summary.has_previous == (expected != 0U));
  CHECK(summary.has_next == (expected + summary.count < matches));
  CHECK(UmiFileSearchPageValidate(page, session) == UMI_STATUS_OK);
  UmiSearchMatch row;
  memset(&row, 0x5a, sizeof row);
  UmiSearchMatch unchanged = row;
  if (matches != 0U) {
    CHECK(UmiFileSearchPageMatchAt(page, 0U, &row) == UMI_STATUS_OK);
    CHECK(row.line == summary.offset + 1U && row.column == 1U);
    CHECK(strcmp(row.path, file) == 0 || umi_path_equal(row.path, file));
  }
  if (strcmp(name, "bounds") == 0) {
    row = unchanged;
    CHECK(UmiFileSearchPageMatchAt(page, summary.count, &row) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&row, &unchanged, sizeof row) == 0);
  } else if (strcmp(name, "limit") == 0) {
    UmiFileSearchPage *sentinel = page;
    CHECK(UmiFileSearchPageCreate(session, 0U, 129U, &sentinel) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(sentinel == page);
  } else if (strcmp(name, "null") == 0) {
    UmiFileSearchPage *sentinel = page;
    CHECK(UmiFileSearchPageCreate(NULL, 0U, 0U, &sentinel) == UMI_STATUS_INVALID_ARGUMENT &&
          sentinel == page);
    CHECK(UmiFileSearchPageCreate(session, 0U, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFileSearchPageRead(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFileSearchPageRead(page, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFileSearchPageMatchAt(page, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFileSearchPageValidate(page, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiFileSearchPageDestroy(NULL);
  } else if (strcmp(name, "new-request") == 0) {
    CHECK(UmiFileSearchStart(session, "different", 1, 0U) == UMI_STATUS_OK);
    CHECK(UmiFileSearchWait(session, 10000U) == UMI_STATUS_OK);
    CHECK(UmiFileSearchPageValidate(page, session) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiFileSearchPageMatchAt(page, 0U, &row) == UMI_STATUS_OK && row.line == 1U);
  } else if (strcmp(name, "invalid-request") == 0) {
    CHECK(UmiFileSearchStart(session, "", 1, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFileSearchPageValidate(page, session) == UMI_STATUS_OK);
  } else if (strcmp(name, "invalidate") == 0) {
    UmiFileSearchInvalidate(session);
    CHECK(UmiFileSearchPageValidate(page, session) == UMI_STATUS_BUSY);
    UmiFileSearchPage *sentinel = page;
    CHECK(UmiFileSearchPageCreate(session, 0U, 0U, &sentinel) == UMI_STATUS_BUSY &&
          sentinel == page);
  } else if (strcmp(name, "index-change") == 0) {
    /* An unchanged scan deliberately preserves the published index revision.
     * Retain the earlier fixture for review; add a real indexed entry before
     * requiring the search page to reject its now-stale result snapshot. */
#if 0
    CHECK(umi_file_index_rebuild(index) == UMI_STATUS_OK);
    CHECK(UmiFileSearchPageValidate(page, session) == UMI_STATUS_BUSY);
#endif
    char additional[UMI_PATH_CAPACITY];
    FixturePath(additional, root, "additional.c");
    CHECK(umi_fs_write_text(additional, "x\n") == UMI_STATUS_OK);
    CHECK(umi_file_index_rebuild(index) == UMI_STATUS_OK);
    CHECK(UmiFileSearchPageValidate(page, session) == UMI_STATUS_BUSY);
    /* Staleness blocks reuse against the owner, not access to the owned copy. */
    CHECK(UmiFileSearchPageRead(page, &summary) == UMI_STATUS_OK);
    CHECK(summary.search.stats.matches == matches);
    CHECK(UmiFileSearchPageMatchAt(page, 0U, &row) == UMI_STATUS_OK);
  } else if (strcmp(name, "index-unchanged") == 0) {
    CHECK(umi_file_index_rebuild(index) == UMI_STATUS_OK);
    CHECK(UmiFileSearchPageValidate(page, session) == UMI_STATUS_OK);
  } else if (strcmp(name, "other-session") == 0) {
    UmiFileSearchSession *other = NULL;
    CHECK(UmiFileSearchCreate(index, &other) == UMI_STATUS_OK);
    CHECK(UmiFileSearchPageValidate(page, other) == UMI_STATUS_INVALID_STATE);
    UmiFileSearchDestroy(other);
  } else if (strcmp(name, "after-owner") == 0) {
    UmiFileSearchDestroy(session);
    session = NULL;
    umi_file_index_destroy(index);
    index = NULL;
    CHECK(UmiFileSearchPageRead(page, &summary) == UMI_STATUS_OK);
    CHECK(UmiFileSearchPageMatchAt(page, 0U, &row) == UMI_STATUS_OK);
  } else if (strcmp(name, "immutable") == 0) {
    CHECK(UmiFileSearchPageMatchAt(page, 0U, &row) == UMI_STATUS_OK);
    row.preview[0] = 'z';
    CHECK(UmiFileSearchPageMatchAt(page, 0U, &row) == UMI_STATUS_OK && row.preview[0] == 'x');
  } else if (strcmp(name, "filter") == 0) {
    CHECK(strcmp(summary.filter.include_patterns, "*.c") == 0);
    CHECK(strcmp(summary.filter.exclude_patterns, "*.tmp") == 0);
  } else if (strcmp(name, "cancel-completed") == 0) {
    CHECK(UmiFileSearchCancel(session) == UMI_STATUS_OK);
    CHECK(UmiFileSearchPageValidate(page, session) == UMI_STATUS_OK);
  }
  UmiFileSearchPageDestroy(page);
  UmiFileSearchDestroy(session);
  umi_file_index_destroy(index);
  return 0;
}
