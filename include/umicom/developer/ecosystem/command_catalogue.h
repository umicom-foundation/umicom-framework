/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/command_catalogue.h
 * PURPOSE: Declare stable commands for package, SDK and component workbenches.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_COMMAND_CATALOGUE_H
#define UMICOM_DEVELOPER_ECOSYSTEM_COMMAND_CATALOGUE_H

#include "umicom/developer/ecosystem/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEcosystemCommand {
    const char *command_id;
    const char *label;
    const char *category;
    bool destructive;
    bool requires_network;
    bool requires_selection;
} UmiEcosystemCommand;

size_t umi_ecosystem_command_count(void);
const UmiEcosystemCommand *umi_ecosystem_command_at(size_t index);
const UmiEcosystemCommand *umi_ecosystem_command_find(const char *command_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_COMMAND_CATALOGUE_H */
