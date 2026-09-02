/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/mime.c
 *
 * PURPOSE:
 *   Implement common static-file MIME type detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/mime.h"
#include <string.h>
/*
 * Provide the web mime from path operation used by this module and its client
 * applications.
 */
const char *umi_web_mime_from_path(const char *path){const char *e;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(path==NULL)return "application/octet-stream";e=strrchr(path,'.');/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL)return "application/octet-stream";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".html")==0||strcmp(e,".htm")==0)return "text/html; charset=utf-8";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".css")==0)return "text/css; charset=utf-8";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".js")==0)return "text/javascript; charset=utf-8";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".json")==0)return "application/json";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".svg")==0)return "image/svg+xml";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".png")==0)return "image/png";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".jpg")==0||strcmp(e,".jpeg")==0)return "image/jpeg";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".txt")==0)return "text/plain; charset=utf-8";return "application/octet-stream";}
