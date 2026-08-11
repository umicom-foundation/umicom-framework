/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/mime.c
 *
 * PURPOSE:
 *   Implement common static-file MIME type detection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/mime.h"
#include <string.h>
const char *umi_web_mime_from_path(const char *path){const char *e;if(path==NULL)return "application/octet-stream";e=strrchr(path,'.');if(e==NULL)return "application/octet-stream";if(strcmp(e,".html")==0||strcmp(e,".htm")==0)return "text/html; charset=utf-8";if(strcmp(e,".css")==0)return "text/css; charset=utf-8";if(strcmp(e,".js")==0)return "text/javascript; charset=utf-8";if(strcmp(e,".json")==0)return "application/json";if(strcmp(e,".svg")==0)return "image/svg+xml";if(strcmp(e,".png")==0)return "image/png";if(strcmp(e,".jpg")==0||strcmp(e,".jpeg")==0)return "image/jpeg";if(strcmp(e,".txt")==0)return "text/plain; charset=utf-8";return "application/octet-stream";}
