/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/cvm_file_translation.h
 *
 * PURPOSE:
 *   Translate an admitted local source file through the selected Umicom C SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TOOLCHAIN_CVM_FILE_TRANSLATION_H
#define UMICOM_TOOLCHAIN_CVM_FILE_TRANSLATION_H

#include "umicom/base/status.h"
#include "umicom_c/translate.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Translate a regular local input and exclusively create a new output file.
 * Both paths must be absolute and the caller must trust their parent folders.
 * Admission or translation refusal creates no file. An I/O error after
 * creation may leave a partial file for review. Existing files are preserved.
 * Optional diagnostics describe compiler refusals with one-based byte
 * locations when available. This synchronous service may block on storage. */
UmiStatus UmiCvmTranslateFile(UmcTranslationDirection direction, const char *input_path,
    const char *output_path, UmcDiagnostic *diagnostic);

#ifdef __cplusplus
}
#endif

#endif
