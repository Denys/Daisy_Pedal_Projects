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

## 2026-08-17 — Harness Lite control inventory source integration

- **Run ID:** `harness-lite-control-inventory-20260817`.
- **Primary lane:** `synth-harness`.
- **Objective:** apply the 2026-08-17 cumulative descriptor/control-inventory handoff to an isolated source branch and verify host plus ARM integration without adding transport or MIDI authority.
- **Repository/workspace:** `Denys/Daisy_Pedal_Projects`, branch `agent/harness-lite-control-inventory`; no merge performed.
- **Base SHA:** `bdb53ce13e83736b6909618adbfda3e237623649`, equal to current `main` and the handoff's declared source base.
- **Active decisions:** ADR-0001 through ADR-0005; no supersession and no new product decision.
- **Artifacts:** cumulative handoff `Denys/custom-pedals@d6acb747ec13d9fc6de113b3e4639a87f395a466`, patch blob `0ccea9cada5e5af12de2bc0c2da29033ddc393ed`; 14 implementation/test files applied, plus this lane state and root state.
- **Patch drift:** original `git apply --check` failed only on three trailing-blank-line deletion hunks because the exact base files already ended at the preceding content line. The repaired application removed those three cosmetic EOF hunks; all semantic hunks and resulting source content were preserved.
- **Host validation:** strict C++20 descriptor `458 checks PASS`; inventory `363 checks PASS`; both UBSan suites `PASS`; no-macro plus five explicit selection probes `PASS`; conflicting target macros rejected as expected.
- **ARM validation:** xPack GNU Arm Embedded GCC `10.3.1 20210824`; pinned submodules initialized; libDaisy, DaisySP and CloudSeed built; no-macro default, `125B`, `1590B`, `1590B_SMD`, `TERRARIUM` and `FUNBOX` all compiled and linked `PASS`.
- **Build failure and repair:** the first parallel default build produced one corrupt generated object, `build/base_hardware_module.o`, and failed at link. A serial rebuild produced a valid ARM ELF; default and all five explicit target builds then linked. This is consistent with transient parallel-build artifact corruption, but the root cause is not proven; no source repair was needed.
- **Boundary verification:** Synth-Harness sources contain no transport/send/receive path; MIDI endpoints are inventory records only; `AuthorizesPhysicalOperation()` remains unconditional false. Existing application MIDI behavior is unchanged and is not authority granted by this patch.
- **Hardware status:** `NOT_RUN`; no flash, boot, timing, electrical, audio, fixture, MIDI transaction or physical validation claim.
- **Acceptance status:** `PASS_WITH_GAPS` — source, host and six ARM configurations pass on the dedicated unmerged branch; hardware and transport remain `HOLD`.
- **Next single action:** review the dedicated branch diff and open a PR if integration review is desired; do not merge until that review is accepted.
- **Impact:** HW — none; FW — descriptor and bounded inventory compiled into all targets; DSP/audio callback — no algorithmic change; Mechanical/BOM — none; Documentation — state advanced with explicit patch drift and build evidence.

## 2026-08-21 — Fail-closed parser/authorization repair and PR gate

- **Run ID:** `harness-fail-closed-pr2-20260821`.
- **Primary lane:** `synth-harness`.
- **Repository/workspace:** `Denys/Daisy_Pedal_Projects`, branch `agent/harness-lite-control-inventory`; PR `#2` is draft/open against `main`.
- **Starting candidate:** `c75ac3c47671997db576ab002897c03da9c57517`; correction work initially advanced through `82095521969d7300ad62931a9c0828c1f4848df3` before review repair.
- **Correctness repair:** `AuthorizesPresetMutation()` now requires `IsValidDescriptor()` before layout/capability authorization. Review then identified a second fail-closed path: a rejected wire parse could leave a structurally valid partially reconstructed descriptor if a caller ignored `ParseStatus`. `ParseResult` now invalidates descriptor target identity whenever status becomes non-`Ok`, including late failures after valid core TLVs.
- **Regression coverage:** existing descriptor test includes the invalid target/carrier preset-mutation negative; new `test_rejected_wire_authorization.cpp` appends an unknown critical TLV after an otherwise valid descriptor and requires `UnknownCriticalTag`, invalid descriptor state, and refusal of preset mutation. The strict host `Makefile` now runs this regression.
- **CI contract:** `.github/workflows/build.yml` now includes strict host descriptor/inventory/selection/conflict tests, descriptor and inventory UBSan, no-macro/default ARM build, plus explicit `125B`, `1590B`, `1590B_SMD`, `TERRARIUM`, and `FUNBOX` ARM builds.
- **Recorded prior evidence:** the 2026-08-17 pre-fix candidate recorded descriptor `458 PASS`, inventory `363 PASS`, both UBSan suites, target-selection probes/conflict negative, and six ARM builds. These results are historical source-recorded evidence and are not relabelled as exact-head PR #2 CI.
- **Exact-head execution status:** GitHub workflow queries for the earlier PR head exposed no workflow runs; the connector environment cannot run the repository locally. The repaired PR head must therefore pass its configured GitHub workflow or equivalent exact-head execution before merge. `PASS` is not claimed for the new parser regression yet.
- **Review status:** Codex review on `82095521969d7300ad62931a9c0828c1f4848df3` found two P1 issues: rejected-parse authorization and stale root/lane state. Both are repaired in the current branch; a new exact-head review is required.
- **Boundary:** `AuthorizesPhysicalOperation()` remains unconditional false. No transport, flash, calibration, fixture actuation, MIDI transaction, physical routing, timing, audio, or electrical authority is added.
- **Acceptance status:** `CHANGES_APPLIED / CI_REVIEW_PENDING / HARDWARE_HOLD`.
- **Next single action:** run/inspect exact-head CI, request a fresh review, repair any valid finding, and merge PR #2 only from the unchanged reviewed passing head.
