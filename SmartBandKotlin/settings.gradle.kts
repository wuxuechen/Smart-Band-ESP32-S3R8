pluginManagement {
    repositories {
        // 阿里云镜像
        maven(url = "https://maven.aliyun.com/repository/gradle-plugin")
        maven(url = "https://maven.aliyun.com/repository/google")
        maven(url = "https://maven.aliyun.com/repository/public")
        // 官方兜底
        gradlePluginPortal()
        google()
        mavenCentral()
    }
}

dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.PREFER_SETTINGS)
    repositories {
        // 阿里云镜像（快）
        maven(url = "https://maven.aliyun.com/repository/google")
        maven(url = "https://maven.aliyun.com/repository/public")
        // 官方兜底（全）
        google()
        mavenCentral()
    }
}

rootProject.name = "SmartBandKotlin" // 改成你的项目名
include(":app")
