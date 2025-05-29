# 迷宫游戏 - 使用说明

## 项目简介
这是一个基于C语言和libgraphics库开发的迷宫游戏程序，具有以下功能：
- 随机生成迷宫
- 手动编辑迷宫
- 自动求解最短路径
- 保存和加载迷宫地图
- 可视化游戏界面

## 环境要求
- Windows操作系统
- CLion IDE (推荐) 或支持CMake的编译环境

## 安装步骤

### 1. 下载项目
下载项目压缩包并解压到任意位置（确保路径中不包含中文字符），例如解压到桌面，得到文件夹。

### 2. 安装CLion
1. 访问JetBrains官网下载CLion: [https://www.jetbrains.com/zh-cn/clion/](https://www.jetbrains.com/zh-cn/clion/)
2. 安装CLion

### 3. 导入项目
1. 打开CLion
2. 点击"File" → "Open"
3. 选择解压后的`Maze_Game-master`文件夹
4. 在弹出的对话框中勾选"Trust project"选项，点击"Trust Project"

### 4. 构建项目
1. 等待CMake自动配置完成（底部状态栏显示"CMake project is loaded"）
2. 如果没有自动生成`cmake-build-debug`文件夹，点击底部工具栏的"CMake"，然后点击重新加载按钮

### 5. 配置运行设置
1. 点击右上角的"Edit Configurations"
2. 确保"Working directory"设置为`Maze_Game-master`文件夹路径
3. 点击"OK"保存设置

## 运行程序
1. 点击右上角的绿色运行按钮编译并运行程序
2. 请确保打开声音，程序会播放背景音乐

## 替代运行方式
如果想直接运行可执行文件：
1. 导航到`cmake-build-debug`文件夹
2. 将`Maze_Game-master.exe`复制到`Maze_Game-master`文件夹中
3. 直接运行`Maze_Game-master.exe`

## 注意事项
- 请确保运行时的路径不包含中文字符
- 程序运行需要访问`MazeList`和`Music`文件夹中的资源文件
- 如果遇到任何问题，请检查工作目录设置是否正确

## 游戏操作
- 使用WASD或方向键移动角色
- 通过菜单可以创建、编辑、保存和加载迷宫
- 游戏提供自动求解功能，可以显示最短路径

祝您游戏愉快！
