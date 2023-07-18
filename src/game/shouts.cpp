#include "shouts.h"

#include "string_util.h"
#include "offset.h"
#include "player.h"

// For game implementation reasons, this also includes spells.
// Lesser powers are spells that go into the shout slot, IIUC.

namespace game
{
	void unequip_spell(RE::BSScript::IVirtualMachine* a_vm,
		RE::VMStackID a_stack_id,
		RE::Actor* a_actor,
		RE::SpellItem* a_spell,
		uint32_t slot)
	{
		using func_t = decltype(&unequip_spell);
		const REL::Relocation<func_t> func{ REL::ID(offset::get_un_equip_spell) };
		func(a_vm, a_stack_id, a_actor, a_spell, slot);
	}

	void un_equip_shout(RE::BSScript::IVirtualMachine* a_vm,
		RE::VMStackID a_stack_id,
		RE::Actor* a_actor,
		RE::TESShout* a_shout)
	{
		using func_t = decltype(&un_equip_shout);
		const REL::Relocation<func_t> func{ REL::ID(offset::get_un_equipShout) };
		func(a_vm, a_stack_id, a_actor, a_shout);
	}

	void unequipShoutSlot(RE::PlayerCharacter*& player)
	{
		auto* selected_power = player->GetActorRuntimeData().selectedPower;
		if (selected_power)
		{
			logger::trace(
				"unequipping shout/power formid=0x{};"sv, util::string_util::int_to_hex(selected_power->formID));
			if (selected_power->Is(RE::FormType::Shout))
			{
				un_equip_shout(nullptr, 0, player, selected_power->As<RE::TESShout>());
			}
			else if (selected_power->Is(RE::FormType::Spell))
			{
				//power
				//2=other
				unequip_spell(nullptr, 0, player, selected_power->As<RE::SpellItem>(), 2);
			}
		}
	}

	void equipShoutByForm(RE::TESForm* form, RE::PlayerCharacter*& player)
	{
		logger::trace("tring to equip shout; name='{}';"sv, form->GetName());

		if (!form->Is(RE::FormType::Shout))
		{
			logger::warn("this is not a shout! name='{}';"sv, form->GetName());
			return;
		}

		if (const auto selected_power = player->GetActorRuntimeData().selectedPower; selected_power)
		{
			logger::trace("current power:  name='{}'; is-shout={}; is-spell={};"sv,
				selected_power->GetName(),
				selected_power->Is(RE::FormType::Shout),
				selected_power->Is(RE::FormType::Spell));
			if (selected_power->formID == form->formID)
			{
				logger::trace("shout already equipped; moving on."sv, form->GetName());
				return;
			}
		}

		auto* shout = form->As<RE::TESShout>();
		if (!player::has_shout(player, shout))
		{
			logger::warn("player does not know shout; name='{}';"sv, shout->GetName());
			return;
		}

		RE::ActorEquipManager::GetSingleton()->EquipShout(player, shout);
		logger::debug("shout equipped! name='{}'"sv, form->GetName());
	}
}