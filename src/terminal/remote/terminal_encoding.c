/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_encoding.c
 *
 * PURPOSE:
 *   Implement allow-listed terminal encodings.
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
#include "umicom/terminal/remote/terminal_encoding.h"
#include <string.h>
bool umi_terminal_remote_terminal_encoding_supported(const char *name) { return name && (strcmp(name,"UTF-8")==0||strcmp(name,"utf-8")==0||strcmp(name,"CP65001")==0||strcmp(name,"ASCII")==0); }
