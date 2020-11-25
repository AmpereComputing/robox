## 1.2 robox源码下载

```shell
【注意】下载anbox主线代码，commit为 3ed2e6d5c360d57b6aa61386e279adf3ff155ded
# cd /home/
# git clone https://github.com/anbox/anbox.git
# cd anbox/
# git reset --hard 3ed2e6d5c360d57b6aa61386e279adf3ff155ded
//显示信息如下信息回退成功
//HEAD is now at 3ed2e6d Merge pull request #1258 from stephengroat/dockerfile

//x86机器上，下载后目录为:/home/anbox
【注意】如源码下载失败，提示验证，通过以下跳过验证
# export GIT_SSL_NO_VERIFY=1
```

## 1.3 安卓源码下载，补丁合入

### 1.3.1 下载安卓源码

```shell
# mkdir ~/bin
# PATH=~/bin:$PATH
//下载 Repo 工具，并确保它可执行：
# curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
# chmod a+x ~/bin/repo
//安卓源码下载
# mkdir -p /home/android
# cd /home/android/
# repo init -u https://github.com/anbox/platform_manifests.git -b anbox
# repo sync -j64  //同步代码，下载后代码目录总计100GB左右

//使用仓中repo仓中快照xml文件，回退repo仓代码，快照位置/binaryFiles/snapshot20191206.xml
# cp snapshot20191206.xml /home/android/.repo/manifests/
# repo init -m snapshot20191206.xml
# repo sync -d -j64   //进行代码回退
```

```shell
//安卓源码下anbox目录替换
//删除/home/android/vendor/目录下的anbox目录
# rm -rf /home/android/vendor/anbox
//将下载的anbox代码，拷贝到指定目录
# cp -r /home/anbox /home/android/vendor/
```

### 1.3.2 合入OpenGLES3.0特性补丁

```shell
//将aosp_patch/目录下的 0001号补丁拷贝到安卓源码如下目录
# cp 0001_opengles3_android.patch /home/android/vendor/anbox/
# cd /home/android/vendor/anbox/
# patch -p1 < 0001_opengles3_android.patch
```

### 1.3.3 合入adb连接相关补丁

```shell
//(1)修改adb转发路径，同时连接台数限制
# cp 0002_android_adb.diff /home/android/system/core/
# cd /home/android/system/core/
# patch -p1 < 0002_android_adb.diff

//(2)修改rootfs的挂载属性
# cp 0003_android_mount_rw.diff /home/android/system/core/
# cd /home/android/system/core/
# patch -p1 < 0003_android_mount_rw.diff
```

### 1.3.4 合入获取FPS数据补丁

```shell
//将patches/Android目录下的 0004号补丁拷贝到安卓源码如下目录
# cp 0004_android_frameworks_native_printFPS.diff  /home/android/frameworks/native/
# cd /home/android/frameworks/native/
# patch -p1 < 0004_android_frameworks_native_printFPS.diff

//使用说明
  (1)使用新编译的安卓镜像启动robox实例；
  (2)adb shell登陆到robox容器实例中，或者使用docker exec登录到robox容器实例中
  (3)在安卓系统内执行 
     setprop debug.egl.call_freq true
  (4)在安卓系统内，通过logcat命令查看帧率显示
     logcat | grep libEGL
```

### 1.3.5 合入arm32toarm64转码补丁

```shell
【说明】Exagear/补丁分为安卓源码补丁，和物理机kernel补丁
# cd Exagear/
# ls
  android      //安卓源码转码补丁
  kernel       //物理机内核的转码补丁
# cd android/
# ls
  android-7.1.1_r13.patch     //安卓系统的转码补丁
  vendor                      //转码所需，拷贝到安卓源码根目录下

//安卓系统所需合入的源码补丁，为/home/exagear/android/android-7.1.1_r13.patch
# cd /home/android/   //进入到下载的安卓源码更目录
//合入补丁
# patch -p1 < /home/exagear/android/android-7.1.1_r13.patch 

//将压缩包中vendor目录拷贝到aosp源码下，合并原来的vendor目录
//说明：/home/android/目录下已经有vender/anbox/目录，拷贝转码中的vender目录过来后，不会直接覆盖，而是会将两个vender目录合并
# cp -r /home/exagear/android/vendor/* /home/android/vendor/
# ls /home/android/vendor/
  anbox
  huawei
//上面的huawei目录包含转码相关

//合入patches/Android目录下的 0005号补丁，使得exagear编译进aosp镜像
# cp 0005_android_anbox_mk.diff /home/android/vendor/anbox/
# cd /home/android/vendor/anbox/
# patch -p1 < 0005_android_anbox_mk.diff
```

### 1.3.6 合入ip特性补丁

```shell
//将patches/Android目录下的 0006号拷贝到安卓源码如下目录
# cp 0006_generate_ipconfig_feature.patch /home/android/vendor/anbox/
# cd /home/android/vendor/anbox/
# patch -p2 < 0006_generate_ipconfig_feature.patch
```

### 1.3.7 合入多实例补丁

```shell
//将patches/Android目录下的 0007号拷贝到安卓源码如下目录
# cp 0007_multi_instances_support.patch /home/android/vendor/anbox/
# cd /home/android/vendor/anbox/
# patch -p2 < 0007_multi_instances_support.patch
```

### 1.3.8 合入Zygtote32和编译选项支持补丁

```shell
//将patches/Android目录下的 0008号拷贝到安卓源码如下目录
# cp 0008_Zygtote32_and_other_compile_options_support.patch /home/android/vendor/anbox/
# cd /home/android/vendor/anbox/
# patch -p2 < 0008_Zygtote32_and_other_compile_options_support.patch
```

### 1.3.9 opengl2.0部分游戏运行崩溃修复补丁

```shell
//将patches/Android目录下的 0009号补丁拷贝到安卓源码如下目录
# cp 0009_android_base_gl2.0_modify.diff /home/android/frameworks/base
# cd /home/android/frameworks/base
# patch -p1 < 0009_android_base_gl2.0_modify.diff
```

### 1.3.10 合入aosp部分的vmalloc性能补丁
```shell
//将patches/Android目录下的 0010号补丁拷贝到aosp源码如下目录
# cp 0010_android_frameworks_base_vmalloc.diff /home/android/frameworks/base
# cd /home/android/frameworks/base
# patch -p4 < 0010_android_frameworks_base_vmalloc.diff
```

### 1.3.11 按home键/或者游戏退出回到home界面没有图标问题修复补丁

```shell
//将patches/Android目录下的 0011号补丁拷贝到vendor/anbox目录
# cp 0011_vendor_anbox_launcher.diff /home/android/vendor/anbox
# cd /home/android/vendor/anbox
# patch -p1 < 0011_vendor_anbox_launcher.diff
```

### 1.3.12 合入屏蔽音频ALSA的错误打印补丁

```shell
//将patches/Android目录下的 0012号补丁拷贝到aosp源码如下目录
# cp 0012_android_audio_ALSA_no_log.diff /home/android/vendor/anbox/
# cd /home/android/vendor/anbox/
# patch -p1 < 0012_android_audio_ALSA_no_log.diff
```

### 1.3.13 合入其他bug fix补丁

```shell
//将patches/Android目录下的 0013 0014号补丁拷贝到aosp源码如下目录
# cp 0013_anbox_other_buxfix.patch /home/android/vendor/anbox/
# cp 0014_fsp_lock.patch /home/android/vendor/anbox/
# cd /home/android/vendor/anbox/
# patch -p2 < 0013_anbox_other_buxfix.patch
# patch -p1 < 0014_fsp_lock.patch
```

### 1.3.13 编译安卓源码

```shell
//编译安卓镜像
# cd /home/android/
# source build/envsetup.sh   //配置编译环境
# lunch anbox_arm64-userdebug   //选择编译环境
# export JACK_EXTRA_CURL_OPTIONS=-k
# export LC_ALL=C
# make -j48      //编译
//在/home/android/out/target/product/arm64/目录下会生成需要的ramdisk.img和system.img 安卓镜像文件
```

### 1.3.14 生成安卓镜像

```shell
//合成robox所需的docker容器镜像文件：android.img文件
# cd /home/android/vendor/anbox/
# scripts/create-package.sh /home/android/out/target/product/arm64/ramdisk.img /home/android/out/target/product/arm64/system.img
//将在当前目录，即/home/android/vendor/anbox/下合成android.img镜像
//将该android.img文件拷贝到鲲鹏920服务器的/home目录下
```
