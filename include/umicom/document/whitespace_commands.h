/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/whitespace_commands.h
 *
 * PURPOSE:
 *   Keep whitespace command identities, presentation labels and document policy resolution shared.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_WHITESPACE_COMMANDS_H
#define UMICOM_DOCUMENT_WHITESPACE_COMMANDS_H
#include "umicom/document/indentation_override.h"
#include "umicom/editor/whitespace.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDocumentWhitespaceCommand {
  const char *id, *label, *keywords;
  UmiEditorWhitespaceOperation operation;
  int selected_lines;
} UmiDocumentWhitespaceCommand;
/* Borrow immutable descriptors for the process lifetime. Count and At never
 * touch document state; an out-of-range index returns NULL. */
size_t UmiDocumentWhitespaceCommandCount(void);
const UmiDocumentWhitespaceCommand *UmiDocumentWhitespaceCommandAt(size_t index);
/* Resolve the exact registered ID and the named document's current indentation
 * width. No command changes policy or text. Failed reads leave output intact. */
UmiStatus UmiDocumentWhitespaceCommandOptions(const UmiDocumentCoordinator *owner, UmiDocumentId id,
                                              const char *command_id,
                                              UmiEditorWhitespaceOptions *out_options);
#ifdef __cplusplus
}
#endif
#endif
