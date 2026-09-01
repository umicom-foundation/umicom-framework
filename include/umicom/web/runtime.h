/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/runtime.h
 *
 * PURPOSE:
 *   Process complete HTTP request text through the reusable web service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_RUNTIME_H
#define UMICOM_WEB_RUNTIME_H
#include "umicom/web/service.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_web_runtime_process(UmiWebService *service,const char *request_text,size_t request_length,char *out_response,size_t capacity,size_t *out_length);
#ifdef __cplusplus
}
#endif
#endif
