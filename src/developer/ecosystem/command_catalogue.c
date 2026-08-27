/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/command_catalogue.c
 * PURPOSE: Implement the canonical developer-ecosystem command catalogue.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/command_catalogue.h"

#include <string.h>

#define COMMAND(id, text, group, danger, network, selection) \
    {id, text, group, danger, network, selection}

static const UmiEcosystemCommand COMMANDS[] = {
    COMMAND("ecosystem.packages.open", "Open Package Manager", "Packages", false, false, false),
    COMMAND("ecosystem.packages.search", "Search Packages", "Packages", false, false, false),
    COMMAND("ecosystem.packages.refresh", "Refresh Packages", "Packages", false, true, false),
    COMMAND("ecosystem.packages.install", "Install Package", "Packages", false, true, true),
    COMMAND("ecosystem.packages.update", "Update Package", "Packages", false, true, true),
    COMMAND("ecosystem.packages.remove", "Remove Package", "Packages", true, false, true),
    COMMAND("ecosystem.packages.enable", "Enable Package", "Packages", false, false, true),
    COMMAND("ecosystem.packages.disable", "Disable Package", "Packages", false, false, true),
    COMMAND("ecosystem.packages.pin", "Pin Package Version", "Packages", false, false, true),
    COMMAND("ecosystem.packages.unpin", "Unpin Package Version", "Packages", false, false, true),
    COMMAND("ecosystem.sources.open", "Open Package Sources", "Sources", false, false, false),
    COMMAND("ecosystem.sources.add-local", "Add Local Source", "Sources", false, false, false),
    COMMAND("ecosystem.sources.add-offline", "Add Offline Feed", "Sources", false, false, false),
    COMMAND("ecosystem.sources.sync", "Synchronise Source", "Sources", false, true, true),
    COMMAND("ecosystem.sources.enable", "Enable Source", "Sources", false, false, true),
    COMMAND("ecosystem.sources.disable", "Disable Source", "Sources", false, false, true),
    COMMAND("ecosystem.sources.offline-mode", "Toggle Offline Mode", "Sources", false, false, false),
    COMMAND("ecosystem.marketplace.open", "Open Extension Marketplace", "Marketplace", false, true, false),
    COMMAND("ecosystem.marketplace.publisher", "Inspect Publisher", "Marketplace", false, true, true),
    COMMAND("ecosystem.marketplace.provenance", "Inspect Provenance", "Marketplace", false, false, true),
    COMMAND("ecosystem.sdk.open", "Open SDK Manager", "SDK", false, false, false),
    COMMAND("ecosystem.sdk.refresh", "Refresh SDK Inventory", "SDK", false, false, false),
    COMMAND("ecosystem.sdk.install", "Install SDK", "SDK", false, true, true),
    COMMAND("ecosystem.sdk.update", "Update SDK", "SDK", false, true, true),
    COMMAND("ecosystem.sdk.remove", "Remove SDK", "SDK", true, false, true),
    COMMAND("ecosystem.sdk.set-default", "Set Default SDK", "SDK", false, false, true),
    COMMAND("ecosystem.sdk.repair", "Repair SDK", "SDK", false, false, true),
    COMMAND("ecosystem.component.open", "Open Component Catalogue", "Components", false, false, false),
    COMMAND("ecosystem.component.new", "New Component", "Components", false, false, false),
    COMMAND("ecosystem.component.preview", "Preview Component Files", "Components", false, false, true),
    COMMAND("ecosystem.component.generate", "Generate Component", "Components", false, false, true),
    COMMAND("ecosystem.component.package", "Package Component", "Components", false, false, true),
    COMMAND("ecosystem.migration.open", "Open Migration Centre", "Migration", false, false, false),
    COMMAND("ecosystem.migration.analyse", "Analyse Migration", "Migration", false, false, true),
    COMMAND("ecosystem.migration.apply", "Apply Migration", "Migration", true, false, true),
    COMMAND("ecosystem.migration.rollback", "Rollback Migration", "Migration", true, false, true),
    COMMAND("ecosystem.extensions.open", "Open Extension Host", "Extensions", false, false, false),
    COMMAND("ecosystem.extensions.restart-host", "Restart Extension Host", "Extensions", true, false, false),
    COMMAND("ecosystem.extensions.quarantine", "Quarantine Extension", "Extensions", true, false, true),
    COMMAND("ecosystem.extensions.recover", "Recover Extension", "Extensions", false, false, true)
};

#undef COMMAND

size_t umi_ecosystem_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiEcosystemCommand *umi_ecosystem_command_at(size_t index)
{
    return index < umi_ecosystem_command_count() ? &COMMANDS[index] : NULL;
}

const UmiEcosystemCommand *umi_ecosystem_command_find(const char *command_id)
{
    size_t index;
    if (command_id == NULL) return NULL;
    for (index = 0U; index < umi_ecosystem_command_count(); ++index) {
        if (strcmp(COMMANDS[index].command_id, command_id) == 0) {
            return &COMMANDS[index];
        }
    }
    return NULL;
}
