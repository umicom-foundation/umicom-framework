# Native C23 federated wire and separate-process demonstration

**Umicom Foundation · Sammy Hegab · MIT**

This optional native Framework module provides a canonical bounded binary
representation of federated assignments and synthetic weight-space deltas.
The codec is **not** a networking stack, a model trainer, a digital signature,
secure aggregation, or a blockchain. It reuses the existing SHA-256
implementation from `src/base/sha256.c`, `UmiMessageEnvelope`, and the
`UmiAiTrainingFederatedJob`/`FederatedRound` services.

The codec uses big-endian integers and IEEE-754 binary32 for floating-point
values. Each frame has a SHA-256 trailer under a fixed domain prefix.
**Unkeyed checksums provide accidental-corruption evidence, not authentication:**
an attacker who can replace a frame can recompute the digest. Authenticate the
sender, check the authorised dataset and verify base checkpoint identity
through the Framework's existing security, identity and source-governance
components before admitting any real task.

`demo.c` runs in distinct operating-system processes under CTest, exchanging
small files in a build directory. It creates three synthetic contributions
(+0.05, −0.01 and +0.08), sample-weighted by 100, 250 and 150 examples,
through the original C23 federated job. The final vector is +0.029. **No
training occurs**, no data is transferred over a real network, and no model
checkpoint is updated. This exercise tests protocol framing and composition,
not distributed training performance.

The frame is not an ABI dump: it has exact byte ordering, no struct padding,
no host pointer, an explicit version and payload length, identity commitments,
finite float validation and bounded capacity. Process-local job tickets are
never serialised. Only the coordinator holds ticket state and maps a verified
round/sequence/worker to its pending admission before calling `Complete`.

The public CMake entry is
`cmake/UmicomAiFederatedWire.cmake`. The corresponding standalone directory
is `learning/native-ai-federated-wire`. Existing modules and their historical
source remain unchanged; this is additive and opt-in.

The test fixture names beginning with `process.` execute multiple native OS
processes using CTest fixture dependencies. Files are temporary experiment
artifacts. Use `ctest --output-on-failure` on the configured build directory.
