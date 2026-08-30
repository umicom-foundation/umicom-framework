/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_environment_overlay.c
 *
 * PURPOSE:
 *   Verify terminal environment overlay bounded state and deterministic score.
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
#include "umicom/terminal/remote/terminal_environment_overlay.h"
int main(void) { UmiTerminalRemoteTerminalEnvironmentOverlay v; umi_terminal_remote_terminal_environment_overlay_init(&v,"item",100U); if(umi_terminal_remote_terminal_environment_overlay_set_metric(&v,50U)!=UMI_STATUS_OK) return 1; if(!umi_terminal_remote_terminal_environment_overlay_within_limit(&v)||umi_terminal_remote_terminal_environment_overlay_score(&v)==0U) return 2; if(umi_terminal_remote_terminal_environment_overlay_set_metric(&v,101U)!=UMI_STATUS_OK) return 3; return umi_terminal_remote_terminal_environment_overlay_within_limit(&v)?4:0; }
