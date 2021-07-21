# vSRO-ServerAddon

Customize Silkroad Online server files (v1.188) behavior through DLL injection.

## Features
- Easy to inject using *Stud_PE*
- Patch values from memory directly
- Define all options from their respective config file
- Execute actions from Gameserver (first gameserver running on the same machine will execute these actions)

## How to use?

1. Make a backup from your `SR_GameServer.exe` and `SR_ShardManager.exe` just in case something goes wrong
2. Download, install, and execute [*Stud_PE*](http://www.cgsoftlabs.ro/zip/Stud_PE.zip)
3. Drag & drop `SR_GameServer.exe` into *Stud_PE*
4. Go to `Functions` tab, right click into any line at left blue panel and click `Add New Import`
5. Click `Dll Select` and find `vSRO-GameServer.dll`
6. Click `Select func` and select the one there, then `OK`
7. Click `Add to list` and click `ADD`, then `OK`
8. Extract `vSRO-ServerAddon.bin.zip` to the same folder where is located `SR_GameServer.exe`
9. Done! Just repeat Step 3 to 7 using `SR_ShardManager.exe` and `vSRO-ShardManager.dll`

## Gameserver Actions

Execute gameserver actions in realtime with a simple `INSERT` query into `SRO_VT_SHARD.dbo._ExeGameServer` which is created automatically if doesn't exist.

### Examples

1. Adds item(s) to the inventory from player 
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param01, -- CodeName
	Param02, -- Amount
	Param03, -- Random stats (0 = Clean, 1 = Random)
	Param04 -- Plus
)
VALUES
(
	1,
	'JellyBitz',
	'ITEM_EU_SWORD_01_A',
	1,
	0,
	3
);
```

2. Updates the gold amount from player by increasing (positive) or decreasing (negative)
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02 -- Gold Offset
)
VALUES
(
	2,
	'JellyBitz',
	10000000 -- Increase by 10m
);
```

3. Updates the Hwan level (Berserk title) from player
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02 -- HWAN level
)
VALUES
(
	3,
	'JellyBitz',
	6 -- "Duke"
);
```

4. Moves the player to the position on map
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02, -- Region Id
	Param03, -- PosX
	Param04, -- PosY
	Param05 -- PosZ
)
VALUES
(
	4,
	'JellyBitz',
	25000,
	0,
	0,
	0
);
```

5. Moves the player to the position on map through game world id
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02, -- GameWorldId
	Param03, -- Region Id
	Param04, -- PosX
	Param05, -- PosY
	Param06 -- PosZ
)
VALUES
(
	5,
	'JellyBitz',
	1, -- Default Map
	25000,
	0,
	0,
	0
);
```

6. Drops an item near player
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param01, -- CodeName
	Param02, -- Amount
	Param03 -- Plus
)
VALUES
(
	6,
	'JellyBitz',
	'ITEM_EU_SWORD_01_A',
	1,
	3
);
```

7. Transform an item to another one from inventory slot specified
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param01, -- New item CodeName
	Param02 -- Inventory slot
)
VALUES
(
	7,
	'JellyBitz',
	'ITEM_EU_SWORD_02_A',
	13 -- First inventory slot
);
```

8. Force reloading the player information by teleporting it on the same place
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16
)
VALUES
(
	8,
	'JellyBitz'
);
```

9. Adds a buff to the player. The duration will not be lost through teleports
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02, -- Skill Id (buffs only)
	Param03 -- Duration (seconds)
)
VALUES
(
	9,
	'JellyBitz',
	8594, -- Ultimate screen (Lv.90)
	30 -- 30 seconds
);
```

10. Creates a mob in the map position
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02, -- RefObjId
	Param03, -- Region Id
	Param04, -- PosX
	Param05, -- PosY
	Param06 -- PosZ
)
VALUES
(
	10,
	'',
	1954, -- Tiger Woman
	24744, -- Jangan (S)
	968,
	-27,
	1114
);
```

11. Creates a mob in the map position through game world id
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02, -- RefObjId
	Param03, -- GameWorldId
	Param04, -- Region Id
	Param05, -- PosX
	Param06, -- PosY
	Param07 -- PosZ
)
VALUES
(
	11,
	'',
	1947, -- tiger
	1, -- Default Map
	24744, -- Jangan (S)
	968,
	-27,
	1114
);
```

12. Set body state from player
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02 -- Body State (0 = None, 1 = Berserk, 2 = Untouchable, 3 = GMInvincible, 4 = GMUntouchable, 5= GMInvisible, 6 = Stealth, 7 = Invisible)
)
VALUES
(
	12,
	'JellyBitz',
	1
);
```

13. Updates the skill points
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02 -- Skill points
)
VALUES
(
	13,
	'JellyBitz',
	1000 -- Increase 1k SP
);
```

14. Changes the guild grantname from the player
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param01 -- GrantName
)
VALUES
(
	14,
	'JellyBitz',
	'Jelly'
);
```

15. Set the life state from player
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02 -- Life State (0 = Dead, 1 = Alive)
)
VALUES
(
	15,
	'JellyBitz',
	0 -- Dead
);
```

16. Updates level experience from player
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02 -- Level Experience
)
VALUES
(
	16,
	'JellyBitz',
	1000000 -- Increase experience by 1m
);
```

17. Add skill points experience to player
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_ExeGameServer]
(
	Action_ID,
	CharName16,
	Param02 -- Skill Points Experience
)
VALUES
(
	17,
	'JellyBitz',
	1000000 -- Increase experience by 1m (equivalent to 2500 SP)
);
```

### Action Result Code

```C++
UNKNOWN = 0
SUCCESS = 1
ACTION_UNDEFINED = 2
UNNEXPECTED_EXCEPTION = 3
PARAMS_NOT_SUPPLIED = 4
CHARNAME_NOT_FOUND = 5
FUNCTION_ERROR = 6
```

## Client Editions

For visualizing some server files changes on game client, a few ASM editions are required in your client. [Check it out.](README.CLIENT.md).

---
> ### Do you feel this project is helping you a lot ?
> ### Support me! [Buy me a coffee <img src="https://twemoji.maxcdn.com/2/72x72/2615.png" width="18" height="18">](https://www.buymeacoffee.com/JellyBitz "Coffee <3")
> 
> ### Created with [<img title="Yes, Code!" src="https://twemoji.maxcdn.com/2/72x72/1f499.png" width="18" height="18">](#)

---
> ##### Hey Thanks!
> - [Elitepvpers](https://www.elitepvpers.com/forum/silkroad-online/) ASM/C++ coders leaving source codes and ideas to create this stuff
