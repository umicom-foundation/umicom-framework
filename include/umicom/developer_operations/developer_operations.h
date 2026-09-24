/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/developer_operations.h
 *
 * PURPOSE:
 *   Aggregate the complete developer operations workbench projection API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_H
#define UMICOM_DEVELOPER_OPERATIONS_H

#include "umicom/developer_operations/types.h"
#include "umicom/developer_operations/service.h"

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

#endif
