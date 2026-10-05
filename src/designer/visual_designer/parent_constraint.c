/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/parent_constraint.c
 *
 * PURPOSE:
 *   Describe which semantic component families a parent slot accepts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/parent_constraint.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer parent constraint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_parent_constraint_init(UmiRadParentConstraint *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->parent_type, sizeof item->parent_type, "parent_constraint");
    (void)umi_rad_copy_text(item->child_family, sizeof item->child_family, "parent_constraint");
    item->accepted = true;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer parent constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_parent_constraint_is_valid(const UmiRadParentConstraint *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->parent_type, '\0', sizeof(item->parent_type)) == NULL) return 0;
    if (memchr(item->child_family, '\0', sizeof(item->child_family)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->parent_type) && umi_rad_id_valid(item->child_family);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadParentConstraintArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x21eeec086404093d);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadParentConstraint *)0)->parent_type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadParentConstraint *)0)->child_family)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadParentConstraintArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadParentConstraint *)0)->parent_type) - 1U +
        8U + sizeof(((UmiRadParentConstraint *)0)->child_family) - 1U +
        8U;
}
static void UmiRadParentConstraintArchiveWrite(UmiArchiveWriter *writer, const UmiRadParentConstraint *value)
{
    UmiArchiveWriteText(writer, value->parent_type, sizeof(value->parent_type));
    UmiArchiveWriteText(writer, value->child_family, sizeof(value->child_family));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->accepted);
}
static void UmiRadParentConstraintArchiveRead(UmiArchiveReader *reader, UmiRadParentConstraint *value)
{
    UmiArchiveReadText(reader, value->parent_type, sizeof(value->parent_type));
    UmiArchiveReadText(reader, value->child_family, sizeof(value->child_family));
    value->accepted = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadParentConstraintArchiveValidate(const UmiRadParentConstraint *value)
{
    return umi_rad_parent_constraint_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_parent_constraint_archive_encode, umi_rad_parent_constraint_archive_decode,
    UmiRadParentConstraint, UmiRadParentConstraintArchiveSchema, UmiRadParentConstraintArchiveBound, UmiRadParentConstraintArchiveWrite, UmiRadParentConstraintArchiveRead, UmiRadParentConstraintArchiveValidate)
