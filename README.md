# vSRO-GameServer

Customizing SR_GameServer (v1.188) behavior through DLL injection.

### Actions

Notifies to gameserver actions in realtime with a simple INSERT query into `SRO_VT_SHARD.dbo.NotifyGameServer`

#### Supported Actions ID:

1. Adds item(s) to the inventory from character 
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_NotifyGameServer]
(
	Action_ID,
	CharName16,
	Param01, -- CodeName
	Param02, -- Amount
	Param03, -- Randomize stats
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

2. Updates the gold amount from character by increasing (positive) or decreasing (negative)
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_NotifyGameServer]
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

3. Updates the title from character by level (primary key)
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_NotifyGameServer]
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

4. Moves the character to the position on map
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_NotifyGameServer]
(
	Action_ID,
	CharName16,
	Param05, -- Region Id
	Param06, -- PosX
	Param07, -- PosY
	Param08 -- PosZ
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

5. Moves the character to the position on map through game world id
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_NotifyGameServer]
(
	Action_ID,
	CharName16,
	Param04, -- GameWorldId
	Param05, -- Region Id
	Param06, -- PosX
	Param07, -- PosY
	Param08 -- PosZ
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

6. Drops an item near character
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_NotifyGameServer]
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
INSERT INTO [SRO_VT_SHARD].[dbo].[_NotifyGameServer]
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

8. Force reloading the character information by teleporting it on the same place
```sql
INSERT INTO [SRO_VT_SHARD].[dbo].[_NotifyGameServer]
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

#### Action Result Code

```C++
UNKNOWN = 0
SUCCESS = 1
ACTION_UNDEFINED = 2
UNNEXPECTED_EXCEPTION = 3
PARAMS_NOT_SUPPLIED = 4
CHARNAME_NOT_FOUND = 5
FUNCTION_ERROR = 6
```