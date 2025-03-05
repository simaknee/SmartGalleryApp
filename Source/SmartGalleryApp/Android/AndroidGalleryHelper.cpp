#include "AndroidGalleryHelper.h"

#include "ImageUtils.h"
#include "Async/Async.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

FOnGalleryImagesLoaded FAndroidGalleryHelper::OnGalleryImagesLoadedCallback;

void FAndroidGalleryHelper::LoadGalleryImages() {
#if PLATFORM_ANDROID
    if (JNIEnv* Env = FAndroidApplication::GetJavaEnv()) {
        jclass Class = FAndroidApplication::FindJavaClass("com/YourCompany/SmartGalleryApp/AndroidGalleryHelper");
        jmethodID Method = Env->GetStaticMethodID(Class, "LoadGalleryImages", "(Landroid/app/Activity;)V");

        jobject Activity = FAndroidApplication::GetGameActivityThis();
        Env->CallStaticVoidMethod(Class, Method, Activity);

        Env->DeleteLocalRef(Class);
    }
#endif
}

void FAndroidGalleryHelper::OnGalleryImagesLoaded(const TArray<FString>& ImagePaths) {
    UE_LOG(LogTemp, Log, TEXT("Gallery Images Loaded: %d images"), ImagePaths.Num());

    AsyncTask(ENamedThreads::GameThread, [ImagePaths]() {
        if (OnGalleryImagesLoadedCallback.IsBound()) {
            OnGalleryImagesLoadedCallback.Execute(ImagePaths);
        }
        });
}

// Java -> C++ 콜백 (여러 이미지 경로 수신)
extern "C"
JNIEXPORT void JNICALL Java_com_YourCompany_SmartGalleryApp_AndroidGalleryHelper_OnGalleryImagesLoaded(JNIEnv* Env, jclass, jobjectArray ImagePaths) {
    TArray<FString> ImageList;
    jsize ArrayLength = Env->GetArrayLength(ImagePaths);

    for (jsize i = 0; i < ArrayLength; i++) {
        jstring JavaString = (jstring)Env->GetObjectArrayElement(ImagePaths, i);
        const char* PathChars = Env->GetStringUTFChars(JavaString, nullptr);
        FString ImagePath(UTF8_TO_TCHAR(PathChars));
        Env->ReleaseStringUTFChars(JavaString, PathChars);
        Env->DeleteLocalRef(JavaString);

        ImageList.Add(ImagePath);
    }

    FAndroidGalleryHelper::OnGalleryImagesLoaded(ImageList);
}