/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_path_mapping.c
 *
 * PURPOSE:
 *   Verify deterministic local-to-remote workspace path translation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_path_mapping.h"
#include <string.h>
int main(void) { UmiTerminalRemoteRemotePathMapping v; char out[64]; umi_terminal_remote_remote_path_mapping_init(&v,"C:/work","/srv/work"); if(umi_terminal_remote_remote_path_mapping_to_remote(&v,"C:/work/src/a.c",out,sizeof(out))!=UMI_STATUS_OK) return 1; return strcmp(out,"/srv/work/src/a.c")==0?0:2; }
