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
