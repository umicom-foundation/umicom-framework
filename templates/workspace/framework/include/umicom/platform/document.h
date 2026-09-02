/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/platform/document.h
 *
 * PURPOSE:
 *   Declare the document contract shared by Framework services and thin
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_DOCUMENT_H
#define UMICOM_PLATFORM_DOCUMENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the document data shared with callers of this public contract.
 */
typedef struct UmiDocument {
    char path[512];
    char *text;
    size_t length;
    uint64_t revision;
    int dirty;
} UmiDocument;

/**
 * Initialise document from caller-provided values so later operations receive a known
 * state.
 */
void umi_document_init(UmiDocument *document);
/**
 * Release or reset state held by document so the same storage can be reused safely.
 */
void umi_document_dispose(UmiDocument *document);
/**
 * Provide the document open operation used by this module and its client applications.
 */
UmiStatus umi_document_open(UmiDocument *document, const char *path);
/**
 * Provide the document set text operation used by this module and its client applications.
 */
UmiStatus umi_document_set_text(UmiDocument *document, const char *text);
/**
 * Add document only after its inputs and available capacity have been checked.
 */
UmiStatus umi_document_append(UmiDocument *document, const char *text);
/**
 * Write document in its stable representation and report capacity or input failures to the
 * caller.
 */
UmiStatus umi_document_save(UmiDocument *document);

#ifdef __cplusplus
}
#endif

#endif
