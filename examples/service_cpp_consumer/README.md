![Umicom](../../resources/brand/umicom-logo.svg)

# Consume shared C services from C++

Use this example when a C++ application needs Framework's existing data, desktop, integration, finance, development or operational services. The reusable implementations remain C23. The application includes the public API and links the installed C library.

This is a header and symbol qualification matrix. It does not open a database, start a server, place an order, activate a desktop or execute a Studio command. Each executable retains a pointer to one representative function without calling that function.

## Select compatible installed components

Use a complete SDK whose headers, libraries and exported targets belong to the same build. Match the C++ toolchain's architecture, calling conventions and runtime requirements to that SDK. Keep this example inside the Framework checkout because it shares its small consumer source with the other SDK checks.

The default selection covers these exported components:

| Component | Independently included headers |
| --- | ---: |
| codeguard | 12 |
| data | 11 |
| desktop | 14 |
| developer | 2 |
| finance | 2 |
| frontend | 11 |
| integration | 29 |
| observability | 8 |
| security | 12 |

The **developer** component owns the two Studio live-preview contracts. There is no separate **Umicom::studio_runtime** library for those headers.

## Configure, build and inspect results

From the Framework source root, replace **C:/sdk/umicom** with the installed SDK directory. Run each command separately and stop on an error.

~~~powershell
cmake -S examples/service_cpp_consumer -B build/service-cpp-sdk -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/sdk/umicom -DBUILD_TESTING=ON
cmake --build build/service-cpp-sdk --target umicom-service-sdk-cpp-checks
ctest --test-dir build/service-cpp-sdk -R "^framework[.]sdk[.]cpp_service[.]" --output-on-failure
~~~

All nine groups register 101 checks. The configure output reports the actual selected groups and count. Missing exported libraries and unknown group names cause an explicit configuration error.

To qualify a smaller installed SDK, use a separate directory and an explicit component list:

~~~powershell
cmake -S examples/service_cpp_consumer -B build/service-cpp-sdk-data -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/sdk/umicom -DBUILD_TESTING=ON "-DUMICOM_CPP_SERVICE_COMPONENTS=data;integration"
cmake --build build/service-cpp-sdk-data --target umicom-service-sdk-cpp-checks
ctest --test-dir build/service-cpp-sdk-data -R "^framework[.]sdk[.]cpp_service[.]" --output-on-failure
~~~

That selection registers 40 checks. It makes no statement about the omitted groups.

## Understand the evidence

A successful compile establishes direct header inclusion for the chosen compiler. Successful linking establishes resolution of one representative C symbol per selected header. The executable's small pointer check confirms that it loaded; it does not exercise the service.

Record installed SDK identity, selected components, compiler versions, architecture and results together. An unresolved C++-encoded name calls for inspecting the exact header and linkage declaration. An unresolved C name calls for checking library identity and dependency closure. Load errors require separate runtime-library investigation.

For source-tree qualification, enable **BUILD_TESTING=ON** and **UMICOM_BUILD_CPP_SERVICE_CONSUMER_TESTS=ON** in the Framework build, select **UMICOM_CPP_SERVICE_COMPONENTS**, and build **umicom-service-cpp-checks**. Use the same CTest pattern. Source-tree results are distinct from installed-SDK evidence.

The [service language-boundary guide](../../docs/development/service-cpp-consumers.md) explains how to keep application code thin.

## Optional broker headers

Two additional groups cover the news-provider snapshot and GTK Gateway presentation headers. They require the corresponding exported SDK components; a minimal toolkit-neutral SDK need not contain the GTK component. The default nine-group selection remains 101 checks.

~~~powershell
cmake -S examples/service_cpp_consumer -B build/service-cpp-sdk-broker -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/sdk/umicom -DBUILD_TESTING=ON "-DUMICOM_CPP_SERVICE_COMPONENTS=ibkr_connection;ibkr_connection_gtk4"
cmake --build build/service-cpp-sdk-broker --target umicom-service-sdk-cpp-checks
ctest --test-dir build/service-cpp-sdk-broker -R "^framework[.]sdk[.]cpp_service[.]broker_connectivity[.]" --output-on-failure
~~~

This selection registers two isolated header/link checks. They retain a symbol reference but do not call it: no socket, broker process, news subscription or GTK widget is created. Header compatibility cannot establish provider access, installed Gateway behavior or a functioning native interface. Qualify those workflows separately through their existing contracts.
