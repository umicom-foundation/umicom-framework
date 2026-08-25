/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_shell_catalogue.c
 *
 * PURPOSE:
 *   Verify shell catalogue catalogue insertion, lookup and duplicate rejection.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/shell_catalogue.h"
int main(void) { UmiTerminalRemoteShellCatalogue v; umi_terminal_remote_shell_catalogue_init(&v); if(umi_terminal_remote_shell_catalogue_add(&v,"one","One")!=UMI_STATUS_OK) return 1; if(!umi_terminal_remote_shell_catalogue_find(&v,"one")) return 2; if(umi_terminal_remote_shell_catalogue_add(&v,"one","Again")!=UMI_STATUS_ALREADY_EXISTS) return 3; return umi_terminal_remote_shell_catalogue_enabled_count(&v)==1U?0:4; }
