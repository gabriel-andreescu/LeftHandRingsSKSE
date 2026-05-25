#pragma once

#include <cstdint>

namespace UI {
enum class SelectionOrigin : std::uint8_t {
    kInventoryMenu,
    kFavoritesMenu,
};

void InstallMenuEventSink();
void RegisterInventoryData();
void RefreshRingRows();
void RefreshFavoritesRows();
void QueueRefreshAfterRingEquip(RE::FormID a_ringFormID);
void RefreshItemRowsForRing(RE::Actor& a_actor, const RE::TESObjectARMO* a_ring);
[[nodiscard]] bool SelectEntryForLeftHand(RE::InventoryEntryData* a_entry, SelectionOrigin a_origin);
}
