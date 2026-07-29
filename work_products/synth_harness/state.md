# Synth Harness lane state

This append-preserving lane state supplements root `state.md`. Root state remains the continuity index.

## 2026-07-29 — DeviceCapabilityDescriptor contract

- **Run ID:** `pedal-synth-harness-20260729-01`.
- **Primary lane:** `synth-harness`.
- **Objective:** define a source-backed identity/capability boundary for diagnostics, editor generation, preset tooling, fixtures, service, and future update tooling across the five current firmware targets.
- **Repository/workspace:** `Denys/Daisy_Pedal_Projects`, branch `main`.
- **Base SHA:** `87caaed73086a7767442aeb572b91fa97f96e5a8`.
- **State before:** no Synth Harness lane state or durable capability-descriptor contract existed. Capability information was distributed across compile-time target selection, carrier initialization, support flags, control vectors, switch metadata, storage identity, and UI reachability.
- **Active decisions:** ADR-0001 through ADR-0005. ADR-0005 accepts the v0.1 documentation contract; it does not claim implementation.
- **Accepted:** a versioned, immutable-after-publication, implementation-scoped `DeviceCapabilityDescriptor`; explicit unknown states; separate compiled-for and observed identity; exact preset-layout compatibility; bounded 238-byte binary profile; fail-closed authorization.
- **Rejected:** boolean-only flags; default-target fallback in external tools; build target as physical identity; JSON as normative control transport; pin maps/mutable state in the descriptor; preset mutation without exact compatibility; any v0.1 physical-operation authority.
- **Deferred:** descriptor code, parser, independent binary golden vector, layout-hash execution, transport exposure, authentication, firmware update/recovery, calibration, physical carrier identity, and hardware qualification.
- **Artifacts:** `work_products/automation_runs/pedal_synth_harness_2026-07-29_device_capability_descriptor.md` (commit `431f50efa79f4d6878a3f52f6989a49ec90b114f`); root `decision_log.md` ADR-0005 (commit `5b8e9fb572c1e3fe8e4da836ff1771aed59eb9a5`).
- **Evidence inspected:** root `AGENTS.md`, `state.md`, and `decision_log.md`; repository `README.md` and MIT `LICENSE`; `Software/GuitarPedal/README.md`, target selection, storage implementation, base effect interface, base hardware module, all five carrier implementations, Makefile, and bootloader UI path. Exact blob SHAs are pinned in the contract.
- **Project-source inventory:** `SOURCE_UNAVAILABLE` — no current Project-source inventory/read route was exposed in this run. Historical filenames from chat were not treated as current evidence; repository primary sources were sufficient for this increment.
- **Tests/measurements:** continuity gate `PASS`; source-matrix verification `PASS`; three frozen independent read-only reviews completed, with terminal verdict `PASS` for the documentation contract (0 critical, 0 major, 3 minor, confidence 0.94); firmware build `NOT_RUN`; parser/serialization `NOT_RUN`; layout-hash execution `NOT_RUN`; transport `NOT_RUN`; hardware measurement `NOT_RUN`.
- **Assumptions:** no physical carrier, connector, revision, codec, transport, recovery, calibration, or measured performance claim was assumed.
- **Risks/open questions:** descriptor/initialization drift; current build target cannot prove installed hardware; exact current layout hash was not executed; v0.1 cannot authorize physical operations; transport and security remain outside scope.
- **Blockers:** none for adopting the documentation contract. Implementation acceptance is blocked until the required host builds, parser/policy tests, layout-hash checks, and independent binary golden vector pass.
- **Acceptance status:** `PASS_WITH_GAPS` — documentation contract accepted; implementation and hardware validation remain `NOT_RUN`.
- **Next single action:** implement shared per-target constants, the immutable descriptor, and host tests for all five explicit targets plus the no-macro 125B default; keep all transport exposure disabled.
- **Impact:** HW — none; FW/DSP — future read-only descriptor and tests, no audio-path change; Mechanical — none; UI/service — future capability gating and accurate carrier labelling; BOM — no change; Documentation — contract, ADR, and lane continuity created.
