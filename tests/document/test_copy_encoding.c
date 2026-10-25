/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_copy_encoding.c
 *
 * PURPOSE:
 *   Check immutable export serialization, Unicode validation and format boundaries independently of
 * disk I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../src/document/copy_internal.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *cases[] = {"utf8",   "bom",          "le",  "be",    "empty",    "mixed",
                         "nonbmp", "invalid-utf8", "nul", "limit", "encoding", "arguments"};
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(name, cases[i]) == 0)
      known = true;
  CHECK(known);
  UmiDocumentCopyInfo info = {.document_id = 1U, .encoding = UMI_DOCUMENT_ENCODING_UTF8};
  strcpy(info.display_name, "copy.c");
  const char *text = "A\n";
  size_t length = 2U;
  const unsigned char *expected = (const unsigned char *)text;
  size_t expectedSize = length;
  const unsigned char bom[] = {0xefU, 0xbbU, 0xbfU, 'A', '\n'},
                      le[] = {0xffU, 0xfeU, 'A', 0U, '\n', 0U},
                      be[] = {0xfeU, 0xffU, 0U, 'A', 0U, '\n'},
                      nonbmp[] = {0xffU, 0xfeU, 0x3dU, 0xd8U, 0U, 0xdeU};
  if (strcmp(name, "bom") == 0) {
    info.encoding = UMI_DOCUMENT_ENCODING_UTF8_BOM;
    expected = bom;
    expectedSize = sizeof bom;
  }
  if (strcmp(name, "le") == 0) {
    info.encoding = UMI_DOCUMENT_ENCODING_UTF16_LE;
    expected = le;
    expectedSize = sizeof le;
  }
  if (strcmp(name, "be") == 0) {
    info.encoding = UMI_DOCUMENT_ENCODING_UTF16_BE;
    expected = be;
    expectedSize = sizeof be;
  }
  if (strcmp(name, "empty") == 0) {
    text = "";
    length = 0U;
    expected = (const unsigned char *)text;
    expectedSize = 0U;
  }
  if (strcmp(name, "mixed") == 0) {
    text = "A\r\nB\rC\n";
    length = strlen(text);
    expected = (const unsigned char *)text;
    expectedSize = length;
  }
  if (strcmp(name, "nonbmp") == 0) {
    text = "\xf0\x9f\x98\x80";
    length = 4U;
    info.encoding = UMI_DOCUMENT_ENCODING_UTF16_LE;
    expected = nonbmp;
    expectedSize = sizeof nonbmp;
  }
  UmiStatus wanted = UMI_STATUS_OK;
  if (strcmp(name, "invalid-utf8") == 0) {
    text = "\xc0\xaf";
    length = 2U;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(name, "nul") == 0) {
    text = "A\0B";
    length = 3U;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(name, "limit") == 0) {
    length = UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U;
    wanted = UMI_STATUS_CAPACITY_EXCEEDED;
  }
  if (strcmp(name, "encoding") == 0) {
    info.encoding = UMI_DOCUMENT_ENCODING_BINARY;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  UmiDocumentCopy *copy = NULL;
  if (strcmp(name, "arguments") == 0) {
    CHECK(DocumentCopyCreate(NULL, text, length, &copy) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(DocumentCopyCreate(&info, NULL, 0U, &copy) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(DocumentCopyCreate(&info, text, length, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiDocumentCopyDestroy(NULL);
    return 0;
  }
  CHECK(DocumentCopyCreate(&info, text, length, &copy) == wanted);
  if (wanted != UMI_STATUS_OK) {
    CHECK(copy == NULL);
    return 0;
  }
  UmiDocumentCopyInfo read;
  CHECK(UmiDocumentCopyInspect(copy, &read) == UMI_STATUS_OK && read.encoding == info.encoding &&
        read.text_bytes == length);
  const unsigned char *bytes = NULL;
  size_t count = 0U;
  CHECK(UmiDocumentCopyRead(copy, &bytes, &count) == UMI_STATUS_OK);
  CHECK(count == expectedSize && read.file_bytes == count && memcmp(bytes, expected, count) == 0);
  UmiDocumentCopyDestroy(copy);
  return 0;
}
