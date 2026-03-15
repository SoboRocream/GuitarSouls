using UnrealBuildTool;

public class GuitarSoulsGAS : ModuleRules
{
    public GuitarSoulsGAS(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(new string[]{"GuitarSoulsGAS"});

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "GuitarSouls"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "GameFeatures",
                "GameplayAbilities",
                "GameplayTasks",
                "GameplayTags"
            }
        );
    }
}