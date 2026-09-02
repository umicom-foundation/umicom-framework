/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/serializer.h
 *
 * PURPOSE:
 *   Serialise semantic documents back into canonical line-oriented .umiapp text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_SERIALIZER_H
#define UMICOM_DECLARATIVE_SERIALIZER_H

#include "umicom/declarative/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Write decl in its stable representation and report capacity or input failures to the
 * caller.
 */
UmiStatus umi_decl_serialize(const UmiDeclDocument *document, char *out_text, size_t capacity, size_t *out_length);
/**
 * Provide the decl serialize file operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_serialize_file(const UmiDeclDocument *document, const char *path);

#ifdef __cplusplus
}
#endif

#endif
