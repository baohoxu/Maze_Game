#include "menu.h"
#include "maze_model.h"
#include "pathfind.h"

void drawChooseMaze() {

    SetPointSize(15);

    double h = GetFontHeight() * 1.5;
    double w = TextStringWidth("编辑") * 2;
    double wlist = TextStringWidth("MaZheDaShuaiGe")*1.2;

    int n = fileCount(fileName);
    string* fileNameList = (string*) malloc((n + 1) * sizeof(string));
    fileNameList[0] = "地图列表";
    FileNode* p = fileName;

    for(int i = 1; i <= n; i++) {

        p = p->next;
        fileNameList[i] = p->name;

    }

    int selection;
    selection = menuList(GenUIID(0), w-0.05, winHeight-2*h, wlist, wlist, h, fileNameList, n+1);
    if(selection > 0) {
        strcpy(name, readMaze(selection, fileName, &playerX, &playerY, &targetX, &targetY, maze));
        isHelping = 0;
        isAbouting = 0;
        isOpening = 0;
        isEditing = 0;
        isAnswering = 0;
        isNexting = 0;
        isShowing = 0;
        win = 0;
        leftKey = 0;
        hasBegin = 0;
        health = 100;
    }

}

//绘制元素选择界面
void drawChooseElement() {

    SetPointSize(15);

    double h = GetFontHeight() * 1.5;
    double w = TextStringWidth("编辑") * 2;
    double wlist = TextStringWidth("MaZheDaShuaiGe")*1.2;

    static string elementList[] = {
            "元素列表",
            "道路",
            "障碍物",
            "起点",
            "终点",
            "门",
            "钥匙"
    };

    int selection;

    selection = menuList(GenUIID(0), w-0.05, winHeight-2*h, wlist, wlist, h, elementList, sizeof(elementList)/sizeof(elementList[0]));
    if(selection > 0) {

        isChoosingElement = 0;
        element = selection - 1;

    }

}

//显示当前元素
void drawCurrElement() {

    SetPointSize(15);

    SetPenColor("Blue");

    double h = GetFontHeight() * 1.5;
    double w = TextStringWidth("编辑") * 2;
    double wlist = TextStringWidth("MaZheDaShuaiGe")*1.2;

    static string elementList[] = {
            "道路",
            "障碍物",
            "起点",
            "终点",
            "门",
            "钥匙"
    };

    MovePen(w+wlist+0.05, winHeight-2*h);
    DrawTextString("编辑中,当前元素为:");
    DrawTextString(elementList[element]);

}

//绘制菜单栏
void drawMenu() {

    static string menuListFile[] = {
            "文件",
            "新建地图 | Ctrl-N",
            "打开地图 | Ctrl-O",
            "保存地图 | Ctrl-S",
            "退出 | Ctrl-T"
    };

    static string menuListEdit[] = {
            "编辑",
            "随机生成 | Ctrl-L",
            "手动编辑 | Ctrl-P",
            "元素选择"
    };

    static string menuListAnswer[] = {
            "求解",
            "手动求解 | Ctrl-Q",
            "提示一步 | Ctrl-W",
            "程序求解 | Ctrl-E",
            "自动执行",
            "求解过程"
    };

    static string menuListHelp[] = {
            "帮助",
            "使用说明",
            "关于"
    };

    SetPointSize(15);

    double h = GetFontHeight() * 1.5;
    double w = TextStringWidth(menuListEdit[0]) * 2;
    double wlist = TextStringWidth(menuListFile[3])*1.2;
    int selection;

    if(button(GenUIID(0), w*6+0.4, winHeight-2*h, w, h, isRenaming ? "确认" : "重命名")) {
        isRenaming = !isRenaming;
    }

    if(isRenaming) textbox(GenUIID(0), w*7+0.5, winHeight-2*h, 20*TextStringWidth("W"), h, name, 21);
    else drawLabel(w*7+0.5, winHeight-2*h+0.06, name);

    drawMenuBar(0, winHeight - h, winWidth, h);

    //FILE
    selection = menuList(GenUIID(0), 0, winHeight-h, w, wlist, h, menuListFile, sizeof(menuListFile)/sizeof(menuListFile[0]));
    if(selection == 1) {

        //新建地图
        isHelping = 0;
        isAbouting = 0;
        isEditing = 0;
        isAnswering = 0;
        isNexting = 0;
        isShowing = 0;
        win = 0;
        hasBegin = 0;
        health = 100;
        playerX = 1;
        playerY = MAZESIZE - 2;
        targetX = MAZESIZE - 2;
        targetY = 1;

        memset(maze, 0, sizeof(maze));
        for(int i = 0; i < MAZESIZE; i++) {
            maze[i][0] = 1;
            maze[0][i] = 1;
            maze[MAZESIZE-1][i] = 1;
            maze[i][MAZESIZE-1] = 1;
        }

    }else if(selection == 2) {

        //打开地图
        isOpening = 1;
        isChoosingElement = 0;

    }else if(selection == 3) {

        //保存地图
        if(!win) {
            FILE *fp;
            if (filePush(fileName, name)) {

                fp = fopen("FileName.txt", "a+");
                fprintf(fp, "%s\n", name);
                fclose(fp);

            }

            string file = (string) malloc(32 * sizeof(char));
            strcpy(file, "./MazeList/");
            strcat(file, name);

            fp = fopen(file, "w");
            fprintf(fp, "%d %d\n", playerX, playerY);
            fprintf(fp, "%d %d\n", targetX, targetY);

            for (int i = 0; i < MAZESIZE; i++) {
                for (int j = 0; j < MAZESIZE; j++) {
                    fprintf(fp, "%d ", maze[i][j]);
                }
                fprintf(fp, "\n");
            }

            fclose(fp);

        }


    }else if(selection == 4) {

        //退出
        exit(-1);

    }

    //Edit
    selection = menuList(GenUIID(0), w, winHeight-h, w, wlist, h, menuListEdit, sizeof(menuListEdit)/sizeof(menuListEdit[0]));
    if(selection == 1) {

        //随机生成
        isHelping = 0;
        isAbouting = 0;
        isEditing = 0;
        isAnswering = 0;
        isNexting = 0;
        isShowing = 0;
        win = 0;
        hasDoor = 0;
        hasKey = 0;
        hasBegin = 0;
        health = 100;

        initMaze();
        while(hasAns() <= 30) initMaze();

    }else if(selection == 2) {

        //手动编辑
        isEditing = !isEditing;
        hasBegin = 0;
        health = 100;

    }else if(selection == 3) {

        //元素选择
        isChoosingElement = !isChoosingElement;
        isOpening = 0;

    }

    //Answer
    selection = menuList(GenUIID(0), 2*w, winHeight-h, w, wlist, h, menuListAnswer, sizeof(menuListAnswer)/sizeof(menuListAnswer[0]));
    if(selection == 1) {

        //手动求解
        isAnswering = 0;
        isNexting = 0;
        isPlaying = 0;
        isShowing = 0;

    }else if(selection == 2) {

        //提示一步
        isNexting = 1;
        isAnswering = 0;

    }else if(selection == 3) {

        //程序求解
        if(!isEditing) {
            isAnswering = 1;
            isNexting = 0;
        }

    }else if(selection == 4) {

        //自动执行
        isShowing = 0;
        isPlaying = !isPlaying;
        isAnswering = isPlaying;
        hasBegin = 0;

    }else if(selection == 5) {

        //求解过程
        isShowing = !isShowing;
        isAnswering = 0;
        isPlaying = 0;
        fakeLeftKey = leftKey;
        for(int i = 0; i < MAZESIZE; i++) {
            for(int j = 0; j < MAZESIZE; j++) {
                fakeMaze[i][j] = maze[i][j];
            }
        }

        show = (Path*) malloc(sizeof(Path));
        show->next = (Path*) malloc(sizeof(Path));
        show->next->x = playerX;
        show->next->y = playerY;
        show->next->next = NULL;

        memset(showVisited, 0, sizeof(showVisited));

        showVisited[playerX][playerY] = 1;

    }

    //Help
    selection = menuList(GenUIID(0), 3*w, winHeight-h, w, wlist, h, menuListHelp, sizeof(menuListHelp)/sizeof(menuListHelp[0]));
    if(selection == 1) {

        //使用说明
        isAbouting = 0;
        isHelping = 1;
        hasBegin = 0;

    }else if(selection == 2) {

        //关于
        isHelping = 0;
        isAbouting = 1;
        hasBegin = 0;

    }

    SetPointSize(15);

}
