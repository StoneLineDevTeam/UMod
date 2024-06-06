using UnrealBuildTool;
using System.Collections.Generic;

public class UModEditor : ModuleRules
{
    public UModEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMod" });

        PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "Slate", "SlateCore" });

        // Add any additional module dependencies your module needs here
    }
}
