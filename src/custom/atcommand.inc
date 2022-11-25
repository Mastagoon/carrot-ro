// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Atcommands
 * Place the body of custom atcommands in this file.
 * Format:
 *	ACMD_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/

//ACMD_FUNC(newcommand)
//{
//	clif_displaymessage(fd, "It works!");
//	clif_specialeffect(&sd->bl, EF_HEARTCASTING, AREA);
//	return 0;
//}

// [Vykimo] No recall command
ACMD_FUNC(norecall) {
	nullpo_retr(-1, sd);
	sd->state.norecall = (sd->state.norecall==0);
	if(sd->state.norecall)	clif_displaymessage(fd, "@norecall : Êã ÇÓÊËäÇÄß ãä ãåÇÑÉ ÇáÅÓÊÏÚÇÁ ÇáÚÇÌá.");
	else clif_displaymessage(fd, "@norecall : ÓíÊã ÇÓÊÏÚÇÆß ÈãåÇÑÉ ÇáÅÓÊÏÚÇÁ ÇáÚÇÌá.");
	return 0;
}

void atcommand_battlestats_sub(int fd, struct map_session_data *sd) {
	status_data *status = status_get_status_data(&sd->bl);
	char output[CHAT_SIZE_MAX];

#define battlestats_msg(__msg__) (__msg__); clif_displaymessage(fd, output)

	battlestats_msg(sprintf(output, "Battle Status: %s", sd->status.name));

	clif_displaymessage(fd, "|--- Basic Info ---|");
	battlestats_msg(sprintf(output, "   -> ATK: %d + %d", pc_leftside_atk(sd), pc_rightside_atk(sd)));
	battlestats_msg(sprintf(output, "   -> MATK: %d + %d", pc_leftside_matk(sd), pc_rightside_matk(sd)));
	battlestats_msg(sprintf(output, "   -> HIT: %d", status->hit));
	battlestats_msg(sprintf(output, "   -> CRIT: %d", status->cri / 10));
	battlestats_msg(sprintf(output, "   -> DEF: %d + %d", pc_leftside_def(sd), pc_rightside_def(sd)));
	battlestats_msg(sprintf(output, "   -> MDEF: %d + %d", pc_leftside_mdef(sd), pc_rightside_mdef(sd)));
	battlestats_msg(sprintf(output, "   -> FLEE: %d + %d", status->flee, status->flee2 / 10));
	battlestats_msg(sprintf(output, "   -> ASPD: %d", (2000 - status->amotion) / 10));

	// clif_displaymessage(fd, "|--- Cast Time, Cooldowns, And After-cast Delay ---|");
	// battlestats_msg(sprintf(output, "   -> Cast Rate: %d%%", sd->castrate));
	// clif_displaymessage(fd, "Cast Reduction From Status (2DEX + INT = 530) :");
	// float statuscast = 2*sd->base_status.dex + sd->base_status.int_;
	// battlestats_msg(sprintf(output, "   -> %d / 530, %d%%", statuscast, 100*statuscast/530));
	battlestats_msg(sprintf(output, "   -> After Cast Delay: %d%%", sd->delayrate));

	clif_displaymessage(fd, "|--- Bonus Physical Damages ---|");
	if (sd->right_weapon.addsize[SZ_MEDIUM] + sd->left_weapon.addsize[SZ_MEDIUM]) {
		battlestats_msg(sprintf(output, "   -> Medium Size: %d%%", sd->right_weapon.addsize[SZ_MEDIUM] + sd->left_weapon.addsize[SZ_MEDIUM]));
	}
	if (sd->right_weapon.addsize[SZ_ALL] + sd->left_weapon.addsize[SZ_ALL]) {
		battlestats_msg(sprintf(output, "   -> All Sizes: %d%%", sd->right_weapon.addsize[SZ_ALL] + sd->left_weapon.addsize[SZ_ALL]));
	}
	if (sd->right_weapon.addrace[RC_PLAYER] + sd->left_weapon.addrace[RC_PLAYER]) {
		battlestats_msg(sprintf(output, "   -> Player: %d%%", sd->right_weapon.addrace[RC_PLAYER] + sd->left_weapon.addrace[RC_PLAYER]));
	}
	if (sd->right_weapon.addrace[RC_ALL] + sd->left_weapon.addrace[RC_ALL]) {
		battlestats_msg(sprintf(output, "   -> All Races: %d%%", sd->right_weapon.addrace[RC_ALL] + sd->left_weapon.addrace[RC_ALL]));
	}
	if (sd->right_weapon.addclass[CLASS_NORMAL] + sd->left_weapon.addclass[CLASS_NORMAL]) {
		battlestats_msg(sprintf(output, "   -> Normal Class: %d%%", sd->right_weapon.addclass[CLASS_NORMAL] + sd->left_weapon.addclass[CLASS_NORMAL]));
	}
	if (sd->right_weapon.addclass[CLASS_ALL] + sd->left_weapon.addclass[CLASS_ALL]) {
		battlestats_msg(sprintf(output, "   -> All Classes: %d%%", sd->right_weapon.addclass[CLASS_ALL] + sd->left_weapon.addclass[CLASS_ALL]));
	}

	clif_displaymessage(fd, "|--- Bonus Magic Damages ---|");
	if (sd->magic_addsize[SZ_MEDIUM]) {
		battlestats_msg(sprintf(output, "   -> Medium Size: %d%%", sd->magic_addsize[SZ_MEDIUM]));
	}
	if (sd->magic_addsize[SZ_ALL]) {
		battlestats_msg(sprintf(output, "   -> All Sizes: %d%%", sd->magic_addsize[SZ_ALL]));
	}
	if (sd->magic_addrace[RC_PLAYER]) {
		battlestats_msg(sprintf(output, "   -> Player: %d%%", sd->magic_addrace[RC_PLAYER]));
	}
	if (sd->magic_addrace[RC_ALL]) {
		battlestats_msg(sprintf(output, "   -> All Races: %d%%", sd->magic_addrace[RC_ALL]));
	}
	if (sd->magic_addclass[CLASS_NORMAL]) {
		battlestats_msg(sprintf(output, "   -> Normal Class: %d%%", sd->magic_addclass[CLASS_NORMAL]));
	}
	if (sd->magic_addclass[CLASS_ALL]) {
		battlestats_msg(sprintf(output, "   -> All Classes: %d%%", sd->magic_addclass[CLASS_ALL]));
	}

	clif_displaymessage(fd, "|--- Elemental Resistance ---|");
	struct {
		const char *format;
		int value;
	} ele_resist_info[] = {
		{ "   -> Neutral: %d%%", sd->subele_script[ELE_NEUTRAL] },
		{ "   -> Water: %d%%", sd->subele_script[ELE_WATER] },
		{ "   -> Earth: %d%%", sd->subele_script[ELE_EARTH] },
		{ "   -> Fire: %d%%", sd->subele_script[ELE_FIRE] },
		{ "   -> Wind: %d%%", sd->subele_script[ELE_WIND] },
		{ "   -> Poison: %d%%", sd->subele_script[ELE_POISON] },
		{ "   -> Holy: %d%%", sd->subele_script[ELE_HOLY] },
		{ "   -> Dark: %d%%", sd->subele_script[ELE_DARK] },
		{ "   -> Ghost: %d%%", sd->subele_script[ELE_GHOST] },
		{ "   -> Undead: %d%%", sd->subele_script[ELE_UNDEAD] },
		{ "   -> ALL: %d%%", sd->subele_script[ELE_ALL] },
	};
	for (int i = 0; i < ARRAYLENGTH(ele_resist_info); i++) {
		if (ele_resist_info[i].value == 0)
			continue;
		battlestats_msg(sprintf(output, ele_resist_info[i].format, ele_resist_info[i].value));
	}

	clif_displaymessage(fd, "|--- Race Resistance ---|");
	struct {
		const char *format;
		int value;
	} race_resist_info[] = {
		{ "   -> Formless: %d%%", sd->subrace[RC_FORMLESS] },
		{ "   -> Undead: %d%%", sd->subrace[RC_UNDEAD] },
		{ "   -> Beast: %d%%", sd->subrace[RC_BRUTE] },
		{ "   -> Plant: %d%%", sd->subrace[RC_PLANT] },
		{ "   -> Insect: %d%%", sd->subrace[RC_INSECT] },
		{ "   -> Fish: %d%%", sd->subrace[RC_FISH] },
		{ "   -> Demon: %d%%", sd->subrace[RC_DEMON] },
		{ "   -> DemiHuman: %d%%", sd->subrace[RC_DEMIHUMAN] },
		{ "   -> Angel: %d%%", sd->subrace[RC_ANGEL] },
		{ "   -> Dragon: %d%%", sd->subrace[RC_DRAGON] },
		{ "   -> Player: %d%%", sd->subrace[RC_PLAYER] },
		{ "   -> ALL: %d%%", sd->subrace[RC_ALL] },
		{ "   -> Normal Class: %d%%", sd->subclass[CLASS_NORMAL] },
	};
	for (int i = 0; i < ARRAYLENGTH(race_resist_info); i++) {
		if (race_resist_info[i].value == 0)
			continue;
		battlestats_msg(sprintf(output, race_resist_info[i].format, race_resist_info[i].value));
	}

#undef battlestats_msg
}

ACMD_FUNC(battlestats) {
	nullpo_retr(-1, sd);
	atcommand_battlestats_sub(fd, sd);
	return 0;
}

ACMD_FUNC(enemystats) {
	struct map_session_data *pl_sd;

	nullpo_retr(-1, sd);

	memset(atcmd_player_name, '\0', sizeof atcmd_player_name);

	if (!message || !*message || sscanf(message, "%23[^\n]", atcmd_player_name) < 1) {
		clif_displaymessage(fd, "Please enter a player name (usage: @enemystats <char name>)");
		return -1;
	}

	pl_sd = map_nick2sd(atcmd_player_name, true);
	if (pl_sd == NULL ||
		strncmp(pl_sd->status.name, atcmd_player_name, NAME_LENGTH) != 0 ||
		(pc_has_permission(pl_sd, PC_PERM_HIDE_SESSION) && pc_get_group_level(pl_sd) > pc_get_group_level(sd))
		) {
		clif_displaymessage(fd, msg_txt(sd, 3)); // Character not found.
		return -1;
	}

	atcommand_battlestats_sub(fd, pl_sd);
	return 0;
}