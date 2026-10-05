/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/test_profile.c
 *
 * PURPOSE:
 *   Implement the test profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Test Profile | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/test_profile.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise project workspace test profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_test_profile_init(UmiProjectWorkspaceTestProfile *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace test profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_project_workspace_test_profile_validate(const UmiProjectWorkspaceTestProfile *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->base.id, '\0', sizeof(value->base.id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->base.name, '\0', sizeof(value->base.name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->base.detail, '\0', sizeof(value->base.detail)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace test profile set name operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_test_profile_set_name(UmiProjectWorkspaceTestProfile *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace test profile set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_test_profile_set_detail(UmiProjectWorkspaceTestProfile *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace test profile set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_test_profile_set_state(UmiProjectWorkspaceTestProfile *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace test profile set metric operation used by this module and
 * its client applications.
 */
void umi_project_workspace_test_profile_set_metric(UmiProjectWorkspaceTestProfile *value,uint64_t metric) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
/*
 * Provide the project workspace test profile same identity operation used by this module
 * and its client applications.
 */
bool umi_project_workspace_test_profile_same_identity(const UmiProjectWorkspaceTestProfile *left,const UmiProjectWorkspaceTestProfile *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiProjectWorkspaceTestProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xde09425fc911c826);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceTestProfile *)0)->base.id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceTestProfile *)0)->base.name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceTestProfile *)0)->base.detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiProjectWorkspaceTestProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiProjectWorkspaceTestProfile *)0)->base.id) - 1U +
        8U + sizeof(((UmiProjectWorkspaceTestProfile *)0)->base.name) - 1U +
        8U + sizeof(((UmiProjectWorkspaceTestProfile *)0)->base.detail) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiProjectWorkspaceTestProfileArchiveWrite(UmiArchiveWriter *writer, const UmiProjectWorkspaceTestProfile *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->base.api_version);
    UmiArchiveWriteText(writer, value->base.id, sizeof(value->base.id));
    UmiArchiveWriteText(writer, value->base.name, sizeof(value->base.name));
    UmiArchiveWriteText(writer, value->base.detail, sizeof(value->base.detail));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->base.revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->base.flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->base.priority);
    UmiArchiveWriteSigned(writer, (int64_t)value->base.state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->base.enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->metric);
}
static void UmiProjectWorkspaceTestProfileArchiveRead(UmiArchiveReader *reader, UmiProjectWorkspaceTestProfile *value)
{
    value->base.structure_size = (uint32_t)sizeof(value->base);
    value->base.api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->base.id, sizeof(value->base.id));
    UmiArchiveReadText(reader, value->base.name, sizeof(value->base.name));
    UmiArchiveReadText(reader, value->base.detail, sizeof(value->base.detail));
    value->base.revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->base.flags = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->base.priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->base.state = (UmiProjectWorkspaceState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->base.enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->metric = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiProjectWorkspaceTestProfileArchiveValidate(const UmiProjectWorkspaceTestProfile *value)
{
    return umi_project_workspace_test_profile_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_project_workspace_test_profile_archive_encode, umi_project_workspace_test_profile_archive_decode,
    UmiProjectWorkspaceTestProfile, UmiProjectWorkspaceTestProfileArchiveSchema, UmiProjectWorkspaceTestProfileArchiveBound, UmiProjectWorkspaceTestProfileArchiveWrite, UmiProjectWorkspaceTestProfileArchiveRead, UmiProjectWorkspaceTestProfileArchiveValidate)
