# Federated Model-Delta Aggregation (Native C23)

A small, reproducible **mathematical** laboratory for federated averaging, built in the shared Umicom Framework rather than in an application or scripting language.

The service receives independently authorised per-worker **weight-space deltas** produced from the same pinned checkpoint. It uses distinct-example sample counts as weights, checks duplicate contributor IDs and basic numerical bounds, and finalises a bounded FP32 mean. It does **not** execute local neural training, validate source rights, authenticate submitted digests/signatures, securely aggregate hidden client updates, establish Byzantine robustness or write a blockchain. The digests represent *opaque, externally verified identities*; nonzero bytes are not proof of any claim.

The canonical arithmetic is `delta_global = sum(n_i * delta_i) / sum(n_i)`; a coordinator later decides whether a candidate checkpoint may be evaluated and approved. In particular, LoRA factor pairs should first be converted to actual weight-space deltas (`B_i * A_i`); separately averaging `B_i` and `A_i` is generally wrong.

The standalone CMake entry point is `learning/native-ai-federated-round`; the opt-in reusable CMake component is `cmake/UmicomAiFederatedRound.cmake`. The tests cover a worked three-worker example, duplicate/base/norm/non-finite/capacity rejection, and a counterexample to naive LoRA-factor averaging. Full distributed communication and signed, privacy-preserving attestation are different problems.

Public learning continuation: Bits2Banking LLM Lessons 377–392 and the new introductory Distributed Learning course.

Author: Sammy Hegab, Umicom Foundation. MIT.
