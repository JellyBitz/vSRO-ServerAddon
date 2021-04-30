# vSRO-ServerAddon

This is a compilations of ASM/editions for vSRO (1.188) that you'll need to change in order to use the DLL correctly.

## vSRO-GameServer.dll

### SERVER_LEVEL_MAX

0x6E = 110

```
008A99A2 | 80FB 6E                  | cmp bl,6E                         | Level Up limit

0069C7C8 | 3C 6E                    | cmp al,6E                         | Mastery Limit

0073940E | 6A 6E                    | push 6E                           | Party Match

00739453 | 6A 6E                    | push 6E                           | Party Match

0073AFAE | 6A 6E                    | push 6E                           | Party Match

0073B013 | 6A 6E                    | push 6E                           | Party Match

0073B030 | 6A 6E                    | push 6E                           | Party Match

0073FA4C | 6A 6E                    | push 6E                           | Party Match

0073FAAF | 6A 6E                    | push 6E                           | Party Match

0073FACC | 6A 6E                    | push 6E                           | Party Match
```

### SERVER_STALL_PRICE_LIMIT

0x02540BE3FF = 9999999999 (9.99b)

```
005DF9DE | 6A 02                    | push 2                            |
005DF9E0 | 68 FFE30B54              | push 540BE3FF                     |

005EA683 | 6A 02                    | push 2                            |
005EA685 | 68 FFE30B54              | push 540BE3FF                     |

006B2003 | 6A 02                    | push 2                            |
006B2005 | 68 FFE30B54              | push 540BE3FF                     |

006BA765 | 6A 02                    | push 2                            |
006BA767 | 68 FFE30B54              | push 540BE3FF                     |
```

### RACE_CH_TOTAL_MASTERIES

0x14A = 330, 0xDC = 220

```
006A51BC | BF 4A010000              | mov edi,14A                       | Chinese

006AA4C3 | BE 4A010000              | mov esi,14A                       | Chinese

006A5197 | 3D F0000000              | cmp eax,DC                        | European

006A51A2 | BF F0000000              | mov edi,DC                        | European

006AA498 | 3D F0000000              | cmp eax,DC                        | European

006AA4A3 | BE F0000000              | mov esi,DC                        | European
```

### GUILD_UNION_CHAT_PARTICIPANTS

0x0C = 12
```
005AC538 | 3C 0C                    | cmp al,0C                         |
```

## vSRO-ShardManager.dll

### ACCOUNT_CHARACTERS_MAX

0x04 = 4

```
0085DE67 | 80BE 2C010000 04         | cmp byte ptr ds:[esi+12C],4       |
```

It requires also to edit the value from procedure `_AddNewChar` on `SRO_VT_SHARD`

```sql
	IF (@temp >= 4) -- 4 chars per account
	BEGIN      
		RETURN -2
	END
```