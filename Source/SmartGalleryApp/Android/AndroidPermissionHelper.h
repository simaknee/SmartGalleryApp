#pragma once

#include "CoreMinimal.h"

#if PLATFORM_ANDROID
#include "AndroidPermissionFunctionLibrary.h"
#endif

class FAndroidPermissionHelper {
public:
    static void RequestStoragePermission();
};