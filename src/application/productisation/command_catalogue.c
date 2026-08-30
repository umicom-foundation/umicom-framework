/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/command_catalogue.c
 *
 * PURPOSE:
 *   Define stable commands for product inventory, shared-gap planning and
 *   acceptance evidence without implementing a product-specific UI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/command_catalogue.h"

#include <string.h>

static const UmiProductisationCommandDefinition COMMANDS[] = {
    { "umicom.productisation.refresh", "Refresh Productisation Evidence",
      "Productisation", "Rebuild inventory, coverage, gaps and completion order.", 0 },
    { "umicom.productisation.openInventory", "Open Application Inventory",
      "Productisation", "Inspect all declared features, panels and layouts.", 0 },
    { "umicom.productisation.openCapabilityMatrix", "Open Capability Matrix",
      "Productisation", "Cross-reference applications, Framework capabilities and components.", 0 },
    { "umicom.productisation.openCoverage", "Open Coverage",
      "Productisation", "Inspect implementation, adoption, testing and acceptance coverage.", 0 },
    { "umicom.productisation.openSharedGaps", "Open Shared Framework Gaps",
      "Productisation", "Prioritise reusable gaps affecting multiple applications.", 0 },
    { "umicom.productisation.openApplicationGaps", "Open Application Gaps",
      "Productisation", "Inspect thin-product adoption and external adapter work.", 0 },
    { "umicom.productisation.openCompletionPlan", "Open Completion Plan",
      "Productisation", "Review Framework-first, Studio, Trader and remaining stages.", 0 },
    { "umicom.productisation.recordEvidence", "Record Evidence",
      "Productisation", "Record implementation, adoption, test or acceptance evidence.", 1 },
    { "umicom.productisation.blockAsset", "Block Product Asset",
      "Productisation", "Record an explicit blocker against a feature, panel or layout.", 1 },
    { "umicom.productisation.evaluateReleaseGate", "Evaluate Release Gate",
      "Productisation", "Evaluate catalogue, Framework and product completion gates.", 0 },
    { "umicom.productisation.beginStudioCompletion", "Begin Studio Completion",
      "Productisation", "Select the next allowed Studio adoption step.", 0 },
    { "umicom.productisation.beginTraderCompletion", "Begin Trader Completion",
      "Productisation", "Select the next allowed Trader adoption step without bypassing live-trading gates.", 0 },
    { "umicom.productisation.exportReport", "Export Productisation Report",
      "Productisation", "Export the current inventory, evidence, gaps and plan.", 0 }
};

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

size_t umi_productisation_command_catalogue_count(void)
{
    return COUNT_OF(COMMANDS);
}

const UmiProductisationCommandDefinition *
umi_productisation_command_catalogue_at(size_t index)
{
    return index < COUNT_OF(COMMANDS) ? &COMMANDS[index] : NULL;
}

const UmiProductisationCommandDefinition *
umi_productisation_command_catalogue_find(const char *command_id)
{
    size_t index;
    if (command_id == NULL) return NULL;
    for (index = 0U; index < COUNT_OF(COMMANDS); ++index) {
        if (strcmp(COMMANDS[index].command_id, command_id) == 0)
            return &COMMANDS[index];
    }
    return NULL;
}
