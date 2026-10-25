/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_config_result.c
 *
 * PURPOSE:
 *   Verify source precedence, copied provenance and explicit interpretation of project settings.
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
  const char *name, *source, *file, *key, *value, *pattern;
  size_t line;
} Example;
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const Example cases[] = {
      {"section-order", "[*]\na=one\n[*.c]\na=two\n", "main.c", "a", "two", "*.c", 4U},
      {"same-section", "[*]\na=one\na=two\n", "main.c", "a", "two", "*", 3U},
      {"unmatched", "[*.h]\na=one\n", "main.c", "a", NULL, NULL, 0U},
      {"preamble-ignored", "a=one\n[*]\nb=two\n", "main.c", "a", NULL, NULL, 0U},
      {"unset", "[*]\na=one\n[*.c]\na=UnSeT\n", "main.c", "a", NULL, NULL, 0U},
      {"unset-then-restore", "[*]\na=one\na=unset\na=three\n", "main.c", "a", "three", "*", 4U},
      {"case-key", "[*]\nA=one\na=two\n", "main.c", "A", "two", "*", 3U},
      {"unknown-property", "[*]\ncustom=value #kept\n", "main.c", "CUSTOM", "value #kept", "*", 2U},
      {"basename-nested", "[*.c]\na=one\n", "deep/main.c", "a", "one", "*.c", 2U},
      {"brace-pattern", "[*.{c,h}]\na=one\n", "main.h", "a", "one", "*.{c,h}", 2U},
      {"empty-value", "[*]\na=\n", "main.c", "a", "", "*", 2U},
      {"empty-config", "", "main.c", "a", NULL, NULL, 0U}};
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i].name)) {
      UmiEditorConfigResult *r = NULL;
      CHECK(ResolveOne(cases[i].source, cases[i].file, &r) == UMI_STATUS_OK);
      UmiEditorConfigProperty p = {0};
      UmiStatus status = UmiEditorConfigResultFind(r, cases[i].key, &p);
      if (cases[i].value) {
        CHECK(status == UMI_STATUS_OK);
        CHECK(!strcmp(p.value, cases[i].value) && !strcmp(p.source_name, ".editorconfig") &&
              !strcmp(p.pattern, cases[i].pattern) && p.line == cases[i].line);
      } else
        CHECK(status == UMI_STATUS_NOT_FOUND && p.value == NULL);
      UmiEditorConfigResultDestroy(r);
      return 0;
    }
  if (!strcmp(argv[1], "layer-precedence") || !strcmp(argv[1], "inner-root") ||
      !strcmp(argv[1], "layer-unset")) {
    UmiEditorConfig *a = NULL, *b = NULL;
    const char *outer = "root=true\n[*]\na=outer\nb=outer\n";
    const char *inner = !strcmp(argv[1], "inner-root")    ? "root=true\n[*]\na=inner\n"
                        : !strcmp(argv[1], "layer-unset") ? "[*]\na=unset\n"
                                                          : "[*]\na=inner\n";
    CHECK(UmiEditorConfigParse(outer, strlen(outer), &a, NULL) == UMI_STATUS_OK);
    CHECK(UmiEditorConfigParse(inner, strlen(inner), &b, NULL) == UMI_STATUS_OK);
    UmiEditorConfigLayer layers[] = {{a, "src/main.c", "root/.editorconfig"},
                                     {b, "main.c", "root/src/.editorconfig"}};
    UmiEditorConfigResult *r = NULL;
    CHECK(UmiEditorConfigResolve(layers, 2U, &r) == UMI_STATUS_OK);
    UmiEditorConfigDestroy(a);
    UmiEditorConfigDestroy(b);
    UmiEditorConfigProperty p;
    if (!strcmp(argv[1], "layer-unset"))
      CHECK(UmiEditorConfigResultFind(r, "a", &p) == UMI_STATUS_NOT_FOUND);
    else {
      CHECK(UmiEditorConfigResultFind(r, "a", &p) == UMI_STATUS_OK);
      CHECK(!strcmp(p.value, "inner") && !strcmp(p.source_name, "root/src/.editorconfig"));
    }
    if (!strcmp(argv[1], "inner-root"))
      CHECK(UmiEditorConfigResultFind(r, "b", &p) == UMI_STATUS_NOT_FOUND);
    else {
      CHECK(UmiEditorConfigResultFind(r, "b", &p) == UMI_STATUS_OK);
      CHECK(!strcmp(p.source_name, "root/.editorconfig"));
    }
    UmiEditorConfigResultDestroy(r);
    return 0;
  }
  if (!strcmp(argv[1], "malformed-pattern")) {
    UmiEditorConfigResult *r = NULL;
    CHECK(ResolveOne("[[*]\na=x\n", "main.c", &r) == UMI_STATUS_PARSE_ERROR && r == NULL);
    return 0;
  }
  if (!strcmp(argv[1], "invalid-empty-path")) {
    UmiEditorConfigResult *r = NULL;
    CHECK(ResolveOne("", "", &r) == UMI_STATUS_INVALID_ARGUMENT && r == NULL);
    return 0;
  }
  if (!strcmp(argv[1], "arguments")) {
    UmiEditorConfigResult *r = NULL;
    CHECK(UmiEditorConfigResolve(NULL, 1U, &r) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorConfigResolve(NULL, 0U, &r) == UMI_STATUS_OK &&
          UmiEditorConfigResultCount(r) == 0U);
    UmiEditorConfigProperty p = {0};
    CHECK(UmiEditorConfigResultAt(r, 0U, &p) == UMI_STATUS_NOT_FOUND && p.key == NULL);
    UmiEditorConfigResultDestroy(r);
    return 0;
  }
  if (!strcmp(argv[1], "layer-capacity")) {
    UmiEditorConfigLayer layers[UMI_EDITOR_CONFIG_MAXIMUM_LAYERS + 1U] = {{0}};
    UmiEditorConfigResult *r = NULL;
    CHECK(UmiEditorConfigResolve(layers, UMI_EDITOR_CONFIG_MAXIMUM_LAYERS + 1U, &r) ==
              UMI_STATUS_CAPACITY_EXCEEDED &&
          r == NULL);
    return 0;
  }
  return 2;
}
