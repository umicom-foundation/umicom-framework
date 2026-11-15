![Umicom](../../resources/brand/umicom-logo.svg)

# Own model-output text with an installed SDK

This native C23 example feeds an authored text fixture into Framework's request-owned response stream. It rejects an event from an older model revision, reconstructs a split UTF-8 scalar, finishes the stream and keeps a copied report after destroying the stream. It does not connect to a model, download weights or perform inference.

1. Build and install a Framework SDK containing the `Umicom::ai` component and the new response-stream API.
2. Copy this example directory to your own project folder if you want to work independently of the Framework checkout.
3. Configure it with the absolute SDK installation prefix and the same compatible compiler environment.
4. Build and run the example. Stop on errors.

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUMICOM_SDK_ROOT="C:\SDKs\UmicomFramework"
cmake --build build
& ".\build\umicom-sdk-ai-stream.exe"
```

The SDK prefix above is an example; select your actual installation. The project calls Framework's installed-package and environment guards. It does not fall back to an adjacent Framework clone. The package also owns SVG/ICO resource staging and Windows icon generation.

Expected output describes an **authored text fixture**, prints `Native € text.` and identifies request 1, model revision 7, 16 accepted bytes and complete state 2. A terminal font or encoding setting can affect how the euro sign is displayed; the stream API preserves its three UTF-8 bytes.

The two identity values are local admission tokens assigned by the client. They are not proof that a server loaded a particular model. A real integration must associate them with a verified runtime session, turn provider messages into text events, handle transport errors and stop/join producers before stream destruction.

A stream's Copy operation returns owned text and a matching snapshot. Use `UmiAiResponseStreamTextFree` to release that text. Cancel retains prior text and refuses later events; it does not interrupt a provider or wait for its worker to finish. Do not release a stream while another thread might still use its pointer.

See [request-owned streaming](../../docs/learning/request-owned-ai-streams.html) and [incremental UTF-8](../../docs/learning/streaming-utf8-in-c.html). The example is authored and requires installed-SDK compilation and execution; configuration of Framework alone does not qualify it.


## Build from the installed example

The Framework and FrameworkSDK components include this directory under **share/umicom/framework/examples/sdk_ai_stream**. The adjacent **docs/learning** and **resources/brand** layout preserves the relative lesson links and approved SVG/ICO artwork. Older documentation destinations remain available for compatibility. Other examples linked by a lesson may still require their own installation component.

From a configured Framework checkout, build the SDK libraries and install to a separate prefix. Run each command separately and stop on an error; these commands are a qualification procedure, not a record of completed work.

~~~powershell
Set-Location 'C:\umicom\Umicom-Applications\framework'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake --build --preset windows-ucrt64-debug --target umicom-sdk-libraries
cmake --install build/windows-ucrt64-debug --prefix 'C:/umicom/_local/sdk/stream-review' --component FrameworkSDK
cmake -S 'C:/umicom/_local/sdk/stream-review/share/umicom/framework/examples/sdk_ai_stream' -B 'C:/umicom/_local/build/sdk-ai-stream' -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DUMICOM_SDK_ROOT:PATH='C:/umicom/_local/sdk/stream-review' -DCMAKE_PREFIX_PATH:PATH=C:/msys64/ucrt64
cmake --build 'C:/umicom/_local/build/sdk-ai-stream'
ctest --test-dir 'C:/umicom/_local/build/sdk-ai-stream' --output-on-failure --no-tests=error
~~~

The test **sdk.ai_stream.owned_ranges** runs the same authored-text example. Before releasing the stream, **VerifyRanges** reconstructs the expected observation through windows of at most four payload bytes. A window ends at a complete UTF-8 scalar; the euro sign is never split. The byte cursor advances only after successful reads. At the visible end the API returns an owned empty string, which the example also releases. The original full copy is still printed after its stream is destroyed.

Inspect a failed configuration for the selected prefix and package guards. A missing **UmiAiResponseStreamCopyRange** symbol at link time means the selected libraries do not provide the required API; install matching headers and libraries instead of adding source-tree include paths. Keep the failed build directory and logs when investigating a stale package. Producer configuration cannot establish that this independent consumer compiles or runs.

No real model, thread, HTTP service or GUI is used. An executed passing test qualifies the installed exports for this fixture and toolchain; it does not validate inference, external DLL deployment on another machine or rendered branding. Review those workflows separately.
