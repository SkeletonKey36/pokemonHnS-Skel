#ifndef GUARD_MOM_SAVINGS_H
#define GUARD_MOM_SAVINGS_H

#include "global.h"

// Constants
#define MOM_RANDOM_THRESHOLD 2300   // Every $2300 triggers random berry purchase
#define MOM_BERRY_QUANTITY 5        // Mom buys 5 berries at a time
#define MOM_BERRY_COST 100          // Cost per berry purchase
#define MOM_MAX_MONEY 999999        // Maximum money mom can hold

// SaveBlock structure for Mom's savings data
struct MomSavingsData
{
    u32 momsMoney;           // Mom's current savings balance
    u16 normalGiftFlags;     // Bitflags for which sequential items purchased (up to 16)
    u8 isSavingMoney;        // Boolean: is the feature active?
    u8 padding;              // Alignment padding
};

// Item table structure for sequential gifts
struct MomGiftSequential
{
    u16 itemId;      // Item to purchase
    u32 threshold;   // When balance reaches this amount
    u16 cost;        // Cost to deduct from savings
};

// Public function declarations
void Mom_EnableSaving(bool8 enable);
bool8 Mom_IsSavingEnabled(void);
u32 Mom_GetBalance(void);
bool8 Mom_TryDepositMoney(u32 amount);
bool8 Mom_TryWithdrawMoney(u32 amount);
bool8 Mom_CheckForGiftPurchase(u32 newBalance, u32 oldBalance);
void InitMomSavings(void);

#endif // GUARD_MOM_SAVINGS_H