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
import android.os.Build;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.content.Context;

public class AndroidGalleryHelper {
    private static native void OnGalleryImagesLoaded(String[] ImagePaths);
    private static final String TAG = "AndroidGalleryHelper";
    private static final int REQUEST_CODE_PICK_FOLDER = 9999;
    private static AndroidGalleryHelper instance;
    private Activity activity;

    public static void init(Activity activity) {
        if (instance == null) {
            instance = new AndroidGalleryHelper(activity);
        } else {
            instance.activity = activity; // 🟢 항상 최신 activity 유지
        }
    }

    public static AndroidGalleryHelper getInstance(Activity activity) {
        if (instance == null) {
            instance = new AndroidGalleryHelper(activity);
        }
        return instance;
    }

    private AndroidGalleryHelper(Activity activity) {
        this.activity = activity;
    }

    public void openFolderPicker() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        activity.startActivityForResult(intent, REQUEST_CODE_PICK_FOLDER);
    }

    public static void handleFolderResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_CODE_PICK_FOLDER && resultCode == Activity.RESULT_OK) {
            Uri folderUri = data.getData();
            if (folderUri != null) {
                Log.d("AndroidGalleryHelper", "Selected Folder: " + folderUri.toString());

                ArrayList<String> imagePaths = getImagesFromFolder(folderUri);
                
                nativeOnGalleryFolderSelected(imagePaths.toArray(new String[0]));
            }
        }

        
    }

    private static ArrayList<String> getImagesFromFolder(Uri folderUri) {
        /*
        ArrayList<String> imagePaths = new ArrayList<>();
        String folderDocId = DocumentsContract.getTreeDocumentId(folderUri);
        Uri childrenUri = DocumentsContract.buildChildDocumentsUriUsingTree(folderUri, folderDocId);

        Cursor cursor = instance.activity.getContentResolver().query(childrenUri, new String[]{DocumentsContract.Document.COLUMN_DOCUMENT_ID}, null, null, null);
        if (cursor != null) {
            try {
                while (cursor.moveToNext()) {
                    String docId = cursor.getString(0);
                    Uri fileUri = DocumentsContract.buildDocumentUriUsingTree(folderUri, docId);

                    // 파일이 이미지인지 확인
                    if (fileUri.toString().endsWith(".jpg") || fileUri.toString().endsWith(".png")) {
                        String realPath = getRealPathFromURI(fileUri);
                        if (realPath != null && !realPath.isEmpty()) {
                            imagePaths.add(realPath);
                        } else {
                            Log.w(TAG, "Skipping file: Could not resolve real path for " + fileUri.toString());
                        }
                    }
                }
            } finally {
                cursor.close();
            }
        }
        return imagePaths;
        */
    ArrayList<String> imagePaths = new ArrayList<>();

    // 1️⃣ 선택된 폴더의 실제 경로 변환
        String folderPath = getFolderPathFromUri(folderUri);
        if (folderPath == null || folderPath.isEmpty()) {
            Log.e(TAG, "getImagesFromFolder: Failed to resolve folder path.");
            return imagePaths;
        }

        Log.d(TAG, "Searching images in folder: " + folderPath);

        // 2️⃣ MediaStore에서 해당 폴더의 이미지들만 필터링
        Uri uri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
        String[] projection = {MediaStore.Images.Media.DATA};
        String selection;
        String[] selectionArgs;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            String relativePath = folderPath.replace("/storage/emulated/0/", "") + "/";
            selection = MediaStore.Images.Media.RELATIVE_PATH + " LIKE ?";
            selectionArgs = new String[]{ relativePath + "%" };
        } 
        else {
            selection = MediaStore.Images.Media.DATA + " LIKE ?";
            selectionArgs = new String[]{ folderPath + "/%" };
        }
        Cursor cursor = instance.activity.getContentResolver().query(uri, projection, selection,selectionArgs,null);

        if (cursor != null) {
            try {
                int columnIndex = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
                while (cursor.moveToNext()) {
                    String imagePath = cursor.getString(columnIndex);
                    imagePaths.add(imagePath);
                    Log.d(TAG, "getImagesFromFolder: collect image: " + imagePath);
                }
            } finally {
                cursor.close();
            }
        }
        else {
            Log.w(TAG, "getImagesFromFolder: Create cursor failed.");
        }
        Log.d(TAG, "Found " + imagePaths.size() + " images in folder.");
        return imagePaths;
    
    }

    private static String getFolderPathFromUri(Uri folderUri) {
        String folderPath = null;
        String docId = DocumentsContract.getTreeDocumentId(folderUri);
        String[] split = docId.split(":");

        if (split.length >= 2) {
            String storageType = split[0];
            String relativePath = split[1];

            if ("primary".equalsIgnoreCase(storageType)) {
                folderPath = Environment.getExternalStorageDirectory() + "/" + relativePath;
            } else {
                // SD 카드나 외장 저장소 경로 처리
                File[] externalDirs = instance.activity.getExternalFilesDirs(null);
                for (File dir : externalDirs) {
                    if (dir.getAbsolutePath().contains(storageType)) {
                        folderPath = dir.getAbsolutePath().replace("/Android/data/" + instance.activity.getPackageName() + "/files", "") + "/" + relativePath;
                        break;
                    }
                }
            }
        }

        if (folderPath != null) {
            Log.d(TAG, "Resolved folder path: " + folderPath);
        } else {
            Log.e(TAG, "Failed to resolve folder path from URI: " + folderUri.toString());
        }
        
        return folderPath;
    }
    private static String getRealPathFromURI(Uri contentUri) {
        String result = null;
        Cursor cursor = instance.activity.getContentResolver().query(contentUri, new String[]{MediaStore.Images.Media.DATA}, null, null,  MediaStore.Images.Media.DATE_ADDED + " DESC");
        if (cursor != null) {
            try {
                int columnIndex = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
                if (cursor.moveToFirst()) {
                    result = cursor.getString(columnIndex);
                }
            } catch (IllegalArgumentException e) {
                Log.e(TAG, "Column not found: " + e.getMessage());
            } finally {
                cursor.close();
            }
        }
        else {
            Log.w(TAG, "getRealPathFromURI: Create cursor failed.");
        }
        if (result == null) {
            Log.w(TAG, "getRealPathFromURI: Could not resolve path, returning empty string.");
            return "";
        }
        Log.d(TAG, "Found Image Path: "+result);
        return result;
    }

    public static native void nativeOnGalleryFolderSelected(String[] imagePaths);
    
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