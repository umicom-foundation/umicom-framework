/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/controller.h
 *
 * PURPOSE:
 *   Define the Layout Persistence Slave Controller that accepts typed commands from the Master Controller and delegates bounded work to the Framework service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_CONTROLLER_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_CONTROLLER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/workbench_layout_data/service.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named workbench layout data command kind values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataCommandKind {
    UMI_WORKBENCH_LAYOUT_DATA_COMMAND_START = 1,
    UMI_WORKBENCH_LAYOUT_DATA_COMMAND_QUIESCE = 2,
    UMI_WORKBENCH_LAYOUT_DATA_COMMAND_STOP = 3,
    UMI_WORKBENCH_LAYOUT_DATA_COMMAND_SAVE_LAYOUT = 4,
    UMI_WORKBENCH_LAYOUT_DATA_COMMAND_DELETE_LAYOUT = 5,
    UMI_WORKBENCH_LAYOUT_DATA_COMMAND_SHARE_LAYOUT = 6,
    UMI_WORKBENCH_LAYOUT_DATA_COMMAND_ACQUIRE_LEASE = 7,
    UMI_WORKBENCH_LAYOUT_DATA_COMMAND_CHECKPOINT_SESSION = 8,
    UMI_WORKBENCH_LAYOUT_DATA_COMMAND_BACKUP = 9,
    UMI_WORKBENCH_LAYOUT_DATA_COMMAND_RESTORE = 10,
    UMI_WORKBENCH_LAYOUT_DATA_COMMAND_HEALTH = 11
} UmiWorkbenchLayoutDataCommandKind;

/**
 * Represent the workbench layout data command data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutDataCommand {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataCommandKind kind;
    char command_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char actor_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char correlation_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char target_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint64_t expected_revision;
    uint64_t timestamp_ms;
    const void *payload;
    size_t payload_size;
} UmiWorkbenchLayoutDataCommand;

/**
 * Represent the workbench layout data command result data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutDataCommandResult {
    uint32_t structure_size;
    UmiStatus status;
    uint64_t resulting_revision;
    uint64_t fencing_token;
    UmiWorkbenchLayoutDataServiceSnapshot snapshot;
    char message[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutDataCommandResult;

/**
 * Represent the workbench layout data slave controller data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutDataSlaveController {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataService *service;
    UmiWorkbenchLayoutDataControllerState state;
    char controller_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char master_controller_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint64_t handled_command_count;
    uint64_t failed_command_count;
    uint64_t revision;
} UmiWorkbenchLayoutDataSlaveController;

/**
 * Initialise workbench layout data controller from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_data_controller_init(
    UmiWorkbenchLayoutDataSlaveController *controller,
    UmiWorkbenchLayoutDataService *service,
    const char *controller_id,
    const char *master_controller_id);

/**
 * Provide the workbench layout data controller initialise operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_data_controller_initialise(
    UmiWorkbenchLayoutDataSlaveController *controller);

/**
 * Provide the workbench layout data controller start operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_controller_start(
    UmiWorkbenchLayoutDataSlaveController *controller,
    uint64_t now_ms);

/**
 * Provide the workbench layout data controller quiesce operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_controller_quiesce(
    UmiWorkbenchLayoutDataSlaveController *controller);

/**
 * Provide the workbench layout data controller stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_controller_stop(
    UmiWorkbenchLayoutDataSlaveController *controller,
    uint64_t now_ms);

/**
 * Perform workbench layout data controller through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_workbench_layout_data_controller_handle(
    UmiWorkbenchLayoutDataSlaveController *controller,
    const UmiWorkbenchLayoutDataCommand *command,
    UmiWorkbenchLayoutDataCommandResult *out_result);

/**
 * Provide the workbench layout data command kind text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_command_kind_text(
    UmiWorkbenchLayoutDataCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif
