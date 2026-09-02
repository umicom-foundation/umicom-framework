/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/qt6/test_component_factory.cpp
 *
 * PURPOSE:
 *   Verify the test component factory behaviour and report a clear failure when
 *   its contract changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/qt6/test_component_factory.cpp
 *
 * PURPOSE:
 *   Verify the public Qt6 adapter descriptor and stable surface contract for qt6.component_factory.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <cstring>
#include "umicom/ui/qt6/component_factory.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main() {
    const UmiQt6SurfaceDescriptor *descriptor = umi_qt6_component_factory_descriptor();
    CHECK(descriptor != nullptr);
    CHECK(umi_qt6_surface_descriptor_validate(descriptor) == UMI_STATUS_OK);
    CHECK(std::strcmp(descriptor->surface_id, "qt6.component_factory") == 0);
    CHECK(descriptor->semantic_contract[0] != '\0');
    CHECK(descriptor->native_class[0] != '\0');
    CHECK((descriptor->capability_flags & UMI_QT6_CAP_ACCESSIBILITY) != 0U);
    return 0;
}
