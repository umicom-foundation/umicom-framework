/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_auth_method.c
 *
 * PURPOSE:
 *   Implement simple authentication-risk ordering for policy decisions.
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
#include "umicom/terminal/remote/remote_auth_method.h"
/*
 * Provide the terminal remote remote auth method risk operation used by this module and
 * its client applications.
 */
uint32_t umi_terminal_remote_remote_auth_method_risk(UmiTerminalRemoteRemoteAuthMethodMethod method,bool interactive) { uint32_t risk; /* Select the behaviour associated with the requested command or state value. */ switch(method) { case UMI_TERMINAL_REMOTE_REMOTE_AUTH_METHOD_AGENT:risk=1U;break; case UMI_TERMINAL_REMOTE_REMOTE_AUTH_METHOD_KEY:risk=2U;break; case UMI_TERMINAL_REMOTE_REMOTE_AUTH_METHOD_PASSWORD:risk=5U;break; default:risk=10U;break; } /* Apply this operation only while the related capability or state is available. */ if(interactive&&risk>0U) risk--; return risk; }
