#include <windows.h>
#include <psapi.h>
#include "toml++/toml.hpp"
#include "nya_commonhooklib.h"

#include "game.h"

enum eHealthDisplayMode {
	HEALTH_DISPLAY_ZERO,
	HEALTH_DISPLAY_FULL,
	HEALTH_DISPLAY_UPGRADED,
};
int nHealthDisplayMode = HEALTH_DISPLAY_ZERO;
bool bNoIdleGrappleDamage = true;
bool bNoPopstarLead1TackleDamage = true;

void RunOHKO() {
	auto pPlayer = UFG::GetLocalPlayer();
	if (!pPlayer) return;
	auto pHealth = UFG::FindComponentOfType<UFG::HealthComponent>(pPlayer);
	if (!pHealth) return;

	pHealth->m_fMaxHealth = 1;
	switch (nHealthDisplayMode) {
		case HEALTH_DISPLAY_FULL:
			pHealth->m_fMaxUpgradedHealth = 2;
			break;
		case HEALTH_DISPLAY_UPGRADED:
			pHealth->m_fMaxUpgradedHealth = 1;
			break;
		default:
			break;
	}
	if (pHealth->m_fHealth > 1) pHealth->m_fHealth = 1;
	pHealth->m_fArmour = 0;

	UFG::HealthComponent::ms_bEnableAudioEffects = false;
}

auto UpdateGameSystems_RetAddress = (bool(__fastcall*)(float))(module + 0xC290);
bool UpdateGameSystemsPatch(float delta_time) {
	RunOHKO();
	return UpdateGameSystems_RetAddress(delta_time);
}

bool ShouldDamageBeDiscarded(UFG::HealthComponent* pThis, const int iDamage, bool bProjectileDamage) {
	if (bProjectileDamage) return false;

	auto pPlayer = UFG::GetLocalPlayer();
	if (!pPlayer) return false;
	auto pHealth = UFG::FindComponentOfType<UFG::HealthComponent>(pPlayer);
	if (!pHealth) return false;
	if (pHealth != pThis) return false;

	// discard idle damage from grappling, it's 5hp per second or so
	// doing this since multiple missions force you into being grappled and it just results in an instant fail screen
	// there's gotta be a better way to do this, but looking at the track/task/actioncontroller system makes me wanna pull my hair out >.<
	if (iDamage == 5 && bNoIdleGrappleDamage) return true;

	// unavoidable 50 damage when getting tackled by a cop in Popstar Lead 1
	if (iDamage == 50 && bNoPopstarLead1TackleDamage) {
		UFG::qVector3 playerPos = {0,0,0};
		UFG::GetPosition(&playerPos, pPlayer);
		if (playerPos.x > 1140 && playerPos.x < 1150 && playerPos.y > 160 && playerPos.y < 180) return true;
	}
	return false;
}

auto ApplyHealthDamagePatch_RetAddress = (bool(__fastcall*)(UFG::HealthComponent*, const int, UFG::SimObject*, void*, bool))(module + 0x5221C0);
bool ApplyHealthDamagePatch(UFG::HealthComponent *pThis, const int iDamage, UFG::SimObject *pAttacker, void *pHitRecord, bool bProjectileDamage) {
	if (ShouldDamageBeDiscarded(pThis, iDamage, bProjectileDamage)) return false;
	return ApplyHealthDamagePatch_RetAddress(pThis, iDamage, pAttacker, pHitRecord, bProjectileDamage);
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	static bool bInitialized = false;
	if (bInitialized) return TRUE;
	bInitialized = true;

	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			MODULEINFO info;
			GetModuleInformation(GetCurrentProcess(), (HMODULE)module, &info, sizeof(info));
			if ((uintptr_t)info.EntryPoint - (uintptr_t)info.lpBaseOfDll != 0x12BA5B4) {
				MessageBoxA(nullptr, "Unsupported game version! Make sure you're using the latest Steam release (.exe size of 37490688 bytes)", "nya?!~", MB_ICONERROR);
				return TRUE;
			}

			auto config = toml::parse_file("SDHDOneHitKO_gcp.toml");
			bNoIdleGrappleDamage = config["main"]["no_idle_grapple_damage"].value_or(true);
			bNoPopstarLead1TackleDamage = config["main"]["no_cop_tackle_damage"].value_or(true);
			nHealthDisplayMode = config["main"]["health_display_mode"].value_or(HEALTH_DISPLAY_ZERO);

			UpdateGameSystems_RetAddress = (bool (__fastcall*)(float)) NyaHookLib::ReadRelative(module + 0xA401D2 + 1);
			NyaHookLib::PatchRelative(NyaHookLib::CALL, module + 0xA401D2, &UpdateGameSystemsPatch);

			if (bNoIdleGrappleDamage || bNoPopstarLead1TackleDamage) {
				ApplyHealthDamagePatch_RetAddress = (bool (__fastcall *)(UFG::HealthComponent *, const int,
																		 UFG::SimObject *, void *,
																		 bool)) NyaHookLib::ReadRelative(
						module + 0x2F5E8F + 1);
				NyaHookLib::PatchRelative(NyaHookLib::CALL, module + 0x2F5E8F, &ApplyHealthDamagePatch);
			}
		} break;
		default:
			break;
	}
	return TRUE;
}