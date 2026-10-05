/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_frontend_signal.c
 * PURPOSE: Exercise frontend signal snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/signal.h"
#include "umicom/frontend/signal.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiFrontendSignalSnapshot
#define CONTRACT_REGISTRY UmiFrontendSignalRegistry
#define CONTRACT_CAPACITY UMI_FRONTEND_SIGNAL_CAPACITY
#define CONTRACT_VALIDATE umi_frontend_signal_snapshot_validate
#define CONTRACT_BATCH umi_frontend_signal_registry_upsert_many
#define CONTRACT_CREATE umi_frontend_signal_registry_create
#define CONTRACT_DESTROY umi_frontend_signal_registry_destroy
#define CONTRACT_UPSERT umi_frontend_signal_registry_upsert
#define CONTRACT_REMOVE umi_frontend_signal_registry_remove
#define CONTRACT_FIND umi_frontend_signal_registry_find
#define CONTRACT_AT umi_frontend_signal_registry_at
#define CONTRACT_COUNT umi_frontend_signal_registry_count
#define CONTRACT_REVISION umi_frontend_signal_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiFrontendSignalSnapshot, id), sizeof(((UmiFrontendSignalSnapshot *)0)->id), 1 },
    {"widget_id", offsetof(UmiFrontendSignalSnapshot, widget_id), sizeof(((UmiFrontendSignalSnapshot *)0)->widget_id), 0 },
    {"signal_name", offsetof(UmiFrontendSignalSnapshot, signal_name), sizeof(((UmiFrontendSignalSnapshot *)0)->signal_name), 0 },
    {"command_id", offsetof(UmiFrontendSignalSnapshot, command_id), sizeof(((UmiFrontendSignalSnapshot *)0)->command_id), 0 },
    {"argument", offsetof(UmiFrontendSignalSnapshot, argument), sizeof(((UmiFrontendSignalSnapshot *)0)->argument), 0 }
};
static int ContractSnapshotEqual(const UmiFrontendSignalSnapshot *left,
    const UmiFrontendSignalSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->widget_id, right->widget_id, sizeof(left->widget_id)) == 0 &&
        memcmp(left->signal_name, right->signal_name, sizeof(left->signal_name)) == 0 &&
        memcmp(left->command_id, right->command_id, sizeof(left->command_id)) == 0 &&
        memcmp(left->argument, right->argument, sizeof(left->argument)) == 0 &&
        left->enabled == right->enabled &&
        left->once == right->once &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiFrontendSignalSnapshot *item)
{
    item->widget_id[0] = 'v';
    item->signal_name[0] = 'v';
    item->command_id[0] = 'v';
    item->argument[0] = 'v';
    item->enabled = (int)8U;
    item->once = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_frontend_signal_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_frontend_signal_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiFrontendSignalEdit
#define CONTRACT_EDIT_CURRENT umi_frontend_signal_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_frontend_signal_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiFrontendSignalSnapshot ArchiveSample(void)
{
    UmiFrontendSignalSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiFrontendSignalSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->widget_id) + 1U;
        memset(value->widget_id + used, 0xa5, sizeof(value->widget_id) - used);
    }
    {
        size_t used = strlen(value->signal_name) + 1U;
        memset(value->signal_name + used, 0xa5, sizeof(value->signal_name) - used);
    }
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
    {
        size_t used = strlen(value->argument) + 1U;
        memset(value->argument + used, 0xa5, sizeof(value->argument) - used);
    }
}
#define ARCHIVE_TYPE UmiFrontendSignalSnapshot
#define ARCHIVE_ENCODE umi_frontend_signal_snapshot_archive_encode
#define ARCHIVE_DECODE umi_frontend_signal_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_frontend_signal_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_frontend_signal_registry_archive_restore
#include "snapshot_contract_cases.h"
