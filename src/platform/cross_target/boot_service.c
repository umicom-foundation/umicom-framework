/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/boot_service.c
 *
 * PURPOSE:
 *   Bind OS services to boot phases and validate dependency phase ordering.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/boot_service.h"
#include "../../base/value_archive_internal.h"

/* Check that ct boot service satisfies its contract before another service relies on it. */
UmiStatus umi_ct_boot_service_validate(const UmiCtBootService*s){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (s == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(s->service_id, '\0', sizeof(s->service_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_ct_id_valid(s->service_id)||(unsigned)s->phase>7U||s->timeout_ms==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/*
 * Check that ct boot dependency phase satisfies its contract before another service relies
 * on it.
 */
bool umi_ct_boot_dependency_phase_valid(const UmiCtBootService*s,const UmiCtBootService*d){return s!=NULL&&d!=NULL&&(unsigned)d->phase<=(unsigned)s->phase;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtBootServiceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc9b44f35394d7522);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtBootService *)0)->service_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCtBootServiceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCtBootService *)0)->service_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiCtBootServiceArchiveWrite(UmiArchiveWriter *writer, const UmiCtBootService *value)
{
    UmiArchiveWriteText(writer, value->service_id, sizeof(value->service_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->phase);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->essential);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timeout_ms);
}
static void UmiCtBootServiceArchiveRead(UmiArchiveReader *reader, UmiCtBootService *value)
{
    UmiArchiveReadText(reader, value->service_id, sizeof(value->service_id));
    value->phase = (UmiCtBootPhase)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->essential = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->timeout_ms = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiCtBootServiceArchiveValidate(const UmiCtBootService *value)
{
    return umi_ct_boot_service_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_boot_service_archive_encode, umi_ct_boot_service_archive_decode,
    UmiCtBootService, UmiCtBootServiceArchiveSchema, UmiCtBootServiceArchiveBound, UmiCtBootServiceArchiveWrite, UmiCtBootServiceArchiveRead, UmiCtBootServiceArchiveValidate)
