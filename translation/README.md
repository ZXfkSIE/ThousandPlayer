This page describes notes about Simplified-to-Traditional Chinese conversion.

# 简繁转换指南

可以使用[OpenCC](https://github.com/BYVoid/OpenCC)将`zh_CN`翻译文件转换为`zh_TW`，命令行格式为（具体路径请自行手动补全）：
```shell
opencc -i ThousandPlayer_zh_CN.ts -o ThousandPlayer_zh_TW.ts -c s2twp.json
```
但要注意的是该软件并非完美，转换后应手工处理以下问题：

- 该软件会将“项目”转换为“專案”，故转换后应手动将“專案”全盘替换为“項目”。