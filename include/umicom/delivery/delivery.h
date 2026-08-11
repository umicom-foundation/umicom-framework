/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/delivery.h
 *
 * PURPOSE:
 *   Provide one aggregate public include for the complete application-delivery platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Applications may include this umbrella header when they use several delivery contracts together.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_DELIVERY_H
#define INCLUDE_UMICOM_DELIVERY_DELIVERY_H

#include "umicom/delivery/types.h"
#include "umicom/delivery/artifact.h"
#include "umicom/delivery/artifact_set.h"
#include "umicom/delivery/manifest.h"
#include "umicom/delivery/build_evidence.h"
#include "umicom/delivery/stage.h"
#include "umicom/delivery/package.h"
#include "umicom/delivery/package_provider.h"
#include "umicom/delivery/directory_package.h"
#include "umicom/delivery/checksum.h"
#include "umicom/delivery/integrity.h"
#include "umicom/delivery/signature.h"
#include "umicom/delivery/signing.h"
#include "umicom/delivery/sbom.h"
#include "umicom/delivery/license_inventory.h"
#include "umicom/delivery/provenance.h"
#include "umicom/delivery/release.h"
#include "umicom/delivery/release_channel.h"
#include "umicom/delivery/release_registry.h"
#include "umicom/delivery/publish.h"
#include "umicom/delivery/update.h"
#include "umicom/delivery/update_feed.h"
#include "umicom/delivery/update_plan.h"
#include "umicom/delivery/rollback.h"
#include "umicom/delivery/generation.h"
#include "umicom/delivery/install_plan.h"
#include "umicom/delivery/install_state.h"
#include "umicom/delivery/runtime_bundle.h"
#include "umicom/delivery/dependency_scan.h"
#include "umicom/delivery/smoke_test.h"
#include "umicom/delivery/gate.h"
#include "umicom/delivery/pipeline.h"
#include "umicom/delivery/pipeline_state.h"
#include "umicom/delivery/retention.h"
#include "umicom/delivery/cleanup.h"
#include "umicom/delivery/report.h"
#include "umicom/delivery/policy.h"
#include "umicom/delivery/inventory.h"
#include "umicom/delivery/environment.h"
#include "umicom/delivery/deployment.h"
#include "umicom/delivery/health.h"
#include "umicom/delivery/compatibility.h"
#include "umicom/delivery/metadata.h"
#include "umicom/delivery/store.h"
#include "umicom/delivery/audit.h"
#include "umicom/delivery/channel_registry.h"
#include "umicom/delivery/rollback_history.h"
#include "umicom/delivery/verification.h"
#include "umicom/delivery/release_note.h"
#include "umicom/delivery/platform_matrix.h"
#include "umicom/delivery/artifact_index.h"
#include "umicom/delivery/release_candidate.h"
#include "umicom/delivery/promotion.h"
#include "umicom/delivery/release_lock.h"

#endif
