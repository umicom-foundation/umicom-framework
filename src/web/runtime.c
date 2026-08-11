/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/runtime.c
 *
 * PURPOSE:
 *   Run a complete in-memory HTTP request through the web service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/runtime.h"
#include "umicom/web/parser.h"
UmiStatus umi_web_runtime_process(UmiWebService *service,const char *request_text,size_t request_length,char *out_response,size_t capacity,size_t *out_length){UmiWebRequest request;UmiWebResponse response;UmiStatus s;if(service==NULL||request_text==NULL||out_response==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_web_parse_request(request_text,request_length,&request);if(s!=UMI_STATUS_OK)return s;s=umi_web_service_handle(service,&request,&response);if(s!=UMI_STATUS_OK)return s;return umi_web_response_format(&response,out_response,capacity,out_length);}
