#include "global.h"
#include "mom_savings.h"
#include "item.h"
#include "random.h"
#include "constants/items.h"
#include "event_data.h"

// Sequential gift table (purchased in order, one-time only)
// These items are purchased when the player's savings reach specific thresholds
static const struct MomGiftSequential sMomGifts_Sequential[] = {
    {ITEM_SUPER_POTION, 900,   600},
    {ITEM_REPEL,        4000,  270},
    {ITEM_SUPER_POTION, 7000,  600},
    {ITEM_SILK_SCARF,   10000, 100},
    {ITEM_MOON_STONE,   15000, 2100},
    {ITEM_HYPER_POTION, 19000, 1200},
    {ITEM_REVIVE,       30000, 1500},
    {ITEM_CHOICE_BAND,  40000, 100},
    {ITEM_MASTER_BALL,  50000, 5000},
    // Add more items as desired (max 16 due to u16 bitflags)
};
/* 
HGSS ITEM LIST (for reference)
Trigger	Cost	Item
$900	$600	Super Potion	Super Potion
$4000	$270	Repel	Repel
$7000	$600	Super Potion	Super Potion
$10000	$100	Silk Scarf	Silk Scarf
$15000	$3000	Moon Stone	Moon Stone
$19000	$900	Hyper Potion	Hyper Potion
$30000	$200	Choice Scarf	Choice Scarf
$40000	$200	Muscle Band	Muscle Band
$50000	$200	Focus Sash	Focus Sash
*/

#define MOM_ITEMS_SEQUENTIAL_COUNT ARRAY_COUNT(sMomGifts_Sequential)

// Berry gift list (purchased randomly at multiples of threshold)
// Mom will randomly select one berry from this list every $2300
// This list consists of berries that are used by Kurt in Azalea Town
static const u16 sMomGifts_Berries[] = {
    ITEM_PECHA_BERRY,
    ITEM_RAWST_BERRY,
    ITEM_CHERI_BERRY,
    ITEM_ORAN_BERRY,
    ITEM_CHESTO_BERRY,
    ITEM_ASPEAR_BERRY,
    ITEM_PERSIM_BERRY,
};

#define MOM_BERRIES_COUNT ARRAY_COUNT(sMomGifts_Berries)

static bool8 Mom_CheckSequentialGifts(u32 balance, u16 *purchasedItem);
static bool8 Mom_CheckRandomBerries(u32 newBalance, u32 oldBalance, u16 *purchasedItem);
static void Mom_AddItemToPC(u16 itemId, u16 quantity);

// Initialize mom's savings data (called on new game)
void InitMomSavings(void)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;
    mom->momsMoney = 0;
    mom->normalGiftFlags = 0;
    mom->isSavingMoney = FALSE;
    mom->padding = 0;
}

// Enable or disable mom's automatic saving feature
void Mom_EnableSaving(bool8 enable)
{
    gSaveBlock1Ptr->momSavings.isSavingMoney = enable;
}

// Check if mom's saving feature is currently enabled
bool8 Mom_IsSavingEnabled(void)
{
    return gSaveBlock1Ptr->momSavings.isSavingMoney;
}

// Get the current balance of mom's savings
u32 Mom_GetBalance(void)
{
    return gSaveBlock1Ptr->momSavings.momsMoney;
}

// Attempt to deposit money into mom's savings
// Returns TRUE if successful, FALSE if saving is disabled
bool8 Mom_TryDepositMoney(u32 amount)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;
    
    // Check if feature is enabled
    if (!mom->isSavingMoney)
        return FALSE;
    
    // Store old balance for tier comparison
    u32 oldBalance = mom->momsMoney;
    
    // Add money to savings
    mom->momsMoney += amount;
    
    // Check if balance would overflow (max is 999999)
    if (mom->momsMoney > MOM_MAX_MONEY)
        mom->momsMoney = MOM_MAX_MONEY;
    
    // Check if any gifts should be purchased
    Mom_CheckForGiftPurchase(mom->momsMoney, oldBalance);
    
    return TRUE;
}

// Attempt to withdraw money from mom's savings
// Returns TRUE if successful, FALSE if insufficient funds
bool8 Mom_TryWithdrawMoney(u32 amount)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;
    
    // Check if enough funds
    if (mom->momsMoney < amount)
        return FALSE;
    
    // Withdraw money
    mom->momsMoney -= amount;
    
    return TRUE;
}

// Check if mom should purchase a gift based on the new balance
// This is called after depositing money
// Returns TRUE if a gift was purchased
bool8 Mom_CheckForGiftPurchase(u32 newBalance, u32 oldBalance)
{
    u16 purchasedItem = ITEM_NONE;
    u16 quantity = 1;
    
    // Try sequential gifts first
    // Sequential gifts are one-time purchases at specific thresholds
    if (Mom_CheckSequentialGifts(newBalance, &purchasedItem))
    {
        quantity = 1;
        Mom_AddItemToPC(purchasedItem, quantity);
        
        // Set flag/var to trigger mom's phone call
        VarSet(VAR_MOM_GIFT_ITEM, purchasedItem);
        VarSet(VAR_MOM_GIFT_QUANTITY, quantity);
        FlagSet(FLAG_MOM_HAS_GIFT);
        
        return TRUE;
    }
    
    // If no sequential items triggered, try random berries
    // Berry gifts are recurring purchases at multiples of the threshold
    if (Mom_CheckRandomBerries(newBalance, oldBalance, &purchasedItem))
    {
        quantity = MOM_BERRY_QUANTITY;
        Mom_AddItemToPC(purchasedItem, quantity);
        
        // Set flag/var to trigger mom's phone call
        VarSet(VAR_MOM_GIFT_ITEM, purchasedItem);
        VarSet(VAR_MOM_GIFT_QUANTITY, quantity);
        FlagSet(FLAG_MOM_HAS_GIFT);
        
        return TRUE;
    }
    
    return FALSE;
}

// ===== Internal Functions =====

// Check if any sequential gifts should be purchased
// Sequential gifts are purchased in order, one at a time, when thresholds are met
// Returns TRUE if an item was purchased
static bool8 Mom_CheckSequentialGifts(u32 balance, u16 *purchasedItem)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;
    
    // Loop through all sequential items
    for (u32 i = 0; i < MOM_ITEMS_SEQUENTIAL_COUNT; i++)
    {
        // Skip if already purchased (check bit flag)
        if (mom->normalGiftFlags & (1 << i))
            continue;
        
        // Skip if can't afford yet
        if (balance < sMomGifts_Sequential[i].threshold)
            continue;
        
        // Purchase this item!
        // Set the bit flag to mark this item as purchased
        mom->normalGiftFlags |= (1 << i);
        
        // Deduct the cost from savings
        mom->momsMoney -= sMomGifts_Sequential[i].cost;
        
        // Return the item that was purchased
        *purchasedItem = sMomGifts_Sequential[i].itemId;
        
        return TRUE;  // Only one purchase per deposit
    }
    
    return FALSE;  // No items were affordable or all already purchased
}

// Check if a random berry should be purchased
// Berries are purchased when the player crosses into a new "tier" (multiple of threshold)
// Returns TRUE if a berry was purchased
static bool8 Mom_CheckRandomBerries(u32 newBalance, u32 oldBalance, u16 *purchasedItem)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;
    
    // Calculate which "tier" we're in
    // Tier 0 = $0-2299, Tier 1 = $2300-4599, Tier 2 = $4600-6899, etc.
    u32 newTier = newBalance / MOM_RANDOM_THRESHOLD;
    u32 oldTier = oldBalance / MOM_RANDOM_THRESHOLD;
    
    // Only purchase if we crossed into a new tier
    // This prevents re-purchasing when withdrawing money
    if (newTier <= oldTier)
        return FALSE;
    
    // Select random berry from list
    u32 randomIndex = Random() % MOM_BERRIES_COUNT;
    u16 berryId = sMomGifts_Berries[randomIndex];
    
    // Deduct cost
    mom->momsMoney -= MOM_BERRY_COST;
    
    // Return the berry ID
    *purchasedItem = berryId;
    
    return TRUE;
}

/* Special script functions related to mom's savings (called from event scripts) */

static void Mom_AddItemToPC(u16 itemId, u16 quantity)
{
    AddPCItem(itemId, quantity);
}

void Special_MomEnableSaving(void)
{
    Mom_EnableSaving(TRUE);
}

void Special_MomDisableSaving(void)
{
    Mom_EnableSaving(FALSE);
}

void Special_MomGetBalance(void)
{
    gSpecialVar_0x8000 = Mom_GetBalance();
}

void Special_MomDeposit(void)
{
    u32 amount = gSpecialVar_0x8000;
    bool8 success = Mom_TryDepositMoney(amount);
    gSpecialVar_Result = success;
}

void Special_MomWithdraw(void)
{
    u32 amount = gSpecialVar_0x8000;
    bool8 success = Mom_TryWithdrawMoney(amount);
    gSpecialVar_Result = success;
}

void Special_MomIsSavingEnabled(void)
{
    gSpecialVar_Result = Mom_IsSavingEnabled();
}
