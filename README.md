
#项目介绍

只能拖拽文件到listcontrol里面，
点击上传之后会上传到oss

##配置介绍
;OSS的必要4个配置
[oss]
OSS_ENDPOINT=
ACCESS_KEY_ID=
ACCESS_KEY_SECRET=
BUCKET_NAME=

[system]
;是否默认开启gzip，可以再界面点击取消
gzip=1
;多层次用 '/' 隔开，默认目录，可以再界面修改
oss_dir=a/b/c

##演示

![image](https://github.com/ls9527/aliosscompress/blob/master/doc/show.gif)



##联系我
如果你觉得有用，想要升级这个项目，就在Star那里点击一下，
达到10个之后：会智能判断文件类型，
达到30个之后：支持目录方式的上传

