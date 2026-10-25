/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_language_detection_names.c
 *
 * PURPOSE:
 *   Verify filename identity, path basename handling and exact extension precedence without reading
 * files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/language_detection.h"
#include "umicom/platform/path.h"
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
  const char *name = argv[1];
  UmiDocumentLanguageIdentity identity = {0};
  if (!strcmp(name, "bounded")) {
    char text[UMI_PATH_CAPACITY];
    memset(text, 'x', sizeof text);
    strcpy(identity.language_id, "guard");
    CHECK(umi_document_language_detect(text, &identity) == UMI_STATUS_CAPACITY_EXCEEDED &&
          !strcmp(identity.language_id, "guard"));
    return 0;
  }
  if (!strcmp(name, "arguments")) {
    strcpy(identity.language_id, "guard");
    CHECK(umi_document_language_detect(NULL, &identity) == UMI_STATUS_INVALID_ARGUMENT &&
          !strcmp(identity.language_id, "guard"));
    CHECK(umi_document_language_detect("x.c", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  static const struct {
    const char *name, *input, *language;
  } rows[] = {{"cmake", "CMakeLists.txt", "cmake"},
              {"makefile", "Makefile", "makefile"},
              {"gnumakefile", "GNUmakefile", "makefile"},
              {"shell-dot", ".bashrc", "sh"},
              {"nested-unix", "/src/project/CMakeLists.txt", "cmake"},
              {"nested-windows", "C:\\src\\Makefile", "makefile"},
              {"upper-python", "MODEL.PY", "python3"},
              {"uppercase-c", "source.C", "cpp"},
              {"uppercase-asm", "boot.S", "asm"},
              {"rust", "main.rs", "rust"},
              {"go", "main.go", "go"},
              {"csharp", "App.cs", "c-sharp"},
              {"java", "App.java", "java"},
              {"kotlin", "App.kt", "kotlin"},
              {"swift", "App.swift", "swift"},
              {"jsonc", "config.jsonc", "jsonc"},
              {"ruby", "app.rb", "ruby"},
              {"perl", "app.pl", "perl"},
              {"r", "model.R", "r"},
              {"lua", "app.lua", "lua"},
              {"toml", "Cargo.toml", "toml"},
              {"ini", "settings.ini", "ini"},
              {"old-json", "data.json", "json"},
              {"unknown", "note.unrecognised", "plain"},
              {"directory-dot", "/folder.py/noextension", "plain"},
              {"empty", "", "plain"},
              {"trailing-dot", "name.", "plain"}};
  for (size_t i = 0U; i < sizeof rows / sizeof rows[0]; ++i)
    if (!strcmp(name, rows[i].name)) {
      CHECK(umi_document_language_detect(rows[i].input, &identity) == UMI_STATUS_OK);
      CHECK(!strcmp(identity.language_id, rows[i].language) && identity.text &&
            identity.mime_type[0] && identity.icon_name[0]);
      return 0;
    }
  return 2;
}
