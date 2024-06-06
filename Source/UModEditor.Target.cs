// UModEditor.Target.cs
using UnrealBuildTool;
using System.Collections.Generic;

public class UModEditorTarget : TargetRules
{
    public UModEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;

        ExtraModuleNames.AddRange(new string[] { "UMod" });
    }
}
