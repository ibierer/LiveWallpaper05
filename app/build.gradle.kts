plugins {
    id("com.android.application")
    id("kotlin-android")
    id("kotlin-kapt")
    id("com.google.gms.google-services")
}

android {
    namespace = "com.example.livewallpaper05"
    compileSdk = 33

    defaultConfig {
        applicationId = "com.example.livewallpaper05"
        minSdk = 26
        targetSdk = 33
        versionCode = 1
        versionName = "1.0"
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
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
}



dependencies {
    implementation("com.github.yukuku:ambilwarna:2.0.1")

    implementation("com.google.firebase:firebase-auth:22.3.1")
    implementation("com.google.firebase:firebase-common-ktx:20.4.2")
    var room_version = "2.4.3"
    var activity_version = "1.6.1"

    implementation("androidx.core:core-ktx:1.9.0")
    implementation("org.postgresql:postgresql:42.7.1")

    implementation("androidx.appcompat:appcompat:$activity_version")
    implementation(platform("com.google.firebase:firebase-bom:32.7.2"))
    implementation("com.firebaseui:firebase-ui-auth:7.2.0")
    implementation("androidx.activity:activity-ktx:$activity_version")
    implementation("com.google.android.material:material:1.8.0")
    implementation("androidx.constraintlayout:constraintlayout:2.1.4")
    testImplementation("junit:junit:4.13.2")
    androidTestImplementation("androidx.test.ext:junit:1.1.5")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")

    // room database dependencies
    implementation("androidx.room:room-runtime:$room_version")
    implementation("androidx.room:room-ktx:$room_version")
    kapt("androidx.room:room-compiler:$room_version")
    annotationProcessor("android.arch.persistence.room:compiler:1.1.1")
    //annotationProcessor("androidx.room:room-compiler:$room_version")
    implementation("com.google.code.gson:gson:2.8.8")

    // postgreSQL JDBC dependencies
    implementation("org.postgresql:postgresql:42.7.1")
    implementation("mysql:mysql-connector-java:5.1.46")
    implementation("com.firebase:firebase-client-android:2.5.0")

    // view model dependencies
    implementation("androidx.lifecycle:lifecycle-viewmodel-ktx:2.6.0")
    implementation("androidx.lifecycle:lifecycle-livedata-ktx:2.6.0")
    implementation("androidx.lifecycle:lifecycle-extensions:2.2.0")
    implementation("androidx.fragment:fragment-ktx:1.6.2")
    kapt("androidx.lifecycle:lifecycle-compiler:2.2.0")
    api("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.6.4")
    api("org.jetbrains.kotlinx:kotlinx-coroutines-android:1.6.4")
}