/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/languages.h
 *
 * PURPOSE:
 *   Discover supported document language choices without starting a language server or reading
 * files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_LANGUAGES_H
#define UMICOM_DOCUMENT_LANGUAGES_H
#include "umicom/document/language_detection.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDocumentLanguageChoice {
  const char *language_id;
  const char *display_name;
  /* Representative filename used by the shared detector; never opened. */
  const char *example_filename;
} UmiDocumentLanguageChoice;
/* Descriptors and their strings are immutable static storage, not caller-owned.
 * A choice identifies syntax policy, not installed compiler/server support. */
size_t UmiDocumentLanguageChoiceCount(void);
const UmiDocumentLanguageChoice *UmiDocumentLanguageChoiceAt(size_t index);
UmiStatus UmiDocumentLanguageChoiceFind(const char *language_id,
                                        const UmiDocumentLanguageChoice **out);
/* Resolve through the same filename detector used by ordinary Open. Failed
 * lookups preserve output; identifiers must terminate within language capacity. */
UmiStatus UmiDocumentLanguageChoiceIdentity(const char *language_id,
                                            UmiDocumentLanguageIdentity *out);
#ifdef __cplusplus
}
#endif
#endif
