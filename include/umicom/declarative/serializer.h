/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/serializer.h
 *
 * PURPOSE:
 *   Serialise semantic documents back into canonical line-oriented .umiapp text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

UmiStatus umi_decl_serialize(const UmiDeclDocument *document, char *out_text, size_t capacity, size_t *out_length);
UmiStatus umi_decl_serialize_file(const UmiDeclDocument *document, const char *path);

#ifdef __cplusplus
}
#endif

#endif
