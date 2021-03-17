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
	Param03, -- Variance (white stats)
	Param04 -- Plus
)
VALUES
(
	1,
	'JellyBitz',
	'ITEM_EU_SWORD_01_A',
	1,
	0,
	5
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