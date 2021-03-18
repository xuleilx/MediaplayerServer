# MediaplayerServer
# 功能
1. 音乐、视频播放，暂停，快进，快退，seek等基本功能
2. 获取音乐媒体文件id3信息
3. 通过配置文件定制插件
# 编译
```shell
# mkdir build
# cd build
# cmake ../
# make
```
# 运行
```shell
# 启动dbus服务
export DBUS_SESSION_BUS_ADDRESS="unix:path=/run/dbus/session_bus_help"
dbus-daemon --session --address=unix:path=/run/dbus/session_bus_help &

# 启动多媒体播放服务
./mediaplayerserver &

# 查看所有接口
dbus-send --session --print-reply --dest=com.hsae.mediaplayerserver.mediaplayer /com/hsae/mediaplayerserver org.freedesktop.DBus.Introspectable.Introspect 

# 设置播放文件
dbus-send --session --print-reply --type=method_call --dest=com.hsae.mediaplayerserver.mediaplayer /com/hsae/mediaplayerserver com.hsae.mediaplayerserver.mediaplayer.setFile  string:"/home/xuleilx/mywork/multimedia/video/AVC_high_1280x720_2013.mp4"
# 播放
dbus-send --session --print-reply --type=method_call --dest=com.hsae.mediaplayerserver.mediaplayer /com/hsae/mediaplayerserver com.hsae.mediaplayerserver.mediaplayer.start
```
