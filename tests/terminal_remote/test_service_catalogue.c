/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_service_catalogue.c
 *
 * PURPOSE:
 *   Verify service catalogue catalogue insertion, lookup and duplicate rejection.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/service_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteServiceCatalogue v; umi_terminal_remote_service_catalogue_init(&v); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_service_catalogue_add(&v,"one","One")!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_terminal_remote_service_catalogue_find(&v,"one")) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_terminal_remote_service_catalogue_add(&v,"one","Again")!=UMI_STATUS_ALREADY_EXISTS) return 3; return umi_terminal_remote_service_catalogue_enabled_count(&v)==1U?0:4; }
