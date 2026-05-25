#pragma once

#include <optional>
#include <vector>

#include "Serialization.h"

namespace EventBindings {
struct ScriptBindingSource {
    RE::FormID sourceFormID {0};
    RE::FormID effectSourceFormID {0};
};

class ScopedMirror {
public:
    ScopedMirror(const RE::TESForm& a_source, const RE::TESForm& a_effectSource, bool a_adoptLoadedBinding = false);
    ScopedMirror(const ScopedMirror&) = delete;
    ScopedMirror(ScopedMirror&&) = delete;
    ScopedMirror& operator=(const ScopedMirror&) = delete;
    ScopedMirror& operator=(ScopedMirror&&) = delete;
    ~ScopedMirror();

private:
    bool active_ {false};
};

[[nodiscard]] bool BeginPendingMirror(
    const RE::TESForm& a_source,
    const RE::TESForm& a_effectSource,
    const RE::BSFixedString& a_eventName,
    bool a_adoptLoadedBinding = false,
    std::optional<RE::FormID> a_expectedActorFormID = std::nullopt
);
bool MirrorActiveTarget(
    RE::VMHandle a_handle,
    const RE::BSFixedString& a_eventName,
    std::optional<RE::FormID> a_actorFormID = std::nullopt
);
[[nodiscard]] bool MirrorScriptsAndDispatch(
    const RE::TESForm& a_source,
    RE::FormID a_effectSourceFormID,
    RE::VMHandle a_targetHandle,
    RE::Actor& a_actor,
    const RE::BSFixedString& a_eventName
);
void RemoveForEffectSource(RE::FormID a_effectSourceFormID);
void RemoveForEffectSource(RE::FormID a_effectSourceFormID, RE::Actor& a_unequippedActor);
void RemoveForSource(RE::FormID a_sourceFormID);
void RemoveForSource(RE::FormID a_sourceFormID, RE::Actor& a_unequippedActor);
[[nodiscard]] bool ValidateLoadedRestore(const RE::TESForm& a_source, RE::FormID a_effectSourceFormID);
[[nodiscard]] bool AdoptLoadedBinding(const RE::TESForm& a_source, RE::FormID a_effectSourceFormID);
[[nodiscard]] bool HasLoadedBinding(RE::FormID a_sourceFormID, RE::FormID a_effectSourceFormID);
[[nodiscard]] std::optional<RE::VMHandle> GetHandle(RE::FormID a_sourceFormID, RE::FormID a_effectSourceFormID);
[[nodiscard]] bool ConsumeLoadedFailure(RE::FormID a_sourceFormID, RE::FormID a_effectSourceFormID);
void Save(SKSE::SerializationInterface& a_intfc, const std::vector<ScriptBindingSource>& a_selectedSources);
bool LoadRecord(Serialization::RecordInfo a_recordInfo, SKSE::SerializationInterface& a_intfc);
void Revert();
}
