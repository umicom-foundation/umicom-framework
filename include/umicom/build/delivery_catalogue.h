/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/delivery_catalogue.h
 *
 * PURPOSE:
 *   Publish the canonical command and view contracts for build graphs, tasks,
 *   artifacts and deployments. Frontends contribute placement only.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_DELIVERY_CATALOGUE_H
#define UMICOM_BUILD_DELIVERY_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_DELIVERY_CATALOGUE_API_VERSION 1U

typedef struct UmiBuildDeliveryCommandDescriptor {
    uint32_t structure_size;
    uint32_t api_version;
    const char *command_id;
    const char *label;
    const char *category;
    const char *framework_contract;
    int requires_workspace;
    int requires_trust;
    int cancellable;
} UmiBuildDeliveryCommandDescriptor;

typedef struct UmiBuildDeliveryViewDescriptor {
    uint32_t structure_size;
    uint32_t api_version;
    const char *view_id;
    const char *title;
    const char *framework_contract;
    const char *preferred_region;
    int closable;
    int movable;
} UmiBuildDeliveryViewDescriptor;

size_t umi_build_delivery_command_count(void);
const UmiBuildDeliveryCommandDescriptor *umi_build_delivery_command_at(
    size_t position);
const UmiBuildDeliveryCommandDescriptor *umi_build_delivery_command_find(
    const char *command_id);
size_t umi_build_delivery_view_count(void);
const UmiBuildDeliveryViewDescriptor *umi_build_delivery_view_at(
    size_t position);
const UmiBuildDeliveryViewDescriptor *umi_build_delivery_view_find(
    const char *view_id);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BUILD_DELIVERY_CATALOGUE_H */
