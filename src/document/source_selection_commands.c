/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/source_selection_commands.c
 *
 * PURPOSE:
 *   Keep source selection command identities and labels shared across application frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/source_selection.h"
#include <string.h>
static const UmiDocumentSourceSelectionCommand Commands[] = {
    {"umicom.document.select-source-word", "Select source word", UMI_EDITOR_SELECT_SOURCE_WORD},
    {"umicom.document.select-line-content", "Select line content", UMI_EDITOR_SELECT_LINE_CONTENT},
    {"umicom.document.select-line-with-ending", "Select line with ending",
     UMI_EDITOR_SELECT_LINE_WITH_ENDING}};
size_t UmiDocumentSourceSelectionCommandCount(void) { return sizeof Commands / sizeof Commands[0]; }
const UmiDocumentSourceSelectionCommand *UmiDocumentSourceSelectionCommandAt(size_t index) {
  return index < UmiDocumentSourceSelectionCommandCount() ? &Commands[index] : NULL;
}
UmiStatus UmiDocumentSourceSelectionCommandFromId(const char *id,
                                                  UmiEditorSourceSelectionKind *out) {
  if (id == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0U; i < UmiDocumentSourceSelectionCommandCount(); ++i)
    if (!strcmp(id, Commands[i].id)) {
      *out = Commands[i].kind;
      return UMI_STATUS_OK;
    }
  return UMI_STATUS_NOT_FOUND;
}
