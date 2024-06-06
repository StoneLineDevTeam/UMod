// UMod.Target.cs
using UnrealBuildTool;
using System.Collections.Generic;

public class UModTarget : TargetRules
{
    public UModTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;

        ExtraModuleNames.AddRange(new string[] { "UMod" });
    }
}
