# 迷宫游戏重构计划

> 分支：`refac`　基准提交：`3d5716b`
> 本文档是**对照用的施工单**。每完成一项就把 `[ ]` 改成 `[x]`，并在"验证记录"里写一句结果。
> 遇到与本文件描述不符的代码，先在"偏差记录"里记下来，再决定是否修改本文档。

---

## 0. 重构纪律（先读这一节）

这四条是后面所有步骤的前提，违反任何一条都会让进度无法验证。

### 0.1 重构与修 bug 必须分开提交

当前代码里"看起来像 bug"和"故意这么写的"混在一起。最典型的例子：

`main.c:39-41` 先 `readFileName()`，紧接着 `initMazePoint()`，而 `initMazePoint()`（`maze.c:424-425`）内部会调 `initMaze()` **无条件重新随机一个迷宫**。

这到底是想让程序"随机开局"还是"忘了删一行"？现在无法判断。所以：

- **阶段 1–5 只做结构搬移，不改变任何可观测行为。**
- 发现的可疑行为一律先记入第 8 节，**不改**。
- 阶段 6 才动行为，且每改一条都要能对应到第 8 节的编号。

### 0.2 每步都可编译、可运行

不要出现"改到一半编译不过，攒着最后一起修"。每个小项做完就 `cmake --build build --parallel` 并跑一次程序。

### 0.3 第三方库当黑盒

`libgraphics/`（8 个 `.c`）和 `include/`（11 个头文件）**本次重构完全不动**。它们的问题（GBK 编码、`WNDPROC` 返回类型等）已在 `README.md` 的常见问题里记录，与本次重构无关。

### 0.4 先冻结数据

`MazeList/` 三个文件、`FileName.txt`、`Music/魔术先生.wav` 在重构期间**不要修改**。它们是测试夹具。

---

## 1. 阶段总览

| 阶段 | 目标 | 产出 | 风险 |
|---|---|---|---|
| 1 | 建安全网 | 特征测试基线 | 低 |
| 2 | 拆依赖 | `maze_model` / `pathfind` / `maze_io` 三个无 UI 依赖的模块 | **高** |
| 3 | 消灭魔法数 | `CellType` 枚举 + `cell_passable()` | 中 |
| 4 | 状态机化 | `GameState` 枚举替掉 12 个布尔 | **高** |
| 5 | 收敛重复逻辑 | 单一 `try_move()` | 中 |
| 6 | 修 bug | 逐条修复 + 验证 | 中 |
| 7 | 隔离图形库 | `render.c` 成为唯一接触点 | 低 |
| 8 | 工程化 | CMake 分 target、开警告、自动化测试 | 低 |

**依赖关系**：1 → 2 → (3, 4 可并行) → 5 → 6 → 7 → 8。
阶段 2 和 4 是仅有的两次"推翻性"改动，务必在其他事情之前做完。

---

## 2. 阶段 1：建安全网

**这一步不能跳。** 后面每一步都靠它判断"行为有没有变"。当前代码几乎不可测，这正是它难以修改的根本原因。

### 1.1 确认基线可运行

- [x] `git status` 干净，确认在 `refac` 分支
- [x] `cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug`
- [x] `cmake --build build --parallel` 成功
- [x] 在**项目根目录**运行 `.\build\GraphicsProject.exe`（不能在 `build\` 里运行，见 `README.md` 第 85 行）
- [x] 手动玩一遍，确认：能移动、能随机生成、能编辑、能保存、能打开、能求解、能自动执行
- [x] 截图存档到 `docs/baseline/`（界面视觉也是行为的一部分）

### 1.2 建立"纯逻辑"可执行文件

这是本阶段的核心。目标：**不开窗口**就能跑 BFS 和地图读写。

- [ ] 新建 `tests/probe.c`，写一个 `int main()`（注意：业务代码里没有 `main()`，`libgraphics/graphics.c` 提供 `WinMain` 并回调 `Main()`；测试程序必须独立，不链接 `graphics.c`）
- [ ] `probe.c` 只链接 `maze.c` 中**纯逻辑部分**——但此时 `maze.c` 还跟 `graphics.h` 耦合，所以本项在阶段 2 完成后才能真正干净。先做下面的"临时版"：
  - [ ] 临时版：`probe.c` 仍然链接全部源码，但把 `main()` 命名为 `Main()` 之外的名字，并**不调用任何渲染函数**，只调用 `hasAns()` / `readMaze()`
  - [ ] 记录这个临时版能跑通的最小调用序列
- [ ] 阶段 2 完成后，回来把临时版改成只链接 `maze_core`

### 1.3 记录基线输出

- [ ] 对 `MazeList/` 三个地图文件，逐个输出以下数值，存到 `docs/baseline/logic_baseline.txt`：

| 记录项 | 来源函数 | 说明 |
|---|---|---|
| 地图文件名 | `readMaze(n)` 返回值 | 注意它返回 `p->name`，是链表内部缓冲区 |
| 玩家/终点坐标 | `file.c:63-64` 读入的 `playerX/playerY/targetX/targetY` | |
| `hasAns()` 返回值 | `maze.c:343` | 当前语义是"最少步数"，但见第 8 节 B-3 |
| `path` 链表的坐标序列 | `maze.c:359-368` 遍历 | **必须逐点打印**，这是最强的行为断言 |
| `count(path)` | `queue.c:46` | |

- [ ] **同一个地图连续跑 3 次，确认输出完全一致**（确认 `hasAns()` 没有依赖未初始化内存）
- [ ] 随机生成的地图无法固定，所以额外做一份：把 `rand()` 的种子固定（`srand(12345)`），记录一次 `initMaze()` 后的完整 `maze[14][14]` 矩阵，作为随机路径的基线
- [ ] 把基线文件提交到 git（`git add docs/baseline/ && git commit`）

### 1.4 阶段 1 验收

- [ ] `docs/baseline/logic_baseline.txt` 已提交
- [ ] `docs/baseline/` 有界面截图
- [ ] 能一条命令重现基线输出

**验证记录**：
```
（在此填写：日期 / 命令 / 实际输出是否与预期一致）
```

---

## 3. 阶段 2：拆依赖（最高优先级）

### 2.1 先看清现状

当前依赖方向是**反的**：

| 文件 | 反向依赖 |
|---|---|
| `maze.c:3-14` | 14 个 `extern` 全局变量 |
| `file.c:3-7` | 6 个 `extern` 全局变量 |
| `main.c:3-25` | 定义了 12 个全局变量，供上面两个文件反向引用 |

同时 `maze.h:9-14` 把 `graphics.h`/`extgraph.h`/`imgui.h` 全暴露给了所有包含者——**连 `file.h:7` 都 `#include "maze.h"`**，意味着读写文件也间接依赖图形库。

### 2.2 目标结构

```
src/
  core/           ← 无任何 graphics 依赖，可独立测试
    maze_model.h/.c   地图数据、玩家/终点、钥匙/血量
    pathfind.h/.c     BFS、最短路径
    maze_io.h/.c      地图文件读写（替换 file.c）
  ui/             ← 唯一接触 libgraphics 的地方
    render.h/.c       所有绘制
    input.h/.c        键盘/鼠标/字符事件
    menu.h/.c         菜单栏
    app.h/.c          状态机与主循环
  main.c          ← 只剩初始化和 Main()
third_party/libgraphics/   （保持原样，阶段 7 再规整目录）
tests/probe.c
```

### 2.3 搬迁清单

**关键约束：`core/` 下的任何文件都不允许 `#include "graphics.h"` 或 `"extgraph.h"`。** 这点在 code review 时用一条 grep 就能验证。

- [ ] 新建 `src/core/maze_model.h`，定义：
  - [ ] `typedef struct { int x, y; } Cell;`
  - [ ] 地图网格（尺寸来自 `MAZESIZE`，见 `maze.h:18`）
  - [ ] 玩家、终点、`leftKey`、`health`
  - [ ] **不导出**裸数组，改为导出 `CellType maze_at(Cell c)` / `void maze_set(Cell c, CellType t)`
- [ ] `src/core/maze_model.c` 承接 `maze.c` 中的状态变量
- [ ] 新建 `src/core/pathfind.h/.c`，搬迁：
  - [ ] `bfs()`（`maze.c:296-340`）
  - [ ] `hasAns()`（`maze.c:343-388`）
- [ ] 新建 `src/core/maze_io.h/.c`，搬迁 `file.c` 全部
- [ ] 新建 `src/ui/render.h/.c`，搬迁所有 `draw*` 函数（`maze.c:468-1066`）
- [ ] 新建 `src/ui/input.h/.c`，搬迁 `CharEventProcess` / `KeyboardEventProcess` / `MouseEventProcess`（`maze.c:143-272`）
- [ ] 新建 `src/ui/app.h/.c`，搬迁 `display()` / `TimerEventProcess` / `Main()`
- [ ] 删除 `main.c` 中的全局变量定义，改为 `app.c` 内的状态
- [ ] 更新 `CMakeLists.txt`，先保持"编译所有文件"，等到阶段 8 再分 target

### 2.4 处理 `queue.c`

`queue.h:5-9` 的类型名叫 `queue`，但 `prev`/`next` 是双向链表，实际只当 FIFO 用。

- [ ] 把它改名为它真实的东西：`src/core/path.h/.c`，类型名 `Path`
- [ ] **不要顺便改实现**，这个阶段只改名搬家
- [ ] 确认 `push()` 的第二个参数 `prev` 的三个调用点（`maze.c:93`、`maze.c:315` 附近、`maze.c:354`）在改名后语义不变

### 2.5 统一坐标约定（**本阶段最容易改出 bug 的地方，务必先读完**）

#### 已核实的结论：`maze` 实际是 `[行][列]`，而变量名 `playerX/playerY` 是**反的**

三条独立证据：

**证据 1 —— 数据文件。** `MazeList/求求你了呜呜` 第 1、14 行全为 `1`（是上下边框）。`file.c:66-70` 的加载循环是：
```c
for(int i = 0; i < MAZESIZE; i++)
    for(int j = 0; j < MAZESIZE; j++)
        fscanf(fp, "%d", &maze[i][j]);
```
即第一维 `i` 对应文件中"自上而下"的行，是**视觉上的 y（竖直方向）**；第二维 `j` 是**列**。

**证据 2 —— 存档坐标。** 该文件前两行是玩家 `10 11`、终点 `12 4`。用证据 1 的约定查 `maze[10][11]`：文件第 11 行（0-based 行号 10）为
```
1 0 1 1 1 1 0 0 1 1 0 0 1 1
```
索引 11 → `0`，是道路。玩家站位合法。若按 `[x][y]` 理解则应为 `maze[11][10]`，而索引 10 → `0` 也合法，**两个约定都能自洽**，所以这一条只能作为佐证，决定性的是证据 3。

**证据 3 —— 碰撞逻辑必须与加载一致，否则"贴边"行为会错。** 键盘处理（`maze.c:205-206`）中"S / 下"执行的是：
```c
if(!maze[playerX][playerY-1]) playerY -= 1;
```
改变的是**第二维**。结合证据 1（第二维是列），这意味着"下"键实际在左右移动。

#### 这意味着什么

- **当前行为是自洽的**：加载、存档、移动、BFS、绘制五处都遵守同一个 `[i][j]` 约定，所以游戏能正常玩。存档再读回也能还原（`file.c:66-70` 与 `maze.c:748-752` 是对称的循环）。
- **但命名全部是错的**：`playerX` 存的是行号，`playerY` 存的是列号，`mazePoint[i][j].x`（`maze.c:419`）里的 `i` 也是行号。
- **`mazePoint` 与 `maze` 是一致的**（两者都是 `[i][j]`，`i` 为行），所以绘制是对的。

#### 绝对不要做的事

> ⚠️ **不要"修正" `file.c:66-70` 的循环顺序，也不要单独调整任何一处的下标顺序。**
> 只改一部分必然破坏自洽性——地图会转置、玩家会走出边界、存档会错位。
> 这类 bug 不会崩溃，只会让游戏"看起来怪怪的"，极难排查。

#### 安全的做法：一次到位的机械重命名

- [ ] **先写一个金标准测试**：用同一张地图，把 `maze[14][14]` 完整矩阵、玩家坐标、`hasAns()` 返回值全部 dump 到基线文件（阶段 1 已要求）
- [ ] 在 `maze_model.h` 顶部写死这条注释：
  ```c
  /* 坐标约定：grid[row][col]，第一维是行（视觉上的 y），第二维是列（视觉上的 x）。
   * 与 MazeList/ 文件格式一致：文件第 i 行第 j 列 -> grid[i][j]。
   * 不要改成 [x][y]，除非同时改 file.c 的加载循环并重跑全部基线。 */
  ```
- [ ] 引入 `Cell` 结构体并把语义写进字段注释：
  ```c
  typedef struct { int row, col; } Cell;   /* 与 grid[row][col] 对应 */
  ```
- [ ] 把 `playerX/playerY` 重命名为 `player_row/player_col`（或 `playerRow/playerCol`），`targetX/targetY` 同理
- [ ] `bfs()` 的形参 `int x, int y`（`maze.c:296`）一并更名为 `row, col`
- [ ] `queue`/`Path` 节点里的 `int x, y`（`queue.h:6`）同样更名
- [ ] 每改一个文件就重跑金标准测试，逐字节比对
- [ ] 完成后再把 `mazePoint[i][j].x` 那处的 `i/j` 改为 `row/col`

**这是一个纯机械的批量重命名**，风险可控，但必须一次做完、且全程比对基线。收益是后面所有阶段都不会再被这套反命名误导。

#### 顺带处理

- [ ] 把 `maze.c:351-352` 那个 `int** visited` + 指针数组的写法去掉。它确实是多余的：`visited[i] = p[i]` 之后 `visited` 就是 `p` 的别名，`memset(p,0,...)` 实际在清 `visited`。`bfs()` 的第二个参数可以直接改成 `int visited[MAZESIZE][MAZESIZE]` 或一个封装类型
  - 注意：**这不改变行为**，`maze.c:373/380` 的 `memset(p,0,...)` 依赖的正是这个别名关系，改的时候要保证重置逻辑等价

### 2.6 阶段 2 验收

- [ ] `grep -r "graphics.h\|extgraph.h\|imgui.h" src/core/` **无任何结果**
- [ ] 程序行为与阶段 1 截图完全一致
- [ ] `docs/baseline/logic_baseline.txt` 逐字节一致
- [ ] `tests/probe.c` 改成只链接 `core/`，能独立编译运行
- [ ] 提交（建议拆成 2–3 个提交，不要一个巨型提交）

**验证记录**：
```
（在此填写）
```

---

## 4. 阶段 3：消灭魔法数字

### 3.1 现状

`0` / `1` / `4` / `5` 散落在至少 20 处，无符号名。已核实的分布：

| 值 | 含义 | 出现位置（部分） |
|---|---|---|
| 0 | 道路 | `maze.c:172, 189, 206, 223, 314-332` 等 |
| 1 | 障碍 | `maze.c:393-398, 406, 441, 911` |
| 4 | 门 | `maze.c:54, 173, 190, 207, 224, 308, 366, 442, 916` |
| 5 | 钥匙 | `maze.c:57, 179, 308, 376, 457, 922` |

### 3.2 任务

- [ ] 在 `maze_model.h` 定义：
  ```c
  typedef enum {
      CELL_ROAD = 0,
      CELL_WALL = 1,
      CELL_DOOR = 4,
      CELL_KEY  = 5
  } CellType;
  ```
  **注意**：枚举值刻意与文件格式一致，这样阶段 3 可以零行为变化。等到阶段 6 再考虑把文件格式与内存表示解耦。
- [ ] 把**所有**上述位置的裸数字替换成枚举名
- [ ] 抽出通行判定的唯一入口：
  ```c
  bool cell_passable(CellType t, int keys_left);
  ```
- [ ] 用它替换以下三处重复实现：
  - [ ] `maze.c:92-110`（求解过程动画）
  - [ ] `maze.c:314-332`（BFS 扩展，4 个方向四段重复代码）
  - [ ] `maze.c:171-237`（键盘移动，4 个方向四份复制粘贴）
- [ ] 把 `maze.c:314-332` 的四段重复改成对方向数组的循环：
  ```c
  static const int DX[4] = { 1, 0, -1, 0 };
  static const int DY[4] = { 0, 1, 0, -1 };
  ```
  **本次只改 BFS 一处**，键盘那处留到阶段 5

### 3.3 注意事项

`cell_passable` 的语义必须**先对齐再统一**。当前三处的写法有微妙差异：

- `maze.c:314` 的条件是 `!maze[x+1][y] || maze[x+1][y] == 5 || maze[x+1][y] == 4 && leftKey`
- `maze.c:77` 的条件是 `fakeMaze[x][y] == 4 && fakeLeftKey || fakeMaze[x][y] == 5`（注意用的 `fakeLeftKey` 而非 `leftKey`）

- [ ] 先把三处的真值表写成表格（`t` × `keys_left` × 结果），确认完全一致再统一
- [ ] 若不一致，**不改**，记入第 8 节
- [ ] `fakeMaze` / `fakeLeftKey`（`maze.c:33, 21`）是"求解过程"动画的独立副本，属于**刻意的状态复制**，本阶段保留原样

> 注：操作符优先级（`&&` 高于 `||`）在这三处是**一致的**，且写法都符合直觉，不构成 bug。本项只是确认，不是修 bug。

### 3.4 阶段 3 验收

- [ ] `grep -n "maze\[.*\] *== *[0-9]" src/` 无结果
- [ ] `cell_passable` 是唯一判定通行的地方
- [ ] 基线输出一致

**验证记录**：
```
（在此填写）
```

---

## 5. 阶段 4：状态机化

### 4.1 现状：12 个互斥布尔

| 变量 | 定义位置 | 含义 |
|---|---|---|
| `win` | `main.c:3` | 注意是 **3 值**：0 进行中 / 1 赢 / -1 输 |
| `isOpening` | `maze.c:24` | 地图选择中 |
| `isEditing` | `main.c:23` | 手动编辑中 |
| `isChoosingElement` | `maze.c:23` | 元素选择中 |
| `isRenaming` | `maze.c:25` | 重命名中 |
| `isPlaying` | `maze.c:26` | 自动执行中 |
| `isAnswering` | `main.c:19` | 程序求解中 |
| `isNexting` | `main.c:21` | 提示一步 |
| `isShowing` | `maze.c:29` | 求解过程动画 |
| `isHelping` | `maze.c:27` | 帮助界面 |
| `isAbouting` | `maze.c:28` | 关于界面 |
| `hasBegin` | `maze.c:16` | 是否已开始（控制掉血） |

**问题证据**：`maze.c:152-164` 的 ESC 分支一口气置 9 个布尔为 0——这正是"非法状态可表达"的代价。而 `display()`（`maze.c:1069-1111`）用 6 层 `if/else` 把这些布尔按优先级串成状态机。

### 4.2 任务

- [ ] 在 `app.h` 定义：
  ```c
  typedef enum {
      ST_PLAYING,          /* 正常游玩 */
      ST_EDITING,          /* 手动编辑地图 */
      ST_CHOOSING_MAZE,    /* 地图列表 */
      ST_CHOOSING_ELEMENT, /* 元素列表 */
      ST_RENAMING,         /* 重命名输入中 */
      ST_SOLVING,          /* 程序求解：画出完整路径 */
      ST_AUTO_PLAY,        /* 自动执行 */
      ST_SHOWING_PATH,     /* 求解过程动画 */
      ST_HELP,
      ST_ABOUT,
      ST_WIN,
      ST_LOSE
  } GameState;
  ```
- [ ] 把 `win` 的三值语义拆开：
  - [ ] `GameState` 用 `ST_WIN` / `ST_LOSE` 表达胜负
  - [ ] `health` 只表示血量，**不再兼任胜负标志**（现在 `maze.c:41` 是 `if(!health) win = -1;`）
- [ ] 逐个替换 12 个布尔，每替换一个就编译+跑一次
- [ ] 改写 `display()`（`maze.c:1069`）为 `switch (state)`
- [ ] 改写 ESC 分支（`maze.c:152-164`）为单条 `state = ST_PLAYING;`
- [ ] 逐个改写 `drawMenu()`（`maze.c:646-879`）里的 `selection` 分支，把散落的布尔赋值改成状态迁移
  - [ ] FILE 菜单（`maze.c:695-765`）
  - [ ] Edit 菜单（`maze.c:768-800`）
  - [ ] Answer 菜单（`maze.c:803-857`）
  - [ ] Help 菜单（`maze.c:860-875`）

### 4.3 陷阱

有些布尔**不是**状态，别硬塞进枚举：

- `hasDoor` / `hasKey`（`maze.c:18-19`）：是"本局是否已放置门/钥匙"，属于**地图属性**，应放进 `maze_model`
- `hasBegin`（`maze.c:16`）：是"是否已开始计时掉血"，属于**游戏会话属性**，可并入 `maze_model` 或单独一个 `GameSession`
- `element`（`maze.c:22`）：是"当前编辑元素"，是 `ST_EDITING` 的**子状态**，建议独立成 `CellType current_element;`
- `leftKey` / `fakeLeftKey`：前者是真实状态，后者是动画副本

- [ ] 先给每个布尔归类（状态 / 地图属性 / 会话属性 / 子状态），写成表格再动手
- [ ] 归类完成后请人 review 一遍再改代码

### 4.4 验收

- [ ] `grep -n "is[A-Z]" src/` 无结果（或只剩已在表格中明确保留的）
- [ ] 所有状态迁移集中在少数几个函数里，不在渲染函数中修改状态
- [ ] **`draw*` 函数全部变成只读的**——这是本阶段最重要的产出
- [ ] 基线一致
- [ ] `maze.c:152` 那个 ESC 分支已变成一行（这是本阶段的进度条）

**验证记录**：
```
（在此填写）
```

---

## 6. 阶段 5：收敛重复的移动逻辑

### 5.1 现状：同一规则写了 3 遍

| 位置 | 场景 | 特点 |
|---|---|---|
| `maze.c:171-237` | 键盘移动 | 4 个方向各抄一遍，共约 66 行 |
| `maze.c:46-64` | 自动执行（timer） | 不判墙，直接赋坐标 |
| `maze.c:304-336` | BFS 扩展 | 4 个方向各抄一遍，判 `visited` |

### 5.2 任务

- [ ] 在 `maze_model` 定义**唯一**的移动原语：
  ```c
  /* 尝试把玩家向 (dx, dy) 移动一格。
   * 返回实际发生的结果，供 UI 决定要不要播放音效/动画。 */
  typedef enum {
      MOVE_OK,          /* 正常移动 */
      MOVE_BLOCKED,     /* 撞墙 */
      MOVE_UNLOCKED,    /* 用钥匙开门并移动 */
      MOVE_NO_KEY,      /* 遇到门但没钥匙 */
      MOVE_GOT_KEY      /* 吃到钥匙并移动 */
  } MoveResult;

  MoveResult maze_try_move(int dx, int dy);
  ```
- [ ] 用 `maze_try_move` 重写键盘处理，4 份复制粘贴变成：
  ```c
  if (key == 'W' || key == VK_UP)    result = maze_try_move(0, +1);
  if (key == 'S' || key == VK_DOWN)  result = maze_try_move(0, -1);
  if (key == 'A' || key == VK_LEFT)  result = maze_try_move(-1, 0);
  if (key == 'D' || key == VK_RIGHT) result = maze_try_move(+1, 0);
  ```
- [ ] 让 BFS 复用**同一个**通行判定（`cell_passable`），但**不要**复用 `maze_try_move`——BFS 是只读的，不能真的移动玩家。这是关键区别，请在头文件里注释清楚
- [ ] 处理自动执行（`maze.c:46-64`）

### 5.3 必须显式决策的两个语义分歧

这两点当前行为不一致，阶段 5 会强迫你面对。**先决策，再改码。**

**分歧一：移动失败的表现**

- 键盘路径：撞墙时**静默不动**（`maze.c:172` 的 `if` 直接不进入）
- BFS 路径：跳过该邻居（`maze.c:314` 的条件不满足）

- [ ] 决策：`MOVE_BLOCKED` 时玩家是否应该有反馈（音效/抖动）？
- [ ] 记录决策：

**分歧二：`reverse(path)` 被调用了两次**

- `hasAns()` 内部：`maze.c:309` 调 `copy(path, p->next)`，而 `p->next` 所在的链表此刻是**从玩家出发的 BFS 顺序**
- `TimerEventProcess`：`maze.c:49` 又调 `path = reverse(path)`
- `initStart()`：`maze.c:943, 956` 直接遍历 `path`，**没有** reverse

所以"自动执行"和"程序求解"对 `path` 的方向假设**很可能不一致**，且 `hasAns()` 每次调用都可能改变 `path` 的方向。

- [ ] **写一个实验**：固定一张地图，连续调用 `hasAns()` 两次，打印 `path` 的坐标序列，看是否相同
- [ ] 把实验结果记入第 8 节 B-4
- [ ] 决策：`path` 统一约定为"从玩家到目标"还是"从目标到玩家"
- [ ] 在 `pathfind.h` 里把该约定写成注释，并让所有消费方遵守

### 5.4 验收

- [ ] `grep -c "playerX += \|playerY += \|playerX -= \|playerY -= " src/` 结果 ≤ 1（只在 `maze_try_move` 内）
- [ ] 键盘移动行为与重构前逐帧一致（用同一张地图走同一条路线，对比截图）
- [ ] 基线一致

**验证记录**：
```
（在此填写）
```

---

## 7. 阶段 6：修 bug

终于可以改行为了。**每修一条单独提交**，提交信息写第 8 节的编号（如 `fix(B-2): 统一坐标约定`）。

建议顺序：先内存，再边界，最后小问题。理由：内存问题会让后面的验证结果不可信。

### 6.1 内存（最高优先级）

**已核实**：全项目**唯一**的 `free()` 在 `file.c:74`，释放的是临时串 `file`。其余全部泄漏。

- [ ] 引入明确的所有权约定，在 `maze_io.h` 注释里写清"谁分配、谁释放"
- [ ] 给 `Path` 加 `path_free(Path *)`，让每个 `malloc`（`queue.c:5`、`maze.c:345`、`maze.c:847-848`）都有对应释放点
- [ ] 修 `hasAns()` 的泄漏：它每次调用都 `malloc` 一个 `currPath`（`maze.c:345`）从不释放，而 `TimerEventProcess` 通过 `maze.c:48` 每秒调它多次
- [ ] 修 `queue.c:15-21` 的 `pop()`：只改指针不 `free`，节点泄漏
- [ ] 修 `queue.c:23-35` 的 `copy()`：清空 `a` 时也走 `pop()`，同样泄漏
- [ ] `main.c:31` 的 `path` 初始化只设了 `next`，**没设 `prev`**（`path->prev` 未初始化）
- [ ] 用 Dr. Memory 或 `-fsanitize=address`（MinGW 下可能需要换 MSVC 或 WSL）跑一轮，把报告存进 `docs/`
- [ ] 确认修复后基线输出**仍然一致**（内存修复不应改变可观测行为）

### 6.2 边界安全

- [ ] 在 `maze_model` 里给地图**外围加一圈哨兵墙**。现在靠 `maze.c:393-398` 那圈硬编码赋值，加哨兵后所有 `x±1` / `y±1` 访问天然安全
  - 注意：`MAZESIZE` 是 14 且地图本身已含边框（`MazeList/求求你了呜呜` 第 1、14 行全 1），所以哨兵可以是"访问前先判定坐标是否在 `[0, MAZESIZE)` 内"的**内联检查函数**，不一定要真扩数组
- [ ] 给 `Cell` 加校验函数 `bool cell_valid(Cell c);`
- [ ] 在 `maze_at()` / `maze_set()` 内部做校验，越界时返回 `CELL_WALL` 并断言
- [ ] 修 `file.c:17` `fscanf(name, "%s", ss)` 写入 `char ss[20]` 的栈溢出风险：改成 `"%20s"`，并检查 `fscanf` 返回值
- [ ] `file.c:61` `fopen` 返回值**未检查**，文件不存在时 `fscanf(NULL, ...)` 直接崩
- [ ] `file.c:57` `malloc(32 * sizeof(char))` 是硬编码长度，`strcpy`+`strcat` 拼接目录名可能溢出。改成按需计算长度
- [ ] 检查所有 `fscanf` / `fprintf` 的返回值（`file.c:17, 63-68`、`maze.c:734, 745-752`）

### 6.3 悬垂指针与生命周期

- [ ] `file.c:76` `return p->name;` 返回链表节点内部缓冲区，调用方 `maze.c:572` `strcpy(name, readMaze(selection))` 依赖它还没被释放。改成让调用方提供缓冲区（`void read_maze(int n, char *name_out, size_t cap)`），或明确返回新分配的串并注明所有权
- [ ] `maze.c:30` 的 `static char name[21]` 与 `FileNode.name[21]`（`file.h:10`）长度必须一致，且 `maze.c:689` 传给 `textbox` 的容量参数是 `21`——统一成一个 `MAZE_NAME_MAX` 宏
- [ ] `FileName.txt` 与实际 `MazeList/` 目录可能不同步（外部删除文件后列表仍显示）。加一次启动时的存在性校验，或明确记录为已知限制

### 6.4 已知的确定性 bug

- [ ] **`maze.c:919` 颜色名拼错**：`SetPenColor("Dark gray")`，而库里定义的是 `"Dark Gray"`（`libgraphics/graphics.c:1713`）。字符串比较大小写敏感，这行**当前完全没生效**，门显示的是上一次的颜色
- [ ] **`maze.c:406` 循环条件逻辑错**：`while(maze[playerX][playerY] == 1 || maze[targetX][targetY] == 1)` 内部**同时**随机了玩家和终点，但循环条件只检查"两者都是墙"的情况；应该是"任一为墙就重试"
- [ ] **`readMaze` 越界**：`file.c:53-55` 用 `for(i=0;i<n;i++) p=p->next;` 取第 n 个节点，若 `n > 链表长度` 会读到 `NULL` 并解引用。`maze.c:570` 传入的 `selection` 来自菜单，理论上受 `fileCount` 约束，但缺少防御
- [ ] **`bfs()` 递归**（`maze.c:338`）：深度随最短步数增长。当前靠 `visited` 兜住不会无限递归，但仍是 O(步数) 的栈深度。改成迭代（BFS 本来就不需要递归）
- [ ] `maze.h:39` 自造的 `IntToString` 与 `strlib.h` 的 `IntegerToString` 功能重复；`IntToString` 还占用了易混的 `string` 名字。二选一
- [ ] **`main.c:39-41` 的初始化顺序**：`readFileName()` 后紧跟 `initMazePoint()`，后者无条件 `initMaze()` 重新随机。若原意是"启动时随机一局"，加注释说明；若是 bug，删掉

### 6.5 阶段 6 验收

- [ ] 每条修复都有独立提交，提交信息含编号
- [ ] 内存检测报告挂在 `docs/`
- [ ] **预期行为变化**已更新到 `docs/baseline/`（基线允许变，但要记录为什么变）
- [ ] 程序手动跑一遍全功能无崩溃

**验证记录**：
```
（在此填写）
```

---

## 8. 阶段 7：隔离图形库

### 7.1 任务

- [ ] 把 `libgraphics/` + `include/` 移到 `third_party/libgraphics/`（`git mv` 保留历史）
- [ ] `src/ui/render.c` 成为**唯一** `#include "extgraph.h"` 的文件
- [ ] 定义自己的渲染接口，不泄漏库类型：
  ```c
  /* render.h —— 不 include 任何 libgraphics 头文件 */
  void render_init(const char *title, double w, double h);
  void render_clear(void);
  void render_cell(Cell c, CellType t);
  void render_player(Cell c);
  void render_target(Cell c);
  void render_text(double x, double y, const char *s);
  ...
  ```
- [ ] 验证：`grep -rn "extgraph\|graphics\.h" src/ | grep -v "src/ui/render.c"` 无结果
- [ ] 结论：将来换 SDL / raylib 只需重写 `render.c` 一个文件

### 7.2 注意事项

- [ ] `libgraphics` 的头文件是 **GBK 编码**。`git mv` 不会改编码，但请勿在编辑器中以 UTF-8 保存它们，否则中文注释会乱码（`include/imgui.h` 全是中文注释）
- [ ] 这是一个**高收益低风险**的阶段，可以随时插入，不必等前六个阶段全做完

---

## 9. 阶段 8：工程化

- [ ] `CMakeLists.txt` 拆成多 target：
  ```cmake
  add_library(maze_core STATIC src/core/maze_model.c src/core/pathfind.c src/core/maze_io.c)
  add_library(graphics  STATIC third_party/libgraphics/*.c)
  add_executable(MazeGame src/main.c src/ui/render.c src/ui/input.c src/ui/menu.c src/ui/app.c)
  target_link_libraries(MazeGame maze_core graphics winmm)
  ```
  **关键收益**：`maze_core` 不链接 `graphics`，编译期就保证了阶段 2 的边界不被破坏
- [ ] 打开警告：`target_compile_options(maze_core PRIVATE -Wall -Wextra)`
  - 预期会爆出一批问题，逐个处理或显式 `// NOLINT` 并注明理由
- [ ] 加测试 target：
  ```cmake
  add_executable(probe tests/probe.c)
  target_link_libraries(probe maze_core)
  add_test(NAME logic COMMAND probe)
  ```
- [ ] `enable_testing()` + `ctest`，让阶段 1 的基线**自动比对**
- [ ] 把 `tests/probe.c` 的输出改成与 `docs/baseline/logic_baseline.txt` 对比，不一致则退出码非 0
- [ ] 更新 `README.md`：新的目录结构、新的构建命令、`Main()` 入口约定的说明（`README.md:21` 已正确记录了这个约定，保留）
- [ ] 考虑：源码编码统一。当前业务代码是 **GBK**（`main.c`/`maze.c`/`file.c`/`queue.c` 含中文注释和字符串），推荐转成 UTF-8 并在 CMake 里加 `-finput-charset=UTF-8 -fexec-charset=UTF-8`
  - **风险**：`GraphicsProject` 用的 `DrawTextString` 走 Win32 GDI，字符串字面量按执行字符集编码传给 `CreateFont`/`TextOut`。改成 UTF-8 后**中文界面会变乱码**，需要同步改用 `-fexec-charset=GBK` 或改 `render.c` 里的宽字符转换。这一项建议**单独做、单独验证**

---

## 10. 已知问题清单（阶段 6 的输入，也是重构期间的"偏差记录"）

| 编号 | 位置 | 问题 | 严重度 | 处理阶段 |
|---|---|---|---|---|
| A-1 | 全项目 | 除 `file.c:74` 外**无任何 `free()`** | 高 | 6.1 |
| A-2 | `queue.c:15-21` | `pop()` 不释放节点 | 高 | 6.1 |
| A-3 | `queue.c:23-35` | `copy()` 清空时不释放 | 高 | 6.1 |
| A-4 | `maze.c:345` | `hasAns()` 每次调用泄漏一个 `currPath`，被 timer 高频调用 | 高 | 6.1 |
| A-5 | `main.c:31-32` | `path->prev` 未初始化 | 中 | 6.1 |
| B-1 | `maze.c:172-237` 等 | 全项目命名与下标语义相反：`playerX` 实为行号、`playerY` 实为列号。当前自洽但极易误导 | 中 | 2.5 |
| B-2 | `file.c:61` | `fopen` 返回值未检查 | 高 | 6.2 |
| B-3 | `file.c:17` | `fscanf("%s")` 写入 `char[20]` 无宽度限制 | 高 | 6.2 |
| B-4 | `maze.c:49` vs `:309` vs `:943` | `reverse(path)` 调用点不一致，方向语义不明 | 高 | 5.3 |
| B-5 | `maze.c:406` | `while` 条件应为"任一为墙" | 中 | 6.4 |
| B-6 | `maze.c:338` | `bfs` 递归，栈深随步数增长 | 中 | 6.4 |
| B-7 | `file.c:53-55` | 链表索引无上界检查 | 中 | 6.4 |
| C-1 | `maze.c:919` | `"Dark gray"` 应为 `"Dark Gray"`，当前配色未生效 | 低 | 6.4 |
| C-2 | `file.c:76` | 返回链表内部缓冲区，生命周期靠运气 | 中 | 6.3 |
| C-3 | `maze.h:39` | `IntToString` 与 `strlib.h` 的 `IntegerToString` 重复 | 低 | 6.4 |
| C-4 | `main.c:39-41` | `initMazePoint()` 无条件覆盖已加载地图 | 中 | 6.4 |
| C-5 | `maze.c:30` / `file.h:10` | `name[21]` 长度在 3 处硬编码 | 低 | 6.3 |

**偏差记录**（重构时发现本文件没写到的问题，记在这里）：

| 日期 | 编号 | 位置 | 现象 | 决定 |
|---|---|---|---|---|
| | | | | |

**勘误记录**（本文档自身被证伪的结论，保留痕迹以免重复踩坑）：

| 原结论 | 实际情况 | 修正时间 |
|---|---|---|
| §2.5 初稿称"`maze` 是 `[x][y]`，与 `visited[x][y]` 约定混用，属高severity bug" | **错误**。经数据文件与碰撞逻辑交叉验证，`maze` 是 `[row][col]`，且全项目五处（加载/存档/移动/BFS/绘制）**约定一致、行为自洽**。真正的问题是**变量命名与语义相反**，属可维护性隐患而非功能缺陷 | 初稿复核时 |

---

## 11. 进度追踪

- [ ] 阶段 1：建安全网
- [ ] 阶段 2：拆依赖
- [ ] 阶段 3：消灭魔法数
- [ ] 阶段 4：状态机化
- [ ] 阶段 5：收敛重复逻辑
- [ ] 阶段 6：修 bug
- [ ] 阶段 7：隔离图形库
- [ ] 阶段 8：工程化

---

## 12. 不要做的事

1. **不要一次性重写。** 自研代码约 1300 行（`maze.c` 1111 + `main.c` 53 + `queue.c` 53 + `file.c` 77），规模不大但状态耦合极深。全量重写会丢掉阶段 1 的基线，等于用"看起来对了"替换"确定对了"。
2. **不要动 `libgraphics/` 和 `include/`。** 它们是黑盒，问题已在 `README.md` 记录。
3. **不要在阶段 1–5 改任何可观测行为。** 包括那个 `"Dark gray"` 拼写——它虽然是 bug，但改了会让基线变化，必须等到阶段 6。
4. **不要把 `libgraphics` 改成 C++ 或 `.cpp`。** 该库对 C++ 的适配性已单独评估过，结论是"可以但没必要"，本次重构选定 C 路线。
5. **不要为了"更优雅"而换数据结构。** `queue` 那个双向链表虽然过度设计（BFS 只需要 FIFO），但阶段 2 只改名不改实现，实现层面的简化留到阶段 6 之后单独评估。
