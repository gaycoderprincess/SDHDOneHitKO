auto module = (uintptr_t)GetModuleHandle(nullptr);

namespace UFG {
	template<typename T>
	class qArray {
	public:
		uint32_t size;
		uint32_t capacity;
		T* p;
	};

	class HealthComponent {
	public:
		uint8_t _0[0x54];
		float m_fHealth;
		float m_fMinHealth;
		float m_fMaxHealth;
		float m_fMaxUpgradedHealth;
		float m_NonPlayerDamageMultiplier;
		float m_ProjectileDamageMultiplier;
		float m_fHealthBoost;
		float m_fArmour;

		static inline auto& _TypeUID = *(uint32_t*)(module + 0x20870B8);
		static inline auto& ms_bEnableAudioEffects = *(bool*)(module + 0x20872F5);
	};

	class SimComponentHolder {
	public:
		void* m_pComponent;
		uint32_t m_TypeUID;
	};

	class SimObject {
	public:
		uint8_t _0[0x60];
		qArray<SimComponentHolder> m_Components;
	};

	template<typename T>
	T* FindComponentOfType(UFG::SimObject* obj)
	{
		for (int i = 0; i < obj->m_Components.size; i++)
		{
			auto p = &obj->m_Components.p[i];
			if (p->m_TypeUID == T::_TypeUID && p->m_pComponent) return (T*)p->m_pComponent;
		}
		return nullptr;
	}

	auto GetLocalPlayer = (SimObject*(__fastcall*)())(module + 0x360930);
}