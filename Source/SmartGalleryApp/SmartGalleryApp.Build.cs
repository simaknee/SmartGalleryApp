// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SmartGalleryApp : ModuleRules
{
	public SmartGalleryApp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NNE", "ImageWrapper", "RenderCore", "RHI", "SlateCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "AssetRegistry", "ApplicationCore" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "Launch",
                "AndroidRuntimeSettings",
                "AndroidPermission",
            });

            // Android JNI 연동 활성화
            PrivateIncludePaths.Add("SmartGalleryApp/Private");
        }
    }
}
