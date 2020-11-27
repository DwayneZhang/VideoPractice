package com.dwayne.com.audioplayer;

/**
 * @author admin
 * @email dev1024@foxmail.com
 * @time 2020/11/27 14:52
 * @change
 * @chang time
 * @class describe
 */

public class TimeInfoBean {

    private int currentTime;
    private int totalTime;

    public int getCurrentTime() {
        return currentTime;
    }

    public void setCurrentTime(int currentTime) {
        this.currentTime = currentTime;
    }

    public int getTotalTime() {
        return totalTime;
    }

    public void setTotalTime(int totalTime) {
        this.totalTime = totalTime;
    }

    @Override
    public String toString() {
        return "TimeInfoBean{" +
                "currentTime=" + currentTime +
                ", totalTime=" + totalTime +
                '}';
    }
}
