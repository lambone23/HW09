﻿// ChatBaseball.Build.cs

using UnrealBuildTool;

public class ChatBaseball : ModuleRules
{
	public ChatBaseball(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
        PublicDependencyModuleNames.AddRange(new string[]
        {
            // Initial Dependencies
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
    
            // UI
            "UMG", "Slate", "SlateCore",
        });

        PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.AddRange(new string[] { "ChatBaseball" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}

