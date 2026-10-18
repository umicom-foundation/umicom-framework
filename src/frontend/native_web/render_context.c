/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/render_context.c
 *
 * PURPOSE:
 *   Carry session, route, theme, density, locale and revision state through web rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../base/conditional_archive_internal.h"
#include "umicom/frontend/native_web/render_context.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise native web render context from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_native_web_render_context_init(UmiNativeWebRenderContext *context,const char *session_id,const char *route){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(context==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(context,0,sizeof(*context));s=umi_native_web_copy_text(context->session_id,sizeof(context->session_id),session_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(context->route,sizeof(context->route),route);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;(void)umi_native_web_copy_text(context->theme,sizeof(context->theme),"system");(void)umi_native_web_copy_text(context->density,sizeof(context->density),"compact");(void)umi_native_web_copy_text(context->locale,sizeof(context->locale),"en-GB");context->revision=1U;return UMI_STATUS_OK;}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_native_web_render_context_init_build_value(UmiNativeWebRenderContext *context,const char *session_id,const char *route){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(context==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(context,0,sizeof(*context));s=umi_native_web_copy_text(context->session_id,sizeof(context->session_id),session_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(context->route,sizeof(context->route),route);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;(void)umi_native_web_copy_text(context->theme,sizeof(context->theme),"system");(void)umi_native_web_copy_text(context->density,sizeof(context->density),"compact");(void)umi_native_web_copy_text(context->locale,sizeof(context->locale),"en-GB");context->revision=1U;return UMI_STATUS_OK;}

UMI_DEFINE_STAGED_VALUE_INIT(umi_native_web_render_context_init, UmiNativeWebRenderContext, context,
    (UmiNativeWebRenderContext *context, const char *session_id, const char *route),
    umi_native_web_render_context_init_build_value(candidate, session_id, route))

/*
 * Check that native web render context satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_native_web_render_context_validate(const UmiNativeWebRenderContext *context){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->session_id, '\0', sizeof(context->session_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->route, '\0', sizeof(context->route)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->theme, '\0', sizeof(context->theme)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->density, '\0', sizeof(context->density)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->locale, '\0', sizeof(context->locale)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(context==NULL||context->session_id[0]=='\0'||context->route[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}


/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiNativeWebRenderContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3226d847cd2f1324);
    schema = (schema ^ (uint64_t)sizeof(((UmiNativeWebRenderContext *)0)->session_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiNativeWebRenderContext *)0)->route)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiNativeWebRenderContext *)0)->theme)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiNativeWebRenderContext *)0)->density)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiNativeWebRenderContext *)0)->locale)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiNativeWebRenderContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiNativeWebRenderContext *)0)->session_id) - 1U +
        8U + sizeof(((UmiNativeWebRenderContext *)0)->route) - 1U +
        8U + sizeof(((UmiNativeWebRenderContext *)0)->theme) - 1U +
        8U + sizeof(((UmiNativeWebRenderContext *)0)->density) - 1U +
        8U + sizeof(((UmiNativeWebRenderContext *)0)->locale) - 1U +
        8U;
}
static void UmiNativeWebRenderContextArchiveWrite(UmiArchiveWriter *writer, const UmiNativeWebRenderContext *value)
{
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteText(writer, value->route, sizeof(value->route));
    UmiArchiveWriteText(writer, value->theme, sizeof(value->theme));
    UmiArchiveWriteText(writer, value->density, sizeof(value->density));
    UmiArchiveWriteText(writer, value->locale, sizeof(value->locale));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiNativeWebRenderContextArchiveRead(UmiArchiveReader *reader, UmiNativeWebRenderContext *value)
{
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    UmiArchiveReadText(reader, value->route, sizeof(value->route));
    UmiArchiveReadText(reader, value->theme, sizeof(value->theme));
    UmiArchiveReadText(reader, value->density, sizeof(value->density));
    UmiArchiveReadText(reader, value->locale, sizeof(value->locale));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiNativeWebRenderContextArchiveValidate(const UmiNativeWebRenderContext *value)
{
    return umi_native_web_render_context_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_native_web_render_context_archive_encode, umi_native_web_render_context_archive_decode,
    UmiNativeWebRenderContext, UmiNativeWebRenderContextArchiveSchema, UmiNativeWebRenderContextArchiveBound, UmiNativeWebRenderContextArchiveWrite, UmiNativeWebRenderContextArchiveRead, UmiNativeWebRenderContextArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_native_web_render_context_archive_decode_with_workspace, UmiNativeWebRenderContext,
    UmiNativeWebRenderContextArchiveSchema, UmiNativeWebRenderContextArchiveBound, UmiNativeWebRenderContextArchiveRead, UmiNativeWebRenderContextArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_native_web_render_context_archive_decode_if_unchanged, umi_native_web_render_context_archive_encode,
    UmiNativeWebRenderContext, UmiNativeWebRenderContextArchiveSchema, UmiNativeWebRenderContextArchiveBound, UmiNativeWebRenderContextArchiveRead, UmiNativeWebRenderContextArchiveValidate)
