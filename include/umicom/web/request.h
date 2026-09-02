/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/request.h
 *
 * PURPOSE:
 *   Represent one parsed HTTP request independently of a socket or GUI toolkit.
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

#ifndef UMICOM_WEB_REQUEST_H
#define UMICOM_WEB_REQUEST_H
#include "umicom/web/header.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web request data shared with callers of this public contract.
 */
typedef struct UmiWebRequest {
    UmiHttpMethod method;
    char path[UMI_WEB_PATH_CAPACITY];
    char query[UMI_WEB_QUERY_CAPACITY];
    char version[16];
    UmiWebHeader headers[UMI_WEB_MAX_HEADERS];
    size_t header_count;
    char body[UMI_WEB_BODY_CAPACITY];
    size_t body_length;
} UmiWebRequest;
/**
 * Initialise web request from caller-provided values so later operations receive a known
 * state.
 */
void umi_web_request_init(UmiWebRequest *request);
/**
 * Provide the web request set target operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_request_set_target(UmiWebRequest *request,const char *target);
/**
 * Provide the web request header operation used by this module and its client
 * applications.
 */
const char *umi_web_request_header(const UmiWebRequest *request,const char *name);
#ifdef __cplusplus
}
#endif
#endif
