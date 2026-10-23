# Native C23 KV-cache prefix-reuse laboratory

This is an **optional, separately configured** Framework laboratory, based on
`cmake/UmicomNativeLlmTraining.cmake` plus the additive
`cmake/UmicomNativeLlmPrefixReuse.cmake`. It is not a replacement for the
canonical `learning/native-llm` build.

The new public service compares independent `UmiAiTrainingKvCachePrefill`
operations at prefix lengths 1..N against a single initial Prefill followed by
Append operations. Both use the same decoder, learned token positions and
weight snapshots. It tests every output logit and the actual computed-row
statistics. Because it does not left-pad the prefix, this experiment has
different initial conditions from the unchanged canonical generation service.

An N=6 example needs 21 rows without reuse and 6 with reuse, provided N never
exceeds the decoder's context length. This does not prove a 3.5x wall-clock
speedup. CPU samples and allocation metrics are provided separately.

See `docs/learning/NATIVE_LLM_PREFIX_REUSE.md` for the PowerShell configure,
build and CTest instructions. Only the new full files are included in this
package. The parent project's original CMake source remains untouched;
to enable these APIs in a consolidated build, the top-level CMake composition
must include `cmake/UmicomNativeLlmPrefixReuse.cmake` **after** the existing
training composition. Do not add the extra module twice.
