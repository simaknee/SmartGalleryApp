package com.YourCompany.SmartGalleryApp;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import java.util.ArrayList;
import android.util.Log;
import java.io.File;

public class AndroidGalleryHelper {
    private static native void OnGalleryImagesLoaded(String[] ImagePaths);
    private static final String TAG = "AndroidGalleryHelper";
    
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

    public static boolean moveImageFile(String sourcePath, String destinationPath) {
        try {
            File sourceFile = new File(sourcePath);
            File destinationFile = new File(destinationPath);

            if (!sourceFile.exists()) {
                Log.e(TAG, "Source file does not exist: " + sourcePath);
                return false;
            }

            File parentDir = destinationFile.getParentFile();
            if (parentDir != null && !parentDir.exists()) {
                boolean dirCreated = parentDir.mkdirs();
                if (!dirCreated) {
                    Log.e(TAG, "Failed to create destination directory: " + parentDir.getAbsolutePath());
                    return false;
                }
            }

            boolean success = sourceFile.renameTo(destinationFile);
            if (success) {
                Log.i(TAG, "File moved successfully: " + destinationPath);
            } else {
                Log.e(TAG, "Failed to move file: " + sourcePath + " -> " + destinationPath);
            }
            return success;
        } catch (Exception e) {
            Log.e(TAG, "Exception in moveImageFile", e);
            return false;
        }
    }
}