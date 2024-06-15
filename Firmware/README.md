# 板卡固件烧录

---
**注意：当前固件仅编译完成，尚未完善，如果没有软件开发能力，暂时请勿烧录！！！！！！**

---

## 一、环境配置
1. 下载并安装[VSCode](https://code.visualstudio.com)；
2. 打开VSCode，进入插件市场，搜索Chinese，安装简体中文插件，之后重启VSCode；

<div style="display: flex; align-items: center;">  
  <img src="image/plugin.png" alt="Image 1" style="width: auto; height: 300px; margin-right: 10px;">  
  <img src="image/Chinese.png" alt="Image 2" style="width: auto; height: 300px;">  
</div>

<!-- ![图片](image/plugin.png)
![img](image/Chinese.png) -->
3. 重启后进入插件市场，搜索platformio，选择第一个结果安装，之后重启VSCode；
<div style="display: flex; align-items: center;">  
  <img src="image/platformio.png" alt="Image 2" style="width: auto; height: 300px;">  
</div>

## 二、导入工程
1. 进入VSCode后打开platformio，等待platformio core初始化完成；
<div style="display: flex; align-items: center;">  
  <img src="image/openplatformio.png" alt="Image 2" style="width: 500px; height: auto;">  
</div>

2. 点击**Pick a folder**，选择当前仓库的**Firmware**目录；
<div style="display: flex; align-items: center;">  
  <img src="image/pickfolder.png" alt="Image 1" style="width: auto; height: 300px; margin-right: 10px;">  
  <img src="image/folder.png" alt="Image 2" style="width: auto; height: 300px;">  
</div>

3. 等待工程初始化完成（第一次初始化会比较慢）。

## 三、固件下载
1. 使用USB线连接板卡和电脑；
2. 点击下方Upload按钮，等待固件烧录完成后即可。
<div style="display: flex; align-items: center;">  
  <img src="image/upload.png" alt="Image 2" style="width: 500px; height: auto;">  
</div>