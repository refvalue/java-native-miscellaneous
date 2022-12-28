package com.glasssix;

public class Program {
    public static void main(String[] args) {
        int code = Process.startSync("python.exe D:/test.py", content -> System.out.println(content));

        System.out.println("Code: " + code);
    }
}
