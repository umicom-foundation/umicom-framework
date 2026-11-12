![Umicom](../../resources/brand/umicom-logo.svg)

# Call shared services through their C interface

An application that needs C++ should still use Framework's shared C services. This keeps storage admission, integration policy, desktop state, finance records and operational evidence under their existing owners.

The service headers establish C linkage for their external declarations when read by a C++ compiler. Includes remain outside the linkage block. Ordinary C23 compilation retains the same types, signatures and C implementations.

## Include the contract you use

~~~cpp
#include "umicom/data/query_policy.h"
~~~

Include the specific public header rather than depending on an unrelated umbrella header to establish its declarations. Do not surround arbitrary C++ headers with an external C block.

The [independent consumer matrix](../../examples/service_cpp_consumer/README.md) covers the corrected service headers. It deliberately includes each header before any other Framework API. A C++ standard header follows it, which also helps detect a linkage block that was not closed.

## Separate a declaration from permission to act

A successful link makes a function callable. It does not authorize a database transaction, remote request, secret access or application launch. Continue using the shared service's admission, ownership and policy contracts.

The matrix only takes function addresses. It does not call those operations or treat a symbol check as functional qualification. Use each service's existing tests and an appropriate application workflow to qualify its behavior.

## Keep callbacks and records compatible

Pass the C records required by the public interface. Keep their owning storage alive for the documented lifetime. A C++ object with constructors, virtual methods or library-specific container fields is not a replacement for a C record.

If a callback is implemented in C++, contain exceptions inside that adapter and translate failure into the C contract's status. Keep synchronization and cancellation ownership explicit. Language linkage does not add either one.

## Choose the actual exported component

Most groups map directly to **Umicom::data**, **Umicom::integration** and the corresponding exported service target. Studio live-preview profiles belong to **Umicom::developer**. The consumer matrix records this mapping instead of inventing another runtime library.

The matrix supports an explicit subset when an installed SDK contains only selected components. It refuses unknown group names and missing targets. Report the groups actually qualified; a successful subset does not establish the omitted services.

Use the [UI consumer examples](../../examples/ui_cpp_consumer/README.md) for semantic memory workflows and the [compiler consumer guide](compiler-cpp-consumers.md) for shared build planning. Native UI, device, database and network workflows need their own execution evidence.

## Optional broker headers

Two additional groups cover the news-provider snapshot and GTK Gateway presentation headers. They require the corresponding exported SDK components; a minimal toolkit-neutral SDK need not contain the GTK component. The default nine-group selection remains 101 checks.

~~~powershell
cmake -S examples/service_cpp_consumer -B build/service-cpp-sdk-broker -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/sdk/umicom -DBUILD_TESTING=ON "-DUMICOM_CPP_SERVICE_COMPONENTS=ibkr_connection;ibkr_connection_gtk4"
cmake --build build/service-cpp-sdk-broker --target umicom-service-sdk-cpp-checks
ctest --test-dir build/service-cpp-sdk-broker -R "^framework[.]sdk[.]cpp_service[.]broker_connectivity[.]" --output-on-failure
~~~

This selection registers two isolated header/link checks. They retain a symbol reference but do not call it: no socket, broker process, news subscription or GTK widget is created. Header compatibility cannot establish provider access, installed Gateway behavior or a functioning native interface. Qualify those workflows separately through their existing contracts.
