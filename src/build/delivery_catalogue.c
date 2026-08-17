/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/delivery_catalogue.c
 *
 * PURPOSE:
 *   Implement the stable command and view catalogue for every frontend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/delivery_catalogue.h"

#include <string.h>

#define COMMAND(id, text, category, contract, workspace, trust, cancellable) \
    { sizeof(UmiBuildDeliveryCommandDescriptor),                          \
      UMI_BUILD_DELIVERY_CATALOGUE_API_VERSION, id, text, category,       \
      contract, workspace, trust, cancellable }

static const UmiBuildDeliveryCommandDescriptor COMMANDS[] = {
    COMMAND("build.graph.open", "Open Build Graph", "Build Graph",
            "umicom.build.incremental", 1, 0, 0),
    COMMAND("build.graph.refresh", "Refresh Build Graph", "Build Graph",
            "umicom.build.incremental", 1, 0, 0),
    COMMAND("build.graph.focus-dirty", "Focus Dirty Nodes", "Build Graph",
            "umicom.build.incremental", 1, 0, 0),
    COMMAND("build.graph.force-all", "Force Full Rebuild", "Build Graph",
            "umicom.build.incremental", 1, 0, 0),
    COMMAND("build.configure", "Configure", "Build",
            "umicom.build.graph", 1, 0, 1),
    COMMAND("build.workspace", "Build Workspace", "Build",
            "umicom.build.graph", 1, 0, 1),
    COMMAND("build.selected", "Build Selected Target", "Build",
            "umicom.build.graph", 1, 0, 1),
    COMMAND("build.rebuild", "Rebuild", "Build",
            "umicom.build.graph", 1, 0, 1),
    COMMAND("build.clean", "Clean", "Build",
            "umicom.build.graph", 1, 0, 1),
    COMMAND("build.cancel", "Cancel Build", "Build",
            "umicom.build.graph", 1, 0, 1),
    COMMAND("task.open", "Open Tasks", "Tasks",
            "umicom.build.task", 1, 0, 0),
    COMMAND("task.run", "Run Task", "Tasks",
            "umicom.build.task-orchestrator", 1, 0, 1),
    COMMAND("task.run-default", "Run Default Task", "Tasks",
            "umicom.build.task-orchestrator", 1, 0, 1),
    COMMAND("task.rerun", "Rerun Last Task", "Tasks",
            "umicom.build.task-orchestrator", 1, 0, 1),
    COMMAND("task.retry-failed", "Retry Failed Task", "Tasks",
            "umicom.build.task-orchestrator", 1, 0, 1),
    COMMAND("task.stop", "Stop Tasks", "Tasks",
            "umicom.build.task-orchestrator", 1, 0, 1),
    COMMAND("artifact.open", "Open Artifacts", "Artifacts",
            "umicom.build.artifact-manifest", 1, 0, 0),
    COMMAND("artifact.reveal", "Reveal Artifact", "Artifacts",
            "umicom.build.artifact-manifest", 1, 0, 0),
    COMMAND("artifact.copy-path", "Copy Artifact Path", "Artifacts",
            "umicom.build.artifact-manifest", 1, 0, 0),
    COMMAND("artifact.verify", "Verify Artifact", "Artifacts",
            "umicom.build.artifact-manifest", 1, 1, 1),
    COMMAND("artifact.select-primary", "Select Primary Artifact", "Artifacts",
            "umicom.build.artifact-manifest", 1, 0, 0),
    COMMAND("deployment.targets.open", "Open Deployment Targets", "Deployment",
            "umicom.build.deployment-target", 1, 0, 0),
    COMMAND("deployment.plan", "Plan Deployment", "Deployment",
            "umicom.build.deployment-plan", 1, 1, 0),
    COMMAND("deployment.preview", "Preview Deployment", "Deployment",
            "umicom.build.deployment-plan", 1, 0, 0),
    COMMAND("deployment.approve", "Approve Deployment", "Deployment",
            "umicom.build.deployment-plan", 1, 1, 0),
    COMMAND("deployment.run", "Deploy", "Deployment",
            "umicom.build.deployment-controller", 1, 1, 1),
    COMMAND("deployment.cancel", "Cancel Deployment", "Deployment",
            "umicom.build.deployment-controller", 1, 1, 1),
    COMMAND("deployment.rollback", "Rollback Deployment", "Deployment",
            "umicom.build.deployment-controller", 1, 1, 1),
    COMMAND("deployment.history.open", "Open Deployment History", "Deployment",
            "umicom.build.deployment-controller", 1, 0, 0),
    COMMAND("pipeline.open", "Open Delivery Pipeline", "Pipeline",
            "umicom.build.pipeline", 1, 0, 0),
    COMMAND("pipeline.run", "Run Delivery Pipeline", "Pipeline",
            "umicom.build.pipeline", 1, 1, 1),
    COMMAND("pipeline.stop", "Stop Delivery Pipeline", "Pipeline",
            "umicom.build.pipeline", 1, 1, 1),
    COMMAND("pipeline.refresh", "Refresh Pipeline", "Pipeline",
            "umicom.build.pipeline", 1, 0, 0),
    COMMAND("pipeline.clear", "Clear Pipeline Evidence", "Pipeline",
            "umicom.build.pipeline", 1, 1, 0),
    COMMAND("pipeline.copy-summary", "Copy Pipeline Summary", "Pipeline",
            "umicom.build.pipeline", 1, 0, 0),
    COMMAND("pipeline.export-evidence", "Export Pipeline Evidence", "Pipeline",
            "umicom.build.pipeline", 1, 0, 1)
};

#undef COMMAND

#define VIEW(id, text, contract, region)                                  \
    { sizeof(UmiBuildDeliveryViewDescriptor),                             \
      UMI_BUILD_DELIVERY_CATALOGUE_API_VERSION, id, text, contract,       \
      region, 1, 1 }

static const UmiBuildDeliveryViewDescriptor VIEWS[] = {
    VIEW("build.graph", "Build Graph", "umicom.build.incremental",
         "editor-area"),
    VIEW("build.tasks", "Tasks", "umicom.build.task-orchestrator",
         "left-sidebar"),
    VIEW("build.progress", "Build Progress", "umicom.build.graph",
         "bottom-panel"),
    VIEW("build.artifacts", "Artifacts", "umicom.build.artifact-manifest",
         "bottom-panel"),
    VIEW("deployment.targets", "Deployment Targets",
         "umicom.build.deployment-target", "secondary-sidebar"),
    VIEW("deployment.plan", "Deployment Plan",
         "umicom.build.deployment-plan", "editor-area"),
    VIEW("deployment.history", "Deployment History",
         "umicom.build.deployment-controller", "bottom-panel"),
    VIEW("build.delivery-pipeline", "Delivery Pipeline",
         "umicom.build.pipeline", "bottom-panel")
};

#undef VIEW

size_t umi_build_delivery_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiBuildDeliveryCommandDescriptor *umi_build_delivery_command_at(
    size_t position)
{
    if (position >= umi_build_delivery_command_count()) return NULL;
    return &COMMANDS[position];
}

const UmiBuildDeliveryCommandDescriptor *umi_build_delivery_command_find(
    const char *command_id)
{
    size_t index;
    if (command_id == NULL) return NULL;
    for (index = 0U; index < umi_build_delivery_command_count(); ++index) {
        if (strcmp(COMMANDS[index].command_id, command_id) == 0) {
            return &COMMANDS[index];
        }
    }
    return NULL;
}

size_t umi_build_delivery_view_count(void)
{
    return sizeof(VIEWS) / sizeof(VIEWS[0]);
}

const UmiBuildDeliveryViewDescriptor *umi_build_delivery_view_at(
    size_t position)
{
    if (position >= umi_build_delivery_view_count()) return NULL;
    return &VIEWS[position];
}

const UmiBuildDeliveryViewDescriptor *umi_build_delivery_view_find(
    const char *view_id)
{
    size_t index;
    if (view_id == NULL) return NULL;
    for (index = 0U; index < umi_build_delivery_view_count(); ++index) {
        if (strcmp(VIEWS[index].view_id, view_id) == 0) return &VIEWS[index];
    }
    return NULL;
}
