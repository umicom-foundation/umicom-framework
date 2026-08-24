/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/command.c
 *
 * PURPOSE:
 *   Define the complete cross-domain IDE command catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/command.h"

#include <string.h>

static const UmiIdeCommandDescriptor COMMANDS[] = {
    {"ide.navigate.back","Navigate Back","Navigate","Navigate to the previous cross-domain location.",1,0,0,0,0,0,0,0,1},
    {"ide.navigate.forward","Navigate Forward","Navigate","Navigate to the next cross-domain location.",1,0,0,0,0,0,0,0,1},
    {"ide.open.problem","Open Problem","Problems","Open the selected Problem in the authoritative DocumentCoordinator.",1,1,0,0,0,0,0,0,1},
    {"ide.open.test","Open Test","Testing","Open the selected test source location.",1,0,1,0,0,0,0,0,1},
    {"ide.open.source-control","Open Source Change","Source Control","Open the selected source-control file.",1,0,0,1,0,0,0,0,1},
    {"ide.open.symbol","Open Symbol","Navigate","Open the selected provider-neutral language symbol.",1,0,0,0,0,1,0,0,1},
    {"ide.open.diagnostic","Open Diagnostic","Problems","Open the selected language diagnostic.",1,0,0,0,0,1,0,0,1},
    {"ide.open.debug-frame","Open Debug Frame","Debug","Open the active debugger stack frame source location.",1,0,0,0,1,0,0,0,1},
    {"ide.inline.request","Request Inline AI Completion","AI","Generate a low-latency inline code suggestion.",1,0,0,0,0,0,0,1,1},
    {"ide.inline.accept","Accept Inline AI Completion","AI","Apply the active suggestion after revision conflict checks.",1,0,0,0,0,0,0,1,1},
    {"ide.inline.reject","Reject Inline AI Completion","AI","Reject the active inline suggestion.",1,0,0,0,0,0,0,1,1},
    {"ide.inline.cancel","Cancel Inline AI Completion","AI","Cancel the active inline suggestion.",1,0,0,0,0,0,0,1,1},
    {"ide.ai.explain-selection","Explain Selection","AI","Explain the active editor selection with repository context.",1,0,0,0,0,0,1,0,1},
    {"ide.ai.refactor-selection","Refactor Selection","AI","Prepare a governed refactoring patch for the selection.",1,0,0,0,0,0,1,0,1},
    {"ide.ai.generate-tests","Generate Tests","AI","Prepare governed tests for the selected code.",1,0,0,0,0,0,1,0,1},
    {"ide.ai.fix-problem","Fix Problem with AI","AI","Prepare a governed repair for the selected Problem.",1,1,0,0,0,0,1,0,1},
    {"ide.workflow.refresh","Refresh Workflow Gates","Workflow","Refresh build/test/source/debug/AI readiness gates.",0,0,0,0,0,0,0,0,1},
    {"ide.workflow.profile.development","Development Workflow","Workflow","Use interactive development readiness policy.",0,0,0,0,0,0,0,0,1},
    {"ide.workflow.profile.pre-commit","Pre-Commit Workflow","Workflow","Use pre-commit readiness policy.",0,0,0,0,0,0,0,0,1},
    {"ide.workflow.profile.self-host","Self-Host Workflow","Workflow","Use Umicom IDE self-host readiness policy.",0,0,0,0,0,0,0,0,1},
    {"ide.workflow.profile.release","Release Workflow","Workflow","Use strict release readiness policy.",0,0,0,0,0,0,0,0,1},
    {"ide.workflow.profile.autonomous-ai","Autonomous AI Workflow","Workflow","Use autonomous AI repair readiness policy.",0,0,0,0,0,0,0,0,1},
    {"ide.document.save","Save Active Document","File","Save the active authoritative working copy.",1,0,0,0,0,0,0,0,1},
    {"ide.document.sync","Sync Active Document","File","Synchronize the active document with its provider.",1,0,0,0,0,0,0,0,1},
    {"ide.document.check-external","Check External Change","File","Check the active document for external changes.",1,0,0,0,0,0,0,0,0},
    {"ide.self-host.verify","Verify Self-Host Readiness","Workflow","Evaluate strict self-hosting readiness.",0,0,0,0,0,0,0,0,0},
    {"ide.workflow.ready","Workflow Ready","Workflow","Report current workflow readiness.",0,0,0,0,0,0,0,0,0},
    {"ide.ai.open-overview","Open AI Developer","AI","Open the AI Developer overview.",0,0,0,0,0,0,1,0,1},
    {"ide.ai.open-review","Open AI Patch Review","AI","Open the current AI governed patch review.",0,0,0,0,0,0,1,0,1}
};

size_t umi_ide_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiIdeCommandDescriptor *umi_ide_command_at(size_t index)
{
    return index < umi_ide_command_count() ? &COMMANDS[index] : NULL;
}

const UmiIdeCommandDescriptor *umi_ide_command_find(const char *command_id)
{
    size_t index;

    if (command_id == NULL) return NULL;

    for (index = 0U; index < umi_ide_command_count(); ++index) {
        if (strcmp(COMMANDS[index].command_id, command_id) == 0) {
            return &COMMANDS[index];
        }
    }

    return NULL;
}
