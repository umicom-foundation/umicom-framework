/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_icon_descriptor.c
 *
 * PURPOSE:
 *   Verify describe a semantic icon identity, directionality and scalable/symbolic capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_descriptor.h"
int main(void) {
    UmiAppearanceIconDescriptor item;
    if (umi_appearance_icon_descriptor_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_icon_descriptor_is_valid(&item)) return 2;
    return 0;
}
