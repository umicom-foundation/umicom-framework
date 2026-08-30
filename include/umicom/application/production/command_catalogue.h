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

typedef struct UmiApplicationProductionControlCommand {
    const char *command_id;
    const char *title;
    const char *category;
    const char *description;
    int mutates_state;
} UmiApplicationProductionControlCommand;

size_t umi_application_production_control_command_count(void);
const UmiApplicationProductionControlCommand *
umi_application_production_control_command_at(size_t index);
const UmiApplicationProductionControlCommand *
umi_application_production_control_command_find(const char *command_id);

#ifdef __cplusplus
}
#endif
#endif
