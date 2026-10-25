/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/copy_internal.h
 *
 * PURPOSE:
 *   Keep owned serialization private while the coordinator supplies authoritative draft identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_COPY_INTERNAL_H
#define UMICOM_DOCUMENT_COPY_INTERNAL_H
#include "umicom/document/copy.h"
/* Only the coordinator supplies this identity; applications use CaptureCopy. */
UmiStatus DocumentCopyCreate(const UmiDocumentCopyInfo *info, const char *text, size_t bytes,
                             UmiDocumentCopy **outCopy);
#endif
