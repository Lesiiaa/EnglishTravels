using UnrealBuildTool;

public class EnglishTravelsTarget : TargetRules
{
	public EnglishTravelsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;

		ExtraModuleNames.AddRange( new string[] { "EnglishTravels" } );
	}
}
