/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_link_detection.c
 *
 * PURPOSE:
 *   Implement bounded HTTP/HTTPS terminal link detection.
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
 * Provide the terminal remote terminal link detection find first operation used by this
 * module and its client applications.
 */
size_t umi_terminal_remote_terminal_link_detection_find_first(const char *text,char *out,size_t capacity) { const char *p; size_t n=0U; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!text||!out||capacity==0U) return 0U; p=strstr(text,"https://"); if(!p) p=strstr(text,"http://"); if(!p) { out[0]='\0'; return 0U; } while(p[n]&&p[n]!=' '&&p[n]!='\n'&&n+1U<capacity) { out[n]=p[n]; n++; } out[n]='\0'; return n; }
