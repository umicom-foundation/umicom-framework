/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_service.c
 *
 * PURPOSE:
 *   Expose aggregate readiness for Framework-owned production appearance services consumed by every thin application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_service.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_service_init(UmiAppearanceAppearanceService *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->service_id,sizeof item->service_id,"ui.appearance");
    item->themes_ready=true;
    item->typography_ready=true;
    item->scaling_ready=true;
    item->accessibility_ready=true;
    item->renderers_ready=true;
    item->revision=1U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_service_is_valid(const UmiAppearanceAppearanceService *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->service_id, '\0', sizeof(item->service_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->service_id) && item->revision > 0U);
}
/*
 * Provide the appearance service ready operation used by this module and its client
 * applications.
 */
int umi_appearance_service_ready(const UmiAppearanceAppearanceService *item){return item!=NULL&&item->themes_ready&&item->typography_ready&&item->scaling_ready&&item->accessibility_ready&&item->renderers_ready;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceAppearanceServiceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x92b133561e4f1160);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceAppearanceService *)0)->service_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceAppearanceServiceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceAppearanceService *)0)->service_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceAppearanceServiceArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceAppearanceService *value)
{
    UmiArchiveWriteText(writer, value->service_id, sizeof(value->service_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->themes_ready);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->typography_ready);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->scaling_ready);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->accessibility_ready);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->renderers_ready);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiAppearanceAppearanceServiceArchiveRead(UmiArchiveReader *reader, UmiAppearanceAppearanceService *value)
{
    UmiArchiveReadText(reader, value->service_id, sizeof(value->service_id));
    value->themes_ready = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->typography_ready = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->scaling_ready = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->accessibility_ready = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->renderers_ready = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiAppearanceAppearanceServiceArchiveValidate(const UmiAppearanceAppearanceService *value)
{
    return umi_appearance_service_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_service_archive_encode, umi_appearance_service_archive_decode,
    UmiAppearanceAppearanceService, UmiAppearanceAppearanceServiceArchiveSchema, UmiAppearanceAppearanceServiceArchiveBound, UmiAppearanceAppearanceServiceArchiveWrite, UmiAppearanceAppearanceServiceArchiveRead, UmiAppearanceAppearanceServiceArchiveValidate)
