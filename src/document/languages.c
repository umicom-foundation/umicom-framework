/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/languages.c
 *
 * PURPOSE:
 *   Keep explicit language selection and filename detection on one shared identity policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/languages.h"
#include <string.h>
static const UmiDocumentLanguageChoice choices[] = {{"plain", "Plain text", "source.txt"},
                                                    {"c", "C", "source.c"},
                                                    {"cpp", "C++", "source.cpp"},
                                                    {"asm", "Assembly", "source.s"},
                                                    {"python3", "Python", "source.py"},
                                                    {"javascript", "JavaScript", "source.js"},
                                                    {"typescript", "TypeScript", "source.ts"},
                                                    {"json", "JSON", "source.json"},
                                                    {"jsonc", "JSON with comments", "source.jsonc"},
                                                    {"xml", "XML", "source.xml"},
                                                    {"html", "HTML", "source.html"},
                                                    {"css", "CSS", "source.css"},
                                                    {"markdown", "Markdown", "source.md"},
                                                    {"cmake", "CMake", "CMakeLists.txt"},
                                                    {"makefile", "Makefile", "Makefile"},
                                                    {"sh", "Shell", "source.sh"},
                                                    {"powershell", "PowerShell", "source.ps1"},
                                                    {"sql", "SQL", "source.sql"},
                                                    {"yaml", "YAML", "source.yaml"},
                                                    {"toml", "TOML", "source.toml"},
                                                    {"ini", "INI", "source.ini"},
                                                    {"java", "Java", "source.java"},
                                                    {"c-sharp", "C#", "source.cs"},
                                                    {"rust", "Rust", "source.rs"},
                                                    {"go", "Go", "source.go"},
                                                    {"kotlin", "Kotlin", "source.kt"},
                                                    {"swift", "Swift", "source.swift"},
                                                    {"ruby", "Ruby", "source.rb"},
                                                    {"perl", "Perl", "source.pl"},
                                                    {"r", "R", "source.r"},
                                                    {"lua", "Lua", "source.lua"}};
size_t UmiDocumentLanguageChoiceCount(void) { return sizeof choices / sizeof choices[0]; }
const UmiDocumentLanguageChoice *UmiDocumentLanguageChoiceAt(size_t index) {
  return index < UmiDocumentLanguageChoiceCount() ? &choices[index] : NULL;
}
UmiStatus UmiDocumentLanguageChoiceFind(const char *id, const UmiDocumentLanguageChoice **out) {
  if (id == NULL || out == NULL || id[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0U;
  while (length < UMI_DOCUMENT_LANGUAGE_CAPACITY && id[length] != '\0')
    ++length;
  if (length == UMI_DOCUMENT_LANGUAGE_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  for (size_t i = 0U; i < UmiDocumentLanguageChoiceCount(); ++i)
    if (!strcmp(id, choices[i].language_id)) {
      *out = &choices[i];
      return UMI_STATUS_OK;
    }
  return UMI_STATUS_NOT_FOUND;
}
UmiStatus UmiDocumentLanguageChoiceIdentity(const char *id, UmiDocumentLanguageIdentity *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  const UmiDocumentLanguageChoice *choice = NULL;
  UmiStatus status = UmiDocumentLanguageChoiceFind(id, &choice);
  if (status != UMI_STATUS_OK)
    return status;
  UmiDocumentLanguageIdentity result;
  status = umi_document_language_detect(choice->example_filename, &result);
  if (status == UMI_STATUS_OK && strcmp(result.language_id, id))
    return UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    *out = result;
  return status;
}
