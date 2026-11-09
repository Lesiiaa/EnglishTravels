using UnrealBuildTool;

public class EnglishTravelsEditorTarget : TargetRules
{
	public EnglishTravelsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;

		ExtraModuleNames.AddRange( new string[] { "EnglishTravels" } );
	}
}
