/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/comment_style.c
 *
 * PURPOSE:
 *   Centralize known language comment tokens for document, native and application clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/comment_style.h"
#include <string.h>
static const UmiEditorLineCommentStyle styles[] = {{"c", "C", "//"},
                                                   {"cpp", "C++", "//"},
                                                   {"javascript", "JavaScript", "//"},
                                                   {"typescript", "TypeScript", "//"},
                                                   {"java", "Java", "//"},
                                                   {"c-sharp", "C#", "//"},
                                                   {"csharp", "C#", "//"},
                                                   {"rust", "Rust", "//"},
                                                   {"go", "Go", "//"},
                                                   {"kotlin", "Kotlin", "//"},
                                                   {"swift", "Swift", "//"},
                                                   {"jsonc", "JSON with comments", "//"},
                                                   {"python3", "Python", "#"},
                                                   {"python", "Python", "#"},
                                                   {"sh", "Shell", "#"},
                                                   {"bash", "Bash", "#"},
                                                   {"powershell", "PowerShell", "#"},
                                                   {"cmake", "CMake", "#"},
                                                   {"makefile", "Makefile", "#"},
                                                   {"yaml", "YAML", "#"},
                                                   {"toml", "TOML", "#"},
                                                   {"ruby", "Ruby", "#"},
                                                   {"perl", "Perl", "#"},
                                                   {"r", "R", "#"},
                                                   {"sql", "SQL", "--"},
                                                   {"lua", "Lua", "--"},
                                                   {"ini", "INI", ";"}};
size_t UmiEditorLineCommentStyleCount(void) { return sizeof styles / sizeof styles[0]; }
const UmiEditorLineCommentStyle *UmiEditorLineCommentStyleAt(size_t index) {
  return index < UmiEditorLineCommentStyleCount() ? &styles[index] : NULL;
}
UmiStatus UmiEditorLineCommentStyleFind(const char *language,
                                        const UmiEditorLineCommentStyle **out) {
  if (language == NULL || out == NULL || language[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0U;
  while (length < 128U && language[length] != '\0')
    ++length;
  if (length == 128U)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  for (size_t i = 0U; i < UmiEditorLineCommentStyleCount(); ++i)
    if (strcmp(language, styles[i].language_id) == 0) {
      *out = &styles[i];
      return UMI_STATUS_OK;
    }
  return UMI_STATUS_NOT_FOUND;
}
