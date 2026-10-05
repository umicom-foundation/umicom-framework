/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/renderer_profile.c
 *
 * PURPOSE:
 *   renderer identity, capability and policy metadata used by conformance evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/renderer_profile.h"
#include "../../base/value_archive_internal.h"

/*
 * Provide the fc renderer profile make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_renderer_profile_make(const char *id,UmiFcFrontendKind kind,uint64_t capabilities,uint32_t api_version,UmiFcRendererProfile *out_profile){ UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_profile==NULL)return UMI_STATUS_INVALID_ARGUMENT; *out_profile=(UmiFcRendererProfile){0}; st=umi_fc_copy_text(out_profile->id,sizeof(out_profile->id),id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; out_profile->kind=kind; out_profile->capabilities=capabilities; out_profile->api_version=api_version; return umi_fc_renderer_profile_validate(out_profile); }
/*
 * Check that fc renderer profile satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fc_renderer_profile_validate(const UmiFcRendererProfile *profile){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(profile->id, '\0', sizeof(profile->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
 /* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile==NULL||profile->id[0]=='\0'||profile->api_version==0U||profile->kind<UMI_FC_FRONTEND_GTK4||profile->kind>UMI_FC_FRONTEND_HEADLESS)return UMI_STATUS_INVALID_ARGUMENT; return UMI_STATUS_OK; }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFcRendererProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5b51e3cabce43721);
    schema = (schema ^ (uint64_t)sizeof(((UmiFcRendererProfile *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFcRendererProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFcRendererProfile *)0)->id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiFcRendererProfileArchiveWrite(UmiArchiveWriter *writer, const UmiFcRendererProfile *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->capabilities);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->production_ready);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->remote_session);
}
static void UmiFcRendererProfileArchiveRead(UmiArchiveReader *reader, UmiFcRendererProfile *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->kind = (UmiFcFrontendKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->capabilities = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->production_ready = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->remote_session = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFcRendererProfileArchiveValidate(const UmiFcRendererProfile *value)
{
    return umi_fc_renderer_profile_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fc_renderer_profile_archive_encode, umi_fc_renderer_profile_archive_decode,
    UmiFcRendererProfile, UmiFcRendererProfileArchiveSchema, UmiFcRendererProfileArchiveBound, UmiFcRendererProfileArchiveWrite, UmiFcRendererProfileArchiveRead, UmiFcRendererProfileArchiveValidate)
