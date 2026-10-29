/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning_pack_inspector/main.c
 *
 * PURPOSE:
 *   Show how a native application discovers and verifies an offline learning pack.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_pack.h"
#include "umicom/teacher/learning_navigation.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 5) {
        (void)fprintf(stderr, "Usage: umicom-learning-pack-inspect <absolute-root> <manifest-path> [resource-id [local-link]]\n");
        return 2;
    }
    /* This example is a Framework guide reader. A product-specific reader
     * supplies that product's version instead of these Framework constants. */
    UmiVersion host = {UMICOM_FRAMEWORK_VERSION_MAJOR, UMICOM_FRAMEWORK_VERSION_MINOR,
                       UMICOM_FRAMEWORK_VERSION_PATCH};
    UmiLearningPack *pack = NULL;
    UmiStatus status = UmiLearningPackLoad(argv[1], argv[2], host, &pack);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Could not load learning pack: %s\n", umi_status_text(status));
        return 1;
    }
    UmiLearningPackInfo info;
    status = UmiLearningPackCopyInfo(pack, &info);
    if (status == UMI_STATUS_OK) {
        (void)printf("%s (%s), product %s, locale %s\n", info.title, info.id, info.product, info.locale);
        (void)printf("Resources: %zu\n", info.resource_count);
        for (size_t i = 0U; i < info.resource_count; ++i) {
            UmiLearningPackResource resource;
            status = UmiLearningPackResourceAt(pack, i, &resource);
            if (status != UMI_STATUS_OK) break;
            (void)printf("%s: %s [%s]\n", resource.id, resource.title, resource.topic);
        }
    }
    if (status == UMI_STATUS_OK && argc >= 4) {
        const char *resource_id = argv[3];
        UmiLearningPackLink selected = {0};
        if (argc == 5) {
            /* Resolve metadata first; the subsequent read verifies the exact
             * selected bytes before a renderer would receive them. */
            status = UmiLearningPackResolveLink(pack, argv[3], argv[4], &selected);
            if (status == UMI_STATUS_OK) {
                resource_id = selected.resource.id;
                (void)printf("Selected %s, anchor [%s]\n", resource_id, selected.fragment);
            }
        }
        unsigned char *bytes = NULL;
        size_t size = 0U;
        if (status == UMI_STATUS_OK)
            status = UmiLearningPackReadResource(pack, resource_id, 4U * 1024U * 1024U, &bytes, &size);
        if (status == UMI_STATUS_OK) {
            /* A real reader consumes these exact bytes using its safe document
             * renderer. This inspector only reports verification and size. */
            (void)printf("Verified %s: %zu bytes\n", resource_id, size);
        }
        UmiLearningPackFreeBytes(bytes);
    }
    UmiLearningPackDestroy(pack);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Learning resource inspection failed: %s\n", umi_status_text(status));
        return 1;
    }
    return 0;
}
