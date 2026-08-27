/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_evidence_add.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: evidence add.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_evidence_add(void){UmiAiHelixEvidenceContext c;umi_ai_helix_evidence_context_init(&c);TEST_CHECK(umi_ai_helix_evidence_context_add(&c,"build","green")==UMI_STATUS_OK);TEST_CHECK(strstr(c.text,"[build]\ngreen\n")!=NULL);TEST_CHECK(c.item_count==1U);return 0;}
