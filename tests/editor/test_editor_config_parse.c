/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_config_parse.c
 *
 * PURPOSE:
 *   Exercise configuration text and pattern boundaries with explicit memory-only expectations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "editor_config_fixture.h"
typedef struct Example {
  const char *name, *text;
  size_t sections, pairs;
  int root;
  const char *key, *value;
  UmiStatus status;
  size_t line;
} Example;
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const Example cases[] = {
      {"empty", "", 0U, 0U, 0, NULL, NULL, UMI_STATUS_OK, 0U},
      {"comments", "# comment\n ; comment\r\n\n", 0U, 0U, 0, NULL, NULL, UMI_STATUS_OK, 0U},
      {"root", " root = TRUE\n", 0U, 1U, 1, NULL, NULL, UMI_STATUS_OK, 0U},
      {"root-last", "root=true\nroot=false\n", 0U, 2U, 0, NULL, NULL, UMI_STATUS_OK, 0U},
      {"section-root", "[*]\nroot=true\n", 1U, 1U, 0, NULL, NULL, UMI_STATUS_OK, 0U},
      {"trimming", " [*.c] \n Indent_Style = space \n", 1U, 1U, 0, "indent_style", "space",
       UMI_STATUS_OK, 0U},
      {"inline-comment", "[*]\ncustom = value #literal;still\n", 1U, 1U, 0, "custom",
       "value #literal;still", UMI_STATUS_OK, 0U},
      {"empty-value", "[*]\ncustom=\n", 1U, 1U, 0, "custom", "", UMI_STATUS_OK, 0U},
      {"equals-value", "[*]\na=b=c\n", 1U, 1U, 0, "a", "b=c", UMI_STATUS_OK, 0U},
      {"unicode", "[é.c]\ncustom = café\n", 1U, 1U, 0, "custom", "café", UMI_STATUS_OK, 0U},
      {"bom", "﻿root=true\n", 0U, 1U, 1, NULL, NULL, UMI_STATUS_OK, 0U},
      {"crlf", "[*]\r\ncharset=utf-8\r\n", 1U, 1U, 0, "charset", "utf-8", UMI_STATUS_OK, 0U},
      {"bare-cr", "[*]\r", 0U, 0U, 0, NULL, NULL, UMI_STATUS_PARSE_ERROR, 1U},
      {"bare-cr-middle", "[*]\nx=y\rc=z", 0U, 0U, 0, NULL, NULL, UMI_STATUS_PARSE_ERROR, 2U},
      {"no-assignment", "[*]\ninvalid", 0U, 0U, 0, NULL, NULL, UMI_STATUS_PARSE_ERROR, 2U},
      {"empty-key", "[*]\n=bad", 0U, 0U, 0, NULL, NULL, UMI_STATUS_PARSE_ERROR, 2U},
      {"empty-section", "[]\nx=y", 0U, 0U, 0, NULL, NULL, UMI_STATUS_PARSE_ERROR, 1U},
      {"section-trailing", "[*.c] #wrong", 0U, 0U, 0, NULL, NULL, UMI_STATUS_PARSE_ERROR, 1U},
      {"unknown-key", "[*]\nCUSTOM OPTION = VaLue", 1U, 1U, 0, "custom option", "VaLue",
       UMI_STATUS_OK, 0U},
      {"preserve-value-case", "[*]\ncharset = UTF-8", 1U, 1U, 0, "charset", "UTF-8", UMI_STATUS_OK,
       0U}};
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i].name)) {
      const Example *e = &cases[i];
      UmiEditorConfig *config = NULL;
      UmiEditorConfigDiagnostic d;
      CHECK(UmiEditorConfigParse(e->text, strlen(e->text), &config, &d) == e->status);
      CHECK(d.status == e->status && d.line == e->line);
      if (e->status != UMI_STATUS_OK) {
        CHECK(config == NULL);
        return 0;
      }
      CHECK(UmiEditorConfigSectionCount(config) == e->sections &&
            UmiEditorConfigPairCount(config) == e->pairs &&
            UmiEditorConfigIsRoot(config) == e->root);
      if (e->key) {
        UmiEditorConfigPair pair;
        CHECK(UmiEditorConfigPairAt(config, 0U, &pair) == UMI_STATUS_OK);
        CHECK(!strcmp(pair.key, e->key) && !strcmp(pair.value, e->value));
      }
      UmiEditorConfigDestroy(config);
      return 0;
    }
  UmiEditorConfig *config = NULL;
  UmiEditorConfigDiagnostic diagnostic;
  if (!strcmp(argv[1], "arguments")) {
    CHECK(UmiEditorConfigParse(NULL, 0, &config, &diagnostic) == UMI_STATUS_INVALID_ARGUMENT &&
          config == NULL);
    CHECK(UmiEditorConfigParse("", 0, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorConfigPairAt(NULL, 0, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorConfigSectionAt(NULL, 0, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiEditorConfigDestroy(NULL);
    return 0;
  }
  if (!strcmp(argv[1], "nul")) {
    CHECK(UmiEditorConfigParse("[*]\na=x\0y", 10U, &config, NULL) == UMI_STATUS_INVALID_ARGUMENT &&
          config == NULL);
    return 0;
  }
  if (!strcmp(argv[1], "utf8")) {
    CHECK(UmiEditorConfigParse("[*]\na=\xc0\xaf", 8U, &config, NULL) != UMI_STATUS_OK &&
          config == NULL);
    return 0;
  }
  if (!strcmp(argv[1], "size")) {
    CHECK(UmiEditorConfigParse("", UMI_EDITOR_CONFIG_MAXIMUM_BYTES + 1U, &config, NULL) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
  }
  if (!strcmp(argv[1], "maximum-key") || !strcmp(argv[1], "long-key") ||
      !strcmp(argv[1], "maximum-value") || !strcmp(argv[1], "long-value")) {
    int key = strstr(argv[1], "key") != NULL, excess = !strncmp(argv[1], "long", 4U);
    size_t length =
        (key ? UMI_EDITOR_CONFIG_MAXIMUM_KEY : UMI_EDITOR_CONFIG_MAXIMUM_VALUE) + (size_t)excess;
    char *text = malloc(length + 16U);
    CHECK(text != NULL);
    memcpy(text, "[*]\n", 4U);
    size_t at = 4U;
    if (!key) {
      memcpy(text + at, "a=", 2U);
      at += 2U;
    }
    memset(text + at, 'a', length);
    at += length;
    if (key) {
      memcpy(text + at, "=v", 2U);
      at += 2U;
    }
    text[at] = 0;
    UmiStatus status = UmiEditorConfigParse(text, at, &config, NULL);
    free(text);
    CHECK(status == (excess ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK));
    UmiEditorConfigDestroy(config);
    return 0;
  }
  if (!strcmp(argv[1], "section-limit") || !strcmp(argv[1], "pair-limit")) {
    int sections = !strcmp(argv[1], "section-limit");
    size_t count =
        (sections ? UMI_EDITOR_CONFIG_MAXIMUM_SECTIONS : UMI_EDITOR_CONFIG_MAXIMUM_PAIRS) + 1U;
    char *text = calloc(count * 8U + 8U, 1U);
    CHECK(text != NULL);
    strcpy(text, "[*]\n");
    for (size_t i = sections ? 1U : 0U; i < count; ++i)
      strcat(text, sections ? "[*]\n" : "a=b\n");
    UmiStatus status = UmiEditorConfigParse(text, strlen(text), &config, NULL);
    free(text);
    CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && config == NULL);
    return 0;
  }
  if (!strcmp(argv[1], "ownership")) {
    char text[] = "[*]\na=original\n";
    CHECK(UmiEditorConfigParse(text, strlen(text), &config, NULL) == UMI_STATUS_OK);
    memset(text, 'x', strlen(text));
    UmiEditorConfigPair pair;
    CHECK(UmiEditorConfigPairAt(config, 0, &pair) == UMI_STATUS_OK &&
          !strcmp(pair.value, "original"));
    UmiEditorConfigPair sentinel = pair;
    CHECK(UmiEditorConfigPairAt(config, 99, &pair) == UMI_STATUS_NOT_FOUND &&
          pair.value == sentinel.value);
    UmiEditorConfigDestroy(config);
    return 0;
  }
  return 2;
}
