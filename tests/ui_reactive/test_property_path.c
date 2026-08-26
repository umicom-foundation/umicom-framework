/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_property_path.c
 *
 * PURPOSE:
 *   Exercise the property path reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/property_path.h"
int main(void) { UmiUiReactivePropertyPath p; if(umi_ui_reactive_property_path_parse(&p,"trade.counterparty.name")!=UMI_STATUS_OK) return 1; return p.segment_count==3U?0:2; }
