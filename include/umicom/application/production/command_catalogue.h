/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/command_catalogue.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_COMMAND_CATALOGUE_H
#define UMICOM_APPLICATION_PRODUCTION_COMMAND_CATALOGUE_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/types.h"

/**
 * Represent the application production control command data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionControlCommand {
    const char *command_id;
    const char *title;
    const char *category;
    const char *description;
    int mutates_state;
} UmiApplicationProductionControlCommand;

/**
 * Return the number of records represented by application production control command
 * without changing their state.
 */
size_t umi_application_production_control_command_count(void);
/**
 * Find application production control command while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionControlCommand *
umi_application_production_control_command_at(size_t index);
/**
 * Find application production control command while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionControlCommand *
umi_application_production_control_command_find(const char *command_id);

#ifdef __cplusplus
}
#endif
#endif
