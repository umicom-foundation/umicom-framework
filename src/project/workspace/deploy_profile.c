/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/deploy_profile.c
 *
 * PURPOSE:
 *   Implement the deploy profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Deploy Profile | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/deploy_profile.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise project workspace deploy profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_deploy_profile_init(UmiProjectWorkspaceDeployProfile *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace deploy profile satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_deploy_profile_validate(const UmiProjectWorkspaceDeployProfile *value) {
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
 * Provide the project workspace deploy profile set name operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_deploy_profile_set_name(UmiProjectWorkspaceDeployProfile *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace deploy profile set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_deploy_profile_set_detail(UmiProjectWorkspaceDeployProfile *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace deploy profile set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_deploy_profile_set_state(UmiProjectWorkspaceDeployProfile *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace deploy profile set metric operation used by this module
 * and its client applications.
 */
void umi_project_workspace_deploy_profile_set_metric(UmiProjectWorkspaceDeployProfile *value,uint64_t metric) {
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
 * Provide the project workspace deploy profile same identity operation used by this module
 * and its client applications.
 */
bool umi_project_workspace_deploy_profile_same_identity(const UmiProjectWorkspaceDeployProfile *left,const UmiProjectWorkspaceDeployProfile *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiProjectWorkspaceDeployProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1ef5ec5fb344a797);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceDeployProfile *)0)->base.id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceDeployProfile *)0)->base.name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceDeployProfile *)0)->base.detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiProjectWorkspaceDeployProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiProjectWorkspaceDeployProfile *)0)->base.id) - 1U +
        8U + sizeof(((UmiProjectWorkspaceDeployProfile *)0)->base.name) - 1U +
        8U + sizeof(((UmiProjectWorkspaceDeployProfile *)0)->base.detail) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiProjectWorkspaceDeployProfileArchiveWrite(UmiArchiveWriter *writer, const UmiProjectWorkspaceDeployProfile *value)
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
static void UmiProjectWorkspaceDeployProfileArchiveRead(UmiArchiveReader *reader, UmiProjectWorkspaceDeployProfile *value)
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
static UmiStatus UmiProjectWorkspaceDeployProfileArchiveValidate(const UmiProjectWorkspaceDeployProfile *value)
{
    return umi_project_workspace_deploy_profile_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_project_workspace_deploy_profile_archive_encode, umi_project_workspace_deploy_profile_archive_decode,
    UmiProjectWorkspaceDeployProfile, UmiProjectWorkspaceDeployProfileArchiveSchema, UmiProjectWorkspaceDeployProfileArchiveBound, UmiProjectWorkspaceDeployProfileArchiveWrite, UmiProjectWorkspaceDeployProfileArchiveRead, UmiProjectWorkspaceDeployProfileArchiveValidate)
