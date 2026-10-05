/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_descriptor.c
 *
 * PURPOSE:
 *   Describe a semantic component using canonical Umicom component kinds plus design-system metadata.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_descriptor.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise design component descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_component_descriptor_init(UmiDesignComponentDescriptor *descriptor,const char *id,const char *display_name,UmiUiComponentKind kind,UmiDesignSemanticRole default_role,int interactive){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(descriptor==NULL||id==NULL||display_name==NULL||kind<UMI_UI_COMPONENT_WINDOW||kind>UMI_UI_COMPONENT_CUSTOM||default_role<UMI_DESIGN_ROLE_NEUTRAL||default_role>UMI_DESIGN_ROLE_ACCENT)return UMI_STATUS_INVALID_ARGUMENT;memset(descriptor,0,sizeof *descriptor);s=umi_design_copy_text(descriptor->id,sizeof descriptor->id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_design_copy_text(descriptor->display_name,sizeof descriptor->display_name,display_name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;descriptor->kind=kind;descriptor->default_role=default_role;descriptor->interactive=interactive?1:0;return UMI_STATUS_OK;}
/*
 * Check that design component descriptor satisfies its contract before another service
 * relies on it.
 */
int umi_design_component_descriptor_valid(const UmiDesignComponentDescriptor *descriptor){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (descriptor == NULL) return 0;
    if (memchr(descriptor->id, '\0', sizeof(descriptor->id)) == NULL) return 0;
    if (memchr(descriptor->display_name, '\0', sizeof(descriptor->display_name)) == NULL) return 0;
return descriptor!=NULL&&descriptor->id[0]!='\0'&&descriptor->display_name[0]!='\0'&&descriptor->kind>=UMI_UI_COMPONENT_WINDOW&&descriptor->kind<=UMI_UI_COMPONENT_CUSTOM?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignComponentDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb54c42bc429822f4);
    schema = (schema ^ (uint64_t)sizeof(((UmiDesignComponentDescriptor *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDesignComponentDescriptor *)0)->display_name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDesignComponentDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDesignComponentDescriptor *)0)->id) - 1U +
        8U + sizeof(((UmiDesignComponentDescriptor *)0)->display_name) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignComponentDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiDesignComponentDescriptor *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->display_name, sizeof(value->display_name));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->default_role);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->capability_flags);
    UmiArchiveWriteSigned(writer, (int64_t)value->interactive);
}
static void UmiDesignComponentDescriptorArchiveRead(UmiArchiveReader *reader, UmiDesignComponentDescriptor *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->display_name, sizeof(value->display_name));
    value->kind = (UmiUiComponentKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->default_role = (UmiDesignSemanticRole)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->capability_flags = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->interactive = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignComponentDescriptorArchiveValidate(const UmiDesignComponentDescriptor *value)
{
    return umi_design_component_descriptor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_component_descriptor_archive_encode, umi_design_component_descriptor_archive_decode,
    UmiDesignComponentDescriptor, UmiDesignComponentDescriptorArchiveSchema, UmiDesignComponentDescriptorArchiveBound, UmiDesignComponentDescriptorArchiveWrite, UmiDesignComponentDescriptorArchiveRead, UmiDesignComponentDescriptorArchiveValidate)
