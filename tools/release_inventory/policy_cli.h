/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_RELEASE_INVENTORY_POLICY_CLI_H
#define UMICOM_RELEASE_INVENTORY_POLICY_CLI_H
#include "umicom/distribution/runtime/inventory.h"
int UmiReleaseInventoryPolicyCommand(int argc, char **argv,
    UmiStatus (*readInventory)(const char *, UmiReleaseInventory **),
    void (*printText)(const char *));
#endif
