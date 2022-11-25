// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Script Commands
 * Place the body of custom script commands in this file.
 * Format:
 *	BUILDIN_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/

//BUILDIN_FUNC(example)
//{
//	ShowInfo("buildin_example: It works!\n");
//	script_pushint(st,1);
//	return 0;
//}
BUILDIN_FUNC(pcblockall)
{
	struct block_list *bl = NULL;

	if (script_getnum(st, 2))
		bl = map_id2bl(script_getnum(st,2));
	else
		bl = map_id2bl(st->rid);

	if (bl) {
		struct unit_data *ud = unit_bl2ud(bl);

		if (ud)
			ud->state.blockedall = script_getnum(st,3) > 0;
	}

	return SCRIPT_CMD_SUCCESS;
}
BUILDIN_FUNC(issitting) {
	TBL_PC* sd;
	if(script_rid2sd(sd) == NULL )
		return 0;// no player attached, report source

	if( pc_issit(sd) )
		script_pushint(st, 1);
	else
		script_pushint(st, 0);

	return 0;
}
// hotkeys
BUILDIN_FUNC(gethotkeys) {
	TBL_PC *sd;
	TBL_PC *tsd;
	int i;

	if( !script_rid2sd(sd) )
		return SCRIPT_CMD_FAILURE;

	if (!script_charid2sd(2,tsd))
		return SCRIPT_CMD_FAILURE;

	for(i=0;i<MAX_HOTKEYS;i++){
		pc_setreg(sd,reference_uid(add_str("@hotkey_type"), i),tsd->status.hotkeys[i].type);
		pc_setreg(sd,reference_uid(add_str("@hotkey_id"), i),tsd->status.hotkeys[i].id);
		pc_setreg(sd,reference_uid(add_str("@hotkey_lv"), i),tsd->status.hotkeys[i].lv);
		pc_setreg(sd,reference_uid(add_str("@hotkey_pos"), i),i);
	}
	pc_setreg(sd,add_str("@hotkey_count"),i);

	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(updatehotkey) {
	TBL_PC *sd;
	int type,id,lv,pos,j, flag = 0;
	short i;
	struct script_data *data;

	if (!script_charid2sd(7,sd))
		return SCRIPT_CMD_FAILURE;

	type = script_getnum(st,2);
	if(type < 0 || type > 1){
		clif_displaymessage(sd->fd, "Update Hotkey failed. Invalid type.");
		return SCRIPT_CMD_FAILURE;
	}
	data = script_getdata(st, 3);
	get_val(st, data); // Convert into value in case of a variable
	id = ( data_isstring(data) ? skill_name2id(script_getstr(st,3)) : script_getnum(st,3) );
	lv = script_getnum(st,4);
	pos = script_getnum(st,5);
	if(pos < 0 || pos >= MAX_HOTKEYS){
		clif_displaymessage(sd->fd, "Update Hotkey failed. Invalid hotkey position.");
		return SCRIPT_CMD_FAILURE;
	}
	if( script_hasdata(st,6) )
		flag = script_getnum(st,6);

	if(type == 0 && id > 0){ // item
		ARR_FIND(0, MAX_INVENTORY, i, sd->inventory.u.items_inventory[i].nameid == id);
		if(i<MAX_INVENTORY){
			if(sd->inventory.u.items_inventory[i].amount != lv)
				lv = sd->inventory.u.items_inventory[i].amount;
		} else
			lv = 0;
	} else if(type == 1 && id > 0){ // skill
		j = pc_checkskill(sd,id);
		if(j < lv)
			lv = j;
	}

	if(flag == 1){ // Delete duplicate on same row
		i = 0;
		if(pos >= 0 && pos <= 8){
			ARR_FIND(0, 9, i, sd->status.hotkeys[i].type == type && sd->status.hotkeys[i].id == id && sd->status.hotkeys[i].lv == lv);
			if(i < 9){
				sd->status.hotkeys[i].type = 0;
				sd->status.hotkeys[i].id = 0;
				sd->status.hotkeys[i].lv = 0;
			}
		} else if(pos >= 9 && pos <= 17){
			ARR_FIND(9, 18, i, sd->status.hotkeys[i].type == type && sd->status.hotkeys[i].id == id && sd->status.hotkeys[i].lv == lv);
			if(i < 18){
				sd->status.hotkeys[i].type = 0;
				sd->status.hotkeys[i].id = 0;
				sd->status.hotkeys[i].lv = 0;
			}
		} else if(pos >= 18 && pos <= 26){
			ARR_FIND(18, 27, i, sd->status.hotkeys[i].type == type && sd->status.hotkeys[i].id == id && sd->status.hotkeys[i].lv == lv);
			if(i < 27){
				sd->status.hotkeys[i].type = 0;
				sd->status.hotkeys[i].id = 0;
				sd->status.hotkeys[i].lv = 0;
			}
		}
#if PACKETVER >= 20090603
		else if(pos >= 27 && pos <= 35){
			ARR_FIND(27, 36, i, sd->status.hotkeys[i].type == type && sd->status.hotkeys[i].id == id && sd->status.hotkeys[i].lv == lv);
			if(i < 36){
				sd->status.hotkeys[i].type = 0;
				sd->status.hotkeys[i].id = 0;
				sd->status.hotkeys[i].lv = 0;
			}
		}
#endif
#if PACKETVER >= 20090617
		else if(pos >= 36 && pos < MAX_HOTKEYS){
			ARR_FIND(36, MAX_HOTKEYS, i, sd->status.hotkeys[i].type == type && sd->status.hotkeys[i].id == id && sd->status.hotkeys[i].lv == lv);
			if(i < MAX_HOTKEYS){
				sd->status.hotkeys[i].type = 0;
				sd->status.hotkeys[i].id = 0;
				sd->status.hotkeys[i].lv = 0;
			}
		}
#endif
	} else if(flag == 2){ // Delete duplicate on all rows
		for(i = 0; i < MAX_HOTKEYS; i++){
			if(sd->status.hotkeys[i].type == type && sd->status.hotkeys[i].id == id && sd->status.hotkeys[i].lv == lv){
				sd->status.hotkeys[i].type = 0;
				sd->status.hotkeys[i].id = 0;
				sd->status.hotkeys[i].lv = 0;
			}
		}
	}

	sd->status.hotkeys[pos].type = type;
	sd->status.hotkeys[pos].id = id;
	sd->status.hotkeys[pos].lv = lv;

	clif_hotkeys_send(sd,0);

	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(clearhotkeys){
	TBL_PC *sd;
	int i;
	if (!script_charid2sd(2,sd))
		return SCRIPT_CMD_FAILURE;

	for(i=0;i<MAX_HOTKEYS;i++){
		sd->status.hotkeys[i].type = 0;
		sd->status.hotkeys[i].id = 0;
		sd->status.hotkeys[i].lv = 0;
	}
	clif_hotkeys_send(sd,0);
	return SCRIPT_CMD_SUCCESS;
}
/*BUILDIN_FUNC(sitdown) {
	TBL_PC* sd;
	if(script_rid2sd(sd) == NULL)
		return 0;
	pc_setsit(sd);
    clif_sitting(&sd->bl);
	return 1;
}

BUILDIN_FUNC(standup) {
	TBL_PC* sd;
	if(script_rid2sd(sd) == NULL)
		return 0;
	pc_setstand(sd);
    clif_standing(&sd->bl);
	return 1;
}*/

/*==========================================
 * Spawn a monster with group id:
 * *monster "<map name>",<x>,<y>,"<name to show>",<mob id>,<amount>,<battle_group_id>{,"<event label>",<size>,<ai>};
 *------------------------------------------*/
BUILDIN_FUNC(monster2)
{
	const char* mapn	= script_getstr(st,2);
	int x				= script_getnum(st,3);
	int y				= script_getnum(st,4);
	const char* str		= script_getstr(st,5);
	int class_			= script_getnum(st,6);
	int amount			= script_getnum(st,7);
	int battle_group_id = script_getnum(st,8);
	const char* event	= "";
	unsigned int size	= SZ_SMALL;
	enum mob_ai ai		= AI_NONE;

	struct map_session_data* sd;
	int16 m;
	int i;

	if (script_hasdata(st, 9)) {
		event = script_getstr(st, 9);
		check_event(st, event);
	}

	// ShowInfo("buildin_monster: Spawning mob with battle_group_id of %d\n", battle_group_id);

	if (script_hasdata(st, 10)) {
		size = script_getnum(st, 10);
		if (size > SZ_BIG) {
			ShowWarning("buildin_monster: Attempted to spawn non-existing size %d for monster class %d\n", size, class_);
			return SCRIPT_CMD_FAILURE;
		}
	}

	if (script_hasdata(st, 11)) {
		ai = static_cast<enum mob_ai>(script_getnum(st, 11));
		if (ai >= AI_MAX) {
			ShowWarning("buildin_monster: Attempted to spawn non-existing ai %d for monster class %d\n", ai, class_);
			return SCRIPT_CMD_FAILURE;
		}
	}

	if (class_ >= 0 && !mobdb_checkid(class_)) {
		ShowWarning("buildin_monster: Attempted to spawn non-existing monster class %d\n", class_);
		return SCRIPT_CMD_FAILURE;
	}

	sd = map_id2sd(st->rid);

	if (sd && strcmp(mapn, "this") == 0)
		m = sd->bl.m;
	else
		m = map_mapname2mapid(mapn);

	for(i = 0; i < amount; i++) { //not optimised
		int mobid = mob_once_spawn(sd, m, x, y, str, class_, 1, event, size, ai, battle_group_id);

		if (mobid)
			mapreg_setreg(reference_uid(add_str("$@mobid"), i), mobid);
	}

	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(setbattlegroup)
{
	struct block_list* bl;
	int group_id = script_getnum(st, 3);
	ShowInfo("group id is: %d\n", group_id);

	if(!script_rid2bl(2,bl) || !group_id)
	{
		script_pushint(st, -1);
		return SCRIPT_CMD_FAILURE;
	}
	TBL_MOB* md;
	TBL_PC* sd;

	switch(bl->type) {
		case BL_MOB: 
			md = map_id2md(bl->id);
			md->battle_group_id = group_id;
			break;
		case BL_PC: 
			sd = map_id2sd(bl->id);
			sd->battle_group_id = group_id;
			break;
		default:
			ShowError("setbattlegroup: Error! invalid type provided.");
			script_pushint(st, -1);
			return SCRIPT_CMD_FAILURE;
			break;
	}

	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(getbattlegroup)
{
	struct block_list* bl;
	if(!script_rid2bl(2,bl))
	{
		script_pushint(st, -1);
		return SCRIPT_CMD_FAILURE;
	}
	TBL_MOB* md;
	TBL_PC* sd;

	switch(bl->type) {
		case BL_MOB: 
			md = map_id2md(bl->id);
			script_pushint(st,md->battle_group_id);
			break;
		case BL_PC: 
			sd = map_id2sd(bl->id);
			script_pushint(st,sd->battle_group_id);
			break;
		default:
			ShowError("setbattlegroup: Error! invalid type provided.");
			script_pushint(st, -1);
			return SCRIPT_CMD_FAILURE;
			break;
	}
	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(checkmap)
{
	int16 m;
	const char *str;

	str=script_getstr(st,2);

	m = map_mapname2mapid(str);
	if (m < 0) {
		ShowWarning("buildin_getmapflag: Invalid map name %s.\n", str);
		return SCRIPT_CMD_FAILURE;
	}

	if(map_getmapflag(m, MF_PVP) || map_getmapflag(m, MF_PVP_NOGUILD) || map_getmapflag(m, MF_PVP_NOPARTY)) {
		script_pushint(st, 1);
		return SCRIPT_CMD_SUCCESS;
	}

	if(map_getmapflag(m, MF_GVG) || map_getmapflag(m, MF_GVG_NOPARTY)) {
		script_pushint(st, 2);
		return SCRIPT_CMD_SUCCESS;
	}

	script_pushint(st, 0);
		return SCRIPT_CMD_SUCCESS;
}