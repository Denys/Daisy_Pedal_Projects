#include "device_capability_descriptor.hpp"

#include <algorithm>
#include <array>
#include <cstring>

namespace synth_harness {
namespace {

constexpr std::uint8_t kCompiledApplicationScope = 1;
constexpr std::uint16_t kCritical = 1;
constexpr std::uint8_t kNone = 0xff;

class Writer {
  public:
    explicit Writer(std::span<std::uint8_t> output) : output_(output) {}

    void U8(std::uint8_t value) { output_[offset_++] = value; }
    void U16(std::uint16_t value) {
        U8(static_cast<std::uint8_t>(value));
        U8(static_cast<std::uint8_t>(value >> 8));
    }
    void U32(std::uint32_t value) {
        U16(static_cast<std::uint16_t>(value));
        U16(static_cast<std::uint16_t>(value >> 16));
    }
    void Bytes(std::span<const std::uint8_t> bytes) {
        std::copy(bytes.begin(), bytes.end(), output_.begin() + static_cast<std::ptrdiff_t>(offset_));
        offset_ += bytes.size();
    }
    void Tlv(std::uint16_t tag, std::span<const std::uint8_t> payload) {
        U16(tag); U16(kCritical); U16(static_cast<std::uint16_t>(payload.size())); Bytes(payload);
    }
    std::size_t size() const { return offset_; }

  private:
    std::span<std::uint8_t> output_;
    std::size_t offset_ = 0;
};

std::uint16_t ReadU16(std::span<const std::uint8_t> in, std::size_t offset) {
    return static_cast<std::uint16_t>(in[offset]) |
           static_cast<std::uint16_t>(in[offset + 1] << 8);
}

std::uint32_t ReadU32(std::span<const std::uint8_t> in, std::size_t offset) {
    return static_cast<std::uint32_t>(ReadU16(in, offset)) |
           (static_cast<std::uint32_t>(ReadU16(in, offset + 2)) << 16);
}

CapabilityRecord Known(std::uint8_t id, bool supported) {
    return {id, kCompiledApplicationScope,
            supported ? Support::Supported : Support::Unsupported,
            Evidence::ImplementedSource};
}

CapabilityRecord Reserved(std::uint8_t id) {
    return {id, kCompiledApplicationScope, Support::Unknown, Evidence::Unavailable};
}

bool ValidSupport(std::uint8_t scope, Support support, Evidence evidence) {
    if (scope != kCompiledApplicationScope) return false;
    if (support == Support::Supported || support == Support::Unsupported)
        return evidence == Evidence::ImplementedSource;
    return support == Support::Unknown && evidence == Evidence::Unavailable;
}

bool IsAllZero(std::span<const std::uint8_t> bytes) {
    return std::all_of(bytes.begin(), bytes.end(), [](std::uint8_t v) { return v == 0; });
}

bool ValidLayoutIdentity(const LayoutIdentity& layout) {
    if (layout.state == 0) return layout.algorithm == 0 && layout.value == 0;
    return layout.state == 1 && layout.algorithm == 1 && layout.value != 0;
}

bool ValidDescriptor(const Descriptor& d) {
    const auto target = static_cast<std::uint16_t>(d.target);
    if (target < 1 || target > 5 || d.target != d.compiled_for_carrier || !ValidLayoutIdentity(d.layout_identity))
        return false;
    for (std::size_t i = 0; i < d.capabilities.size(); ++i) {
        const auto& cap = d.capabilities[i];
        if (cap.id != i + 1 || !ValidSupport(cap.scope, cap.support, cap.evidence) ||
            (cap.id >= 12 && (cap.support != Support::Unknown || cap.evidence != Evidence::Unavailable)))
            return false;
    }
    for (std::size_t i = 0; i < d.roles.size(); ++i) {
        const auto& role = d.roles[i];
        if (role.id != i + 1 || !ValidSupport(kCompiledApplicationScope, role.support, role.evidence) ||
            (role.support == Support::Supported && role.physical_index >= d.resources.switches) ||
            (role.support != Support::Supported && role.physical_index != kNone))
            return false;
    }
    return true;
}

constexpr std::uint32_t LayoutHashRound(std::uint32_t hash, std::uint32_t value) {
    return (hash ^ value) * 16777619u;
}

} // namespace

Descriptor MakeSourceBackedDescriptor(TargetId target, const RuntimeCarrierFacts& facts,
                                      std::uint32_t layout_hash) {
    Descriptor d{};
    d.target = target;
    d.compiled_for_carrier = target;
    d.resources = facts.resources;
    if (layout_hash != 0) d.layout_identity = {1, 1, layout_hash};

    d.capabilities[0] = Known(1, facts.stereo);
    d.capabilities[1] = Known(2, facts.midi);
    d.capabilities[2] = Known(3, facts.display);
    d.capabilities[3] = Known(4, facts.encoder);
    d.capabilities[4] = Known(5, facts.true_bypass);
    d.capabilities[5] = Known(6, facts.true_bypass);
    d.capabilities[6] = Known(7, true);
    d.capabilities[7] = Known(8, true);
    d.capabilities[8] = Known(9, facts.display && facts.encoder);
    d.capabilities[9] = Known(10, false);
    d.capabilities[10] = Known(11, true);
    for (std::uint8_t id = 12; id <= 23; ++id) d.capabilities[id - 1] = Reserved(id);

    d.roles = {{{1, facts.resources.switches > 0 ? std::uint8_t{0} : kNone,
                 facts.resources.switches > 0 ? Support::Supported : Support::Unsupported,
                 Evidence::ImplementedSource},
                {2, facts.resources.switches > 1 ? std::uint8_t{1} : kNone,
                 facts.resources.switches > 1 ? Support::Supported : Support::Unsupported,
                 Evidence::ImplementedSource}}};
    return d;
}

SerializeResult Serialize(const Descriptor& d, std::span<std::uint8_t> output) {
    if (output.size() < kCanonicalDescriptorBytes) return {SerializeStatus::OutputTooSmall, 0};
    if (!ValidDescriptor(d)) return {SerializeStatus::InvalidDescriptor, 0};
    Writer out(output.first(kCanonicalDescriptorBytes));
    const std::array<std::uint8_t, 4> magic{{'S', 'H', 'D', '1'}};
    out.Bytes(magic);
    out.U16(1);
    out.U16(12);
    out.U16(static_cast<std::uint16_t>(kCanonicalDescriptorBytes));
    out.U16(0);

    std::array<std::uint8_t, 2> two{};
    two[0] = static_cast<std::uint8_t>(static_cast<std::uint16_t>(d.target));
    out.Tlv(1, two);
    two[0] = static_cast<std::uint8_t>(static_cast<std::uint16_t>(d.compiled_for_carrier));
    out.Tlv(2, two);
    two[0] = 1;
    out.Tlv(3, two);

    const std::array<std::uint8_t, 6> observed{};
    out.Tlv(4, observed);
    const std::array<std::uint8_t, 42> firmware{};
    out.Tlv(5, firmware);
    std::array<std::uint8_t, 10> settings{{10, 0, 32, d.layout_identity.state,
                                           static_cast<std::uint8_t>(d.layout_identity.algorithm),
                                           static_cast<std::uint8_t>(d.layout_identity.algorithm >> 8),
                                           static_cast<std::uint8_t>(d.layout_identity.value),
                                           static_cast<std::uint8_t>(d.layout_identity.value >> 8),
                                           static_cast<std::uint8_t>(d.layout_identity.value >> 16),
                                           static_cast<std::uint8_t>(d.layout_identity.value >> 24)}};
    out.Tlv(6, settings);

    const std::array<std::uint8_t, 8> resources{{
        d.resources.callback_inputs, d.resources.callback_outputs,
        d.resources.analog_controls, d.resources.switches,
        d.resources.encoders, d.resources.leds,
        static_cast<std::uint8_t>(d.resources.storage_region_words),
        static_cast<std::uint8_t>(d.resources.storage_region_words >> 8)}};
    out.Tlv(7, resources);

    std::array<std::uint8_t, 92> capabilities{};
    for (std::size_t i = 0; i < d.capabilities.size(); ++i) {
        capabilities[4 * i] = d.capabilities[i].id;
        capabilities[4 * i + 1] = d.capabilities[i].scope;
        capabilities[4 * i + 2] = static_cast<std::uint8_t>(d.capabilities[i].support);
        capabilities[4 * i + 3] = static_cast<std::uint8_t>(d.capabilities[i].evidence);
    }
    out.Tlv(8, capabilities);

    std::array<std::uint8_t, 8> roles{};
    for (std::size_t i = 0; i < d.roles.size(); ++i) {
        roles[4 * i] = d.roles[i].id;
        roles[4 * i + 1] = d.roles[i].physical_index;
        roles[4 * i + 2] = static_cast<std::uint8_t>(d.roles[i].support);
        roles[4 * i + 3] = static_cast<std::uint8_t>(d.roles[i].evidence);
    }
    out.Tlv(9, roles);
    return {SerializeStatus::Ok, out.size()};
}

bool IsValidDescriptor(const Descriptor& descriptor) {
    return ValidDescriptor(descriptor);
}

ParseResult Parse(std::span<const std::uint8_t> bytes) {
    ParseResult r{ParseStatus::BadHeader, {}};
    if (bytes.size() > kMaximumDescriptorBytes) { r.status = ParseStatus::Oversize; return r; }
    if (bytes.size() < 12) { r.status = ParseStatus::SizeMismatch; return r; }
    if (std::memcmp(bytes.data(), "SHD1", 4) != 0) { r.status = ParseStatus::BadMagic; return r; }
    if (ReadU16(bytes, 4) != 1) { r.status = ParseStatus::UnsupportedVersion; return r; }
    if (ReadU16(bytes, 6) != 12 || ReadU16(bytes, 10) != 0) { r.status = ParseStatus::BadHeader; return r; }
    const auto total = ReadU16(bytes, 8);
    if (total != bytes.size() || total < 12) { r.status = ParseStatus::SizeMismatch; return r; }

    std::array<bool, 10> seen{};
    std::uint16_t previous_tag = 0;
    std::size_t cursor = 12;
    while (cursor < bytes.size()) {
        if (bytes.size() - cursor < 6) { r.status = ParseStatus::BadLength; return r; }
        const auto tag = ReadU16(bytes, cursor);
        const auto flags = ReadU16(bytes, cursor + 2);
        const auto length = ReadU16(bytes, cursor + 4);
        cursor += 6;
        if (flags & ~1u) { r.status = ParseStatus::BadFlags; return r; }
        if (tag < previous_tag) { r.status = ParseStatus::BadTagOrder; return r; }
        if (tag == previous_tag) { r.status = ParseStatus::DuplicateTag; return r; }
        previous_tag = tag;
        if (length > bytes.size() - cursor) { r.status = ParseStatus::BadLength; return r; }
        const auto p = bytes.subspan(cursor, length);
        cursor += length;
        if (tag < 1 || tag > 9) {
            if (flags & 1u) { r.status = ParseStatus::UnknownCriticalTag; return r; }
            continue;
        }
        if (flags != kCritical) { r.status = ParseStatus::BadFlags; return r; }
        seen[tag] = true;

        switch (tag) {
        case 1:
        case 2: {
            if (length != 2 || p[1] != 0 || p[0] < 1 || p[0] > 5) { r.status = ParseStatus::BadEnum; return r; }
            const auto v = static_cast<TargetId>(p[0]);
            if (tag == 1) r.descriptor.target = v; else r.descriptor.compiled_for_carrier = v;
            break;
        }
        case 3:
            if (length != 2 || p[0] != 1 || p[1] != 0) { r.status = ParseStatus::BadEnum; return r; }
            break;
        case 4:
            if (length != 6 || !IsAllZero(p)) { r.status = ParseStatus::BadIdentityState; return r; }
            break;
        case 5: {
            if (length != 42 || p[0] > 1 || p[1] > 1 || p[3] != 0) { r.status = ParseStatus::BadIdentityState; return r; }
            const bool semver_zero = IsAllZero(p.subspan(4, 6));
            if (p[0] == 0 && !semver_zero) { r.status = ParseStatus::BadIdentityState; return r; }
            const bool digest_zero = IsAllZero(p.subspan(10, 32));
            if (p[1] == 0) {
                if (p[2] != 0 || !digest_zero) { r.status = ParseStatus::BadIdentityState; return r; }
            } else if (p[2] != 1 || digest_zero) { r.status = ParseStatus::BadIdentityState; return r; }
            break;
        }
        case 6: {
            if (length != 10 || ReadU16(p, 0) != 10 || p[2] != 32 || p[3] > 1) { r.status = ParseStatus::BadIdentityState; return r; }
            const auto algorithm = ReadU16(p, 4);
            const bool value_zero = IsAllZero(p.subspan(6, 4));
            if (p[3] == 0) {
                if (algorithm != 0 || !value_zero) { r.status = ParseStatus::BadIdentityState; return r; }
            } else if (algorithm != 1 || value_zero) { r.status = ParseStatus::BadIdentityState; return r; }
            r.descriptor.layout_identity = {p[3], algorithm, ReadU32(p, 6)};
            break;
        }
        case 7:
            if (length != 8) { r.status = ParseStatus::BadLength; return r; }
            r.descriptor.resources = {p[0], p[1], p[2], p[3], p[4], p[5], ReadU16(p, 6)};
            break;
        case 8:
            if (length != 92) { r.status = ParseStatus::BadLength; return r; }
            for (std::size_t i = 0; i < 23; ++i) {
                const auto id = p[4 * i];
                const auto scope = p[4 * i + 1];
                const auto support = static_cast<Support>(p[4 * i + 2]);
                const auto evidence = static_cast<Evidence>(p[4 * i + 3]);
                if (id != i + 1 || !ValidSupport(scope, support, evidence) ||
                    (id >= 12 && (support != Support::Unknown || evidence != Evidence::Unavailable))) {
                    r.status = ParseStatus::BadCapability; return r;
                }
                r.descriptor.capabilities[i] = {id, scope, support, evidence};
            }
            break;
        case 9:
            if (length != 8) { r.status = ParseStatus::BadLength; return r; }
            for (std::size_t i = 0; i < 2; ++i) {
                const auto id = p[4 * i];
                const auto index = p[4 * i + 1];
                const auto support = static_cast<Support>(p[4 * i + 2]);
                const auto evidence = static_cast<Evidence>(p[4 * i + 3]);
                if (id != i + 1 || !ValidSupport(kCompiledApplicationScope, support, evidence) ||
                    (support != Support::Supported && index != kNone)) {
                    r.status = ParseStatus::BadRole; return r;
                }
                r.descriptor.roles[i] = {id, index, support, evidence};
            }
            break;
        }
    }

    for (std::size_t tag = 1; tag <= 9; ++tag) {
        if (!seen[tag]) { r.status = ParseStatus::MissingCoreTag; return r; }
    }
    if (r.descriptor.target != r.descriptor.compiled_for_carrier) {
        r.status = ParseStatus::TargetCarrierMismatch; return r;
    }
    for (const auto& role : r.descriptor.roles) {
        if (role.support == Support::Supported && role.physical_index >= r.descriptor.resources.switches) {
            r.status = ParseStatus::BadRole; return r;
        }
    }
    r.status = ParseStatus::Ok;
    return r;
}

bool AuthorizesPhysicalOperation(const Descriptor&) {
    return false; // v0.1 observed identity is fixed unknown and grants no physical authority.
}

bool AuthorizesPresetMutation(const Descriptor& descriptor, std::uint32_t expected_layout_hash) {
    const auto& presets = descriptor.capabilities[6];
    return presets.id == 7 && presets.scope == kCompiledApplicationScope &&
           presets.support == Support::Supported && presets.evidence == Evidence::ImplementedSource &&
           descriptor.layout_identity.state == 1 && descriptor.layout_identity.algorithm == 1 &&
           descriptor.layout_identity.value != 0 && descriptor.layout_identity.value == expected_layout_hash;
}

std::uint32_t ComputeEffectLayoutHash(std::span<const EffectLayoutView> effects) {
    std::uint32_t hash = 2166136261u;
    hash = LayoutHashRound(hash, static_cast<std::uint32_t>(effects.size()));
    for (const auto& effect : effects) {
        for (const unsigned char c : effect.name)
            hash = LayoutHashRound(hash, static_cast<std::uint32_t>(c));
        hash = LayoutHashRound(hash, 0xffu);
        hash = LayoutHashRound(hash, static_cast<std::uint32_t>(effect.parameter_types.size()));
        for (const auto type : effect.parameter_types) hash = LayoutHashRound(hash, type);
    }
    return hash;
}

} // namespace synth_harness
