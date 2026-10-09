/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/source.c
 *
 * PURPOSE:
 *   Implement the DAP source request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/source.h"

#include <string.h>
#include <limits.h>
#include "umicom/language_runtime/json_writer.h"

/*
 * Provide the debug runtime request source operation used by this module and its client
 * applications.
 */
/* Source references now use the protocol int32 bound and appear in both the Source descriptor and the legacy field. This supports generated source without an empty ambiguous descriptor. The previous serializer is retained for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus umi_debug_runtime_request_source(
    UmiDebugRuntimeAdapter *adapter,
    const char *source_path,
    uint64_t source_reference,
    uint64_t *out_sequence)
{
    char arguments[4096];
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL ||
        ((source_path == NULL || source_path[0] == '\0') &&
         source_reference == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"source\":{");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source_path != NULL && source_path[0] != '\0') {
        (void)umi_language_runtime_json_writer_raw(&writer, "\"path\":");
        (void)umi_language_runtime_json_writer_string(&writer, source_path);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "},\"sourceReference\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, source_reference);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "source", arguments,
        source_path != NULL ? source_path : "", out_sequence);
}
#endif
UmiStatus umi_debug_runtime_request_source(
    UmiDebugRuntimeAdapter *adapter,const char *source_path,uint64_t source_reference,uint64_t *out_sequence)
{
    if(adapter==NULL||source_reference>INT32_MAX||
        ((source_path==NULL||source_path[0]=='\0')&&source_reference==0U))return UMI_STATUS_INVALID_ARGUMENT;
    char arguments[4096];UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer,arguments,sizeof arguments);
    (void)umi_language_runtime_json_writer_raw(&writer,"{\"source\":{");
    if(source_path!=NULL&&source_path[0]!='\0'){
        (void)umi_language_runtime_json_writer_raw(&writer,"\"path\":");
        (void)umi_language_runtime_json_writer_string(&writer,source_path);
        (void)umi_language_runtime_json_writer_raw(&writer,",");
    }
    /* Both protocol locations carry the same reference. Older adapters read the
     * top-level field; modern adapters may resolve the nested Source descriptor. */
    (void)umi_language_runtime_json_writer_raw(&writer,"\"sourceReference\":");
    (void)umi_language_runtime_json_writer_uint64(&writer,source_reference);
    (void)umi_language_runtime_json_writer_raw(&writer,"},\"sourceReference\":");
    (void)umi_language_runtime_json_writer_uint64(&writer,source_reference);
    (void)umi_language_runtime_json_writer_raw(&writer,"}");
    if(writer.status!=UMI_STATUS_OK)return writer.status;
    return umi_debug_runtime_request_raw(adapter,"source",arguments,source_path!=NULL?source_path:"",out_sequence);
}
