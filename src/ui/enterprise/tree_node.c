/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_node.c
 *
 * PURPOSE:
 *   Describe a lazily materialised node in an enterprise tree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_node.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent tree node from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_tree_node_init(UmiUiEntTreeNode *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->node_id[0]='\0';value->parent_id[0]='\0';value->label[0]='\0';value->depth=0;value->has_children=0;value->children_loaded=0;value->revision=0;return UMI_STATUS_OK;}
/* Check that ui ent tree node satisfies its contract before another service relies on it. */
int umi_ui_ent_tree_node_validate(const UmiUiEntTreeNode *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->node_id, '\0', sizeof(value->node_id)) == NULL) return 0;
    if (memchr(value->parent_id, '\0', sizeof(value->parent_id)) == NULL) return 0;
    if (memchr(value->label, '\0', sizeof(value->label)) == NULL) return 0;
return value!=NULL&&umi_ui_ent_id_valid(value->node_id)&&value->depth>=0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntTreeNodeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8c411bf4a6461793);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntTreeNode *)0)->node_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntTreeNode *)0)->parent_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntTreeNode *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntTreeNodeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiEntTreeNode *)0)->node_id) - 1U +
        8U + sizeof(((UmiUiEntTreeNode *)0)->parent_id) - 1U +
        8U + sizeof(((UmiUiEntTreeNode *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiEntTreeNodeArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntTreeNode *value)
{
    UmiArchiveWriteText(writer, value->node_id, sizeof(value->node_id));
    UmiArchiveWriteText(writer, value->parent_id, sizeof(value->parent_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteSigned(writer, (int64_t)value->depth);
    UmiArchiveWriteSigned(writer, (int64_t)value->has_children);
    UmiArchiveWriteSigned(writer, (int64_t)value->children_loaded);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiUiEntTreeNodeArchiveRead(UmiArchiveReader *reader, UmiUiEntTreeNode *value)
{
    UmiArchiveReadText(reader, value->node_id, sizeof(value->node_id));
    UmiArchiveReadText(reader, value->parent_id, sizeof(value->parent_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->depth = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->has_children = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->children_loaded = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiEntTreeNodeArchiveValidate(const UmiUiEntTreeNode *value)
{
    return umi_ui_ent_tree_node_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_tree_node_archive_encode, umi_ui_ent_tree_node_archive_decode,
    UmiUiEntTreeNode, UmiUiEntTreeNodeArchiveSchema, UmiUiEntTreeNodeArchiveBound, UmiUiEntTreeNodeArchiveWrite, UmiUiEntTreeNodeArchiveRead, UmiUiEntTreeNodeArchiveValidate)
