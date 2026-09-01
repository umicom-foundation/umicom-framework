/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/response.h
 *
 * PURPOSE:
 *   Build and serialise HTTP responses with bounded headers and bodies.
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

#ifndef UMICOM_WEB_RESPONSE_H
#define UMICOM_WEB_RESPONSE_H
#include "umicom/web/header.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebResponse {
    int status;
    UmiWebHeader headers[UMI_WEB_MAX_HEADERS];
    size_t header_count;
    char body[UMI_WEB_BODY_CAPACITY];
    size_t body_length;
} UmiWebResponse;
void umi_web_response_init(UmiWebResponse *response);
UmiStatus umi_web_response_set_header(UmiWebResponse *response,const char *name,const char *value);
UmiStatus umi_web_response_set_text(UmiWebResponse *response,int status,const char *content_type,const char *body);
UmiStatus umi_web_response_format(const UmiWebResponse *response,char *out_text,size_t capacity,size_t *out_length);
#ifdef __cplusplus
}
#endif
#endif
