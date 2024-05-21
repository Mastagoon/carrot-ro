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

BUILDIN_FUNC(readstat)
{
	int value;
	struct script_data *data = script_getdata(st, 2);
	TBL_PC *sd = NULL;
	if(data_isint(data)) {
		value = data->u.num;
	} else {
		script_pushint(st, -1);
		return SCRIPT_CMD_FAILURE;
	}

	if( script_hasdata(st, 3) ){
		if (script_isint(st, 3))
			script_charid2sd(3, sd);
		else
			script_nick2sd(3, sd);
	}else{
		script_rid2sd(sd);
	}

	if( !sd ){
		script_pushint(st, -1);
		return SCRIPT_CMD_FAILURE;
	}

  struct status_data *sstatus = status_get_status_data(&sd->bl);

	// lets do this the dumb way
	switch(value) {
		case 1: script_pushint(st, sstatus->str); break;
		case 2: script_pushint(st, sstatus->agi); break;
		case 3: script_pushint(st, sstatus->vit); break;
		case 4: script_pushint(st, sstatus->int_); break;
		case 5: script_pushint(st, sstatus->dex); break;
		case 6: script_pushint(st, sstatus->luk); break;
		default: {
			script_pushint(st, -1);
			return SCRIPT_CMD_FAILURE;
		}
	}

	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(removeelemental)
{
	TBL_PC* sd;

	if (!script_charid2sd(3,sd))
		return SCRIPT_CMD_FAILURE;

	if (!sd->ed)
		return SCRIPT_CMD_FAILURE;

	elemental_delete(sd->ed);

	return SCRIPT_CMD_SUCCESS;
}
