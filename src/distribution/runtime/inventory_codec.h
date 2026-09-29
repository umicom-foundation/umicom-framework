/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Internal format helpers shared by registration inventories and test policy. */
#ifndef UMICOM_RELEASE_INVENTORY_CODEC_H
#define UMICOM_RELEASE_INVENTORY_CODEC_H
#include "umicom/distribution/runtime/inventory.h"
UmiStatus UmiReleaseInventoryDecodeField(char *text);
size_t UmiReleaseInventorySplitFields(char *line, char **fields, size_t capacity);
bool UmiReleaseInventoryGenerationValid(const char *text);
#endif
