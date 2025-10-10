#pragma once

UENUM(BlueprintType)
enum class EGSEnumDefine : uint8
{
    Stamina,
    Health,
};

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
    Front,
    Back,
    Left,
    Right,
};

UENUM(BlueprintType)
enum class ESwitchingDirection : uint8
{
    None,
    Left,
    Right,
};

UENUM(BlueprintType)
enum class ECombatType : uint8
{
    None,
    SwordShield,
    TwoHanded,
    MeleeFists
};

UENUM(BlueprintType)
enum class EWeaponCollisionType : uint8
{
    MainCollision,
    SecondCollision,
};

UENUM(BlueprintType)
enum class EGSAIBehavior : uint8
{
    Idle,
    Patrol,
    MeleeAttack,
    Approach,
    Stunned,
    Strafe,
};

UENUM(BlueprintType)
enum class EGSArmourType : uint8
{
    Chest,
    Pants,
    Boots,
    Gloves,
};

UENUM(BlueprintType)
enum class EGSEquipmentType : uint8
{
    Weapon,
    Shield,
};

UENUM(BlueprintType)
enum class EGSDamageType : uint8
{
    Normal,
    KnockBack,
};