/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/static_files.c
 *
 * PURPOSE:
 *   Implement safe static-file loading below a configured root.
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

#include "umicom/web/static_files.h"
#include "umicom/web/mime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "umicom/platform/rooted_files.h"
/*
 * Initialise web static files from caller-provided values so later operations receive a
 * known state.
 */
/* Static roots are now explicit absolute directories validated by Framework path policy, removing dependence on a changing process working directory.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_web_static_files_init(UmiWebStaticFiles *files,const char *root){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(files==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_web_copy_text(files->root,sizeof(files->root),root);}
#endif
UmiStatus umi_web_static_files_init(UmiWebStaticFiles *files, const char *root)
{
    if (files == NULL || root == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < sizeof(files->root) && root[length] != '\0')
        ++length;
    if (length == sizeof(files->root))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiRootedFileValidatePath(root, "index.html");
    if (status != UMI_STATUS_OK)
        return status;
    /* Reinitialising from the captured root is valid too. */
    memmove(files->root, root, length + 1U);
    return UMI_STATUS_OK;
}
/*
 * Provide the web static files serve operation used by this module and its client
 * applications.
 */

static int StaticHex(unsigned char value)
{
    if (value >= '0' && value <= '9') return (int)(value - '0');
    if (value >= 'a' && value <= 'f') return (int)(value - 'a' + 10U);
    if (value >= 'A' && value <= 'F') return (int)(value - 'A' + 10U);
    return -1;
}
/* Decode once, then pass one portable relative name to the rooted file owner.
 * A URL cannot inject separators by hiding them inside a percent escape. */
static UmiStatus StaticRelative(const char *path, char *relative, size_t capacity)
{
    if (path == NULL || path[0] != '/')
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_WEB_PATH_CAPACITY && path[length] != '\0')
        ++length;
    if (length == UMI_WEB_PATH_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t used = 0U;
    for (size_t i = 1U; i < length; ++i)
    {
        unsigned char value = (unsigned char)path[i];
        if (value == '%')
        {
            if (length - i < 3U)
                return UMI_STATUS_INVALID_ARGUMENT;
            int high = StaticHex((unsigned char)path[i + 1U]), low = StaticHex((unsigned char)path[i + 2U]);
            if (high < 0 || low < 0)
                return UMI_STATUS_INVALID_ARGUMENT;
            value = (unsigned char)((unsigned)high * 16U + (unsigned)low);
            i += 2U;
            if (value == '/' || value == '\\')
                return UMI_STATUS_INVALID_ARGUMENT;
        }
        if (value < 0x20U || value == 0x7fU || value == '\\' || value == '?' || value == '#')
            return UMI_STATUS_INVALID_ARGUMENT;
        if (used + 1U >= capacity)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        relative[used++] = (char)value;
    }
    if (used == 0U || relative[used - 1U] == '/')
    {
        const char index[] = "index.html";
        if (sizeof(index) > capacity - used)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(relative + used, index, sizeof(index));
        used += sizeof(index) - 1U;
    }
    relative[used] = '\0';
    return UMI_STATUS_OK;
}
/* Static reads now use retained parent handles and complete bounded files. This replaces unchecked root concatenation and successful partial reads while retaining MIME selection and HTTP error responses.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_web_static_files_serve(const UmiWebStaticFiles *files,const char *request_path,UmiWebResponse *response){char full[UMI_WEB_PATH_CAPACITY*2U];FILE *f;size_t n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(files==NULL||request_path==NULL||response==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(request_path,"..")!=NULL)return umi_web_response_set_text(response,UMI_HTTP_STATUS_FORBIDDEN,"text/plain","Forbidden");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(snprintf(full,sizeof(full),"%s/%s",files->root,request_path[0]=='/'?request_path+1:request_path)<0)return UMI_STATUS_INTERNAL_ERROR;f=fopen(full,"rb");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL)return umi_web_response_set_text(response,UMI_HTTP_STATUS_NOT_FOUND,"text/plain","Not Found");n=fread(response->body,1U,sizeof(response->body)-1U,f);(void)fclose(f);response->body[n]='\0';response->body_length=n;response->status=UMI_HTTP_STATUS_OK;response->header_count=0U;return umi_web_response_set_header(response,"Content-Type",umi_web_mime_from_path(full));}
#endif
UmiStatus umi_web_static_files_serve(const UmiWebStaticFiles *files, const char *request_path,
                                     UmiWebResponse *response)
{
    if (files == NULL || request_path == NULL || response == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(files->root, '\0', sizeof(files->root)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    char relative[UMI_WEB_PATH_CAPACITY];
    UmiStatus status = StaticRelative(request_path, relative, sizeof(relative));
    if (status == UMI_STATUS_OK)
        status = UmiRootedFileValidatePath(files->root, relative);
    if (status != UMI_STATUS_OK)
        return umi_web_response_set_text(response, 403, "text/plain; charset=utf-8", "Forbidden\n");
    unsigned char *bytes = NULL;
    size_t length = 0U;
    status = UmiRootedFileRead(files->root, relative, UMI_WEB_BODY_CAPACITY - 1U, &bytes, &length);
    if (status != UMI_STATUS_OK)
    {
        UmiRootedFileFree(bytes);
        if (status == UMI_STATUS_NOT_FOUND)
            return umi_web_response_set_text(response, 404, "text/plain; charset=utf-8", "Not Found\n");
        if (status == UMI_STATUS_CAPACITY_EXCEEDED)
            return umi_web_response_set_text(response, 413, "text/plain; charset=utf-8",
                                             "File exceeds the local preview limit.\n");
        if (status == UMI_STATUS_PERMISSION_DENIED || status == UMI_STATUS_INVALID_ARGUMENT)
            return umi_web_response_set_text(response, 403, "text/plain; charset=utf-8", "Forbidden\n");
        return status;
    }
    umi_web_response_init(response);
    memcpy(response->body, bytes, length);
    response->body[length] = '\0';
    response->body_length = length;
    UmiRootedFileFree(bytes);
    status = umi_web_response_set_header(response, "Content-Type", umi_web_mime_from_path(relative));
    if (status == UMI_STATUS_OK)
        status = umi_web_response_set_header(response, "X-Content-Type-Options", "nosniff");
    /* Development previews should reflect the current file after a reload. */
    if (status == UMI_STATUS_OK)
        status = umi_web_response_set_header(response, "Cache-Control", "no-store");
    return status;
}

UmiStatus UmiWebStaticFilesHandle(const UmiWebRequest *request, UmiWebResponse *response, void *context)
{
    if (request == NULL || response == NULL || context == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (request->method != UMI_HTTP_METHOD_GET && request->method != UMI_HTTP_METHOD_HEAD)
    {
        UmiStatus status =
            umi_web_response_set_text(response, 405, "text/plain; charset=utf-8", "Method Not Allowed\n");
        return status == UMI_STATUS_OK ? umi_web_response_set_header(response, "Allow", "GET, HEAD") : status;
    }
    return umi_web_static_files_serve(context, request->path, response);
}
