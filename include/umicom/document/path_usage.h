/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/path_usage.h
 *
 * PURPOSE:
 *   Identify open document paths before workspace filesystem operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_PATH_USAGE_H
#define UMICOM_DOCUMENT_PATH_USAGE_H
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Inspect every open working copy, including clean and inactive documents.
 * The absolute source and optional destination are compared lexically using
 * Framework path rules. Equal paths and descendants count as in use.
 * No I/O, save, focus, close or document mutation occurs. Symlink/hard-link
 * aliases are not resolved. Use alongside link-rejecting workspace operations.
 * Serialize coordinator access and the following file operation on its owner
 * thread. This is an observation, not a lasting lock. Output is cleared first. */
UmiStatus UmiDocumentCoordinatorPathInUse(const UmiDocumentCoordinator *coordinator,
                                          const char *source, const char *destination,
                                          int *outInUse);
#ifdef __cplusplus
}
#endif
#endif
