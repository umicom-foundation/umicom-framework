/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/patch_serializer.c
 *
 * PURPOSE:
 *   Serialize patch batches into the compact native-web JSON patch protocol.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/patch_serializer.h"

#include <stdio.h>
/* Direct output construction could expose partial JSON and read beyond
 * malformed inline strings or an oversized public batch count. The bounded,
 * staged encoder below replaces it; retain the original wire layout for review. */
#if 0
/* Provide the json string operation used by this module and its client applications. */
static UmiStatus json_string(UmiNativeWebMarkupBuffer *b,const char *s){const char *p;UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)s="";st=umi_native_web_markup_buffer_append(b,"\"");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;/* Visit each bounded item once so every record receives the same rule. */ for(p=s;*p!='\0';++p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(*p=='\"'||*p=='\\'){st=umi_native_web_markup_buffer_append_n(b,"\\",1U);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if((unsigned char)*p<0x20U)return UMI_STATUS_PARSE_ERROR;st=umi_native_web_markup_buffer_append_n(b,p,1U);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;}return umi_native_web_markup_buffer_append(b,"\"");}
/*
 * Write native web patch in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_native_web_patch_serialize(const UmiNativeWebPatchBatch *batch,UmiNativeWebMarkupBuffer *out_json){size_t i;char n[64];UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(batch==NULL||out_json==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_native_web_markup_buffer_init(out_json);s=umi_native_web_markup_buffer_append(out_json,"[");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<batch->count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>0U){s=umi_native_web_markup_buffer_append(out_json,",");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}s=umi_native_web_markup_buffer_append(out_json,"{\"k\":");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;(void)snprintf(n,sizeof(n),"%d",(int)batch->items[i].kind);s=umi_native_web_markup_buffer_append(out_json,n);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_json,",\"t\":");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=json_string(out_json,batch->items[i].target_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(batch->items[i].name[0]!='\0'){s=umi_native_web_markup_buffer_append(out_json,",\"n\":");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=json_string(out_json,batch->items[i].name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(batch->items[i].value[0]!='\0'){s=umi_native_web_markup_buffer_append(out_json,",\"v\":");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=json_string(out_json,batch->items[i].value);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}s=umi_native_web_markup_buffer_append(out_json,"}");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}return umi_native_web_markup_buffer_append(out_json,"]");}


#endif

#include <string.h>

/* Keep browser transport validation independent of editor/document services.
 * Reject overlong UTF-8, surrogate encodings and incomplete code points before
 * copying bytes into JSON. Control characters are represented as JSON escapes. */
static bool JsonUtf8(const unsigned char *text, size_t size)
{
    size_t at = 0U;
    while (at < size) {
        uint32_t code, minimum;
        unsigned extra;
        unsigned char first = text[at++];
        if (first < 0x80U) continue;
        if (first >= 0xc2U && first <= 0xdfU) { code = first & 0x1fU; minimum = 0x80U; extra = 1U; }
        else if (first >= 0xe0U && first <= 0xefU) { code = first & 0x0fU; minimum = 0x800U; extra = 2U; }
        else if (first >= 0xf0U && first <= 0xf4U) { code = first & 7U; minimum = 0x10000U; extra = 3U; }
        else return false;
        if ((size_t)extra > size - at) return false;
        for (unsigned i = 0U; i < extra; ++i) {
            unsigned char next = text[at++];
            if ((next & 0xc0U) != 0x80U) return false;
            code = (code << 6U) | (next & 0x3fU);
        }
        if (code < minimum || code > 0x10ffffU || (code >= 0xd800U && code <= 0xdfffU)) return false;
    }
    return true;
}

static UmiStatus JsonString(UmiNativeWebMarkupBuffer *buffer, const char *text, size_t capacity)
{
    static const char hex[] = "0123456789abcdef";
    const char *end = memchr(text, '\0', capacity);
    UmiStatus status;
    if (end == NULL || !JsonUtf8((const unsigned char *)text, (size_t)(end - text)))
        return UMI_STATUS_PARSE_ERROR;
    status = umi_native_web_markup_buffer_append(buffer, "\"");
    for (const char *p = text; p < end && status == UMI_STATUS_OK; ++p) {
        unsigned char byte = (unsigned char)*p;
        if (byte < 0x20U) {
            char escaped[6] = {'\\','u','0','0',hex[byte >> 4U],hex[byte & 15U]};
            status = umi_native_web_markup_buffer_append_n(buffer, escaped, sizeof escaped);
        } else {
            if (byte == '\"' || byte == '\\')
                status = umi_native_web_markup_buffer_append_n(buffer, "\\", 1U);
            if (status == UMI_STATUS_OK) status = umi_native_web_markup_buffer_append_n(buffer, p, 1U);
        }
    }
    if (status == UMI_STATUS_OK) status = umi_native_web_markup_buffer_append(buffer, "\"");
    return status;
}

UmiStatus umi_native_web_patch_serialize(const UmiNativeWebPatchBatch *batch,
    UmiNativeWebMarkupBuffer *out_json)
{
    UmiNativeWebMarkupBuffer candidate = {0};
    UmiStatus status;
    if (batch == NULL || out_json == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (batch->count > UMI_NATIVE_WEB_MAX_PATCHES) return UMI_STATUS_INVALID_ARGUMENT;
    umi_native_web_markup_buffer_init(&candidate);
    status = umi_native_web_markup_buffer_append(&candidate, "[");
    for (size_t i = 0U; i < batch->count && status == UMI_STATUS_OK; ++i) {
        const UmiNativeWebPatch *patch = &batch->items[i];
        char number[32];
        int written;
        if (patch->kind < UMI_NATIVE_WEB_PATCH_SET_TEXT || patch->kind > UMI_NATIVE_WEB_PATCH_FOCUS ||
            patch->target_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
        if (i != 0U) status = umi_native_web_markup_buffer_append(&candidate, ",");
        if (status == UMI_STATUS_OK) status = umi_native_web_markup_buffer_append(&candidate, "{\"k\":");
        written = snprintf(number, sizeof number, "%d", (int)patch->kind);
        if (written < 0 || (size_t)written >= sizeof number) return UMI_STATUS_INTERNAL_ERROR;
        if (status == UMI_STATUS_OK) status = umi_native_web_markup_buffer_append(&candidate, number);
        if (status == UMI_STATUS_OK) status = umi_native_web_markup_buffer_append(&candidate, ",\"t\":");
        if (status == UMI_STATUS_OK) status = JsonString(&candidate, patch->target_id, sizeof patch->target_id);
        if (status == UMI_STATUS_OK && patch->name[0] != '\0') {
            status = umi_native_web_markup_buffer_append(&candidate, ",\"n\":");
            if (status == UMI_STATUS_OK) status = JsonString(&candidate, patch->name, sizeof patch->name);
        }
        if (status == UMI_STATUS_OK && patch->value[0] != '\0') {
            status = umi_native_web_markup_buffer_append(&candidate, ",\"v\":");
            if (status == UMI_STATUS_OK) status = JsonString(&candidate, patch->value, sizeof patch->value);
        }
        if (status == UMI_STATUS_OK) status = umi_native_web_markup_buffer_append(&candidate, "}");
    }
    if (status == UMI_STATUS_OK) status = umi_native_web_markup_buffer_append(&candidate, "]");
    if (status == UMI_STATUS_OK) *out_json = candidate;
    return status;
}

#include "patch_transport_internal.h"

/* Transport overhead can overflow even when the JSON itself fits. Keep both
 * stages private until the complete frame is accepted, preserving wire prefixes. */
UmiStatus UmiNativeWebFramePatch(const UmiNativeWebPatchBatch *batch,
    const char *prefix, const char *suffix, UmiNativeWebMarkupBuffer *out)
{
    UmiNativeWebMarkupBuffer json = {0}, candidate = {0};
    UmiStatus status;
    if (batch == NULL || prefix == NULL || suffix == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_native_web_patch_serialize(batch, &json);
    if (status != UMI_STATUS_OK) return status;
    umi_native_web_markup_buffer_init(&candidate);
    status = umi_native_web_markup_buffer_append(&candidate, prefix);
    if (status == UMI_STATUS_OK) status = umi_native_web_markup_buffer_append_n(&candidate, json.data, json.length);
    if (status == UMI_STATUS_OK) status = umi_native_web_markup_buffer_append(&candidate, suffix);
    if (status == UMI_STATUS_OK) *out = candidate;
    return status;
}
