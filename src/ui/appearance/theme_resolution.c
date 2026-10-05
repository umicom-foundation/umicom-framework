/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_resolution.c
 *
 * PURPOSE:
 *   Record deterministic system/application/workspace/component theme resolution evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_resolution.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_theme_resolution_init(UmiAppearanceThemeResolution *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->requested_pack_id,sizeof item->requested_pack_id,"theme.default.dark");
    (void)umi_appearance_copy_text(item->resolved_pack_id,sizeof item->resolved_pack_id,"theme.default.dark");
    item->winning_scope = UMI_APPEARANCE_SCOPE_SYSTEM;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_theme_resolution_is_valid(const UmiAppearanceThemeResolution *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->requested_pack_id, '\0', sizeof(item->requested_pack_id)) == NULL) return 0;
    if (memchr(item->resolved_pack_id, '\0', sizeof(item->resolved_pack_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->resolved_pack_id) && item->winning_scope >= UMI_APPEARANCE_SCOPE_SYSTEM && item->winning_scope <= UMI_APPEARANCE_SCOPE_COMPONENT);
}
/*
 * Provide the appearance theme resolution choose operation used by this module and its
 * client applications.
 */
UmiStatus umi_appearance_theme_resolution_choose(UmiAppearanceThemeResolution *item,const char *system_id,const char *application_id,const char *workspace_id,const char *component_id){ const char *selected=system_id; UmiAppearanceScope scope=UMI_APPEARANCE_SCOPE_SYSTEM; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||!umi_appearance_id_valid(system_id)) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(application_id&&application_id[0]){selected=application_id;scope=UMI_APPEARANCE_SCOPE_APPLICATION;} /* Protect caller-owned memory by checking that required state is available before it is used. */ if(workspace_id&&workspace_id[0]){selected=workspace_id;scope=UMI_APPEARANCE_SCOPE_WORKSPACE;} /* Protect caller-owned memory by checking that required state is available before it is used. */ if(component_id&&component_id[0]){selected=component_id;scope=UMI_APPEARANCE_SCOPE_COMPONENT;} /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_copy_text(item->resolved_pack_id,sizeof item->resolved_pack_id,selected)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; item->winning_scope=scope; return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceThemeResolutionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xceab0b6a31182528);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceThemeResolution *)0)->requested_pack_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceThemeResolution *)0)->resolved_pack_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceThemeResolutionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceThemeResolution *)0)->requested_pack_id) - 1U +
        8U + sizeof(((UmiAppearanceThemeResolution *)0)->resolved_pack_id) - 1U +
        8U +
        8U;
}
static void UmiAppearanceThemeResolutionArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceThemeResolution *value)
{
    UmiArchiveWriteText(writer, value->requested_pack_id, sizeof(value->requested_pack_id));
    UmiArchiveWriteText(writer, value->resolved_pack_id, sizeof(value->resolved_pack_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->winning_scope);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->inherited_layers);
}
static void UmiAppearanceThemeResolutionArchiveRead(UmiArchiveReader *reader, UmiAppearanceThemeResolution *value)
{
    UmiArchiveReadText(reader, value->requested_pack_id, sizeof(value->requested_pack_id));
    UmiArchiveReadText(reader, value->resolved_pack_id, sizeof(value->resolved_pack_id));
    value->winning_scope = (UmiAppearanceScope)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->inherited_layers = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiAppearanceThemeResolutionArchiveValidate(const UmiAppearanceThemeResolution *value)
{
    return umi_appearance_theme_resolution_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_theme_resolution_archive_encode, umi_appearance_theme_resolution_archive_decode,
    UmiAppearanceThemeResolution, UmiAppearanceThemeResolutionArchiveSchema, UmiAppearanceThemeResolutionArchiveBound, UmiAppearanceThemeResolutionArchiveWrite, UmiAppearanceThemeResolutionArchiveRead, UmiAppearanceThemeResolutionArchiveValidate)
