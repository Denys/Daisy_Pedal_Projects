#include "device_capability_descriptor.hpp"
#include "source_device_capability_adapter.hpp"

#include <array>
#include <cstdlib>
#include <iostream>
#include <span>
#include <string>
#include <vector>

using namespace synth_harness;

namespace {
int tests = 0;

void Check(bool condition, const char* message) {
    ++tests;
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

std::vector<std::uint8_t> SerializeToVector(const Descriptor& descriptor) {
    std::array<std::uint8_t, kCanonicalDescriptorBytes> bytes{};
    const auto result = Serialize(descriptor, bytes);
    Check(result.status == SerializeStatus::Ok, "fixed-buffer serialization succeeds");
    Check(result.bytes_written == bytes.size(), "fixed-buffer byte count");
    return {bytes.begin(), bytes.end()};
}

void Put16(std::vector<std::uint8_t>& out, std::uint16_t value) {
    out.push_back(static_cast<std::uint8_t>(value));
    out.push_back(static_cast<std::uint8_t>(value >> 8));
}

void OracleTlv(std::vector<std::uint8_t>& out, std::uint16_t tag,
               std::span<const std::uint8_t> payload, std::uint16_t flags = 1) {
    Put16(out, tag); Put16(out, flags); Put16(out, payload.size());
    out.insert(out.end(), payload.begin(), payload.end());
}

std::vector<std::uint8_t> Independent125BOracle() {
    std::vector<std::uint8_t> out{'S','H','D','1',1,0,12,0,238,0,0,0};
    const std::array<std::uint8_t, 2> id{{1,0}};
    OracleTlv(out, 1, id); OracleTlv(out, 2, id); OracleTlv(out, 3, id);
    const std::array<std::uint8_t, 6> observed{}; OracleTlv(out, 4, observed);
    const std::array<std::uint8_t, 42> firmware{}; OracleTlv(out, 5, firmware);
    const std::array<std::uint8_t, 10> settings{{10,0,32,0,0,0,0,0,0,0}}; OracleTlv(out, 6, settings);
    const std::array<std::uint8_t, 8> resources{{2,2,6,2,1,2,0,4}}; OracleTlv(out, 7, resources);
    std::array<std::uint8_t, 92> caps{};
    const std::array<std::uint8_t, 11> known{{2,2,2,2,2,2,2,2,2,1,2}};
    for (std::size_t i=0; i<23; ++i) {
        caps[4*i]=i+1; caps[4*i+1]=1;
        caps[4*i+2]=i<11 ? known[i] : 0;
        caps[4*i+3]=i<11 ? 1 : 0;
    }
    OracleTlv(out, 8, caps);
    const std::array<std::uint8_t, 8> roles{{1,0,2,1,2,1,2,1}}; OracleTlv(out, 9, roles);
    return out;
}

std::size_t FindTag(const std::vector<std::uint8_t>& bytes, std::uint16_t wanted) {
    std::size_t cursor=12;
    while(cursor+6<=bytes.size()) {
        const auto tag=bytes[cursor] | (bytes[cursor+1]<<8);
        const auto len=bytes[cursor+4] | (bytes[cursor+5]<<8);
        if(tag==wanted) return cursor;
        cursor += 6 + len;
    }
    return bytes.size();
}

std::vector<std::uint8_t> RemoveTag(std::vector<std::uint8_t> bytes, std::uint16_t wanted) {
    const auto start = FindTag(bytes, wanted);
    const auto length = static_cast<std::size_t>(bytes[start + 4] | (bytes[start + 5] << 8));
    bytes.erase(bytes.begin() + static_cast<std::ptrdiff_t>(start),
                bytes.begin() + static_cast<std::ptrdiff_t>(start + 6 + length));
    const auto size = static_cast<std::uint16_t>(bytes.size());
    bytes[8] = static_cast<std::uint8_t>(size);
    bytes[9] = static_cast<std::uint8_t>(size >> 8);
    return bytes;
}

struct FakeHardware {
    int knobs;
    int switches;
    int encoders;
    int leds;
    bool stereo;
    bool midi;
    bool display;
    bool encoder;
    bool true_bypass;

    int GetKnobCount() { return knobs; }
    int GetSwitchCount() { return switches; }
    int GetEncoderCount() { return encoders; }
    int GetLedCount() { return leds; }
    bool SupportsStereo() { return stereo; }
    bool SupportsMidi() { return midi; }
    bool SupportsDisplay() { return display; }
    bool SupportsEncoder() { return encoder; }
    bool SupportsTrueBypass() { return true_bypass; }
};
} // namespace

int main() {
    const std::array<TargetId,5> targets{{TargetId::Pedal125B,TargetId::Pedal1590B,
        TargetId::Pedal1590BSmd,TargetId::Terrarium,TargetId::Funbox}};
    const std::array<std::uint8_t,5> analog{{6,4,4,6,7}};
    const std::array<std::uint8_t,5> switches{{2,2,2,6,12}};
    const std::array<RuntimeCarrierFacts,5> facts{{
        {{2,2,6,2,1,2,1024},true,true,true,true,true},
        {{2,2,4,2,0,2,1024},true,true,false,false,false},
        {{2,2,4,2,0,2,1024},true,true,false,false,true},
        {{2,2,6,6,0,2,1024},false,false,false,false,false},
        {{2,2,7,12,0,2,1024},true,true,false,false,false},
    }};
    const std::array<std::array<std::uint8_t,11>,5> expected_known_support{{
        {{2,2,2,2,2,2,2,2,2,1,2}},
        {{2,2,1,1,1,1,2,2,1,1,2}},
        {{2,2,1,1,2,2,2,2,1,1,2}},
        {{1,1,1,1,1,1,2,2,1,1,2}},
        {{2,2,1,1,1,1,2,2,1,1,2}},
    }};
    for(std::size_t i=0;i<targets.size();++i) {
        const auto d=MakeSourceBackedDescriptor(targets[i],facts[i]);
        const auto bytes=SerializeToVector(d);
        Check(bytes.size()==238,"canonical size");
        const auto parsed=Parse(bytes);
        Check(parsed.status==ParseStatus::Ok,"all targets parse");
        Check(parsed.descriptor.target==targets[i],"target identity round-trip");
        Check(parsed.descriptor.resources.analog_controls==analog[i],"analog matrix");
        Check(parsed.descriptor.resources.switches==switches[i],"switch matrix");
        for(std::size_t capability=0; capability<11; ++capability) {
            Check(static_cast<std::uint8_t>(parsed.descriptor.capabilities[capability].support)==
                  expected_known_support[i][capability],"source-backed capability matrix");
        }
        for(std::size_t capability=11; capability<23; ++capability) {
            Check(parsed.descriptor.capabilities[capability].support==Support::Unknown,
                  "reserved capability remains unknown");
        }
        Check(!AuthorizesPhysicalOperation(parsed.descriptor),"v0.1 refuses physical operation");
    }

    const auto canonical=SerializeToVector(MakeSourceBackedDescriptor(TargetId::Pedal125B,facts[0]));
    Check(canonical==Independent125BOracle(),"independent 238-byte golden vector");

    std::array<std::uint8_t, kCanonicalDescriptorBytes - 1> short_buffer{};
    short_buffer.fill(0xa5);
    const auto short_result = Serialize(MakeSourceBackedDescriptor(TargetId::Pedal125B,facts[0]), short_buffer);
    Check(short_result.status==SerializeStatus::OutputTooSmall && short_result.bytes_written==0,
          "undersized fixed buffer rejected before publication");
    Check(short_buffer.front()==0xa5 && short_buffer.back()==0xa5,"undersized output remains untouched");
    auto invalid_descriptor=MakeSourceBackedDescriptor(TargetId::Pedal125B,facts[0]);
    invalid_descriptor.compiled_for_carrier=TargetId::Funbox;
    std::array<std::uint8_t,kCanonicalDescriptorBytes> invalid_output{};
    invalid_output.fill(0x5a);
    const auto invalid_result=Serialize(invalid_descriptor,invalid_output);
    Check(invalid_result.status==SerializeStatus::InvalidDescriptor && invalid_result.bytes_written==0,
          "invalid descriptor rejected before publication");
    Check(invalid_output.front()==0x5a && invalid_output.back()==0x5a,"invalid output remains untouched");

    const std::array<std::uint32_t,2> a_types{{2,3}};
    const std::array<std::uint32_t,3> delay_types{{0,4,7}};
    const std::array<EffectLayoutView,0> empty_layout{};
    const std::array<EffectLayoutView,1> one_layout{{EffectLayoutView{"A",a_types}}};
    const std::array<EffectLayoutView,2> two_layout{{EffectLayoutView{"A",a_types},EffectLayoutView{"Delay",delay_types}}};
    const std::array<EffectLayoutView,2> reordered_layout{{EffectLayoutView{"Delay",delay_types},EffectLayoutView{"A",a_types}}};
    Check(ComputeEffectLayoutHash(empty_layout)==84696351u,"independent empty-layout FNV vector");
    Check(ComputeEffectLayoutHash(one_layout)==3845383749u,"independent one-effect FNV vector");
    Check(ComputeEffectLayoutHash(two_layout)==2011535672u,"independent two-effect FNV vector");
    Check(ComputeEffectLayoutHash(reordered_layout)==723477096u,"independent reordered-layout FNV vector");
    Check(ComputeEffectLayoutHash(two_layout)!=ComputeEffectLayoutHash(reordered_layout),"layout order is identity-significant");

    auto layout_descriptor=MakeSourceBackedDescriptor(TargetId::Pedal125B,facts[0],ComputeEffectLayoutHash(two_layout));
    const auto layout_bytes=SerializeToVector(layout_descriptor);
    const auto layout_parsed=Parse(layout_bytes);
    Check(layout_parsed.status==ParseStatus::Ok,"present layout identity parses");
    Check(AuthorizesPresetMutation(layout_parsed.descriptor,2011535672u),"exact layout permits preset mutation");
    Check(!AuthorizesPresetMutation(layout_parsed.descriptor,723477096u),"mismatched layout refuses preset mutation");
    Check(!AuthorizesPresetMutation(Parse(canonical).descriptor,2011535672u),"unknown layout refuses preset mutation");
    auto invalid_layout_descriptor=layout_parsed.descriptor;
    invalid_layout_descriptor.compiled_for_carrier=TargetId::Funbox;
    Check(!AuthorizesPresetMutation(invalid_layout_descriptor,2011535672u),
          "invalid descriptor refuses preset mutation");

    FakeHardware observed{9,3,2,4,true,true,true,true,true};
    const auto observed_facts=ObserveCarrierFacts(observed,777);
    Check(observed_facts.resources.analog_controls==9,"adapter observes runtime analog count");
    Check(observed_facts.resources.switches==3,"adapter observes runtime switch count");
    Check(observed_facts.resources.encoders==2,"adapter observes runtime encoder count");
    Check(observed_facts.resources.leds==4,"adapter observes runtime LED count");
    Check(observed_facts.resources.storage_region_words==777,"adapter uses source storage bound");
    Check(observed_facts.stereo && observed_facts.midi && observed_facts.display &&
          observed_facts.encoder && observed_facts.true_bypass,"adapter observes runtime support flags");
    const auto integrated=BuildSourceDescriptor(observed,777,2011535672u);
    Check(integrated.target==TargetId::Pedal125B,"no-macro adapter selects 125B default");
    Check(integrated.layout_identity.value==2011535672u,"adapter publishes active layout identity");
    Check(integrated.resources.analog_controls==9,"descriptor uses observed rather than fixture resources");
    const auto integrated_parsed=Parse(SerializeToVector(integrated));
    Check(integrated_parsed.status==ParseStatus::Ok,"runtime-derived descriptor round-trips");
    Check(AuthorizesPresetMutation(integrated_parsed.descriptor,2011535672u),
          "runtime-derived exact layout authorizes preset mutation");
    FakeHardware no_controls{0,0,0,0,false,false,false,false,false};
    const auto no_control_descriptor=BuildSourceDescriptor(no_controls,1024,0);
    Check(no_control_descriptor.roles[0].support==Support::Unsupported &&
          no_control_descriptor.roles[0].physical_index==0xff,"missing bypass switch fails closed");
    Check(no_control_descriptor.roles[1].support==Support::Unsupported &&
          no_control_descriptor.roles[1].physical_index==0xff,"missing alternate switch fails closed");

    for(std::uint16_t tag=1;tag<=9;++tag)
        Check(Parse(RemoveTag(canonical,tag)).status==ParseStatus::MissingCoreTag,"omit-each-core-TLV rejected");

    for(std::size_t n=0;n<canonical.size();++n) {
        Check(Parse(std::span(canonical.data(),n)).status!=ParseStatus::Ok,"every truncation rejected");
    }
    auto bad=canonical;
    bad.push_back(0); Check(Parse(bad).status==ParseStatus::SizeMismatch,"trailing byte rejected");
    bad=canonical; bad[0]='X'; Check(Parse(bad).status==ParseStatus::BadMagic,"bad magic rejected");
    bad=canonical; bad[4]=2; Check(Parse(bad).status==ParseStatus::UnsupportedVersion,"bad version rejected");
    bad=canonical; bad[10]=1; Check(Parse(bad).status==ParseStatus::BadHeader,"header flags rejected");

    const auto tag5=FindTag(canonical,5);
    bad=canonical; bad[tag5+6]=1;
    Check(Parse(bad).status==ParseStatus::Ok,"present semver 0.0.0 accepted");
    bad=canonical; bad[tag5+6+4]=1;
    Check(Parse(bad).status==ParseStatus::BadIdentityState,"unknown semver with nonzero tuple rejected");
    bad=canonical; bad[tag5+6+1]=1; bad[tag5+6+2]=1; bad[tag5+6+10]=1;
    Check(Parse(bad).status==ParseStatus::Ok,"present SHA-256 build identity accepted");
    bad=canonical; bad[tag5+6+2]=1;
    Check(Parse(bad).status==ParseStatus::BadIdentityState,"unknown build with algorithm rejected");
    bad=canonical; bad[tag5+6+1]=1; bad[tag5+6+2]=1;
    Check(Parse(bad).status==ParseStatus::BadIdentityState,"present build with zero digest rejected");
    bad=canonical; bad[tag5+6+1]=1; bad[tag5+6+2]=2; bad[tag5+6+10]=1;
    Check(Parse(bad).status==ParseStatus::BadIdentityState,"present build with unknown digest algorithm rejected");

    const auto tag2=FindTag(canonical,2);
    bad=canonical; bad[tag2]=1; Check(Parse(bad).status==ParseStatus::DuplicateTag,"duplicate tag rejected");
    bad=canonical; bad[tag2]=0; Check(Parse(bad).status==ParseStatus::BadTagOrder,"out-of-order tag rejected");
    bad=canonical; bad[12]=0x10; bad[13]=0x80;
    Check(Parse(bad).status==ParseStatus::UnknownCriticalTag,"unknown critical tag rejected");

    const auto tag8=FindTag(canonical,8);
    bad=canonical; bad[tag8+6+4*11+2]=2; bad[tag8+6+4*11+3]=1;
    Check(Parse(bad).status==ParseStatus::BadCapability,"reserved capability overclaim rejected");
    const auto tag9=FindTag(canonical,9);
    bad=canonical; bad[tag9+6+1]=2;
    Check(Parse(bad).status==ParseStatus::BadRole,"role index range rejected");
    bad=canonical; bad[tag9+6+3]=0;
    Check(Parse(bad).status==ParseStatus::BadRole,"supported role without source evidence rejected");
    bad=canonical; bad[tag9+6+2]=1;
    Check(Parse(bad).status==ParseStatus::BadRole,"unsupported role with physical index rejected");
    bad=canonical; bad[tag9+6+1]=0xff; bad[tag9+6+2]=1;
    Check(Parse(bad).status==ParseStatus::Ok,"unsupported evidenced role with no index accepted");
    bad=canonical; bad[tag9+6+1]=0xff; bad[tag9+6+2]=0; bad[tag9+6+3]=0;
    Check(Parse(bad).status==ParseStatus::Ok,"unknown role with no index accepted");
    bad=canonical; bad[tag2+6]=2;
    Check(Parse(bad).status==ParseStatus::TargetCarrierMismatch,"target/carrier mismatch rejected");

    bad=canonical;
    const auto tag6=FindTag(canonical,6);
    bad[tag6+6+3]=1;
    Check(Parse(bad).status==ParseStatus::BadIdentityState,"present zero layout hash rejected");

    bad=canonical;
    const std::array<std::uint8_t,3> optional{{0xaa,0xbb,0xcc}};
    OracleTlv(bad,0x1000,optional,0);
    const auto new_size=static_cast<std::uint16_t>(bad.size()); bad[8]=new_size; bad[9]=new_size>>8;
    Check(Parse(bad).status==ParseStatus::Ok,"bounded unknown optional tag skipped");

    bad.resize(1025,0);
    Check(Parse(bad).status==ParseStatus::Oversize,"1025-byte input rejected");
    std::cout << "PASS " << tests << " checks\n";
}
