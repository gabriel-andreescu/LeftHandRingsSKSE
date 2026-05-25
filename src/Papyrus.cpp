#include "Papyrus.h"

#include "Settings.h"
#include "UI.h"
#include "VirtualRings.h"

namespace Papyrus {
namespace {
    void RefreshAfterEnchantmentStrengthSettingsReload() {
        VirtualRings::RequestRefresh();
        UI::RefreshRingRows();
    }

    void OnConfigCloseNative([[maybe_unused]] RE::TESQuest* a_quest) {
        const auto reload = Settings::GetSingleton()->Reload();
        if (!reload.Changed()) {
            return;
        }

        if (reload.enchantmentStrengthChanged) {
            logger::info("Settings: enchantment strength changed | action=refreshRingEnchantments");
            stl::add_task(RefreshAfterEnchantmentStrengthSettingsReload);
        }
    }

    bool RegisterMCM(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction("OnConfigCloseNative", "LeftHandRingsSKSE_MCM", OnConfigCloseNative);
        logger::info("Papyrus: MCM reload callback registered");
        return true;
    }
}

void Register() {
    const auto* papyrus = SKSE::GetPapyrusInterface();
    if (!papyrus) {
        logger::critical("Papyrus: register skipped | reason=noInterface");
        return;
    }

    papyrus->Register(RegisterMCM);
}
}
