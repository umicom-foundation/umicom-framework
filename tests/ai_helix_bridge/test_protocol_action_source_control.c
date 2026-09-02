/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_action_source_control.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol action source control.
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
 * Exercise test protocol action source control and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_protocol_action_source_control(void){UmiHelixActionKind k=(UmiHelixActionKind)0;TEST_CHECK(umi_ai_helix_protocol_parse_action("source_control",&k)==UMI_STATUS_OK);TEST_CHECK(k==UMI_HELIX_ACTION_SOURCE_CONTROL);return 0;}
