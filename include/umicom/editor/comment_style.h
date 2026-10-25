/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/comment_style.h
 *
 * PURPOSE:
 *   Describe explicit language line-comment tokens without guessing unknown or dialect-dependent
 * syntax.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_COMMENT_STYLE_H
#define UMICOM_EDITOR_COMMENT_STYLE_H
#include "umicom/base/status.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiEditorLineCommentStyle {
  const char *language_id;
  const char *display_name;
  const char *prefix;
} UmiEditorLineCommentStyle;
/** Immutable built-in token catalogue. Strings live for the process lifetime.
 * It describes line syntax only, not a lexer or language-service capability.
 * JSON, HTML, CSS, Markdown, plain text and generic Assembly have no guessed
 * line prefix. Hosts retain explicit literal-prefix commands for such cases.
 * Lookup is exact and case-sensitive. Invalid/unknown IDs preserve out_style.
 * Style does not detect strings, heredocs, templates or embedded languages:
 * users must review selected source and can Undo the resulting draft. */
size_t UmiEditorLineCommentStyleCount(void);
const UmiEditorLineCommentStyle *UmiEditorLineCommentStyleAt(size_t index);
UmiStatus UmiEditorLineCommentStyleFind(const char *language_id,
                                        const UmiEditorLineCommentStyle **out_style);
#ifdef __cplusplus
}
#endif
#endif
