// 根目录 build.gradle.kts
buildscript {
    repositories {
        // 阿里云镜像
        maven(url = "https://maven.aliyun.com/repository/google")
        maven(url = "https://maven.aliyun.com/repository/public")
        // 官方兜底
        google()
        mavenCentral()
    }
    dependencies {

    }
}

allprojects {
    repositories {
        // 阿里云镜像
        maven(url = "https://maven.aliyun.com/repository/google")
        maven(url = "https://maven.aliyun.com/repository/public")
        // 官方兜底
        google()
        mavenCentral()
    }
}

tasks.register<Delete>("clean") {
    delete(rootProject.buildDir)
}
