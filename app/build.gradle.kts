
plugins {
    id("com.android.application")
    id("kotlin-kapt")
    id("org.jetbrains.kotlin.android")
}

android {
    namespace = "com.vizbox4d"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.vizbox4d"
        minSdk = 26
        targetSdk = 34
        versionCode = 9
        versionName = "1.009"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"

        /*ndk { // reduces compilation size by restricting CPU type
            abiFilters += listOf("armeabi-v7a", "arm64-v8a", "x86", "x86_64")
        }*/
    }

    buildTypes {
        release {
            isMinifyEnabled = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
            signingConfig = signingConfigs.getByName("debug")
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }

    kotlinOptions {
        jvmTarget = "1.8"
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    buildFeatures {
        viewBinding = true
    }

    flavorDimensions += "version"
    productFlavors {
        create("free") {
            dimension = "version"
            applicationIdSuffix = ".free"
            resValue("string", "app_name", "Xeratude Free")
        }
        create("paid") {
            dimension = "version"
            //applicationIdSuffix = ".paid"
            resValue("string", "app_name", "Xeratude")
        }
    }

    /*buildscript {
        repositories {
            google()
            mavenCentral()
        }
    }
    allprojects {
        repositories {
            google()
            mavenCentral()
        }
    }*/
}

dependencies {

    //val roomVersion = "2.6.1"
    //val activityVersion = "1.7.0"
    //val ktxVersion = "2.8.2"

    implementation("com.github.yukuku:ambilwarna:2.0.1")

    //implementation("com.google.firebase:firebase-auth:23.0.0")
    //implementation("com.google.firebase:firebase-common-ktx:21.0.0")
    //implementation("com.google.firebase:firebase-database-ktx:21.0.0")

    //implementation("androidx.core:core-ktx:1.13.1")
    //implementation("org.postgresql:postgresql:42.7.3")

    //implementation("androidx.appcompat:appcompat:$activityVersion")
    //implementation(platform("com.google.firebase:firebase-bom:33.1.0"))
    //implementation("com.firebaseui:firebase-ui-auth:8.0.2")
    //implementation("androidx.activity:activity-ktx:1.9.0")
    implementation("com.google.android.material:material:1.12.0")
    //implementation("androidx.constraintlayout:constraintlayout:2.1.4")
    //testImplementation("junit:junit:4.13.2")
    //androidTestImplementation("androidx.test.ext:junit:1.1.5")
    //androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")

    // room database dependencies
    //implementation("androidx.room:room-runtime:$roomVersion")
    //implementation("androidx.room:room-ktx:$roomVersion")
    //kapt("androidx.room:room-compiler:$roomVersion")
    //annotationProcessor("android.arch.persistence.room:compiler:1.1.1")
    //annotationProcessor("androidx.room:room-compiler:$room_version")
    //implementation("com.google.code.gson:gson:2.11.0")

    // postgreSQL JDBC dependencies
    //implementation("org.postgresql:postgresql:42.7.3")
    //implementation("mysql:mysql-connector-java:8.0.33")
    //implementation("com.firebase:firebase-client-android:2.5.2")

    // view model dependencies
    //implementation("androidx.lifecycle:lifecycle-viewmodel-ktx:$ktxVersion")
    //implementation("androidx.lifecycle:lifecycle-livedata-ktx:$ktxVersion")
    //implementation("androidx.lifecycle:lifecycle-extensions:2.2.0")
    implementation("androidx.fragment:fragment-ktx:1.8.0")
    //kapt("androidx.lifecycle:lifecycle-compiler:$ktxVersion")
    //api("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.8.1")
    //api("org.jetbrains.kotlinx:kotlinx-coroutines-android:1.8.1")

    // ad dependencies
    //implementation("com.google.android.gms:play-services-ads:23.1.0")
}