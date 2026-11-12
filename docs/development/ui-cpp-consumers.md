![Umicom](../../resources/brand/umicom-logo.svg)

# Share the UI engine with C++ clients

Umicom's component model, enterprise data views and workspace state belong to the C23 Framework. A client that requires C++ can consume those same services. The public header establishes C language linkage, so the client requests the C symbol implemented by the library.

## Read the ownership contract first

A language boundary does not transfer ownership automatically. A pointer may be borrowed for one call, copied into a record or retained by a service. Read the function's contract and keep the owning object alive for the required period.

For example, the component registry copies a specification:

~~~cpp
#include "umicom/ui/components/registry.h"

UmiStatus register_save_button(UmiUiComponentRegistry *registry)
{
    UmiUiComponentSpec button =
        umi_ui_component_spec_default(UMI_UI_COMPONENT_BUTTON);
    UmiStatus status = umi_ui_component_spec_set_id(&button, "save-record");
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_component_spec_set_text(&button, "Save record");
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_component_registry_add(registry, &button);
}
~~~

Initialize the registry before calling this function and give it one clear owner. The local button value can disappear after a successful add because the registry stores a copy. Large registries should use application-owned storage rather than repeatedly consuming a thread's stack.

## Keep the binary contract explicit

The affected component, enterprise, workspace and GTK adapter headers now wrap their own public declarations with conditional C linkage. Their includes stay outside the block, and the closing brace stays within the header guard. All previous declarations, types and comments remain in place.

C consumers see the same declarations. C++ consumers do not need an application-specific wrapper. The library stays compiled as C23. Compatible architecture, calling conventions and runtime dependencies are still required; linkage alone cannot prove full ABI compatibility.

Keep C++ exceptions inside C++ code. If a callback can throw, its C++ adapter must catch and translate the result before returning through a C interface. Do not pass a C++ container or object layout where the API requires a C record.

## Follow the examples

The [installed consumer example](../../examples/ui_cpp_consumer/README.md) provides exact qualification steps and three small workflows:

- [Component ownership](../../tests/sdk_consumers/ui_components.cpp) copies a button into a registry and checks independent input storage.
- [Enterprise values](../../tests/sdk_consumers/ui_enterprise.cpp) checks typed cells and selection ranges passed by value.
- [Workspace state](../../tests/sdk_consumers/ui_workspace.cpp) checks locking, cloning and the canonical brand palette without creating a window.

The isolated header matrix covers each changed UI contract directly. The compiler sees that header before any other Framework header, and the linker must resolve a representative function. This catches a missing linkage declaration that an umbrella include could conceal.

## Diagnose one stage at a time

1. If the header fails to compile, inspect the exact include path and the first diagnostic. A successful umbrella include does not prove direct inclusion.
2. If linking fails, compare the requested symbol, installed library, toolchain and component dependencies.
3. If execution fails to load, check runtime libraries and architecture.
4. If a memory workflow fails, retain its first failed expression and the tested SDK identity.
5. Run separate native UI qualification for rendering, fonts, icon resources, focus and input.

The examples reuse Framework's palette and exported SVG/ICO assets. They do not define a new product layout. Refer to the [compiler consumer guide](compiler-cpp-consumers.md) for the same boundary around build planning.
