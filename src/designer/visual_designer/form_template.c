/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/form_template.c
 *
 * PURPOSE:
 *   Describe reusable form templates and expected field/action counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/form_template.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer form template from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_form_template_init(UmiRadFormTemplate *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->template_id, sizeof item->template_id, "form_template");
    (void)umi_rad_copy_text(item->name, sizeof item->name, "form_template");
    item->field_count = 2U;
    item->action_count = 2U;
    return UMI_STATUS_OK;
}
/* Check that visual designer form template satisfies its contract before another service relies on it. */
int umi_rad_form_template_is_valid(const UmiRadFormTemplate *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->template_id, '\0', sizeof(item->template_id)) == NULL) return 0;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->template_id) && item->name[0] != '\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadFormTemplateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7b81107575eb605f);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadFormTemplate *)0)->template_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadFormTemplate *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadFormTemplateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadFormTemplate *)0)->template_id) - 1U +
        8U + sizeof(((UmiRadFormTemplate *)0)->name) - 1U +
        8U +
        8U;
}
static void UmiRadFormTemplateArchiveWrite(UmiArchiveWriter *writer, const UmiRadFormTemplate *value)
{
    UmiArchiveWriteText(writer, value->template_id, sizeof(value->template_id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->field_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->action_count);
}
static void UmiRadFormTemplateArchiveRead(UmiArchiveReader *reader, UmiRadFormTemplate *value)
{
    UmiArchiveReadText(reader, value->template_id, sizeof(value->template_id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->field_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->action_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
}
static UmiStatus UmiRadFormTemplateArchiveValidate(const UmiRadFormTemplate *value)
{
    return umi_rad_form_template_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_form_template_archive_encode, umi_rad_form_template_archive_decode,
    UmiRadFormTemplate, UmiRadFormTemplateArchiveSchema, UmiRadFormTemplateArchiveBound, UmiRadFormTemplateArchiveWrite, UmiRadFormTemplateArchiveRead, UmiRadFormTemplateArchiveValidate)
