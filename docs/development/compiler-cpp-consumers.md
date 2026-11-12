![Umicom](../../resources/brand/umicom-logo.svg)

# Call the compiler service from C and C++

C and C++ can share a C interface when both sides agree on the binary contract. A public C function declaration needs C language linkage when read by a C++ compiler. Without it, a C++ client can request a different external symbol even though the source function name looks identical.

## Include the public header directly

~~~cpp
#include "umicom/compiler/language.h"
#include <cstdio>

int main()
{
    std::puts(umi_compiler_language_name(UMI_COMPILER_LANGUAGE_C));
    return 0;
}
~~~

The language header now establishes its own C linkage. The same rule applies to the affected compiler records, provider factories, registries, planning, manifests and toolchain bridge. Includes remain outside each linkage block, and the closing brace remains inside the header guard. This supports direct and repeated inclusion without placing unrelated C++ declarations inside an external C block.

The guarded declarations keep their existing signatures and data layouts. When a C compiler reads them, the C++ linkage block is absent. Existing C implementations and symbol names stay unchanged. Existing callers that already wrap these C headers can continue using their wrapper, although direct inclusion is clearer.

## Keep implementation and consumer requirements separate

Framework remains C23. A C++ consumer is justified when the application or adapter already requires that language; it should call the same shared services instead of duplicating them. The example uses C-style storage ownership and explicit status checks. Language linkage alone does not add exception handling, convert ownership, stabilize incompatible layouts or validate a compiler backend.

Do not allow C++ exceptions to cross a C callback boundary. Keep the ABI, architecture and runtime requirements of the selected SDK explicit. A matching function name cannot repair an incompatible binary.

## Qualify the actual boundary

Use the [installed consumer example](../../examples/compiler_cpp_consumer/README.md) with a matching SDK. It includes each of the 32 corrected headers independently and retains a representative symbol reference for linking. Its additional workflow consumes the umbrella header and exercises request planning, borrowed argument views and manifest language projection.

The opt-in **UMICOM_BUILD_CPP_COMPILER_CONSUMER_TESTS** option enables the in-tree version when **BUILD_TESTING** is on. Ordinary C-only builds retain their existing compiler requirements. Record in-tree and installed-SDK results separately.

The authored matrix covers one linked representative per header and a selected workflow, not every exported function or platform. Static inspection confirms all 122 affected declarations are inside the added guards and their original declaration text is preserved. Compiler, linker and runtime results remain unverified until the checks are executed.

For the concepts behind the example, follow the existing [installed-client qualification lesson](../../../../Umicom-Bits2Banking/content/courses/learning-build-systems/lessons/24-installed-toolchain-lab-qualification.html) when using the ecosystem learning checkout.
