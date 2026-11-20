/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/cvm_preview.h
 *
 * PURPOSE:
 *   Route an opted-in local bytecode preview through the native Umicom C VM.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_CVM_PREVIEW_H
#define UMICOM_WEB_CVM_PREVIEW_H

#include "umicom/web/route.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WEB_CVM_PREVIEW_PATH "/api/cvm/preview"
#define UMI_WEB_CVM_INT_INPUT_PATH "/api/cvm/preview-int"
#define UMI_WEB_CVM_ARTIFACT_MEDIA_TYPE "application/vnd.umicom.c-bytecode"
#define UMI_WEB_CVM_PREVIEW_MAX_FUEL 32768U

typedef struct UmiWebCvmPreviewConfig {
    size_t fuel;
} UmiWebCvmPreviewConfig;

/* Provide one stable configuration with 1..UMI_WEB_CVM_PREVIEW_MAX_FUEL
 * instructions. Mount only on a loopback listener behind the local request
 * gate. The handler reads one complete inline POST body and returns JSON with
 * an integer result or a fixed error category. It opens no files and exposes
 * no host imports to the VM. The listener still owns network admission. */
UmiStatus UmiWebCvmPreviewHandle(const UmiWebRequest *request, UmiWebResponse *response,
    void *context);

/* Use the same body and listener contract with one raw query field, input,
 * containing a signed 32-bit decimal integer. Bind an explicit input artifact
 * using an SDK that supports it. Its absence returns 503 for an admitted request. */
UmiStatus UmiWebCvmPreviewIntInputHandle(const UmiWebRequest *request, UmiWebResponse *response,
    void *context);

#ifdef __cplusplus
}
#endif

#endif
