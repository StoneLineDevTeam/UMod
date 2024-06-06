using UnrealBuildTool;
using System.Collections.Generic;

public class UMod : ModuleRules
{
    public UMod(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Add any additional module dependencies your module needs here

        // Update PublicLibraryPaths to PublicSystemLibraries/PublicSystemLibraryPaths as suggested
        // Example:
        // PublicSystemLibraryPaths.Add("Path/To/Library");

        // Handle any UEBuildConfiguration replacements
        // Example: if (Target.Configuration == UnrealTargetConfiguration.Debug)
        // {
        //    // Do something for Debug configuration
        // }
    }
}
