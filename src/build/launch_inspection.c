/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/launch_inspection.c
 * PURPOSE: Keep launch-file inspection reusable and independent of GTK and Studio.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/launch_inspection.h"
#include "umicom/build/launch_plan.h"
#include <stdlib.h>
#include <string.h>

/* Keep path observations separate from executable approval. A regular file can
 * still have the wrong architecture, missing libraries or no execute access. */
static UmiStatus Inspect(UmiBuildLaunchFile *entry, const char *path, UmiFileKind expected,
                         bool inspect, const UmiCancellationToken *cancel) {
  if (umi_cancellation_token_is_requested(cancel))
    return UMI_STATUS_CANCELLED;
  memcpy(entry->path, path, strlen(path) + 1U);
  entry->status = UMI_STATUS_UNAVAILABLE;
  if (inspect) {
    UmiFileInfo info;
    entry->inspected = true;
    entry->status = umi_directory_stat(path, &info);
    if (entry->status == UMI_STATUS_OK) {
      entry->kind = info.kind;
      entry->size = info.size;
      entry->expected_kind = info.kind == expected;
    }
  }
  return umi_cancellation_token_is_requested(cancel) ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}
UmiStatus UmiBuildLaunchInspect(const UmiBuildProfile *profile,
                                const char *absoluteProjectDirectory,
                                const UmiCancellationToken *cancel, UmiBuildLaunchInspection *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (umi_cancellation_token_is_requested(cancel))
    return UMI_STATUS_CANCELLED;
  /* A launch plan owns a full argument array. Heap storage avoids making UI
   * worker stacks depend on the maximum argument count supported by Build. */
  UmiBuildLaunchPlan *plan = calloc(1U, sizeof(*plan));
  if (plan == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiBuildLaunchInspection result = {0};
  UmiStatus status = UmiBuildLaunchPlanCreate(profile, absoluteProjectDirectory, plan);
  if (status == UMI_STATUS_OK) {
    result.run_uses_program_lookup = plan->run_uses_program_lookup;
    status = Inspect(&result.run_program, plan->run_program, UMI_FILE_KIND_REGULAR,
                     !plan->run_uses_program_lookup, cancel);
  }
  if (status == UMI_STATUS_OK)
    status =
        Inspect(&result.debug_program, plan->debug_program, UMI_FILE_KIND_REGULAR, true, cancel);
  if (status == UMI_STATUS_OK)
    status = Inspect(&result.working_directory, plan->working_directory, UMI_FILE_KIND_DIRECTORY,
                     true, cancel);
  free(plan);
  /* Publish one complete set only. A late cancellation cannot leave a
   * caller holding a mixture of old and newly inspected path facts. */
  if (status == UMI_STATUS_OK)
    *out = result;
  return status;
}
