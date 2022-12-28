package com.glasssix;

public class Process {
    static {
        System.loadLibrary("gx-process-invoker");
    }

    public static native int startSync(String cmdline, IStdoutCallback callback);
}
