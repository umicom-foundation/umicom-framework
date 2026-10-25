/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_archive.c
 * PURPOSE: Check exact observation round trips, provenance limits and all-or-nothing archive
 * rejection. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "csv_fixture.h"
#include "fixture.h"
#include "umicom/bank_feed/archive.h"
#include "umicom/bank_feed/csv_mapping.h"
#include "umicom/base/sha256.h"
#undef CHECK
#define CHECK(value)                                                                               \
  do {                                                                                             \
    if (!(value)) {                                                                                \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                                  \
      exit(1);                                                                                     \
    }                                                                                              \
  } while (0)
static void Equal(const UmiBankFeed *left, const UmiBankFeed *right) {
  UmiBankFeedSummary a, b;
  CHECK(UmiBankFeedRead(left, &a) == UMI_STATUS_OK && UmiBankFeedRead(right, &b) == UMI_STATUS_OK);
  CHECK(a.rows == b.rows && a.missingIdentifiers == b.missingIdentifiers);
  CHECK(a.hasPreviousPage == b.hasPreviousPage && a.hasNextPage == b.hasNextPage);
  for (size_t i = 0U; i < a.rows; ++i) {
    UmiBankFeedRow x, y;
    CHECK(UmiBankFeedAt(left, i, &x) == UMI_STATUS_OK &&
          UmiBankFeedAt(right, i, &y) == UMI_STATUS_OK);
#define SAME(field) CHECK(!strcmp(x.field, y.field))
    SAME(account);
    SAME(transaction);
    SAME(reference);
    SAME(information);
    SAME(bookingTime);
    SAME(currency);
    SAME(amountText);
#undef SAME
    CHECK(x.amount.coefficient == y.amount.coefficient && x.amount.scale == y.amount.scale);
    CHECK(x.booked == y.booked && x.credit == y.credit && x.mutableRecord == y.mutableRecord);
  }
}
static void Reject(const char *archive, const char *find, const char *replacement) {
  char *bad = BankReplace(archive, find, replacement);
  CHECK(bad != NULL);
  UmiBankFeed *out = (UmiBankFeed *)1;
  UmiBankFeedArchiveInfo info, saved;
  memset(&info, 0x5a, sizeof info);
  memcpy(&saved, &info, sizeof info);
  CHECK(UmiBankFeedArchiveDecode(bad, strlen(bad), &out, &info) != UMI_STATUS_OK);
  CHECK(out == NULL && memcmp(&info, &saved, sizeof info) == 0);
  free(bad);
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *mode = argv[1];
  char *input = BankFixture(BANK_TEST_ROW);
  CHECK(input != NULL);
  UmiBankFeedArchiveInfo info = {.sourceFormat = UMI_BANK_FEED_SOURCE_OPEN_BANKING_JSON};
  UmiBankFeed *feed = NULL;
  if (!strcmp(mode, "csv") || !strcmp(mode, "mapped") || !strcmp(mode, "multiline") ||
      !strcmp(mode, "missing-id") || !strcmp(mode, "capacity")) {
    free(input);
    input = BankReplace(BANK_CSV_TEXT, "Example credit",
                        !strcmp(mode, "multiline") ? "\"line one,\r\nline \"\"two\"\"\tend\""
                                                   : "Example credit");
    CHECK(input != NULL);
    if (!strcmp(mode, "missing-id")) {
      free(input);
      const char *rows = BANK_CSV_HEADER "a,,,,2026-10-12,GBP,-0.00,PDNG,Mutable\n"
                                         "a,,,,2026-10-12,GBP,0,BOOK,Immutable\n";
      input = malloc(strlen(rows) + 1U);
      CHECK(input != NULL);
      strcpy(input, rows);
    }
    if (!strcmp(mode, "capacity")) {
      free(input);
      const char *row = "a,,,,2026-10-12,GBP,1,BOOK,Immutable\n";
      size_t length = strlen(BANK_CSV_HEADER) + UMI_BANK_FEED_MAX_ROWS * strlen(row);
      input = malloc(length + 1U);
      CHECK(input != NULL);
      strcpy(input, BANK_CSV_HEADER);
      for (size_t i = 0U; i < UMI_BANK_FEED_MAX_ROWS; ++i)
        strcat(input, row);
    }
    info.sourceFormat = !strcmp(mode, "mapped") ? UMI_BANK_FEED_SOURCE_MAPPED_CSV
                                                : UMI_BANK_FEED_SOURCE_STATEMENT_CSV;
    if (!strcmp(mode, "mapped")) {
      UmiBankFeedCsvMapping mapping;
      UmiBankFeedCsvMappingInit(&mapping);
      CHECK(UmiBankFeedCsvMappedCreate(input, strlen(input), &mapping, &feed, NULL) ==
            UMI_STATUS_OK);
    } else
      CHECK(UmiBankFeedCsvCreate(input, strlen(input), &feed, NULL) == UMI_STATUS_OK);
  } else {
    char *changed = NULL;
    if (!strcmp(mode, "empty"))
      changed = BankFixture("");
    else if (!strcmp(mode, "unicode"))
      changed = BankReplace(input, "Example credit", "caf\xc3\xa9 \\\"quoted\\\" \\\\ path");
    else if (!strcmp(mode, "precision"))
      changed = BankReplace(input, "12.50", "00012.50000");
    else if (!strcmp(mode, "pagination"))
      changed = BankReplace(input, "{\"Data\":",
                            "{\"Links\":{\"Prev\":\"https://example.invalid/"
                            "previous\",\"Next\":\"https://example.invalid/next\"},\"Data\":");
    if (changed != NULL) {
      free(input);
      input = changed;
    }
    CHECK(UmiBankFeedCreate(input, strlen(input), &feed) == UMI_STATUS_OK);
  }
  CHECK(UmiSha256Buffer(input, strlen(input), info.sourceSha256) == UMI_STATUS_OK);
  char *archive = NULL;
  size_t length = 0U;
  CHECK(UmiBankFeedArchiveEncode(feed, &info, &archive, &length) == UMI_STATUS_OK);
  CHECK(archive != NULL && length == strlen(archive));
  UmiBankFeed *copy = NULL;
  UmiBankFeedArchiveInfo restored;
  CHECK(UmiBankFeedArchiveDecode(archive, length, &copy, &restored) == UMI_STATUS_OK);
  CHECK(restored.sourceFormat == info.sourceFormat &&
        !strcmp(restored.sourceSha256, info.sourceSha256));
  Equal(feed, copy);
  if (!strcmp(mode, "unknown-source")) {
    char *unknown = NULL;
    size_t unknownLength = 0U;
    CHECK(UmiBankFeedArchiveEncode(feed, NULL, &unknown, &unknownLength) == UMI_STATUS_OK);
    UmiBankFeedArchiveInfo unknownInfo;
    UmiBankFeed *unknownFeed = NULL;
    CHECK(UmiBankFeedArchiveDecode(unknown, unknownLength, &unknownFeed, &unknownInfo) ==
          UMI_STATUS_OK);
    CHECK(unknownInfo.sourceFormat == UMI_BANK_FEED_SOURCE_UNKNOWN &&
          unknownInfo.sourceSha256[0] == '\0');
    Equal(feed, unknownFeed);
    UmiBankFeedDestroy(unknownFeed);
    UmiBankFeedArchiveFree(unknown);
  } else if (!strcmp(mode, "mapped-control")) {
    char *bad = BankReplace(BANK_CSV_TEXT, "Example credit", "Example\177credit");
    CHECK(bad != NULL);
    UmiBankFeedCsvMapping mapping;
    UmiBankFeedCsvMappingInit(&mapping);
    UmiBankFeedCsvIssue issue = {0};
    UmiBankFeed *rejected = (UmiBankFeed *)1;
    CHECK(UmiBankFeedCsvMappedCreate(bad, strlen(bad), &mapping, &rejected, &issue) ==
          UMI_STATUS_PARSE_ERROR);
    /* Mapped import shares whole-input text validation with normalised
     * CSV. Retain the former row expectation for review; preflight has
     * not parsed a row and must leave both coordinates unknown. */
#if 0
        CHECK(rejected == NULL && issue.record == 2U && issue.column == 4U); free(bad);
#endif
    CHECK(rejected == NULL && issue.record == 0U && issue.column == 0U);
    free(bad);
  } else if (!strcmp(mode, "row-limit")) {
    const char *start = strstr(archive, "\"transactions\":[");
    CHECK(start != NULL);
    start += strlen("\"transactions\":[");
    const char *end = strstr(start, "]}\n");
    CHECK(end != NULL);
    size_t prefix = (size_t)(start - archive), rowLength = (size_t)(end - start);
    size_t count = (size_t)UMI_BANK_FEED_MAX_ROWS + 1U;
    size_t size = prefix + count * rowLength + (count - 1U) + strlen(end);
    char *many = malloc(size + 1U);
    CHECK(many != NULL);
    memcpy(many, archive, prefix);
    size_t at = prefix;
    for (size_t i = 0U; i < count; ++i) {
      if (i != 0U)
        many[at++] = ',';
      memcpy(many + at, start, rowLength);
      at += rowLength;
    }
    strcpy(many + at, end);
    UmiBankFeed *rejected = (UmiBankFeed *)1;
    CHECK(UmiBankFeedArchiveDecode(many, size, &rejected, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(rejected == NULL);
    free(many);
  } else if (!strcmp(mode, "version"))
    Reject(archive, "\"version\":1", "\"version\":2");
  else if (!strcmp(mode, "metadata")) {
    Reject(archive, "\"source_format\":1", "\"source_format\":99");
    Reject(archive, info.sourceSha256, "not-a-digest");
    Reject(archive, "\"previous_page\":false", "\"previous_page\":\"false\"");
  } else if (!strcmp(mode, "duplicate-key")) {
    Reject(archive, "\"version\":1,\"source_format\":1", "\"version\":1,\"version\":1");
    Reject(archive, "\"reference\":\"\"", "\"transaction\":\"other\"");
  } else if (!strcmp(mode, "unknown-field")) {
    Reject(archive, "\"version\":1", "\"unexpected\":1");
    Reject(archive, "\"reference\":\"\"", "\"extra\":\"\"");
  } else if (!strcmp(mode, "date"))
    Reject(archive, "2026-10-12T10:00:00Z", "2026-02-30");
  else if (!strcmp(mode, "amount"))
    Reject(archive, "\"amount\":\"12.50\"", "\"amount\":\"1e2\"");
  else if (!strcmp(mode, "control"))
    Reject(archive, "practice-account", "practice\\u007faccount");
  else if (!strcmp(mode, "invalid-utf8"))
    Reject(archive, "Example credit", "\xc0\xaf");
  else if (!strcmp(mode, "duplicate-id") || !strcmp(mode, "late-failure")) {
    const char *start = strstr(archive, "\"transactions\":[");
    CHECK(start != NULL);
    start += strlen("\"transactions\":[");
    const char *end = strstr(start, "]}\n");
    CHECK(end != NULL);
    size_t prefix = (size_t)(end - archive), rowLength = (size_t)(end - start);
    char *doubled = malloc(length + rowLength + 2U);
    CHECK(doubled != NULL);
    memcpy(doubled, archive, prefix);
    doubled[prefix] = ',';
    memcpy(doubled + prefix + 1U, start, rowLength);
    strcpy(doubled + prefix + 1U + rowLength, end);
    if (!strcmp(mode, "duplicate-id")) {
      UmiBankFeed *rejected = NULL;
      CHECK(UmiBankFeedArchiveDecode(doubled, strlen(doubled), &rejected, NULL) ==
            UMI_STATUS_ALREADY_EXISTS);
      CHECK(rejected == NULL);
    } else {
      char *second = doubled + prefix + 1U;
      char *amount = strstr(second, "12.50");
      CHECK(amount != NULL);
      memcpy(amount, "1e222", 5U);
      UmiBankFeed *rejected = (UmiBankFeed *)1;
      CHECK(UmiBankFeedArchiveDecode(doubled, strlen(doubled), &rejected, NULL) != UMI_STATUS_OK &&
            rejected == NULL);
    }
    free(doubled);
  } else if (!strcmp(mode, "truncated")) {
    for (size_t cut = 1U; cut < 8U; ++cut) {
      UmiBankFeed *rejected = (UmiBankFeed *)1;
      CHECK(UmiBankFeedArchiveDecode(archive, length - cut - 1U, &rejected, NULL) != UMI_STATUS_OK);
      CHECK(rejected == NULL);
    }
  } else if (!strcmp(mode, "invalid-api")) {
    char *bad = (char *)1;
    size_t badLength = 99U;
    UmiBankFeed *rejected = (UmiBankFeed *)1;
    CHECK(UmiBankFeedArchiveEncode(NULL, &info, &bad, &badLength) == UMI_STATUS_INVALID_ARGUMENT &&
          bad == NULL && badLength == 0U);
    CHECK(UmiBankFeedArchiveEncode(feed, &info, NULL, &badLength) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBankFeedArchiveEncode(feed, &info, &bad, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiBankFeedArchiveInfo invalid = info;
    memset(invalid.sourceSha256, 'a', sizeof invalid.sourceSha256);
    CHECK(UmiBankFeedArchiveEncode(feed, &invalid, &bad, &badLength) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          bad == NULL);
    CHECK(UmiBankFeedArchiveDecode(NULL, 1U, &rejected, NULL) == UMI_STATUS_INVALID_ARGUMENT &&
          rejected == NULL);
    CHECK(UmiBankFeedArchiveDecode(archive, length, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBankFeedArchiveDecode(archive, (size_t)UMI_BANK_FEED_MAX_BYTES + 1U, &rejected,
                                   NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
  } else
    CHECK(!strcmp(mode, "json") || !strcmp(mode, "csv") || !strcmp(mode, "mapped") ||
          !strcmp(mode, "multiline") || !strcmp(mode, "missing-id") || !strcmp(mode, "capacity") ||
          !strcmp(mode, "empty") || !strcmp(mode, "unicode") || !strcmp(mode, "precision") ||
          !strcmp(mode, "pagination") || !strcmp(mode, "ownership"));
  /* Neither original bytes nor the first feed remain alive at re-encoding. */
  UmiBankFeedDestroy(feed);
  free(input);
  UmiBankFeedArchiveFree(archive);
  archive = NULL;
  CHECK(UmiBankFeedArchiveEncode(copy, &restored, &archive, &length) == UMI_STATUS_OK);
  CHECK(strstr(archive, restored.sourceSha256) != NULL);
  UmiBankFeedArchiveFree(archive);
  UmiBankFeedDestroy(copy);
  UmiBankFeedArchiveFree(NULL);
  return 0;
}
