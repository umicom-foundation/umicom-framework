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
/* Retain the original extension map for review. The active map adds the
 * registered ICO media type so nosniff previews can display canonical icons. */
#if 0
const char *umi_web_mime_from_path(const char *path){const char *e;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(path==NULL)return "application/octet-stream";e=strrchr(path,'.');/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL)return "application/octet-stream";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".html")==0||strcmp(e,".htm")==0)return "text/html; charset=utf-8";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".css")==0)return "text/css; charset=utf-8";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".js")==0)return "text/javascript; charset=utf-8";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".json")==0)return "application/json";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".svg")==0)return "image/svg+xml";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".png")==0)return "image/png";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".jpg")==0||strcmp(e,".jpeg")==0)return "image/jpeg";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,".txt")==0)return "text/plain; charset=utf-8";return "application/octet-stream";}
#endif
const char *umi_web_mime_from_path(const char *path)
{
    if (path == NULL) return "application/octet-stream";
    const char *extension = strrchr(path, '.');
    if (extension == NULL) return "application/octet-stream";
    static const struct { const char *extension, *type; } types[] = {
        {".html", "text/html; charset=utf-8"}, {".htm", "text/html; charset=utf-8"},
        {".css", "text/css; charset=utf-8"}, {".js", "text/javascript; charset=utf-8"},
        {".json", "application/json"}, {".svg", "image/svg+xml"},
        {".ico", "image/vnd.microsoft.icon"}, {".png", "image/png"},
        {".jpg", "image/jpeg"}, {".jpeg", "image/jpeg"}, {".txt", "text/plain; charset=utf-8"}
    };
    for (size_t i = 0U; i < sizeof(types) / sizeof(types[0]); ++i)
        if (strcmp(extension, types[i].extension) == 0) return types[i].type;
    return "application/octet-stream";
}
