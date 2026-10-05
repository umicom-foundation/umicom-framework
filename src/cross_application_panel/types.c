/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/types.c
 *
 * PURPOSE:
 *   Implement panel lifecycle text and identity validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/types.h"
#include "../base/value_archive_internal.h"
/*
 * Provide the panel lifecycle state text operation used by this module and its client
 * applications.
 */
const char *umi_panel_lifecycle_state_text(UmiPanelLifecycleState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch(state){
case UMI_PANEL_CREATED:return "created";
case UMI_PANEL_REGISTERED:return "registered";
case UMI_PANEL_MOUNTED:return "mounted";
case UMI_PANEL_VISIBLE:return "visible";
case UMI_PANEL_HIDDEN:return "hidden";
case UMI_PANEL_SUSPENDED:return "suspended";
case UMI_PANEL_DESTROYED:return "destroyed";
case UMI_PANEL_FAILED:return "failed";
default:return "unknown";
}
}
/*
 * Provide the panel placement text operation used by this module and its client
 * applications.
 */
const char *umi_panel_placement_text(UmiPanelPlacement placement)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch(placement){
case UMI_PANEL_PLACE_LEFT:return "left";
case UMI_PANEL_PLACE_RIGHT:return "right";
case UMI_PANEL_PLACE_TOP:return "top";
case UMI_PANEL_PLACE_BOTTOM:return "bottom";
case UMI_PANEL_PLACE_DOCUMENT:return "document";
case UMI_PANEL_PLACE_FLOATING:return "floating";
default:return "unknown";
}
}
/* Check that panel identity satisfies its contract before another service relies on it. */
bool umi_panel_identity_valid(const UmiPanelIdentity *identity)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (identity == NULL) return 0;
    if (memchr(identity->panel_id, '\0', sizeof(identity->panel_id)) == NULL) return 0;
    if (memchr(identity->application_id, '\0', sizeof(identity->application_id)) == NULL) return 0;
    if (memchr(identity->instance_id, '\0', sizeof(identity->instance_id)) == NULL) return 0;
    if (memchr(identity->component_id, '\0', sizeof(identity->component_id)) == NULL) return 0;

    return identity!=NULL&&identity->panel_id[0]!='\0'&&identity->application_id[0]!='\0'&&umi_context_text_is_valid(identity->panel_id,sizeof(identity->panel_id))&&umi_context_text_is_valid(identity->application_id,sizeof(identity->application_id))&&umi_context_text_is_valid(identity->instance_id,sizeof(identity->instance_id))&&umi_context_text_is_valid(identity->component_id,sizeof(identity->component_id));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPanelIdentityArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6e49e76fcb738336);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelIdentity *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelIdentity *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelIdentity *)0)->instance_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelIdentity *)0)->component_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPanelIdentityArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPanelIdentity *)0)->panel_id) - 1U +
        8U + sizeof(((UmiPanelIdentity *)0)->application_id) - 1U +
        8U + sizeof(((UmiPanelIdentity *)0)->instance_id) - 1U +
        8U + sizeof(((UmiPanelIdentity *)0)->component_id) - 1U;
}
static void UmiPanelIdentityArchiveWrite(UmiArchiveWriter *writer, const UmiPanelIdentity *value)
{
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->instance_id, sizeof(value->instance_id));
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
}
static void UmiPanelIdentityArchiveRead(UmiArchiveReader *reader, UmiPanelIdentity *value)
{
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->instance_id, sizeof(value->instance_id));
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
}
static UmiStatus UmiPanelIdentityArchiveValidate(const UmiPanelIdentity *value)
{
    return umi_panel_identity_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_panel_identity_archive_encode, umi_panel_identity_archive_decode,
    UmiPanelIdentity, UmiPanelIdentityArchiveSchema, UmiPanelIdentityArchiveBound, UmiPanelIdentityArchiveWrite, UmiPanelIdentityArchiveRead, UmiPanelIdentityArchiveValidate)
