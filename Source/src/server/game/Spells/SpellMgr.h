/*
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SPELLMGR_H
#define _SPELLMGR_H

// For static or at-server-startup loaded spell data

#include "Define.h"
#include "DBCStructure.h"
#include "SharedDefines.h"
#include "Util.h"

#include <map>
#include <set>
#include <vector>
#include <unordered_map>

class SpellInfo;
class Player;
class Unit;
class ProcEventInfo;
struct SkillLineAbilityEntry;

// only used in code
enum SpellCategories
{
    SPELLCATEGORY_HEALTH_MANA_POTIONS = 4,
    SPELLCATEGORY_DEVOUR_MAGIC        = 12,
    SPELLCATEGORY_JUDGEMENT           = 1210,               // Judgement (seal trigger)
    SPELLCATEGORY_FOOD                = 11,
    SPELLCATEGORY_DRINK               = 59
};

//SpellFamilyFlags
enum SpellFamilyFlag
{
    // SPELLFAMILYFLAG  = SpellFamilyFlags[0]
    // SPELLFAMILYFLAG1 = SpellFamilyFlags[1]
    // SPELLFAMILYFLAG2 = SpellFamilyFlags[2]

    // Rogue
    SPELLFAMILYFLAG_ROGUE_VANISH            = 0x00000800,
    SPELLFAMILYFLAG_ROGUE_VAN_EVAS_SPRINT   = 0x00000860,    // Vanish, Evasion, Sprint
    SPELLFAMILYFLAG1_ROGUE_COLDB_SHADOWSTEP = 0x00000240,    // Cold Blood, Shadowstep
    SPELLFAMILYFLAG_ROGUE_KICK              = 0x00000010,   // Kick
    SPELLFAMILYFLAG1_ROGUE_DISMANTLE        = 0x00100000,   // Dismantle
    SPELLFAMILYFLAG_ROGUE_BLADE_FLURRY      = 0x40000000,   // Blade Flurry
    SPELLFAMILYFLAG1_ROGUE_BLADE_FLURRY     = 0x00000800,   // Blade Flurry

    // Warrior
    SPELLFAMILYFLAG_WARRIOR_CHARGE          = 0x00000001,
    SPELLFAMILYFLAG_WARRIOR_SLAM            = 0x00200000,
    SPELLFAMILYFLAG_WARRIOR_EXECUTE         = 0x20000000,
    SPELLFAMILYFLAG_WARRIOR_CONCUSSION_BLOW = 0x04000000,

    // Warlock
    SPELLFAMILYFLAG_WARLOCK_LIFETAP         = 0x00040000,

    // Druid
    SPELLFAMILYFLAG2_DRUID_STARFALL         = 0x00000100,

    // Paladin
    SPELLFAMILYFLAG1_PALADIN_DIVINESTORM    = 0x00020000,

    // Shaman
    SPELLFAMILYFLAG_SHAMAN_FROST_SHOCK      = 0x80000000,
    SPELLFAMILYFLAG_SHAMAN_HEALING_STREAM   = 0x00002000,
    SPELLFAMILYFLAG_SHAMAN_MANA_SPRING      = 0x00004000,
    SPELLFAMILYFLAG2_SHAMAN_LAVA_LASH       = 0x00000004,
    SPELLFAMILYFLAG_SHAMAN_FIRE_NOVA        = 0x28000000,

    // Deathknight
    SPELLFAMILYFLAG_DK_DEATH_STRIKE         = 0x00000010,
    SPELLFAMILYFLAG_DK_DEATH_COIL           = 0x00002000,

    /// @todo Figure out a more accurate name for the following familyflag(s)
    SPELLFAMILYFLAG_SHAMAN_TOTEM_EFFECTS    = 0x04000000  // Seems to be linked to most totems and some totem effects
};


#define SPELL_LINKED_MAX_SPELLS  200000

enum SpellLinkedType
{
    SPELL_LINK_CAST     = 0,            // +: cast; -: remove
    SPELL_LINK_HIT      = 1 * 200000,
    SPELL_LINK_AURA     = 2 * 200000,   // +: aura; -: immune
    SPELL_LINK_REMOVE   = 0
};


// Spell proc event related declarations (accessed using SpellMgr functions)
enum ProcFlags
{
    PROC_FLAG_NONE                            = 0x00000000,

    PROC_FLAG_KILLED                          = 0x00000001,    // 00 Killed by agressor - not sure about this flag
    PROC_FLAG_KILL                            = 0x00000002,    // 01 Kill target (in most cases need XP/Honor reward)

    PROC_FLAG_DONE_MELEE_AUTO_ATTACK          = 0x00000004,    // 02 Done melee auto attack
    PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK         = 0x00000008,    // 03 Taken melee auto attack

    PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS      = 0x00000010,    // 04 Done attack by Spell that has dmg class melee
    PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS     = 0x00000020,    // 05 Taken attack by Spell that has dmg class melee

    PROC_FLAG_DONE_RANGED_AUTO_ATTACK         = 0x00000040,    // 06 Done ranged auto attack
    PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK        = 0x00000080,    // 07 Taken ranged auto attack

    PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS     = 0x00000100,    // 08 Done attack by Spell that has dmg class ranged
    PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS    = 0x00000200,    // 09 Taken attack by Spell that has dmg class ranged

    PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS   = 0x00000400,    // 10 Done positive spell that has dmg class none
    PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS  = 0x00000800,    // 11 Taken positive spell that has dmg class none

    PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG   = 0x00001000,    // 12 Done negative spell that has dmg class none
    PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_NEG  = 0x00002000,    // 13 Taken negative spell that has dmg class none

    PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS  = 0x00004000,    // 14 Done positive spell that has dmg class magic
    PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS = 0x00008000,    // 15 Taken positive spell that has dmg class magic

    PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG  = 0x00010000,    // 16 Done negative spell that has dmg class magic
    PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG = 0x00020000,    // 17 Taken negative spell that has dmg class magic

    PROC_FLAG_DONE_PERIODIC                   = 0x00040000,    // 18 Successful do periodic (damage / healing)
    PROC_FLAG_TAKEN_PERIODIC                  = 0x00080000,    // 19 Taken spell periodic (damage / healing)

    PROC_FLAG_TAKEN_DAMAGE                    = 0x00100000,    // 20 Taken any damage
    PROC_FLAG_DONE_TRAP_ACTIVATION            = 0x00200000,    // 21 On trap activation (possibly needs name change to ON_GAMEOBJECT_CAST or USE)

    PROC_FLAG_DONE_MAINHAND_ATTACK            = 0x00400000,    // 22 Done main-hand melee attacks (spell and autoattack)
    PROC_FLAG_DONE_OFFHAND_ATTACK             = 0x00800000,    // 23 Done off-hand melee attacks (spell and autoattack)

    PROC_FLAG_DEATH                           = 0x01000000,    // 24 Died in any way

    // flag masks
    AUTO_ATTACK_PROC_FLAG_MASK                = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK
                                                | PROC_FLAG_DONE_RANGED_AUTO_ATTACK | PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK,

    MELEE_PROC_FLAG_MASK                      = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK
                                                | PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS
                                                | PROC_FLAG_DONE_MAINHAND_ATTACK | PROC_FLAG_DONE_OFFHAND_ATTACK,

    RANGED_PROC_FLAG_MASK                     = PROC_FLAG_DONE_RANGED_AUTO_ATTACK | PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK
                                                | PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS,

    SPELL_PROC_FLAG_MASK                      = PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS
                                                | PROC_FLAG_DONE_RANGED_AUTO_ATTACK | PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK
                                                | PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS
                                                | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS
                                                | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_NEG
                                                | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS
                                                | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG,

    SPELL_CAST_PROC_FLAG_MASK                  = SPELL_PROC_FLAG_MASK | PROC_FLAG_DONE_TRAP_ACTIVATION,

    PERIODIC_PROC_FLAG_MASK                    = PROC_FLAG_DONE_PERIODIC | PROC_FLAG_TAKEN_PERIODIC,

    DONE_HIT_PROC_FLAG_MASK                    = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_DONE_RANGED_AUTO_ATTACK
                                                 | PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS | PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS
                                                 | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG
                                                 | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG
                                                 | PROC_FLAG_DONE_PERIODIC | PROC_FLAG_DONE_MAINHAND_ATTACK | PROC_FLAG_DONE_OFFHAND_ATTACK,

    TAKEN_HIT_PROC_FLAG_MASK                   = PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK
                                                 | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS
                                                 | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_NEG
                                                 | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG
                                                 | PROC_FLAG_TAKEN_PERIODIC | PROC_FLAG_TAKEN_DAMAGE,

    REQ_SPELL_PHASE_PROC_FLAG_MASK             = SPELL_PROC_FLAG_MASK & DONE_HIT_PROC_FLAG_MASK
};

#define MELEE_BASED_TRIGGER_MASK (PROC_FLAG_DONE_MELEE_AUTO_ATTACK      | \
                                  PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK     | \
                                  PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS  | \
                                  PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS | \
                                  PROC_FLAG_DONE_RANGED_AUTO_ATTACK     | \
                                  PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK    | \
                                  PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS | \
                                  PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS)

enum ProcFlagsSpellType
{
    PROC_SPELL_TYPE_NONE              = 0x0000000,
    PROC_SPELL_TYPE_DAMAGE            = 0x0000001, // damage type of spell
    PROC_SPELL_TYPE_HEAL              = 0x0000002, // heal type of spell
    PROC_SPELL_TYPE_NO_DMG_HEAL       = 0x0000004, // other spells
    PROC_SPELL_TYPE_MASK_ALL          = PROC_SPELL_TYPE_DAMAGE | PROC_SPELL_TYPE_HEAL | PROC_SPELL_TYPE_NO_DMG_HEAL
};

enum ProcFlagsSpellPhase
{
    PROC_SPELL_PHASE_NONE             = 0x0000000,
    PROC_SPELL_PHASE_CAST             = 0x0000001,
    PROC_SPELL_PHASE_HIT              = 0x0000002,
    PROC_SPELL_PHASE_FINISH           = 0x0000004,
    PROC_SPELL_PHASE_MASK_ALL         = PROC_SPELL_PHASE_CAST | PROC_SPELL_PHASE_HIT | PROC_SPELL_PHASE_FINISH
};

enum ProcFlagsHit
{
    PROC_HIT_NONE                = 0x0000000, // no value - PROC_HIT_NORMAL | PROC_HIT_CRITICAL for TAKEN proc type, PROC_HIT_NORMAL | PROC_HIT_CRITICAL | PROC_HIT_ABSORB for DONE
    PROC_HIT_NORMAL              = 0x0000001, // non-critical hits
    PROC_HIT_CRITICAL            = 0x0000002,
    PROC_HIT_MISS                = 0x0000004,
    PROC_HIT_FULL_RESIST         = 0x0000008,
    PROC_HIT_DODGE               = 0x0000010,
    PROC_HIT_PARRY               = 0x0000020,
    PROC_HIT_BLOCK               = 0x0000040, // partial or full block
    PROC_HIT_EVADE               = 0x0000080,
    PROC_HIT_IMMUNE              = 0x0000100,
    PROC_HIT_DEFLECT             = 0x0000200,
    PROC_HIT_ABSORB              = 0x0000400, // partial or full absorb
    PROC_HIT_REFLECT             = 0x0000800,
    PROC_HIT_INTERRUPT           = 0x0001000, // (not used atm)
    PROC_HIT_FULL_BLOCK          = 0x0002000,
    PROC_HIT_MASK_ALL            = 0x0002FFF
};

enum ProcAttributes
{
    PROC_ATTR_REQ_EXP_OR_HONOR   = 0x0000001, // requires proc target to give exp or honor for aura proc
    PROC_ATTR_TRIGGERED_CAN_PROC = 0x0000002, // aura can proc even with triggered spells
    PROC_ATTR_REQ_MANA_COST      = 0x0000004, // requires triggering spell to have a mana cost for aura proc
    PROC_ATTR_REQ_SPELLMOD       = 0x0000008  // requires triggering spell to be affected by proccing aura to drop charges
};

struct SpellProcEntry
{
    uint32 SchoolMask;      // if nonzero - bitmask for matching proc condition based on spell's school
    uint32 SpellFamilyName; // if nonzero - for matching proc condition based on candidate spell's SpellFamilyName
    flag96 SpellFamilyMask; // if nonzero - bitmask for matching proc condition based on candidate spell's SpellFamilyFlags
    uint32 ProcFlags;       // if nonzero - owerwrite procFlags field for given Spell.dbc entry, bitmask for matching proc condition, see enum ProcFlags
    uint32 SpellTypeMask;   // if nonzero - bitmask for matching proc condition based on candidate spell's damage/heal effects, see enum ProcFlagsSpellType
    uint32 SpellPhaseMask;  // if nonzero - bitmask for matching phase of a spellcast on which proc occurs, see enum ProcFlagsSpellPhase
    uint32 HitMask;         // if nonzero - bitmask for matching proc condition based on hit result, see enum ProcFlagsHit
    uint32 AttributesMask;  // bitmask, see ProcAttributes
    float ProcsPerMinute;   // if nonzero - chance to proc is equal to value * aura caster's weapon speed / 60
    float Chance;           // if nonzero - owerwrite procChance field for given Spell.dbc entry, defines chance of proc to occur, not used if ProcsPerMinute set
    Milliseconds Cooldown;  // if nonzero - cooldown in secs for aura proc, applied to aura
    uint32 Charges;         // if nonzero - owerwrite procCharges field for given Spell.dbc entry, defines how many times proc can occur before aura remove, 0 - infinite
};

typedef std::unordered_map<uint32, SpellProcEntry> SpellProcMap;

struct SpellEnchantProcEntry
{
    uint32      customChance;
    float       PPMChance;
    uint32      procEx;
};

typedef std::unordered_map<uint32, SpellEnchantProcEntry> SpellEnchantProcEventMap;

struct SpellBonusEntry
{
    float  direct_damage;
    float  dot_damage;
    float  ap_bonus;
    float  ap_dot_bonus;
};

typedef std::unordered_map<uint32, SpellBonusEntry>     SpellBonusMap;

enum SpellGroup
{
    SPELL_GROUP_NONE             = 0,
    SPELL_GROUP_ELIXIR_BATTLE    = 1,
    SPELL_GROUP_ELIXIR_GUARDIAN  = 2,
    SPELL_GROUP_ELIXIR_UNSTABLE  = 3,
    SPELL_GROUP_ELIXIR_SHATTRATH = 4,
    SPELL_GROUP_CORE_RANGE_MAX   = 5
};

#define SPELL_GROUP_DB_RANGE_MIN 1000

//                  spell_id, group_id
typedef std::multimap<uint32, SpellGroup > SpellSpellGroupMap;
typedef std::pair<SpellSpellGroupMap::const_iterator, SpellSpellGroupMap::const_iterator> SpellSpellGroupMapBounds;

//                      group_id, spell_id
typedef std::multimap<SpellGroup, int32> SpellGroupSpellMap;
typedef std::pair<SpellGroupSpellMap::const_iterator, SpellGroupSpellMap::const_iterator> SpellGroupSpellMapBounds;

enum SpellGroupStackRule
{
    SPELL_GROUP_STACK_RULE_DEFAULT,
    SPELL_GROUP_STACK_RULE_EXCLUSIVE,
    SPELL_GROUP_STACK_RULE_EXCLUSIVE_FROM_SAME_CASTER,
    SPELL_GROUP_STACK_RULE_EXCLUSIVE_SAME_EFFECT,
    SPELL_GROUP_STACK_RULE_EXCLUSIVE_HIGHEST,
    SPELL_GROUP_STACK_RULE_MAX
};

typedef std::map<SpellGroup, SpellGroupStackRule> SpellGroupStackMap;

struct SpellThreatEntry
{
    int32       flatMod;                                    // flat threat-value for this Spell  - default: 0
    float       pctMod;                                     // threat-multiplier for this Spell  - default: 1.0f
    float       apPctMod;                                   // Pct of AP that is added as Threat - default: 0.0f
};

typedef std::map<uint32, SpellThreatEntry> SpellThreatMap;

// coordinates for spells (accessed using SpellMgr functions)
struct SpellTargetPosition
{
    uint32 target_mapId;
    float  target_X;
    float  target_Y;
    float  target_Z;
    float  target_Orientation;
};

typedef std::map<std::pair<uint32 /*spell_id*/, SpellEffIndex /*effIndex*/>, SpellTargetPosition> SpellTargetPositionMap;

// Enum with EffectRadiusIndex and their actual radius
enum EffectRadiusIndex
{
    EFFECT_RADIUS_2_YARDS       = 7,
    EFFECT_RADIUS_5_YARDS       = 8,
    EFFECT_RADIUS_20_YARDS      = 9,
    EFFECT_RADIUS_30_YARDS      = 10,
    EFFECT_RADIUS_45_YARDS      = 11,
    EFFECT_RADIUS_100_YARDS     = 12,
    EFFECT_RADIUS_10_YARDS      = 13,
    EFFECT_RADIUS_8_YARDS       = 14,
    EFFECT_RADIUS_3_YARDS       = 15,
    EFFECT_RADIUS_1_YARD        = 16,
    EFFECT_RADIUS_13_YARDS      = 17,
    EFFECT_RADIUS_15_YARDS      = 18,
    EFFECT_RADIUS_18_YARDS      = 19,
    EFFECT_RADIUS_25_YARDS      = 20,
    EFFECT_RADIUS_35_YARDS      = 21,
    EFFECT_RADIUS_200_YARDS     = 22,
    EFFECT_RADIUS_40_YARDS      = 23,
    EFFECT_RADIUS_65_YARDS      = 24,
    EFFECT_RADIUS_70_YARDS      = 25,
    EFFECT_RADIUS_4_YARDS       = 26,
    EFFECT_RADIUS_50_YARDS      = 27,
    EFFECT_RADIUS_50000_YARDS   = 28,
    EFFECT_RADIUS_6_YARDS       = 29,
    EFFECT_RADIUS_500_YARDS     = 30,
    EFFECT_RADIUS_80_YARDS      = 31,
    EFFECT_RADIUS_12_YARDS      = 32,
    EFFECT_RADIUS_99_YARDS      = 33,
    EFFECT_RADIUS_55_YARDS      = 35,
    EFFECT_RADIUS_0_YARDS       = 36,
    EFFECT_RADIUS_7_YARDS       = 37,
    EFFECT_RADIUS_21_YARDS      = 38,
    EFFECT_RADIUS_34_YARDS      = 39,
    EFFECT_RADIUS_9_YARDS       = 40,
    EFFECT_RADIUS_150_YARDS     = 41,
    EFFECT_RADIUS_11_YARDS      = 42,
    EFFECT_RADIUS_16_YARDS      = 43,
    EFFECT_RADIUS_0_5_YARDS     = 44,   // 0.5 yards
    EFFECT_RADIUS_10_YARDS_2    = 45,
    EFFECT_RADIUS_5_YARDS_2     = 46,
    EFFECT_RADIUS_15_YARDS_2    = 47,
    EFFECT_RADIUS_60_YARDS      = 48,
    EFFECT_RADIUS_90_YARDS      = 49,
    EFFECT_RADIUS_15_YARDS_3    = 50,
    EFFECT_RADIUS_60_YARDS_2    = 51,
    EFFECT_RADIUS_5_YARDS_3     = 52,
    EFFECT_RADIUS_60_YARDS_3    = 53,
    EFFECT_RADIUS_50000_YARDS_2 = 54,
    EFFECT_RADIUS_130_YARDS     = 55,
    EFFECT_RADIUS_38_YARDS      = 56,
    EFFECT_RADIUS_45_YARDS_2    = 57,
    EFFECT_RADIUS_32_YARDS      = 59,
    EFFECT_RADIUS_44_YARDS      = 60,
    EFFECT_RADIUS_14_YARDS      = 61,
    EFFECT_RADIUS_47_YARDS      = 62,
    EFFECT_RADIUS_23_YARDS      = 63,
    EFFECT_RADIUS_3_5_YARDS     = 64,   // 3.5 yards
    EFFECT_RADIUS_80_YARDS_2    = 65
};

// Spell pet auras
class TC_GAME_API PetAura
{
    private:
        typedef std::unordered_map<uint32, uint32> PetAuraMap;

    public:
        PetAura() : removeOnChangePet(false), damage(0) { }

        PetAura(uint32 petEntry, uint32 aura, bool _removeOnChangePet, int _damage) :
        removeOnChangePet(_removeOnChangePet), damage(_damage)
        {
            auras[petEntry] = aura;
        }

        uint32 GetAura(uint32 petEntry) const
        {
            PetAuraMap::const_iterator itr = auras.find(petEntry);
            if (itr != auras.end())
                return itr->second;
            PetAuraMap::const_iterator itr2 = auras.find(0);
            if (itr2 != auras.end())
                return itr2->second;
            return 0;
        }

        void AddAura(uint32 petEntry, uint32 aura)
        {
            auras[petEntry] = aura;
        }

        bool IsRemovedOnChangePet() const
        {
            return removeOnChangePet;
        }

        int32 GetDamage() const
        {
            return damage;
        }

    private:
        PetAuraMap auras;
        bool removeOnChangePet;
        int32 damage;
};
typedef std::map<uint32, PetAura> SpellPetAuraMap;

struct TC_GAME_API SpellArea
{
    uint32 spellId;
    uint32 areaId;                                          // zone/subzone/or 0 is not limited to zone
    uint32 questStart;                                      // quest start (quest must be active or rewarded for spell apply)
    uint32 questEnd;                                        // quest end (quest must not be rewarded for spell apply)
    int32  auraSpell;                                       // spell aura must be applied for spell apply)if possitive) and it must not be applied in other case
    uint32 raceMask;                                        // can be applied only to races
    Gender gender;                                          // can be applied only to gender
    uint32 questStartStatus;                                // QuestStatus that quest_start must have in order to keep the spell
    uint32 questEndStatus;                                  // QuestStatus that the quest_end must have in order to keep the spell (if the quest_end's status is different than this, the spell will be dropped)
    bool autocast;                                          // if true then auto applied at area enter, in other case just allowed to cast

    // helpers
    bool IsFitToRequirements(Player const* player, uint32 newZone, uint32 newArea) const;
};

typedef std::multimap<uint32, SpellArea> SpellAreaMap;
typedef std::multimap<uint32, SpellArea const*> SpellAreaForQuestMap;
typedef std::multimap<uint32, SpellArea const*> SpellAreaForAuraMap;
typedef std::multimap<uint32, SpellArea const*> SpellAreaForAreaMap;
typedef std::multimap<std::pair<uint32, uint32>, SpellArea const*> SpellAreaForQuestAreaMap;
typedef std::pair<SpellAreaMap::const_iterator, SpellAreaMap::const_iterator> SpellAreaMapBounds;
typedef std::pair<SpellAreaForQuestMap::const_iterator, SpellAreaForQuestMap::const_iterator> SpellAreaForQuestMapBounds;
typedef std::pair<SpellAreaForAuraMap::const_iterator, SpellAreaForAuraMap::const_iterator>  SpellAreaForAuraMapBounds;
typedef std::pair<SpellAreaForAreaMap::const_iterator, SpellAreaForAreaMap::const_iterator>  SpellAreaForAreaMapBounds;
typedef std::pair<SpellAreaForQuestAreaMap::const_iterator, SpellAreaForQuestAreaMap::const_iterator> SpellAreaForQuestAreaMapBounds;

// Spell rank chain  (accessed using SpellMgr functions)
struct SpellChainNode
{
    SpellInfo const* prev;
    SpellInfo const* next;
    SpellInfo const* first;
    SpellInfo const* last;
    uint8  rank;
};

typedef std::unordered_map<uint32, SpellChainNode> SpellChainMap;

//                   spell_id  req_spell
typedef std::multimap<uint32, uint32> SpellRequiredMap;
typedef std::pair<SpellRequiredMap::const_iterator, SpellRequiredMap::const_iterator> SpellRequiredMapBounds;

//                   req_spell spell_id
typedef std::multimap<uint32, uint32> SpellsRequiringSpellMap;
typedef std::pair<SpellsRequiringSpellMap::const_iterator, SpellsRequiringSpellMap::const_iterator> SpellsRequiringSpellMapBounds;

// Spell learning properties (accessed using SpellMgr functions)
struct SpellLearnSkillNode
{
    uint16 skill;
    uint16 step;
    uint16 value;                                           // 0  - max skill value for player level
    uint16 maxvalue;                                        // 0  - max skill value for player level
};

typedef std::unordered_map<uint32, SpellLearnSkillNode> SpellLearnSkillMap;

struct SpellLearnSpellNode
{
    uint32 spell;
    bool active;                                            // show in spellbook or not
    bool autoLearned;
};

typedef std::multimap<uint32, SpellLearnSpellNode> SpellLearnSpellMap;
typedef std::pair<SpellLearnSpellMap::const_iterator, SpellLearnSpellMap::const_iterator> SpellLearnSpellMapBounds;

typedef std::multimap<uint32, SkillLineAbilityEntry const*> SkillLineAbilityMap;
typedef std::pair<SkillLineAbilityMap::const_iterator, SkillLineAbilityMap::const_iterator> SkillLineAbilityMapBounds;

typedef std::multimap<uint32, uint32> PetLevelupSpellSet;
typedef std::map<uint32, PetLevelupSpellSet> PetLevelupSpellMap;

typedef std::map<uint32, uint32> SpellDifficultySearcherMap;

struct PetDefaultSpellsEntry
{
    uint32 spellid[MAX_CREATURE_SPELL_DATA_SLOT];
};

// < 0 for petspelldata id, > 0 for creature_id
typedef std::map<int32, PetDefaultSpellsEntry> PetDefaultSpellsMap;

typedef std::vector<uint32> SpellCustomAttribute;
typedef std::vector<bool> EnchantCustomAttribute;

typedef std::vector<SpellInfo*> SpellInfoMap;

typedef std::map<int32, std::vector<int32> > SpellLinkedMap;

bool IsPrimaryProfessionSkill(uint32 skill);

inline bool IsProfessionSkill(uint32 skill)
{
    return  IsPrimaryProfessionSkill(skill) || skill == SKILL_FISHING || skill == SKILL_COOKING || skill == SKILL_FIRST_AID;
}

inline bool IsProfessionOrRidingSkill(uint32 skill)
{
    return  IsProfessionSkill(skill) || skill == SKILL_RIDING;
}

bool IsPartOfSkillLine(uint32 skillId, uint32 spellId);

class TC_GAME_API SpellMgr
{
    // Constructors
    private:
        SpellMgr();
        ~SpellMgr();

    // Accessors (const or static functions)
    public:
        static SpellMgr* instance();

        // Spell correctness for client using
        static bool IsSpellValid(SpellInfo const* spellInfo, Player* player = NULL, bool msg = true);

        // Spell difficulty
        uint32 GetSpellDifficultyId(uint32 spellId) const;
        void SetSpellDifficultyId(uint32 spellId, uint32 id);
        uint32 GetSpellIdForDifficulty(uint32 spellId, Unit const* caster) const;
        SpellInfo const* GetSpellForDifficultyFromSpell(SpellInfo const* spell, Unit const* caster) const;

        // Spell Ranks table
        SpellChainNode const* GetSpellChainNode(uint32 spell_id) const;
        uint32 GetFirstSpellInChain(uint32 spell_id) const;
        uint32 GetLastSpellInChain(uint32 spell_id) const;
        uint32 GetNextSpellInChain(uint32 spell_id) const;
        uint32 GetPrevSpellInChain(uint32 spell_id) const;
        uint8 GetSpellRank(uint32 spell_id) const;
        // not strict check returns provided spell if rank not avalible
        uint32 GetSpellWithRank(uint32 spell_id, uint32 rank, bool strict = false) const;

        // Spell Required table
        SpellRequiredMapBounds GetSpellsRequiredForSpellBounds(uint32 spell_id) const;
        SpellsRequiringSpellMapBounds GetSpellsRequiringSpellBounds(uint32 spell_id) const;
        bool IsSpellRequiringSpell(uint32 spellid, uint32 req_spellid) const;

        // Spell learning
        SpellLearnSkillNode const* GetSpellLearnSkill(uint32 spell_id) const;
        SpellLearnSpellMapBounds GetSpellLearnSpellMapBounds(uint32 spell_id) const;
        bool IsSpellLearnSpell(uint32 spell_id) const;
        bool IsSpellLearnToSpell(uint32 spell_id1, uint32 spell_id2) const;

        // Spell target coordinates
        SpellTargetPosition const* GetSpellTargetPosition(uint32 spell_id, SpellEffIndex effIndex) const;

        // Spell Groups table
        SpellSpellGroupMapBounds GetSpellSpellGroupMapBounds(uint32 spell_id) const;
        bool IsSpellMemberOfSpellGroup(uint32 spellid, SpellGroup groupid) const;

        SpellGroupSpellMapBounds GetSpellGroupSpellMapBounds(SpellGroup group_id) const;
        void GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells) const;
        void GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells, std::set<SpellGroup>& usedGroups) const;

        // Spell Group Stack Rules table
        bool AddSameEffectStackRuleSpellGroups(SpellInfo const* spellInfo, int32 amount, std::map<SpellGroup, int32>& groups) const;
        SpellGroupStackRule CheckSpellGroupStackRules(SpellInfo const* spellInfo1, SpellInfo const* spellInfo2) const;
        SpellGroupStackRule GetSpellGroupStackRule(SpellGroup groupid) const;

        // Spell proc table
        SpellProcEntry const* GetSpellProcEntry(uint32 spellId) const;
        bool CanSpellTriggerProcOnEvent(SpellProcEntry const& procEntry, ProcEventInfo& eventInfo) const;

        // Spell bonus data table
        SpellBonusEntry const* GetSpellBonusData(uint32 spellId) const;

        // Spell threat table
        SpellThreatEntry const* GetSpellThreatEntry(uint32 spellID) const;

        SkillLineAbilityMapBounds GetSkillLineAbilityMapBounds(uint32 spell_id) const;

        PetAura const* GetPetAura(uint32 spell_id, uint8 eff) const;

        SpellEnchantProcEntry const* GetSpellEnchantProcEvent(uint32 enchId) const;
        bool IsArenaAllowedEnchancment(uint32 ench_id) const;

        const std::vector<int32> *GetSpellLinked(int32 spell_id) const;

        PetLevelupSpellSet const* GetPetLevelupSpellList(uint32 petFamily) const;
        PetDefaultSpellsEntry const* GetPetDefaultSpellsEntry(int32 id) const;

        // Spell area
        SpellAreaMapBounds GetSpellAreaMapBounds(uint32 spell_id) const;
        SpellAreaForQuestMapBounds GetSpellAreaForQuestMapBounds(uint32 quest_id) const;
        SpellAreaForQuestMapBounds GetSpellAreaForQuestEndMapBounds(uint32 quest_id) const;
        SpellAreaForAuraMapBounds GetSpellAreaForAuraMapBounds(uint32 spell_id) const;
        SpellAreaForAreaMapBounds GetSpellAreaForAreaMapBounds(uint32 area_id) const;
        SpellAreaForQuestAreaMapBounds GetSpellAreaForQuestAreaMapBounds(uint32 area_id, uint32 quest_id) const;

        // SpellInfo object management
        SpellInfo const* GetSpellInfo(uint32 spellId) const { return spellId < GetSpellInfoStoreSize() ?  mSpellInfoMap[spellId] : NULL; }
        // Use this only with 100% valid spellIds
        SpellInfo const* AssertSpellInfo(uint32 spellId) const
        {
            ASSERT(spellId < GetSpellInfoStoreSize());
            SpellInfo const* spellInfo = mSpellInfoMap[spellId];
            ASSERT(spellInfo);
            return spellInfo;
        }
        uint32 GetSpellInfoStoreSize() const { return mSpellInfoMap.size(); }

    private:
        SpellInfo* _GetSpellInfo(uint32 spellId) { return spellId < GetSpellInfoStoreSize() ?  mSpellInfoMap[spellId] : NULL; }

    // Modifiers
    public:

        // Loading data at server startup
        void UnloadSpellInfoChains();
        void LoadSpellTalentRanks();
        void LoadSpellRanks();
        void LoadSpellRequired();
        void LoadSpellLearnSkills();
        void LoadSpellLearnSpells();
        void LoadSpellTargetPositions();
        void LoadSpellGroups();
        void LoadSpellGroupStackRules();
        void LoadSpellProcs();
        void LoadSpellBonusess();
        void LoadSpellThreats();
        void LoadSkillLineAbilityMap();
        void LoadSpellPetAuras();
        void LoadEnchantCustomAttr();
        void LoadSpellEnchantProcData();
        void LoadSpellLinked();
        void LoadPetLevelupSpellMap();
        void LoadPetDefaultSpells();
        void LoadSpellAreas();
        void LoadSpellInfoStore();
        void UnloadSpellInfoStore();
        void UnloadSpellInfoImplicitTargetConditionLists();
        void LoadSpellInfoCustomAttributes();
        void LoadSpellInfoCorrections();
        void LoadSpellInfoSpellSpecificAndAuraState();
        void LoadSpellInfoDiminishing();

    private:
        SpellDifficultySearcherMap mSpellDifficultySearcherMap;
        SpellChainMap              mSpellChains;
        SpellsRequiringSpellMap    mSpellsReqSpell;
        SpellRequiredMap           mSpellReq;
        SpellLearnSkillMap         mSpellLearnSkills;
        SpellLearnSpellMap         mSpellLearnSpells;
        SpellTargetPositionMap     mSpellTargetPositions;
        SpellSpellGroupMap         mSpellSpellGroup;
        SpellGroupSpellMap         mSpellGroupSpell;
        SpellGroupStackMap         mSpellGroupStack;
        SpellProcMap               mSpellProcMap;
        SpellBonusMap              mSpellBonusMap;
        SpellThreatMap             mSpellThreatMap;
        SpellPetAuraMap            mSpellPetAuraMap;
        SpellLinkedMap             mSpellLinkedMap;
        SpellEnchantProcEventMap   mSpellEnchantProcEventMap;
        EnchantCustomAttribute     mEnchantCustomAttr;
        SpellAreaMap               mSpellAreaMap;
        SpellAreaForQuestMap       mSpellAreaForQuestMap;
        SpellAreaForQuestMap       mSpellAreaForQuestEndMap;
        SpellAreaForAuraMap        mSpellAreaForAuraMap;
        SpellAreaForAreaMap        mSpellAreaForAreaMap;
        SpellAreaForQuestAreaMap   mSpellAreaForQuestAreaMap;
        SkillLineAbilityMap        mSkillLineAbilityMap;
        PetLevelupSpellMap         mPetLevelupSpellMap;
        PetDefaultSpellsMap        mPetDefaultSpellsMap;           // only spells not listed in related mPetLevelupSpellMap entry
        SpellInfoMap               mSpellInfoMap;
};

#define sSpellMgr SpellMgr::instance()

#endif
