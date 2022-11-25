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
