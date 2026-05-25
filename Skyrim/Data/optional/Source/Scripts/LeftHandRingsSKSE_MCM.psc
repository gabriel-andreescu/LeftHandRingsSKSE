Scriptname LeftHandRingsSKSE_MCM extends MCM_ConfigBase

Bool Property bFixedStrengthSelected Auto Hidden

Function SyncEnchantmentStrengthMode()
    Int mode = GetModSettingInt("iEnchantmentStrengthMode:General")
    bFixedStrengthSelected = mode == 1
EndFunction

Event OnConfigInit()
    SyncEnchantmentStrengthMode()
EndEvent

Event OnConfigOpen()
    SyncEnchantmentStrengthMode()
EndEvent

Event OnSettingChange(String a_ID)
    If a_ID == "iEnchantmentStrengthMode:General"
        SyncEnchantmentStrengthMode()
        ForcePageReset()
    EndIf
EndEvent

Event OnConfigClose()
    OnConfigCloseNative()
EndEvent

Function OnConfigCloseNative() Native
