/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_property_path.c
 *
 * PURPOSE:
 *   Exercise the property path reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/property_path.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactivePropertyPath p; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_reactive_property_path_parse(&p,"trade.counterparty.name")!=UMI_STATUS_OK) return 1; return p.segment_count==3U?0:2; }
