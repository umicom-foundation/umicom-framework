/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/qt6/test_workstation_dock_overlay.cpp
 *
 * PURPOSE:
 *   Verify the public Qt6 adapter descriptor and stable surface contract for qt6.ws.dock_overlay.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <cstring>
#include "umicom/ui/qt6/workstation/dock_overlay.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main() {
    const UmiQt6SurfaceDescriptor *descriptor = umi_qt6_ws_dock_overlay_descriptor();
    CHECK(descriptor != nullptr);
    CHECK(umi_qt6_surface_descriptor_validate(descriptor) == UMI_STATUS_OK);
    CHECK(std::strcmp(descriptor->surface_id, "qt6.ws.dock_overlay") == 0);
    CHECK(descriptor->semantic_contract[0] != '\0');
    CHECK(descriptor->native_class[0] != '\0');
    CHECK((descriptor->capability_flags & UMI_QT6_CAP_ACCESSIBILITY) != 0U);
    return 0;
}
