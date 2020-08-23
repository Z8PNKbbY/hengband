﻿#include "angband.h"
#include "avatar.h"
#include "spells-status.h"
#include "player-status.h"


/*!
* @brief 魔法棒の効果を発動する
* @param sval オブジェクトのsval
* @param dir 発動の方向ID
* @param powerful 強力発動上の処理ならばTRUE
* @param magic 魔道具術上の処理ならばTRUE
* @return 発動により効果内容が確定したならばTRUEを返す
*/
bool wand_effect(OBJECT_SUBTYPE_VALUE sval, DIRECTION dir, bool powerful, bool magic)
{
	bool ident = FALSE;
	PLAYER_LEVEL lev = powerful ? p_ptr->lev * 2 : p_ptr->lev;
	POSITION rad = powerful ? 3 : 2;

	/* XXX Hack -- Wand of wonder can do anything before it */
	if (sval == SV_WAND_WONDER)
	{
		int vir = virtue_number(V_CHANCE);
		sval = (OBJECT_SUBTYPE_VALUE)randint0(SV_WAND_WONDER);

		if (vir)
		{
			if (p_ptr->virtues[vir - 1] > 0)
			{
				while (randint1(300) < p_ptr->virtues[vir - 1]) sval++;
				if (sval > SV_WAND_COLD_BALL) sval = randint0(4) + SV_WAND_ACID_BALL;
			}
			else
			{
				while (randint1(300) < (0 - p_ptr->virtues[vir - 1])) sval--;
				if (sval < SV_WAND_HEAL_MONSTER) sval = randint0(3) + SV_WAND_HEAL_MONSTER;
			}
		}
		if (sval < SV_WAND_TELEPORT_AWAY)
			chg_virtue(V_CHANCE, 1);
	}

	/* Analyze the wand */
	switch (sval)
	{
		case SV_WAND_HEAL_MONSTER:
		{
			HIT_POINT dam = damroll((powerful ? 20 : 10), 10);
			if (heal_monster(dir, dam)) ident = TRUE;
			break;
		}

		case SV_WAND_HASTE_MONSTER:
		{
			if (speed_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_WAND_CLONE_MONSTER:
		{
			if (clone_monster(dir)) ident = TRUE;
			break;
		}

		case SV_WAND_TELEPORT_AWAY:
		{
			int distance = MAX_SIGHT * (powerful ? 8 : 5);
			if (teleport_monster(dir, distance)) ident = TRUE;
			break;
		}

		case SV_WAND_DISARMING:
		{
			if (disarm_trap(dir)) ident = TRUE;
			if (powerful && disarm_traps_touch()) ident = TRUE;
			break;
		}

		case SV_WAND_TRAP_DOOR_DEST:
		{
			if (destroy_door(dir)) ident = TRUE;
			if (powerful && destroy_doors_touch()) ident = TRUE;
			break;
		}

		case SV_WAND_STONE_TO_MUD:
		{
			HIT_POINT dam = powerful ? 40 + randint1(60) : 20 + randint1(30);
			if (wall_to_mud(dir, dam)) ident = TRUE;
			break;
		}

		case SV_WAND_LITE:
		{
			HIT_POINT dam = damroll((powerful ? 12 : 6), 8);
			msg_print(_("青く輝く光線が放たれた。", "A line of blue shimmering light appears."));
			(void)lite_line(dir, dam);
			ident = TRUE;
			break;
		}

		case SV_WAND_SLEEP_MONSTER:
		{
			if (sleep_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_WAND_SLOW_MONSTER:
		{
			if (slow_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_WAND_CONFUSE_MONSTER:
		{
			if (confuse_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_WAND_FEAR_MONSTER:
		{
			if (fear_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_WAND_HYPODYNAMIA:
		{
			if (hypodynamic_bolt(dir, 80 + lev)) ident = TRUE;
			break;
		}

		case SV_WAND_POLYMORPH:
		{
			if (poly_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_WAND_STINKING_CLOUD:
		{
			fire_ball(GF_POIS, dir, 12 + lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_MAGIC_MISSILE:
		{
			fire_bolt_or_beam(20, GF_MISSILE, dir, damroll(2 + lev / 10, 6));
			ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BOLT:
		{
			fire_bolt_or_beam(20, GF_ACID, dir, damroll(6 + lev / 7, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_CHARM_MONSTER:
		{
			if (charm_monster(dir, MAX(20, lev)))
				ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BOLT:
		{
			fire_bolt_or_beam(20, GF_FIRE, dir, damroll(7 + lev / 6, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BOLT:
		{
			fire_bolt_or_beam(20, GF_COLD, dir, damroll(5 + lev / 8, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BALL:
		{
			fire_ball(GF_ACID, dir, 60 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_ELEC_BALL:
		{
			fire_ball(GF_ELEC, dir, 40 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BALL:
		{
			fire_ball(GF_FIRE, dir, 70 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BALL:
		{
			fire_ball(GF_COLD, dir, 50 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_WONDER:
		{
			msg_print(_("おっと、謎の魔法棒を始動させた。", "Oops.  Wand of wonder activated."));
			break;
		}

		case SV_WAND_DRAGON_FIRE:
		{
			fire_breath(GF_FIRE, dir, (powerful ? 300 : 200), 3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_COLD:
		{
			fire_breath(GF_COLD, dir, (powerful ? 270 : 180), 3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_BREATH:
		{
			HIT_POINT dam;
			EFFECT_ID typ;

			switch (randint1(5))
			{
			case 1:
				dam = 240;
				typ = GF_ACID;
				break;
			case 2:
				dam = 210;
				typ = GF_ELEC;
				break;
			case 3:
				dam = 240;
				typ = GF_FIRE;
				break;
			case 4:
				dam = 210;
				typ = GF_COLD;
				break;
			default:
				dam = 180;
				typ = GF_POIS;
				break;
			}

			if (powerful) dam = (dam * 3) / 2;

			fire_breath(typ, dir, dam, 3);

			ident = TRUE;
			break;
		}

		case SV_WAND_DISINTEGRATE:
		{
			fire_ball(GF_DISINTEGRATE, dir, 200 + randint1(lev * 2), rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_ROCKETS:
		{
			msg_print(_("ロケットを発射した！", "You launch a rocket!"));
			fire_rocket(GF_ROCKET, dir, 250 + lev * 3, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_STRIKING:
		{
			fire_bolt(GF_METEOR, dir, damroll(15 + lev / 3, 13));
			ident = TRUE;
			break;
		}

		case SV_WAND_GENOCIDE:
		{
			fire_ball_hide(GF_GENOCIDE, dir, magic ? lev + 50 : 250, 0);
			ident = TRUE;
			break;
		}
	}
	return ident;
}

/*!
* @brief 魔法棒を使うコマンドのサブルーチン /
* Aim a wand (from the pack or floor).
* @param item 使うオブジェクトの所持品ID
* @return なし
* @details
* <pre>
* Use a single charge from a single item.
* Handle "unstacking" in a logical manner.
* For simplicity, you cannot use a stack of items from the
* ground.  This would require too much nasty code.
* There are no wands which can "destroy" themselves, in the inventory
* or on the ground, so we can ignore this possibility.  Note that this
* required giving "wand of wonder" the ability to ignore destruction
* by electric balls.
* All wands can be "cancelled" at the "Direction?" prompt for free.
* Note that the basic "bolt" wands do slightly less damage than the
* basic "bolt" rods, but the basic "ball" wands do the same damage
* as the basic "ball" rods.
* </pre>
*/
void do_cmd_aim_wand_aux(INVENTORY_IDX item)
{
	DEPTH lev;
	int ident, chance;
	DIRECTION dir;
	object_type *o_ptr;
	bool old_target_pet = target_pet;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &current_floor_ptr->o_list[0 - item];
	}

	/* Mega-Hack -- refuse to aim a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
		msg_print(_("まずは魔法棒を拾わなければ。", "You must first pick up the wands."));
		return;
	}


	/* Allow direction to be cancelled for free */
	if (object_is_aware(o_ptr) && (o_ptr->sval == SV_WAND_HEAL_MONSTER
		|| o_ptr->sval == SV_WAND_HASTE_MONSTER))
		target_pet = TRUE;
	if (!get_aim_dir(&dir))
	{
		target_pet = old_target_pet;
		return;
	}
	target_pet = old_target_pet;

	take_turn(p_ptr, 100);

	/* Get the level */
	lev = k_info[o_ptr->k_idx].level;
	if (lev > 50) lev = 50 + (lev - 50) / 2;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - lev;

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && one_in_(USE_DEVICE - chance + 1))
	{
		chance = USE_DEVICE;
	}

	if (cmd_limit_time_walk(p_ptr)) return;

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE) || (p_ptr->pclass == CLASS_BERSERKER))
	{
		if (flush_failure) flush();
		msg_print(_("魔法棒をうまく使えなかった。", "You failed to use the wand properly."));
		sound(SOUND_FAIL);
		return;
	}

	/* The wand is already empty! */
	if (o_ptr->pval <= 0)
	{
		if (flush_failure) flush();
		msg_print(_("この魔法棒にはもう魔力が残っていない。", "The wand has no charges left."));
		o_ptr->ident |= (IDENT_EMPTY);
		p_ptr->update |= (PU_COMBINE | PU_REORDER);
		p_ptr->window |= (PW_INVEN);

		return;
	}

	sound(SOUND_ZAP);

	ident = wand_effect(o_ptr->sval, dir, FALSE, FALSE);
	p_ptr->update |= (PU_COMBINE | PU_REORDER);

	if (!(object_is_aware(o_ptr)))
	{
		chg_virtue(V_PATIENCE, -1);
		chg_virtue(V_CHANCE, 1);
		chg_virtue(V_KNOWLEDGE, -1);
	}

	/* Mark it as tried */
	object_tried(o_ptr);

	/* Apply identification */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
	}

	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Use a single charge */
	o_ptr->pval--;

	/* Describe the charges in the pack */
	if (item >= 0)
	{
		inven_item_charges(item);
	}

	/* Describe the charges on the floor */
	else
	{
		floor_item_charges(0 - item);
	}
}

/*!
* @brief 魔法棒を使うコマンドのメインルーチン /
* @return なし
*/
void do_cmd_aim_wand(void)
{
	OBJECT_IDX item;
	concptr q, s;

	if (p_ptr->wild_mode) return;
	if (cmd_limit_arena(p_ptr)) return;

	/* Restrict choices to wands */
	item_tester_tval = TV_WAND;

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	q = _("どの魔法棒で狙いますか? ", "Aim which wand? ");
	s = _("使える魔法棒がない。", "You have no wand to aim.");
	if (!choose_object(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	do_cmd_aim_wand_aux(item);
}