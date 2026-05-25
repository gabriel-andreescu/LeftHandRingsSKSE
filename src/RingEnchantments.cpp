#include "RingEnchantments.h"

#include "Inventory.h"
#include "Settings.h"
#include "VirtualRings.h"

#include <algorithm>
#include <vector>

namespace RE {
MagicTarget::IPostCreationModification::~IPostCreationModification() = default;
}

namespace RingEnchantments {
namespace {
    struct VanillaRingSlotState {
        RE::TESObjectARMO* ring {nullptr};
        RE::ExtraDataList* extraList {nullptr};
        bool enchanted {false};
    };

    [[nodiscard]] RE::TESObjectARMO* GetVanillaRingSlotArmor(RE::Actor& a_actor) {
        auto* armor = a_actor.GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kRing);
        return Inventory::IsRing(armor) ? armor : nullptr;
    }

    [[nodiscard]] VanillaRingSlotState GetVanillaRingSlotState(RE::Actor& a_actor) {
        auto* ring = GetVanillaRingSlotArmor(a_actor);
        if (!ring) {
            return {};
        }

        const auto state = Inventory::GetSourceState(a_actor, *ring);
        if (!state.rightWorn) {
            return {};
        }

        return VanillaRingSlotState {
            .ring = ring,
            .extraList = state.rightWornExtraList,
            .enchanted = state.HasRightWornEnchantment(),
        };
    }

    [[nodiscard]] bool IsLiveRingEffect(const RE::ActiveEffect* a_effect, const RE::TESObjectARMO* a_source) {
        return a_effect
               && a_effect->source
               == a_source
               && a_effect->flags.none(RE::ActiveEffect::Flag::kInactive, RE::ActiveEffect::Flag::kDispelled);
    }

    [[nodiscard]] std::vector<RE::ActiveEffect*> CollectLiveEffects(
        RE::Actor& a_actor,
        const RE::TESObjectARMO* a_source
    ) {
        auto* magicTarget = a_actor.AsMagicTarget();
        if (!magicTarget) {
            return {};
        }

        auto* activeEffects = magicTarget->GetActiveEffectList();
        if (!activeEffects) {
            return {};
        }

        std::vector<RE::ActiveEffect*> effects;
        for (auto* activeEffect : *activeEffects) {
            if (IsLiveRingEffect(activeEffect, a_source)) {
                effects.push_back(activeEffect);
            }
        }

        return effects;
    }

    void DispelEffects(const std::vector<RE::ActiveEffect*>& a_effects) {
        for (auto* effect : a_effects) {
            effect->Dispel(true);
        }
    }

    [[nodiscard]] RE::EnchantmentItem* ResolveEnchantment(
        const RE::TESObjectARMO& a_source,
        const RE::ExtraDataList* a_extraList
    ) {
        if (a_source.formEnchanting) {
            return a_source.formEnchanting;
        }

        if (!a_extraList) {
            return nullptr;
        }

        const auto customData = Inventory::ReadCustomEnchantment(*a_extraList);
        return customData ? customData->enchantment : nullptr;
    }

    class MarkEnchantmentEffect final : public RE::MagicTarget::IPostCreationModification {
    public:
        void ModifyActiveEffect(RE::ActiveEffect* a_effect) override {
            a_effect->flags.set(RE::ActiveEffect::Flag::kEnchanting);
        }
    };

    void ApplyEffect(
        RE::MagicTarget& a_target,
        RE::Actor& a_actor,
        RE::TESObjectARMO& a_effectSource,
        RE::EnchantmentItem& a_enchantment,
        RE::Effect& a_effect,
        RE::MagicTarget::IPostCreationModification& a_callback
    ) {
        RE::MagicTarget::AddTargetData data {};
        data.caster = &a_actor;
        data.magicItem = &a_enchantment;
        data.effect = &a_effect;
        data.source = &a_effectSource;
        data.magnitude = a_effect.effectItem.magnitude;
        data.power = 1.0F;
        data.castingSource = RE::MagicSystem::CastingSource::kInstant;
        data.areaTarget = false;
        data.dualCasted = false;
        data.postCreationCallback = &a_callback;

        static_cast<void>(a_target.AddTarget(data));
    }

    [[nodiscard]] std::uint32_t CountEquippedEnchantedRings(RE::Actor& a_actor) {
        auto count = VirtualRings::CountEnchantedVirtualRings();
        const auto vanillaSlot = GetVanillaRingSlotState(a_actor);
        if (vanillaSlot.enchanted) {
            ++count;
        }

        return count;
    }

    [[nodiscard]] bool SourceIsCountedEnchantedRing(RE::Actor& a_actor, const RE::TESObjectARMO* a_source) {
        if (!a_source) {
            return false;
        }

        if (VirtualRings::IsEnchantedVirtualRingEffectSource(a_source)) {
            return true;
        }

        const auto vanillaSlot = GetVanillaRingSlotState(a_actor);
        return vanillaSlot.enchanted && vanillaSlot.ring == a_source;
    }
}

float GetScale(RE::Actor& a_actor, const RE::TESObjectARMO* a_source) {
    if (!SourceIsCountedEnchantedRing(a_actor, a_source)) {
        return 1.0F;
    }

    const auto count = CountEquippedEnchantedRings(a_actor);
    const auto scale = Settings::GetSingleton()->GetRingEnchantmentScale(count);
    return std::clamp(scale, 0.0F, 1.0F);
}

void DispelEffectsFromSource(RE::Actor& a_actor, const RE::TESObjectARMO& a_source) {
    DispelEffects(CollectLiveEffects(a_actor, &a_source));
}

void ApplyVirtualRingEnchantment(
    RE::Actor& a_actor,
    RE::TESObjectARMO& a_effectSource,
    const RE::ExtraDataList* a_extraList
) {
    auto* enchantment = ResolveEnchantment(a_effectSource, a_extraList);
    if (!enchantment) {
        return;
    }

    auto* magicTarget = a_actor.AsMagicTarget();
    if (!magicTarget) {
        logger::warn(
            "RingEnchantments: apply skipped | source={:08X} | enchantment={:08X} | reason=noMagicTarget",
            a_effectSource.GetFormID(),
            enchantment->GetFormID()
        );
        return;
    }

    MarkEnchantmentEffect callback;

    for (auto* effect : enchantment->effects) {
        if (!effect) {
            continue;
        }

        ApplyEffect(*magicTarget, a_actor, a_effectSource, *enchantment, *effect, callback);
    }
}

void RefreshVanillaRingSlotEffects(RE::Actor& a_actor) {
    const auto vanillaSlot = GetVanillaRingSlotState(a_actor);
    if (!vanillaSlot.enchanted || !vanillaSlot.ring) {
        return;
    }

    DispelEffects(CollectLiveEffects(a_actor, vanillaSlot.ring));
    a_actor.UpdateArmorAbility(vanillaSlot.ring, vanillaSlot.extraList);
}
}
