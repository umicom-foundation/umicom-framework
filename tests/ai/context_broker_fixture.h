/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/context_broker_fixture.h
 *
 * PURPOSE:
 *   Construct valid context provenance for native broker and planning regression cases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_CONTEXT_BROKER_FIXTURE_H
#define UMICOM_TEST_CONTEXT_BROKER_FIXTURE_H
#include "../support/assertions.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/ai/context_broker.h"
static UmiAiContextSource context_fixture(const char *id)
{
    UmiAiContextSource source = {0};
    assert(strlen(id) < sizeof source.source_id);
    strcpy(source.source_id, id);
    strcpy(source.label, "Example document");
    strcpy(source.uri, "workspace://example/document");
    source.kind = UMI_AI_CONTEXT_DOCUMENT;
    source.classification = UMI_AI_DATA_INTERNAL;
    source.estimated_tokens = 32U;
    source.priority = 1U;
    source.enabled = 1;
    source.revision = 900U;
    return source;
}
#endif
