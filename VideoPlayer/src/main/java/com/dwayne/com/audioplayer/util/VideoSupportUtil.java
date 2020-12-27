package com.dwayne.com.audioplayer.util;

import android.media.MediaCodecList;

import java.util.HashMap;
import java.util.Map;

/**
 * @author Dwayne
 * @email dev1024@foxmail.com
 * @time 20/12/27 21:34
 * @change
 * @chang time
 * @class describe
 */

public class VideoSupportUtil {
    private static Map<String, String> codecMap = new HashMap<>();

    static {
        codecMap.put("h264", "video/avc");
    }

    public static String findVideoCodecName(String ffcodename) {
        if(codecMap.containsKey(ffcodename)) {
            return codecMap.get(ffcodename);
        }
        return "";
    }

    public static boolean isSupportCodec(String ffcodename) {
        boolean support = false;
        int count = MediaCodecList.getCodecCount();
        for(int i = 0; i < count; i++) {
            String[] types = MediaCodecList.getCodecInfoAt(i).getSupportedTypes();
            for(int j = 0; j < types.length; j++) {
                if(types[j].equals(findVideoCodecName(ffcodename))) {
                    support = true;
                    break;
                }
            }
            if(support) {
                break;
            }
        }
        return support;
    }
}
