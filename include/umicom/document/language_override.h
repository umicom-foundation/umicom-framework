/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/language_override.h
 *
 * PURPOSE:
 *   Review explicit per-document language selection while preserving draft text, cursor and editing
 * history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_LANGUAGE_OVERRIDE_H
#define UMICOM_DOCUMENT_LANGUAGE_OVERRIDE_H
#include "umicom/document/coordinator.h"
#include "umicom/document/languages.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Owner-thread session policy. The owner pointer identifies the observation;
 * it does not keep the coordinator alive. Overrides expire when a tab closes. */
typedef struct UmiDocumentLanguageReview {
  const UmiDocumentCoordinator *owner;
  UmiDocumentId document_id;
  uint64_t revision;
  char filename[UMI_DOCUMENT_NAME_CAPACITY];
  UmiDocumentLanguageIdentity detected;
  UmiDocumentLanguageIdentity effective;
  int overridden;
} UmiDocumentLanguageReview;
UmiStatus UmiDocumentCoordinatorInspectLanguage(const UmiDocumentCoordinator *owner,
                                                UmiDocumentId id, UmiDocumentLanguageReview *out);
UmiStatus UmiDocumentCoordinatorCheckLanguage(const UmiDocumentCoordinator *owner,
                                              const UmiDocumentLanguageReview *review);
/* NULL restores filename detection; a known language ID sets a session override.
 * The observation must still match, including filename and policy revision.
 * An identical valid choice is a no-op. Read-only text may change its language.
 * No text, history, selection, dirty flag or file changes. Refusals preserve out.
 * Saved/renamed files retain an override until explicitly restored or closed. */
UmiStatus UmiDocumentCoordinatorApplyLanguage(UmiDocumentCoordinator *owner,
                                              const UmiDocumentLanguageReview *review,
                                              const char *language_id,
                                              UmiDocumentLanguageReview *out);
#ifdef __cplusplus
}
#endif
#endif
