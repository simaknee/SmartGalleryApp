#include "AndroidPermissionHelper.h"

#if PLATFORM_ANDROID
#include "AndroidPermissionFunctionLibrary.h"
#endif

void FAndroidPermissionHelper::RequestStoragePermission() {
#if PLATFORM_ANDROID
    TArray<FString> Permissions;
    Permissions.Add(TEXT("android.permission.READ_EXTERNAL_STORAGE"));

    UAndroidPermissionFunctionLibrary::AcquirePermissions(Permissions);
#endif
}