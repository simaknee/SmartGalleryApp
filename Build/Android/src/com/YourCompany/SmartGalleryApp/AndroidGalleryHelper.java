package com.YourCompany.SmartGalleryApp;

import android.app.Activity;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;
import java.util.ArrayList;

public class AndroidGalleryHelper {
    private static native void OnGalleryImagesLoaded(String[] ImagePaths);

    public static void LoadGalleryImages(Activity activity) {
        ArrayList<String> imagePaths = new ArrayList<>();
        Uri uri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;

        String[] projection = {MediaStore.Images.Media.DATA};
        Cursor cursor = activity.getContentResolver().query(uri, projection, null, null, MediaStore.Images.Media.DATE_ADDED + " DESC");

        if (cursor != null) {
            int columnIndex = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
            while (cursor.moveToNext()) {
                String imagePath = cursor.getString(columnIndex);
                imagePaths.add(imagePath);
            }
            cursor.close();
        }

        if (!imagePaths.isEmpty()) {
            OnGalleryImagesLoaded(imagePaths.toArray(new String[0]));
        }
    }
}