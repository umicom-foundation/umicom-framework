/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/service.c
 *
 * PURPOSE:
 *   Provide one discoverable catalogue for the major developer operations
 *   workbench projections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/service.h"

#include <string.h>

#include "umicom/developer_operations/coverage/branch.h"
#include "umicom/developer_operations/coverage/delta.h"
#include "umicom/developer_operations/coverage/file.h"
#include "umicom/developer_operations/coverage/filter.h"
#include "umicom/developer_operations/coverage/function.h"
#include "umicom/developer_operations/coverage/group.h"
#include "umicom/developer_operations/coverage/history.h"
#include "umicom/developer_operations/coverage/line.h"
#include "umicom/developer_operations/coverage/navigation.h"
#include "umicom/developer_operations/coverage/quality_gate.h"
#include "umicom/developer_operations/coverage/region.h"
#include "umicom/developer_operations/coverage/run_compare.h"
#include "umicom/developer_operations/coverage/sort.h"
#include "umicom/developer_operations/coverage/summary.h"
#include "umicom/developer_operations/coverage/test_duration.h"
#include "umicom/developer_operations/coverage/test_failure.h"
#include "umicom/developer_operations/coverage/test_flaky.h"
#include "umicom/developer_operations/coverage/test_impact.h"
#include "umicom/developer_operations/coverage/test_result_summary.h"
#include "umicom/developer_operations/coverage/threshold.h"
#include "umicom/developer_operations/debug/adapter.h"
#include "umicom/developer_operations/debug/breakpoint.h"
#include "umicom/developer_operations/debug/call_stack.h"
#include "umicom/developer_operations/debug/console.h"
#include "umicom/developer_operations/debug/disassembly.h"
#include "umicom/developer_operations/debug/frame.h"
#include "umicom/developer_operations/debug/memory.h"
#include "umicom/developer_operations/debug/module.h"
#include "umicom/developer_operations/debug/navigation.h"
#include "umicom/developer_operations/debug/process.h"
#include "umicom/developer_operations/debug/register.h"
#include "umicom/developer_operations/debug/scope.h"
#include "umicom/developer_operations/debug/session.h"
#include "umicom/developer_operations/debug/snapshot.h"
#include "umicom/developer_operations/debug/source.h"
#include "umicom/developer_operations/debug/stop_reason.h"
#include "umicom/developer_operations/debug/thread.h"
#include "umicom/developer_operations/debug/toolbar.h"
#include "umicom/developer_operations/debug/variable.h"
#include "umicom/developer_operations/debug/watch.h"
#include "umicom/developer_operations/terminal/command.h"
#include "umicom/developer_operations/terminal/environment.h"
#include "umicom/developer_operations/terminal/history.h"
#include "umicom/developer_operations/terminal/output_channel.h"
#include "umicom/developer_operations/terminal/output_filter.h"
#include "umicom/developer_operations/terminal/output_search.h"
#include "umicom/developer_operations/terminal/process.h"
#include "umicom/developer_operations/terminal/profile.h"
#include "umicom/developer_operations/terminal/search.h"
#include "umicom/developer_operations/terminal/session.h"
#include "umicom/developer_operations/terminal/split.h"
#include "umicom/developer_operations/terminal/tab.h"
#include "umicom/developer_operations/terminal/task_cancellation.h"
#include "umicom/developer_operations/terminal/task_execution.h"
#include "umicom/developer_operations/terminal/task_group.h"
#include "umicom/developer_operations/terminal/task_history.h"
#include "umicom/developer_operations/terminal/task_progress.h"
#include "umicom/developer_operations/terminal/task_retry.h"
#include "umicom/developer_operations/terminal/transcript.h"
#include "umicom/developer_operations/terminal/working_directory.h"
#include "umicom/developer_operations/vcs/blame.h"
#include "umicom/developer_operations/vcs/branch.h"
#include "umicom/developer_operations/vcs/change_list.h"
#include "umicom/developer_operations/vcs/commit_graph.h"
#include "umicom/developer_operations/vcs/compare.h"
#include "umicom/developer_operations/vcs/conflict.h"
#include "umicom/developer_operations/vcs/diff.h"
#include "umicom/developer_operations/vcs/directory_compare.h"
#include "umicom/developer_operations/vcs/history.h"
#include "umicom/developer_operations/vcs/merge.h"
#include "umicom/developer_operations/vcs/operation_journal.h"
#include "umicom/developer_operations/vcs/partial_stage.h"
#include "umicom/developer_operations/vcs/patch.h"
#include "umicom/developer_operations/vcs/repository.h"
#include "umicom/developer_operations/vcs/review.h"
#include "umicom/developer_operations/vcs/stage.h"
#include "umicom/developer_operations/vcs/stash.h"
#include "umicom/developer_operations/vcs/status.h"
#include "umicom/developer_operations/vcs/tag.h"
#include "umicom/developer_operations/vcs/worktree.h"

typedef struct UmiDeveloperOperationCatalogueEntry {
    const char *id;
    UmiDeveloperOperationEvaluator evaluator;
} UmiDeveloperOperationCatalogueEntry;

static const UmiDeveloperOperationCatalogueEntry CATALOGUE[] = {
    {"coverage.branch", UmiDeveloperOperationsEvaluateCoverageBranch},
    {"coverage.delta", UmiDeveloperOperationsEvaluateCoverageDelta},
    {"coverage.file", UmiDeveloperOperationsEvaluateCoverageFile},
    {"coverage.filter", UmiDeveloperOperationsEvaluateCoverageFilter},
    {"coverage.function", UmiDeveloperOperationsEvaluateCoverageFunction},
    {"coverage.group", UmiDeveloperOperationsEvaluateCoverageGroup},
    {"coverage.history", UmiDeveloperOperationsEvaluateCoverageHistory},
    {"coverage.line", UmiDeveloperOperationsEvaluateCoverageLine},
    {"coverage.navigation", UmiDeveloperOperationsEvaluateCoverageNavigation},
    {"coverage.quality-gate", UmiDeveloperOperationsEvaluateCoverageQualityGate},
    {"coverage.region", UmiDeveloperOperationsEvaluateCoverageRegion},
    {"coverage.run-compare", UmiDeveloperOperationsEvaluateCoverageRunCompare},
    {"coverage.sort", UmiDeveloperOperationsEvaluateCoverageSort},
    {"coverage.summary", UmiDeveloperOperationsEvaluateCoverageSummary},
    {"coverage.test-duration", UmiDeveloperOperationsEvaluateCoverageTestDuration},
    {"coverage.test-failure", UmiDeveloperOperationsEvaluateCoverageTestFailure},
    {"coverage.test-flaky", UmiDeveloperOperationsEvaluateCoverageTestFlaky},
    {"coverage.test-impact", UmiDeveloperOperationsEvaluateCoverageTestImpact},
    {"coverage.test-result-summary", UmiDeveloperOperationsEvaluateCoverageTestResultSummary},
    {"coverage.threshold", UmiDeveloperOperationsEvaluateCoverageThreshold},
    {"debug.adapter", UmiDeveloperOperationsEvaluateDebugAdapter},
    {"debug.breakpoint", UmiDeveloperOperationsEvaluateDebugBreakpoint},
    {"debug.call-stack", UmiDeveloperOperationsEvaluateDebugCallStack},
    {"debug.console", UmiDeveloperOperationsEvaluateDebugConsole},
    {"debug.disassembly", UmiDeveloperOperationsEvaluateDebugDisassembly},
    {"debug.frame", UmiDeveloperOperationsEvaluateDebugFrame},
    {"debug.memory", UmiDeveloperOperationsEvaluateDebugMemory},
    {"debug.module", UmiDeveloperOperationsEvaluateDebugModule},
    {"debug.navigation", UmiDeveloperOperationsEvaluateDebugNavigation},
    {"debug.process", UmiDeveloperOperationsEvaluateDebugProcess},
    {"debug.register", UmiDeveloperOperationsEvaluateDebugRegister},
    {"debug.scope", UmiDeveloperOperationsEvaluateDebugScope},
    {"debug.session", UmiDeveloperOperationsEvaluateDebugSession},
    {"debug.snapshot", UmiDeveloperOperationsEvaluateDebugSnapshot},
    {"debug.source", UmiDeveloperOperationsEvaluateDebugSource},
    {"debug.stop-reason", UmiDeveloperOperationsEvaluateDebugStopReason},
    {"debug.thread", UmiDeveloperOperationsEvaluateDebugThread},
    {"debug.toolbar", UmiDeveloperOperationsEvaluateDebugToolbar},
    {"debug.variable", UmiDeveloperOperationsEvaluateDebugVariable},
    {"debug.watch", UmiDeveloperOperationsEvaluateDebugWatch},
    {"terminal.command", UmiDeveloperOperationsEvaluateTerminalCommand},
    {"terminal.environment", UmiDeveloperOperationsEvaluateTerminalEnvironment},
    {"terminal.history", UmiDeveloperOperationsEvaluateTerminalHistory},
    {"terminal.output-channel", UmiDeveloperOperationsEvaluateTerminalOutputChannel},
    {"terminal.output-filter", UmiDeveloperOperationsEvaluateTerminalOutputFilter},
    {"terminal.output-search", UmiDeveloperOperationsEvaluateTerminalOutputSearch},
    {"terminal.process", UmiDeveloperOperationsEvaluateTerminalProcess},
    {"terminal.profile", UmiDeveloperOperationsEvaluateTerminalProfile},
    {"terminal.search", UmiDeveloperOperationsEvaluateTerminalSearch},
    {"terminal.session", UmiDeveloperOperationsEvaluateTerminalSession},
    {"terminal.split", UmiDeveloperOperationsEvaluateTerminalSplit},
    {"terminal.tab", UmiDeveloperOperationsEvaluateTerminalTab},
    {"terminal.task-cancellation", UmiDeveloperOperationsEvaluateTerminalTaskCancellation},
    {"terminal.task-execution", UmiDeveloperOperationsEvaluateTerminalTaskExecution},
    {"terminal.task-group", UmiDeveloperOperationsEvaluateTerminalTaskGroup},
    {"terminal.task-history", UmiDeveloperOperationsEvaluateTerminalTaskHistory},
    {"terminal.task-progress", UmiDeveloperOperationsEvaluateTerminalTaskProgress},
    {"terminal.task-retry", UmiDeveloperOperationsEvaluateTerminalTaskRetry},
    {"terminal.transcript", UmiDeveloperOperationsEvaluateTerminalTranscript},
    {"terminal.working-directory", UmiDeveloperOperationsEvaluateTerminalWorkingDirectory},
    {"vcs.blame", UmiDeveloperOperationsEvaluateVcsBlame},
    {"vcs.branch", UmiDeveloperOperationsEvaluateVcsBranch},
    {"vcs.change-list", UmiDeveloperOperationsEvaluateVcsChangeList},
    {"vcs.commit-graph", UmiDeveloperOperationsEvaluateVcsCommitGraph},
    {"vcs.compare", UmiDeveloperOperationsEvaluateVcsCompare},
    {"vcs.conflict", UmiDeveloperOperationsEvaluateVcsConflict},
    {"vcs.diff", UmiDeveloperOperationsEvaluateVcsDiff},
    {"vcs.directory-compare", UmiDeveloperOperationsEvaluateVcsDirectoryCompare},
    {"vcs.history", UmiDeveloperOperationsEvaluateVcsHistory},
    {"vcs.merge", UmiDeveloperOperationsEvaluateVcsMerge},
    {"vcs.operation-journal", UmiDeveloperOperationsEvaluateVcsOperationJournal},
    {"vcs.partial-stage", UmiDeveloperOperationsEvaluateVcsPartialStage},
    {"vcs.patch", UmiDeveloperOperationsEvaluateVcsPatch},
    {"vcs.repository", UmiDeveloperOperationsEvaluateVcsRepository},
    {"vcs.review", UmiDeveloperOperationsEvaluateVcsReview},
    {"vcs.stage", UmiDeveloperOperationsEvaluateVcsStage},
    {"vcs.stash", UmiDeveloperOperationsEvaluateVcsStash},
    {"vcs.status", UmiDeveloperOperationsEvaluateVcsStatus},
    {"vcs.tag", UmiDeveloperOperationsEvaluateVcsTag},
    {"vcs.worktree", UmiDeveloperOperationsEvaluateVcsWorktree},
};

size_t UmiDeveloperOperationsServiceCount(void)
{
    return sizeof(CATALOGUE) / sizeof(CATALOGUE[0]);
}

UmiStatus UmiDeveloperOperationsServiceIdAt(
    size_t index,
    const char **outId)
{
    if (outId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outId = NULL;
    if (index >= UmiDeveloperOperationsServiceCount()) {
        return UMI_STATUS_NOT_FOUND;
    }
    *outId = CATALOGUE[index].id;
    return UMI_STATUS_OK;
}

UmiStatus UmiDeveloperOperationsServiceEvaluate(
    const char *id,
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    size_t index;

    if (id == NULL || id[0] == '\0' || input == NULL ||
        outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < UmiDeveloperOperationsServiceCount(); ++index) {
        if (strcmp(CATALOGUE[index].id, id) == 0) {
            return CATALOGUE[index].evaluator(input, outSnapshot);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
