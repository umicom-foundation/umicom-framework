/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_link_detection.c
 *
 * PURPOSE:
 *   Verify bounded terminal link extraction.
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
#include "umicom/terminal/remote/terminal_link_detection.h"
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { char out[64]; /* Use the stable identifier comparison to choose the matching record or policy. */ if(umi_terminal_remote_terminal_link_detection_find_first("go https://example.test now",out,sizeof(out))==0U) return 1; return strcmp(out,"https://example.test")==0?0:2; }
