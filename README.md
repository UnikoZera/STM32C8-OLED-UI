# これは我的第一个STM工程(写的很糟见谅233)！
- __目前支持像lua一样的tween补全,他们的语法相似(而且有逻辑判断可以在while里面被调用!不会造成多次触发Tween导致的不正常)__
- __目前支持贪吃蛇、2048游戏、视频播放功能 还有部分菜单功能__
- __支持多级菜单__

## 型号和设计
- 使用STM的CUBEIDE配置的底层代码(型号为STM32C8T6)
- OLED配置是SSD1315|SSD1306，分辨率是128X64
- OLED的ui部分还需要灵感来发挥tween的强大233
- 使用C语言来完成主体部分
- _其实tween的灵感来源于psych engine_
- _还需要挺长的维护的233_
- ![引脚定义](image-1.png)

## 格式的要求
- 如果你想要oled播放视频，那你就得需要flash模块！(64kb flash实在难以发挥)
- #### Flash格式:
1. 在BasicAddr之后需要一个int来指示pointer的总数目
2. 紧接着int后面是pointer的数组(uint32_t)
3. pointer之后是你的视频data

- 注意pointer是指向某一帧的地址
- pointer的目的是为了能够轻松实现快进、快退功能
- 通过更改BasicAddr就可以做到换视频
- _建议使用ESP32S3来烧录flash_

## 实物图(暂时不可用)