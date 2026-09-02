/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_action_read.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol action read.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

/*
 * Read test protocol action into validated module state and return a status when input
 * cannot be used.
 */
int test_protocol_action_read(void){UmiHelixActionKind k=(UmiHelixActionKind)0;TEST_CHECK(umi_ai_helix_protocol_parse_action("read",&k)==UMI_STATUS_OK);TEST_CHECK(k==UMI_HELIX_ACTION_READ);return 0;}
