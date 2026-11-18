![Umicom](../resources/brand/umicom-logo.svg)

# Plan a dense model cache before allocating it

Framework owns both the C planning API and the native command. Consumers link **Umicom::local_models** and include **umicom/local_models/memory_plan.h**. The optional LLM console reuses the same command implementation.

1. Obtain the selected backend's actual dense key/value geometry.
2. Choose a cache-payload budget in bytes and a requested token upper bound.
3. Call **UmiModelDenseKvPlanTokens**.
4. Check the returned status before reading output. Invalid geometry and arithmetic overflow preserve the output object.
5. Read **affordable_tokens** and **request_fits**. Zero affordable tokens is a valid answer when one token cannot fit.
6. Assess weights, temporary workspace, runtime and host reserve separately. A cache plan is not launch admission.

For two sequences, three layers, two key/value heads, four dimensions and two-byte elements, the payload is 192 bytes per token. A 1,919-byte budget fits nine of ten requested tokens and leaves 191 bytes unused.

~~~powershell
umicom models cache-plan 1919 10 2 3 2 2 4 4 2 2
umicom-models cache-plan --help
~~~

The numeric arguments are BYTES, TOKENS, SEQUENCES, LAYERS, KEY_HEADS, VALUE_HEADS, KEY_DIM, VALUE_DIM, KEY_BYTES and VALUE_BYTES. They accept unsigned decimal text up to uint64 capacity, with at most twenty digits. Element widths are one, two, four or eight bytes.

The report uses fixed field names and includes its payload-only limitation. Exit zero means the report was delivered, including when no tokens fit. Invalid input/geometry returns two; arithmetic or output failure returns one. This command opens no model files, probes no hardware, reserves no memory and starts no process.

## SDK boundary

**UmiModelDenseKvCommand** in **memory_command.h** takes the ten arguments without the command name and caller-owned output/diagnostic streams. A sole **--help** prints usage. Streams must be live, writable and distinct. Framework retains neither stream and checks delivery errors.

The API is limited to dense layouts. Paged, quantized, sliding-window, recurrent and hybrid caches require their actual layout contracts. GPU memory admission is separate. Shared GPU RAM must not be counted twice as both host capacity and extra device capacity.

## Qualification

After configuring Framework with testing enabled, run commands separately and stop on errors:

~~~powershell
cmake --build build/windows-ucrt64-debug --target umicom-model-memory-plan-test umicom-model-kv-token-plan-test umicom-model-memory-command-test --parallel 2
ctest --test-dir build/windows-ucrt64-debug -R "^framework.local_models.memory_plan." --output-on-failure --no-tests=error
~~~

The regressions use supplied geometry and synthetic observations. They do not establish real model fit, GPU behavior or crash resolution.

## Plan cache capacity after fixed host costs

Use **UmiModelDenseKvPlanHostMemory** when the caller already has the non-cache costs and a current desktop memory observation. It composes the existing assessment and dense-cache arithmetic; it does not capture hardware, load a model or reserve memory.

1. Declare resident weights, temporary workspace and runtime overhead in **UmiModelMemoryRequirements**. These are backend-specific estimates. A file's size alone is not a resident or peak-memory estimate.
2. Mark each known component with its mask bit. A known zero has a bit; an unknown cost has neither a bit nor a nonzero value. Leave **cache_bytes** zero and the **UMI_MODEL_MEMORY_CACHE** bit absent. The composed planner calculates cache cost, so a caller-supplied cache would count it twice and is refused.
3. Supply the actual dense key/value geometry and requested token upper bound. Include concurrent sequence capacity. Obtain the snapshot through the existing desktop-system owner and track its elapsed age with one consistent monotonic clock.
4. Pass a nonzero maximum age and a host reserve. The reserve protects room for other work in this arithmetic estimate; it is not an operating-system reservation.
5. Check the returned **UmiStatus**. An invalid argument or overflow leaves the complete output unchanged.
6. Inspect **cache_plan_available**. If false, read **fixed_memory.verdict**: incomplete, unavailable, fixture, stale or already insufficient evidence cannot support a cache budget. Do not read the empty selected-cache result as a real zero-token estimate.
7. If a plan is available, inspect **cache.affordable_tokens**, **cache.request_fits** and **selected_memory**. A valid arithmetic plan can still select zero tokens. A successful return is never permission to launch a model.

For a synthetic example, suppose available host RAM is 8 GiB, resident weights cost 6 GiB, workspace costs 512 MiB, runtime costs 256 MiB and the reserve is 512 MiB. Here MiB means 1,048,576 bytes and GiB means 1,073,741,824 bytes. The fixed costs and reserve leave 768 MiB. A dense cache with two sequences, 32 layers, eight key and value heads, 128 dimensions and two-byte elements costs 262,144 bytes per token. That remainder fits 3,072 tokens, below a request for 4,096.

The planner validates geometry even if host information is unavailable. It spends only the fixed assessment's remainder, then reassesses the selected cache using the same reserve once. A huge requested capacity can be reduced without first multiplying the full request into an overflowing size. These properties keep the ownership and arithmetic policy in Framework rather than repeated in each application.

This service is a C API; the **cache-plan** command above still accepts an explicitly chosen cache-payload budget. It does not automatically call the host-aware service or change a saved model configuration. The teaching exercise in Bits2Banking's **labs/framework/host-cache-budget** calls the composed API using synthetic observations, including stale, fixture and missing-cost cases.

To qualify the additional contract in a configured Framework checkout, build the **umicom-model-host-kv-plan-test** target and run the **framework.local_models.host_kv_plan.** tests. Use the existing focused memory tests as well. These checks cannot establish actual peak memory, model quality or GPU admission. Recheck current host evidence before any later launch decision; the host can change immediately after planning.
