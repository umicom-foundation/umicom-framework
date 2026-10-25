/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/whitespace_commands.c
 *
 * PURPOSE:
 *   Resolve native whitespace actions through the named document indentation policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/whitespace_commands.h"
#include <string.h>
static const UmiDocumentWhitespaceCommand commands[] = {
    {"editor.trim-trailing-document", "Trim trailing whitespace in document",
     "trim spaces tabs trailing whitespace whole document", UMI_EDITOR_WHITESPACE_TRIM_TRAILING, 0},
    {"editor.trim-trailing-lines", "Trim trailing whitespace in selected lines",
     "trim spaces tabs trailing whitespace selection", UMI_EDITOR_WHITESPACE_TRIM_TRAILING, 1},
    {"editor.expand-tabs-document", "Expand leading tabs in document",
     "expand tabs spaces indentation whole document", UMI_EDITOR_WHITESPACE_EXPAND_LEADING_TABS, 0},
    {"editor.expand-tabs-lines", "Expand leading tabs in selected lines",
     "expand tabs spaces indentation selection", UMI_EDITOR_WHITESPACE_EXPAND_LEADING_TABS, 1},
    {"editor.compact-spaces-document", "Compact leading spaces in document",
     "compact spaces tabs indentation whole document", UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES,
     0},
    {"editor.compact-spaces-lines", "Compact leading spaces in selected lines",
     "compact spaces tabs indentation selection", UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES, 1}};
size_t UmiDocumentWhitespaceCommandCount(void) { return sizeof commands / sizeof commands[0]; }
const UmiDocumentWhitespaceCommand *UmiDocumentWhitespaceCommandAt(size_t index) {
  return index < UmiDocumentWhitespaceCommandCount() ? &commands[index] : NULL;
}
UmiStatus UmiDocumentWhitespaceCommandOptions(const UmiDocumentCoordinator *owner, UmiDocumentId id,
                                              const char *command_id,
                                              UmiEditorWhitespaceOptions *out) {
  if (owner == NULL || id == 0U || command_id == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0U; i < UmiDocumentWhitespaceCommandCount(); ++i)
    if (!strcmp(command_id, commands[i].id)) {
      UmiDocumentIndentationReview review;
      UmiStatus status = UmiDocumentCoordinatorInspectIndentation(owner, id, &review);
      if (status == UMI_STATUS_OK)
        *out = (UmiEditorWhitespaceOptions){commands[i].operation, review.effective.tab_width,
                                            commands[i].selected_lines};
      return status;
    }
  return UMI_STATUS_NOT_FOUND;
}
