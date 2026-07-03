package com.capslock800000.optimizedmg

object MobileGluesLoader {
    @JvmStatic
    fun load() {
        try {
            System.loadLibrary("mobileglues")
        } catch (_: UnsatisfiedLinkError) {
            // If dependency loading fails, OptimizedMG will surface the issue during startup.
        }
    }
}
