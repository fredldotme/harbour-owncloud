package me.fredl.ghostcloud;

import android.app.Application;
import android.content.ContentResolver;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;
import java.lang.Object;
import org.qtproject.qt5.android.QtNative;

public class ShareUtil {
    protected ShareUtil()
    {
    }

    public static String getFilePathForContent(String contentUri)
    {
        String[] projection = {MediaStore.MediaColumns.DATA};
        String path = "";

        ContentResolver cr = QtNative.activity().getApplicationContext().getContentResolver();
        Cursor metaCursor = cr.query(Uri.parse(contentUri),
                                     projection,
                                     null,
                                     null,
                                     null);
        if (metaCursor != null) {
            try {
                if (metaCursor.moveToFirst()) {
                    path = metaCursor.getString(0);
                }
            } finally {
                metaCursor.close();
            }
        }
        return path;
    }
}
