plugins {
    alias(libs.plugins.android.application)
    // Pure native app - no Kotlin needed
    // alias(libs.plugins.kotlin.android)
}

android {
    namespace = "com.sovereigndroid.core"
    compileSdk = 36

    defaultConfig {
        applicationId = "com.sovereigndroid.core"
        minSdk = 24
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        
        // Native build configuration
        // Phase 1: Support both 64-bit and 32-bit ARM architectures
        ndk {
            abiFilters.addAll(listOf("arm64-v8a", "armeabi-v7a"))
        }
        
        externalNativeBuild {
            cmake {
                // CMake arguments for native build
                arguments("-DANDROID_STL=c++_static")
                cFlags("-Wall", "-Werror")
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    
    // External native build configuration
    // Links to CMakeLists.txt in cpp directory
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
    
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    // kotlinOptions removed - pure native app
}

dependencies {
    // Pure native app - no Java/Kotlin dependencies needed
    // All functionality is in C/C++ native code
    testImplementation(libs.junit)
}