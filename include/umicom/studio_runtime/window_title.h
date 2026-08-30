/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/window_title.h
 *
 * PURPOSE:
 *   Build the professional Studio window title from active workspace/document
 *   state while keeping title formatting out of toolkit code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_WINDOW_TITLE_H
#define UMICOM_STUDIO_RUNTIME_WINDOW_TITLE_H

#include "umicom/studio_runtime/document_state.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_window_title_build(
    const char *application_name,
    const char *workspace_name,
    const UmiStudioRuntimeDocumentState *documents,
    UmiStudioRuntimeWindowTitle *out_title);

#ifdef __cplusplus
}
#endif
#endif
