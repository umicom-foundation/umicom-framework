/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/command_catalogue.h
 * PURPOSE: Declare stable commands for package, SDK and component workbenches.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_COMMAND_CATALOGUE_H
#define UMICOM_DEVELOPER_ECOSYSTEM_COMMAND_CATALOGUE_H

#include "umicom/developer/ecosystem/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ecosystem command data shared with callers of this public contract.
 */
typedef struct UmiEcosystemCommand {
    const char *command_id;
    const char *label;
    const char *category;
    bool destructive;
    bool requires_network;
    bool requires_selection;
} UmiEcosystemCommand;

/**
 * Return the number of records represented by ecosystem command without changing their
 * state.
 */
size_t umi_ecosystem_command_count(void);
/**
 * Find ecosystem command while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiEcosystemCommand *umi_ecosystem_command_at(size_t index);
/**
 * Find ecosystem command while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiEcosystemCommand *umi_ecosystem_command_find(const char *command_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_COMMAND_CATALOGUE_H */
