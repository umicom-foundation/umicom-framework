/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_launch_configuration.c
 * PURPOSE: Exercise debug launch_configuration snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/launch_configuration.h"
#include "umicom/debug/launch_configuration.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugLaunchConfigurationSnapshot
#define CONTRACT_REGISTRY UmiDebugLaunchConfigurationRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_LAUNCH_CONFIGURATION_CAPACITY
#define CONTRACT_VALIDATE umi_debug_launch_configuration_snapshot_validate
#define CONTRACT_BATCH umi_debug_launch_configuration_registry_upsert_many
#define CONTRACT_CREATE umi_debug_launch_configuration_registry_create
#define CONTRACT_DESTROY umi_debug_launch_configuration_registry_destroy
#define CONTRACT_UPSERT umi_debug_launch_configuration_registry_upsert
#define CONTRACT_REMOVE umi_debug_launch_configuration_registry_remove
#define CONTRACT_FIND umi_debug_launch_configuration_registry_find
#define CONTRACT_AT umi_debug_launch_configuration_registry_at
#define CONTRACT_COUNT umi_debug_launch_configuration_registry_count
#define CONTRACT_REVISION umi_debug_launch_configuration_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugLaunchConfigurationSnapshot, id), sizeof(((UmiDebugLaunchConfigurationSnapshot *)0)->id), 1 },
    {"name", offsetof(UmiDebugLaunchConfigurationSnapshot, name), sizeof(((UmiDebugLaunchConfigurationSnapshot *)0)->name), 0 },
    {"adapter", offsetof(UmiDebugLaunchConfigurationSnapshot, adapter), sizeof(((UmiDebugLaunchConfigurationSnapshot *)0)->adapter), 0 },
    {"program", offsetof(UmiDebugLaunchConfigurationSnapshot, program), sizeof(((UmiDebugLaunchConfigurationSnapshot *)0)->program), 0 },
    {"arguments", offsetof(UmiDebugLaunchConfigurationSnapshot, arguments), sizeof(((UmiDebugLaunchConfigurationSnapshot *)0)->arguments), 0 },
    {"working_directory", offsetof(UmiDebugLaunchConfigurationSnapshot, working_directory), sizeof(((UmiDebugLaunchConfigurationSnapshot *)0)->working_directory), 0 },
    {"environment", offsetof(UmiDebugLaunchConfigurationSnapshot, environment), sizeof(((UmiDebugLaunchConfigurationSnapshot *)0)->environment), 0 }
};
static int ContractSnapshotEqual(const UmiDebugLaunchConfigurationSnapshot *left,
    const UmiDebugLaunchConfigurationSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->adapter, right->adapter, sizeof(left->adapter)) == 0 &&
        memcmp(left->program, right->program, sizeof(left->program)) == 0 &&
        memcmp(left->arguments, right->arguments, sizeof(left->arguments)) == 0 &&
        memcmp(left->working_directory, right->working_directory, sizeof(left->working_directory)) == 0 &&
        memcmp(left->environment, right->environment, sizeof(left->environment)) == 0 &&
        left->stop_on_entry == right->stop_on_entry &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugLaunchConfigurationSnapshot *item)
{
    item->name[0] = 'v';
    item->adapter[0] = 'v';
    item->program[0] = 'v';
    item->arguments[0] = 'v';
    item->working_directory[0] = 'v';
    item->environment[0] = 'v';
    item->stop_on_entry = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_launch_configuration_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_launch_configuration_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiDebugLaunchConfigurationEdit
#define CONTRACT_EDIT_CURRENT umi_debug_launch_configuration_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_debug_launch_configuration_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiDebugLaunchConfigurationSnapshot ArchiveSample(void)
{
    UmiDebugLaunchConfigurationSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiDebugLaunchConfigurationSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->adapter) + 1U;
        memset(value->adapter + used, 0xa5, sizeof(value->adapter) - used);
    }
    {
        size_t used = strlen(value->program) + 1U;
        memset(value->program + used, 0xa5, sizeof(value->program) - used);
    }
    {
        size_t used = strlen(value->arguments) + 1U;
        memset(value->arguments + used, 0xa5, sizeof(value->arguments) - used);
    }
    {
        size_t used = strlen(value->working_directory) + 1U;
        memset(value->working_directory + used, 0xa5, sizeof(value->working_directory) - used);
    }
    {
        size_t used = strlen(value->environment) + 1U;
        memset(value->environment + used, 0xa5, sizeof(value->environment) - used);
    }
}
#define ARCHIVE_TYPE UmiDebugLaunchConfigurationSnapshot
#define ARCHIVE_ENCODE umi_debug_launch_configuration_snapshot_archive_encode
#define ARCHIVE_DECODE umi_debug_launch_configuration_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_debug_launch_configuration_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_debug_launch_configuration_registry_archive_restore
#include "snapshot_contract_cases.h"
