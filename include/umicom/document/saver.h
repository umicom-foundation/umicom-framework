/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/saver.h
 *
 * PURPOSE:
 *   Define encoding-aware and atomic provider-neutral document persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_SAVER_H
#define UMICOM_DOCUMENT_SAVER_H

#include "umicom/document/provider.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiDocumentSaveOptions umi_document_save_options_default(void);
UmiStatus umi_document_saver_write(const UmiDocumentProvider *provider,
                                   const char *resource,
                                   const char *utf8_text,
                                   size_t text_length,
                                   const UmiDocumentSaveOptions *options,
                                   UmiDocumentSaveResult *out_result);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_SAVER_H */
