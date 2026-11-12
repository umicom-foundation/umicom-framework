# Run local text models with Umicom

![Umicom](../resources/brand/umicom-logo.svg)

Umicom Local Models is a native C console application and Framework service for reviewing, downloading and running local GGUF text models. Use it directly as **umicom-models**, or through **umicom models**. Both commands call the same implementation. A separately installed, trusted **llama-server** performs inference. Python, Streamlit and Conda are not dependencies of this manager.

This version supports public Hugging Face GGUF downloads, CPU inference and the existing loopback chat provider. It does not install an inference engine, create an Umicom-trained model, generate images or videos, enable agent tools, or configure every product automatically. GPU offload, gated downloads, resumable transfers and a graphical model catalogue need separate implementations.

## Prepare the engine and storage

1. Use a trusted llama.cpp server build that supports the flags printed by the run preview, especially **--offline** and **--no-webui**. Review its licence and build identity. Keep the executable in its own trusted folder. Do not silently update a project's pinned third-party checkout.
2. Create a model folder you control. The following is an example; substitute your own absolute path. Keep downloads and model execution outside source repositories.
3. Inspect the catalogue and the compiled capabilities before downloading anything.

~~~powershell
New-Item -ItemType Directory -Path 'C:\Models\Umicom' -Force
umicom models --help
umicom models catalogue
~~~

The built-in entry **qwen2.5-0.5b-q4** identifies Qwen2.5 0.5B Instruct Q4_K_M at repository commit **9217f5db79a29953eb74d5343926648285ec7e67**, with **491400032 bytes** and SHA-256 **74a4da8c9fdbcd15bd1f6d01d621410d31c6fc00986f5eb687824e7b93d7a9db**. Its Apache-2.0 model licence is separate from Framework's MIT licence. These values come from the [publisher's model metadata](https://huggingface.co/api/models/Qwen/Qwen2.5-0.5B-Instruct-GGUF?blobs=true); read the [pinned model card and licence](https://huggingface.co/Qwen/Qwen2.5-0.5B-Instruct-GGUF/tree/9217f5db79a29953eb74d5343926648285ec7e67) before use. Disk size does not estimate total runtime memory: context storage, tensors and engine overhead require additional RAM.

## Review and download

Run the first command to print the exact URL, destination, expected size and digest. It performs no network request or file creation. After reviewing those values, the second command downloads the model.

~~~powershell
umicom models download qwen2.5-0.5b-q4 'C:\Models\Umicom\qwen2.5-0.5b-q4.gguf'
umicom models download qwen2.5-0.5b-q4 'C:\Models\Umicom\qwen2.5-0.5b-q4.gguf' --apply
~~~

The downloader uses HTTPS with certificate verification. Hugging Face can redirect large files to an HTTPS storage host; at most five redirects are followed. No credential file or proxy is discovered automatically. Network access is part of downloading even though subsequent inference uses a local server. A corporate proxy or private model repository is outside this first transport contract.

The destination must be new, under existing trusted directories. A failed or cancelled download can leave partial bytes at that path. The manager never overwrites or deletes that file. Inspect it and choose a different new destination, or remove the rejected file yourself after review. There is no installed-model database: file existence is not a verified installation. Success requires HTTP 200, the exact size, a basic GGUF header check and the expected SHA-256 of the stored bytes.

For another public model, obtain a complete commit identifier, the actual file's SHA-256 and its byte count from the publisher. A Git blob identifier is not the large model file's SHA-256. Use lowercase hexadecimal for both the full commit and SHA-256. The explicit form is:

~~~text
umicom models download-file OWNER/REPOSITORY COMMIT40 FILENAME.gguf SHA256 BYTES NEW_ABSOLUTE_GGUF [--apply]
~~~

Only ordinary repository paths and filenames are admitted. Branch names, tokens in URLs, traversal segments and non-GGUF artifacts are rejected. This version handles one GGUF file per plan; multi-file split models and separate projector files are not managed as an atomic collection.

## Inspect a local file

~~~powershell
umicom models inspect 'C:\Models\Umicom\qwen2.5-0.5b-q4.gguf' '74a4da8c9fdbcd15bd1f6d01d621410d31c6fc00986f5eb687824e7b93d7a9db'
~~~

Inspection streams through the entire file with a 64 KiB buffer. It checks the digest, magic bytes, a little-endian GGUF version 2 or 3 header, and basic count bounds. It does not parse every tensor, certify the model's behaviour, or prove that a particular engine can load it. The selected final file cannot be a symbolic link, reparse point, directory or device; parent directories must remain trusted and stable.

## Start and stop a CPU server

The example engine path below is a placeholder for your reviewed installation. The last three values are port, context capacity and CPU threads. Start with a modest context and thread count for a small model; adjust only after measuring your machine.

~~~powershell
umicom models run qwen2.5-0.5b-q4 'C:\Tools\llama.cpp\llama-server.exe' 'C:\Models\Umicom\qwen2.5-0.5b-q4.gguf' 8080 2048 4
umicom models run qwen2.5-0.5b-q4 'C:\Tools\llama.cpp\llama-server.exe' 'C:\Models\Umicom\qwen2.5-0.5b-q4.gguf' 8080 2048 4 --apply
~~~

The preview lists separate arguments; it does not inspect files or launch a process. Apply rehashes the model, acquires a cooperative guard file and submits the child process. The fixed plan chooses **127.0.0.1**, zero GPU layers, one inference slot, a stable model alias, offline mode and no engine web UI. See the [llama.cpp server interface](https://github.com/ggml-org/llama.cpp/blob/master/tools/server/README.md) for engine flag semantics. An unsupported flag must cause a visible engine failure; the manager does not silently weaken the plan.

Keep this console open. Type **status** and press Enter to read the process state and bounded diagnostic output. **Running means a process is running; it does not mean model loading or HTTP readiness succeeded.** A busy port or insufficient memory may cause the engine to exit. Type **stop** to stop and join the owned process tree. End-of-input also stops the session. Avoid killing the manager abruptly: normal cleanup is the supported portable shutdown path, and POSIX descendants can escape process groups deliberately. This is supervision, not a security sandbox.

The guard file ends in **.umicom-run.lock** and remains after release. Its presence is not evidence of a running server. Do not delete or replace a guard while another manager uses it. All cooperating launchers must use the same trusted model path; path aliases are not a global resource scheduler.

Custom inspected local files can use:

~~~text
umicom models serve ABSOLUTE_LLAMA_SERVER ABSOLUTE_GGUF SHA256 ALIAS PORT CONTEXT THREADS [--apply]
~~~

Limits are ports 1024–65535, contexts 512–32768 and threads 1–256. The model and engine impose their own tighter limits. Known inherited llama agent, tunnel, routing and alternate-model settings are refused. Ordinary process environment is still inherited; select a reviewed engine environment. The server is not isolated from other local users or processes.

## Send a reviewed chat request

Open a second terminal after the engine has loaded. The first command previews the request. The second sends it to the selected loopback endpoint.

~~~powershell
umicom models chat 8080 qwen2.5-0.5b-q4 'Explain a C pointer in two sentences.'
umicom models chat 8080 qwen2.5-0.5b-q4 'Explain a C pointer in two sentences.' --send-reviewed-prompt
~~~

The client uses Framework's existing validated JSON provider and AI runtime. It requests at most 256 output tokens, disables tools, has a 60-second timeout and does not fall back to a cloud provider. It sends one user message; it does not persist a conversation. A successful response is evidence for that engine, model, prompt and machine, not a general quality or safety guarantee. The endpoint is ordinary loopback without authentication: use only a server you trust.

## Connect Umicom applications

The shared GTK AI workspace already uses **UMICOM_AI_WORKSPACE_PORT**, defaulting to 8080. Set the environment before launching a product that includes that workspace:

~~~powershell
$env:UMICOM_AI_WORKSPACE_PORT = '8080'
~~~

Open its chosen AI library, select the local provider and enter the exact model alias printed by the manager. Review the request before execution. The extractive preview option is a separate non-neural provider. A configured endpoint is not a live capability test. Existing workspace requests have their own limits and a 30-second timeout, so a slow CPU model may need further timeout configuration work even when the console's longer request succeeds.

Studio, Trader, Media, LLM, RAG and other AI clients can link **Umicom::local_models**. The common lifecycle is:

1. Create and show a download or run plan without side effects.
2. On an explicit user action, call the blocking download/start operation on a worker.
3. Retain the session owner and poll **UmiModelSessionRead**. Do not label submission as ready.
4. Create a provider with **UmiModelLocalProviderCreate** and register it in the application's existing AI runtime. Registration transfers ownership only on success.
5. Stop and join active requests, destroy their provider/runtime, then call **UmiModelSessionStop**. Keep a non-null owner for retry if stopping fails.

A library being linkable does not add download buttons to every product. This delivery provides the shared service and console application; dedicated product model-management screens and per-product GUI qualification remain separate work. RAG retrieval can supply reviewed text to this chat provider, but this manager does not claim an embeddings endpoint. Media can use the text response to draft prompts; image/video rendering requires its own provider and watermark workflow.

## Build and qualify from source

Use an existing supported Framework toolchain. The following Windows example builds the native manager, unified CLI and regression executables. Stop when a command fails. These are instructions for the operator to execute, not a record of passing results.

~~~powershell
Set-Location 'C:\umicom\Umicom-Applications\framework'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake --preset windows-ucrt64-debug -DUMICOM_MODEL_DOWNLOAD_HTTP=ON -DUMICOM_AI_WORKSPACE_LOCAL_HTTP=ON
cmake --build --preset windows-ucrt64-debug --target umicom-source-contracts umicom umicom-models umicom-model-plan-test umicom-model-inspect-test umicom-model-stream-test umicom-model-session-test
ctest --test-dir build/windows-ucrt64-debug -R '^framework\.local_models\.' --output-on-failure
.\build\windows-ucrt64-debug\bin\umicom-source-contracts.exe headers --root include/umicom/local_models
.\build\windows-ucrt64-debug\bin\umicom-source-contracts.exe comments --root src/local_models --root tests/local_models --root examples/local_models
.\build\windows-ucrt64-debug\bin\umicom.exe branding audit --assets resources/brand --root docs/learning
.\build\windows-ucrt64-debug\bin\umicom-models.exe --help
~~~

**ON** requires pkg-config and libcurl 7.85 or newer; local chat also requires json-c 0.15 or newer. **AUTO** keeps missing transport unavailable, and **OFF** deliberately disables it. Model inspection and the server lifecycle still compile without download HTTP. No fabricated responses replace a missing client. Read the help capability line to see what your build contains.

The regression suite uses synthetic headers and a finite process fixture, with no real model downloads or inference. FIFO/link cases can be skipped on platforms lacking the required filesystem capability; retain the reason. Also qualify the HTTP-OFF build, installed SDK consumer linkage, the real publisher download, an existing-file refusal, bad digest, busy port, engine load failure, a reviewed chat response and normal stop. Record the source revisions, engine identity, model digest and actual outcomes separately.

## Check an installed SDK consumer

Install to a separate reviewed prefix, then build the small consumer from the installed library exports. The consumer reads the catalogue and reports compiled capabilities; it does not download or run a model. Use the same architecture and toolchain as the producer.

~~~powershell
cmake --build --preset windows-ucrt64-debug --target umicom-sdk-libraries
cmake --install build/windows-ucrt64-debug --prefix 'C:\umicom\_local\sdk\local-model-review' --component FrameworkSDK
cmake -S examples/local_models/sdk_consumer -B 'C:\umicom\_local\build\model-sdk-consumer' -G Ninja -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_PREFIX_PATH='C:/umicom/_local/sdk/local-model-review;C:/msys64/ucrt64'
cmake --build 'C:\umicom\_local\build\model-sdk-consumer'
ctest --test-dir 'C:\umicom\_local\build\model-sdk-consumer' --output-on-failure
~~~

Do not use a source-tree include directory to conceal a missing installed header or dependency. A successful source build alone does not prove that consumers can import **Umicom::local_models**.

## Recover from a refusal

| Result | Meaning and next step |
| --- | --- |
| Invalid argument | Check absolute paths, full commit/digest, filename and numeric bounds. The preview should work before apply. |
| Unavailable | Inspect compiled HTTP capabilities and dependencies. Do not substitute a fake inference response. |
| Already exists | The destination was not overwritten. Review the existing file and choose a new destination. |
| Busy | A session owner or guard is already active. Stop the owning session normally; do not delete a lock file to bypass it. |
| Invalid state | A digest may differ, or an inherited engine setting needs review. Compare the complete file identity and environment; do not weaken the expected hash. |
| I/O error or timeout | Check free disk space, certificates, connectivity and retained partial files. Engine diagnostics may identify memory, flags or port failures. |
| Chat fails while process runs | Wait for load completion, verify the alias and endpoint, and inspect engine diagnostics. Running alone is not readiness. |

For the ownership and C implementation concepts, continue with [the local model lifecycle lessons](learning/local-model-lifecycle.html).

Sammy Hegab · Umicom Foundation · Source examples: MIT.
