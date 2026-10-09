# 迷宫游戏

这是一个使用 C 语言和 `libgraphics` 开发的 Windows 迷宫游戏。项目支持随机生成迷宫、手动编辑迷宫、自动寻找最短路径，以及保存和加载迷宫地图。

## 项目结构

```text
Maze_Game/
├── CMakeLists.txt          CMake 构建配置
├── src/
│   ├── main.c              程序入口，定义 Main()
│   ├── common/
│   │   └── types.h         基础类型与常量（MAZESIZE/GRIDSIZE/point），零依赖
│   ├── core/               迷宫逻辑与状态，不依赖图形库
│   │   ├── param.h/.c      全部全局状态的唯一归属地
│   │   ├── maze_model.c    随机生成、放置门与钥匙、记录格子坐标
│   │   ├── pathfind.c      广度优先搜索与求解
│   │   ├── queue.c/.h      路径链表（类型名为 Path）
│   │   └── file.c/.h       迷宫文件读写
│   └── ui/                 应用层，唯一接触 libgraphics 的地方
│       ├── app.c/.h        主循环、定时器回调、界面分发
│       ├── render.c        图元绘制与迷宫渲染
│       ├── menu.c          菜单栏与选择界面
│       └── input.c         键盘/字符/鼠标事件回调
├── libgraphics/            图形库源文件
├── include/                图形库头文件
├── MazeList/               迷宫数据文件
├── Music/                  音乐资源
└── FileName.txt            迷宫文件名记录
```

依赖方向为 `ui → core → common`，只能向下，不能反向。`core/` 下的代码不包含任何图形库头文件，因此可以脱离窗口单独编译测试。

`libgraphics/graphics.c` 定义了 Windows 程序入口 `WinMain()`，然后调用 `src/main.c` 中的 `Main()`。因此，本项目没有常见的 `int main()`。

## 构建环境

本项目面向 Windows，推荐使用以下工具：

- Windows 10 或 Windows 11
- CMake 3.24 或更高版本
- 64 位 MinGW-w64 GCC
- `mingw32-make`，或者 Ninja

在 PowerShell 中检查工具是否可用：

```powershell
cmake --version
gcc --version
mingw32-make --version
```

如果提示无法识别命令，请先安装对应工具，并把其 `bin` 目录加入系统的 `PATH` 环境变量。

建议将项目放在不含中文字符的路径中，以减少旧版图形库和字符编码带来的兼容性问题。

## 使用 CMake 构建

下面的命令均在 PowerShell 中执行。

### 1. 进入项目根目录

请确认当前目录中存在 `CMakeLists.txt`：

```powershell
Get-ChildItem CMakeLists.txt
```

### 2. 配置 Debug 构建

使用 MinGW Makefiles：

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
```

参数说明：

- `-S .`：当前目录是源码目录。
- `-B build`：将生成文件和编译产物放进 `build` 目录。
- `-G "MinGW Makefiles"`：使用 MinGW 的 Makefile 构建器。
- `-DCMAKE_BUILD_TYPE=Debug`：生成便于调试的版本。

### 3. 编译

```powershell
cmake --build build --parallel
```

`--parallel` 会并行编译多个源文件。构建完成后，可执行文件通常位于：

```text
build\GraphicsProject.exe
```

### 4. 运行

程序使用相对路径访问 `Music`、`MazeList` 和 `FileName.txt`，所以运行时的工作目录必须是项目根目录。

保持 PowerShell 位于项目根目录，然后执行：

```powershell
.\build\GraphicsProject.exe
```

不要先进入 `build` 目录再运行，否则程序可能无法找到音乐和迷宫数据。

### 5. 重新编译

修改源代码之后，一般不需要重新配置 CMake，直接执行：

```powershell
cmake --build build --parallel
```

CMake 只会重新编译发生变化的源文件，然后重新链接程序。

### 6. 构建 Release 版本

建议让 Debug 和 Release 使用不同的构建目录：

```powershell
cmake -S . -B build-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel
```

运行 Release 版本：

```powershell
.\build-release\GraphicsProject.exe
```

## 使用 Ninja 构建（可选）

如果已经安装 Ninja，可以将生成器换成 Ninja：

```powershell
cmake -S . -B build-ninja -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build-ninja --parallel
```

运行：

```powershell
.\build-ninja\GraphicsProject.exe
```

同一个构建目录不能随意切换生成器。例如，已经用 `MinGW Makefiles` 配置的 `build` 目录，不能再直接改用 Ninja；应为 Ninja 创建新的构建目录。

## CMakeLists.txt 的作用

项目的 `CMakeLists.txt` 主要完成以下工作：

1. 声明这是一个 C 项目。
2. 指定使用 C99 标准。
3. 将 `include` 添加到头文件搜索路径。
4. 编译项目源码和 `libgraphics` 源码。
5. 生成 `GraphicsProject.exe`。
6. 链接 Windows 多媒体库 `winmm`，供 `PlaySound()` 使用。

CMake 负责生成构建规则，真正执行 C 语言编译的仍然是 GCC。

## 常见问题

### 1. `WNDPROC` 或 `GraphicsEventProc` 类型不兼容

在较新的 64 位 MinGW GCC 中，可能出现以下错误：

```text
assignment to 'WNDPROC' from incompatible pointer type
```

原因是旧版 `libgraphics` 将窗口回调函数的返回值声明成了 32 位 `LONG`，而 64 位 Windows 的 `WNDPROC` 要求使用指针宽度的 `LRESULT`。

需要在 `libgraphics/graphics.c` 中将函数声明和函数定义的返回类型都改为：

```c
static LRESULT CALLBACK GraphicsEventProc(HWND hwnd, UINT msg,
                                           WPARAM wParam, LPARAM lParam)
```

一共修改两处：文件开头附近的函数声明，以及 `GraphicsEventProc` 的函数定义。不要使用强制类型转换来隐藏错误。

修改后直接重新构建：

```powershell
cmake --build build --parallel
```

### 2. 找不到 `graphics.h`

如果出现：

```text
fatal error: graphics.h: No such file or directory
```

请确认 `include/graphics.h` 存在，并且 CMake 配置是从包含 `CMakeLists.txt` 的项目根目录执行的。

### 3. 找不到 `PlaySound` 的实现

如果链接时出现与 `PlaySound` 有关的 `undefined reference`，请检查 `CMakeLists.txt` 中是否包含：

```cmake
target_link_libraries(GraphicsProject winmm)
```

### 4. 程序可以启动，但没有音乐或迷宫列表

这通常不是编译问题，而是运行时工作目录不正确。应从项目根目录执行：

```powershell
.\build\GraphicsProject.exe
```

### 5. CMake 提示生成器或编译器与缓存不一致

这表示当前构建目录曾经被另一个生成器或编译器配置过。最简单的处理方式是换一个新的构建目录，例如：

```powershell
cmake -S . -B build-new -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build-new --parallel
```

## 字符编码（重要）

源码文件统一使用 **UTF-8**，但编译时必须让执行字符集为 **GBK**。`CMakeLists.txt` 已包含：

```cmake
add_compile_options(-finput-charset=UTF-8 -fexec-charset=GBK)
```

原因：`libgraphics` 通过 ANSI 版 Win32 API 绘制文本（`graphics.c` 中的 `TextOut` / `CreateFont`，项目未定义 `UNICODE`），这些 API 按**系统 ANSI 代码页**解释字节，中文 Windows 上是 GBK。`PlaySound` 同样接收 ANSI 路径。如果中文字面量以 UTF-8 字节写入 exe，界面会全部乱码，且 `Music/魔术先生.wav` 找不到而无声。

如果你不用 CMake 而手动调用 gcc，必须自己带上这两个选项：

```powershell
gcc -std=gnu99 -finput-charset=UTF-8 -fexec-charset=GBK `
    -Iinclude -Isrc/common -Isrc/core -Isrc/ui `
    src/main.c src/core/*.c src/ui/*.c libgraphics/*.c `
    -o GraphicsProject.exe -lwinmm -lgdi32 -luser32
```

## 游戏操作

- 使用 WASD 或方向键移动角色。
- 通过菜单创建、编辑、保存或加载迷宫。
- 使用自动求解功能显示最短路径。
