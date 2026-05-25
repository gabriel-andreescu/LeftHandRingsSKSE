#pragma once

#include <cstdint>

namespace RingEnchantments {
[[nodiscard]] float GetScale(RE::Actor& a_actor, const RE::TESObjectARMO* a_source);
void DispelEffectsFromSource(RE::Actor& a_actor, const RE::TESObjectARMO& a_source);
void ApplyVirtualRingEnchantment(
    RE::Actor& a_actor,
    RE::TESObjectARMO& a_effectSource,
    const RE::ExtraDataList* a_extraList
);
void RefreshVanillaRingSlotEffects(RE::Actor& a_actor);
}
