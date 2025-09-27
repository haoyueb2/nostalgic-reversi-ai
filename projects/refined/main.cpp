#include<stdio.h>
#include<time.h>
#define INF 2000000000
#define N 16
#define DEPTH 11
#define DEBUG
#define DEEPDEPTH
//棋子坐标
typedef struct Spos {
	int row;
	int col;
}Spos;
//用于定义八个方向
typedef struct Sdirect {
	int x;
	int y;
}Sdirect;

int g_board[N][N];
int g_valueTable[N][N];

int g_mine,g_oppo;
//当前双方已走的总棋子步数，评估函数主函数都要用避免传参所以设为全局
int g_moveNum;
//撤回函数需要记录使某一些棋翻转的是哪一步棋，搜索时不同深度用不同的二维数组以免互相干扰
Spos g_turnReason[DEPTH][N][N];

//落子并翻转该翻的棋
void Move(Spos pos,int side,int depth);
//加深搜索时需要的撤回先前下的一步的撤回函数
void UndoMove(Spos pos,int side,int depth);
//判断空位某一方是否可下，计算行动力需要
int IfValid(Spos pos,int side);
//初始化棋格表
void CreateTable();
//估值函数
int Evaluate(int side);
//alpha_bata深层搜索
int AbSearch(int alpha,int beta,int side,int depth);
//在所以可走的步中找深层搜索后估值最高的
Spos FindBestMove(int side,int depth);
//判断占角后，从角延伸的成片同色稳定子的个数
int Stabilizer(int side);
//角被占时改变星位和c位的棋格表值
void ChangeValue(int side);
//撤回函数要保存翻转的每个棋对应的先前落子（称之为turnreason)以便撤回，这是对其初始化
void IniTurnReason();
//判断是否为伪稳定子（判定八个方向延伸直到碰到边或对方棋子的棋为伪稳定子）
int IsFalseStable(Spos pos,int side);
//棋格表对应位置是自己其则给1对方棋则给-1
int ValueWeight(Spos pos,int side);
//左上角判断内部稳定子
int TopLeftStable(int insideNum,int side,int Stablizer[][N]);
//左下角判断内部稳定子
int BottomLeftStable(int insideNum,int side,int Stablizer[][N]);
//右上角判断内部稳定子
int TopRightStable(int insideNum,int side,int Stablizer[][N]);
//右下角判断内部稳定子
int BottomRightStable(int insideNum,int side,int Stablizer[][N]);

int main() {
	char command[6];
	Spos pos;
	int depth;
#ifdef DEBUG
	freopen("sample_input_default.txt","r",stdin);
#endif
	CreateTable();
	scanf("%s %d",command,&g_mine);
	printf("OK\n");
	fflush(stdout);
	IniTurnReason();
	g_oppo = (g_mine == 1 ? 2 : 1);
	g_board[N/2 - 1][N/2 - 1] = 2;
	g_board[N/2][N/2] = 2;
	g_board[N/2 - 1][N/2] = 1;
	g_board[N/2][N/2 - 1] = 1;
	g_moveNum = 0;
	depth = 2;
	while(scanf("%s",command) == 1) {
		if('P' == command[0]) {
			scanf("%d %d",&pos.row,&pos.col);
			Move(pos,g_oppo,depth);
			g_moveNum++;
		} else if('T' == command[0]) {
			depth = 2;
			if(g_moveNum > 200 || g_moveNum < 60) {
				depth = 4;
			}
			if(g_moveNum >230) {
				depth = 6;
			}
			if(g_moveNum >= 242) {
				depth = 10;
			}
			pos = FindBestMove(g_mine,depth);
			Move(pos,g_mine,depth);
			g_moveNum++;
#ifdef DEBUG
			printf("%d %d %d\n",g_moveNum,pos.row,pos.col);
#else
			printf("%d %d\n",pos.row,pos.col);
#endif
			fflush(stdout);
		}
	}
	return 0;
}

int Evaluate(int side) {
	Spos pos;
	int value,another,SideMobility,AnotherMobility,sideNum,anotherNum,sideFalseStable,anotherFalseStable;
	another = (side == 1 ? 2 : 1);
	value = 0;
	ChangeValue(side);
	SideMobility = 0;
	AnotherMobility = 0;
	sideNum = 0;
	anotherNum = 0;
	sideFalseStable = 0;
	anotherFalseStable = 0;
    //242步之后只是基于双方棋子个数相减的估值，以节约时间从而加深搜索
	if(g_moveNum >= 242){
		for(pos.row = 0; pos.row < N; pos.row++) {
			for(pos.col = 0; pos.col < N; pos.col++) {
				if(g_board[pos.row][pos.col] == side) sideNum++;
				else if(g_board[pos.row][pos.col] == another) anotherNum++;
			}
		}
		value = sideNum - anotherNum;
		return value;
	}
	for(pos.row = 0; pos.row < N; pos.row++) {
		for(pos.col = 0; pos.col < N; pos.col++) {
			value += g_valueTable[pos.row][pos.col] * ValueWeight(pos,side);
			if(g_board[pos.row][pos.col] == side) {
				sideNum++;
				if(IsFalseStable(pos,side)) {
					sideFalseStable++;
				}
			} else if(g_board[pos.row][pos.col] == another){
				anotherNum++;
				if(IsFalseStable(pos,another)) {
					anotherFalseStable++;
				}
			} else {
				if(IfValid(pos,side)) SideMobility++;
				if(IfValid(pos,another)) AnotherMobility++;
			}
		}
	}
	value += 20 * (Stabilizer(side) - Stabilizer(another));
    value += 2 * (sideFalseStable - anotherFalseStable);
	if(g_moveNum >= 30 && g_moveNum <= 200) {
		value += 4 * (SideMobility - AnotherMobility);
	} else if(g_moveNum > 200) {
		value += (int)(5 * (SideMobility - AnotherMobility));
	} else if(g_moveNum < 30) {
		value += 1 * (SideMobility - AnotherMobility);
	}
	return value;
}

Spos FindBestMove(int side,int depth) {
	int i,k,value,maxValue,another;
	Spos canMove[16 * 16];
	Spos pos,bestMove;
	clock_t start,finish,duration;
	bestMove.row = 0;
	bestMove.col = 0;
	k = 0;
	start = clock();
	another = (side == 1 ? 2 : 1);
	for(pos.row = 0; pos.row < N; pos.row++) {
		for(pos.col = 0; pos.col < N; pos.col++) {
			if(IfValid(pos,side)) {
				canMove[k++] = pos;
			}
		}
	}
	maxValue = -INF;	
	for(i = 0; i < k; i++) {
		finish = clock();
		duration = finish - start;
		if(duration > 4000) break;//计时函数，找最佳步数时超过4s就停止寻找，函数返回当前找到的最好的
		Move(canMove[i],side,depth);
		value = -AbSearch(-INF,INF,another,depth-1);
		UndoMove(canMove[i],side,depth);
		if(value > maxValue) {
			maxValue = value;
			bestMove = canMove[i];
		}
	}
	return bestMove;
}

int AbSearch(int alpha,int beta,int side,int depth) { 
	Spos canMove[16 * 16],pos;
	int k,i,value,maxValue;
	int sign[3],another;
	k = 0;
	sign[g_mine] = 1;
	sign[g_oppo] = -1;
	another = (side == 1 ? 2 : 1);
	if(depth <= 0) {
		return sign[side] * Evaluate(g_mine);
	}
	for(pos.row = 0; pos.row < N; pos.row++) {
		for(pos.col = 0; pos.col < N; pos.col++) {
			if(IfValid(pos,side)) {
				canMove[k++] = pos;
			}
		}
	}
	if(k == 0) return -500-AbSearch(-beta,-alpha,another,depth - 1);
	maxValue = -INF;
	for(i = 0; i < k; i++) {
		Move(canMove[i],side,depth);
		value = -AbSearch(-beta,-alpha,another,depth - 1);
		UndoMove(canMove[i],side,depth);
		if(value > alpha) {
			if(value >= beta) {
				return value;
			}
			alpha = value;
		}
		if(value > maxValue) {
			maxValue = value;
		}
	}
	return maxValue;
}

void IniTurnReason() {
	int i,j,k;
	for(k = 0; k < DEPTH; k++) {
		for(i = 0; i < N; i++) {
			for(j = 0; j < N; j++) {
				g_turnReason[k][i][j].row = -1;
				g_turnReason[k][i][j].col = -1;
			}
		}
	}
}

void Move(Spos pos,int side,int depth) {
	Sdirect direct[8] = {{1,1},{1,0},{1,-1},{0,1},{0,-1},{-1,1},{-1,0},{-1,-1}};
	int i,nRow,nCol,another;
	another = (side == 1 ? 2 : 1);
	g_board[pos.row][pos.col] = side;
	for(i = 0; i < 8; i++) {
		nRow = pos.row + direct[i].x ;
		nCol = pos.col + direct[i].y;
		while(nRow >= 0 && nRow <= N-1 && nCol >= 0 && nCol <= N-1 && g_board[nRow][nCol] == another) {
			nRow += direct[i].x;
			nCol += direct[i].y;
		}
		if(nRow >= 0 && nRow <= N-1 && nCol >= 0 && nCol <= N-1 && g_board[nRow][nCol] == side) {	
			nRow -= direct[i].x;
			nCol -= direct[i].y;
			while(nRow != pos.row || nCol != pos.col) {
				g_board[nRow][nCol] = side;
				g_turnReason[depth-1][nRow][nCol] = pos;
				nRow -= direct[i].x;
				nCol -= direct[i].y;
			}
		}
	}
}

void UndoMove(Spos pos,int side,int depth) {
	Sdirect direct[8] = {{1,1},{1,0},{1,-1},{0,1},{0,-1},{-1,1},{-1,0},{-1,-1}};
	int i,nRow,nCol,another;
	another = (side == 1 ? 2 : 1);
	for(i = 0; i < 8; i++) {
		nRow = pos.row + direct[i].x ;
		nCol = pos.col + direct[i].y;

		while(nRow >= 0 && nRow <= N-1 && nCol >= 0 && nCol <= N-1 &&
			g_turnReason[depth-1][nRow][nCol].row == pos.row && g_turnReason[depth-1][nRow][nCol].col == pos.col) {
				g_board[nRow][nCol] = another;
				g_turnReason[depth-1][nRow][nCol].row = -1;
				g_turnReason[depth-1][nRow][nCol].col = -1;
				nRow += direct[i].x;
				nCol += direct[i].y;
		}
	}
	g_board[pos.row][pos.col] = 0;
	g_turnReason[depth-1][pos.row][pos.col].row = -1;
	g_turnReason[depth-1][pos.row][pos.col].col = -1;
}

int IfValid(Spos pos,int side) {
	Sdirect direct[8] = {{1,1},{1,0},{1,-1},{0,1},{0,-1},{-1,1},{-1,0},{-1,-1}};
	int i,nRow,nCol,another,flag;
	if(g_board[pos.row][pos.col] != 0) {
		return 0;
	}
	another = (side == 1 ? 2 : 1);
	for(i = 0; i < 8; i++) {
		nRow = pos.row + direct[i].x;
		nCol = pos.col + direct[i].y;
		flag = 0;
		while(nRow >= 0 && nRow <= N-1 && nCol >= 0 && nCol <= N-1 &&
			g_board[nRow][nCol] == another ){
				nRow += direct[i].x;
				nCol += direct[i].y;
				flag = 1;
		}
		//flag是为了防止并没有进入循环即没有找到对方旗子，但空格旁边直接就是自己棋子的情况
		if(flag && nRow >= 0 && nRow <= N-1 && nCol >= 0 && nCol <= N-1 &&
			g_board[nRow][nCol] == side) {
				return 1;
		}
	}
	return 0;		
}

void CreateTable() {
	Spos pos;
	for(pos.row = 2; pos.row <= N-3; pos.row++) {
		for(pos.col = 2; pos.col <= N-3; pos.col++) {
			g_valueTable[pos.row][pos.col] = 1;
		}
	}
	g_valueTable[0][0] = 500;
	g_valueTable[0][N-1] = 500;
	g_valueTable[N-1][0] = 500;
	g_valueTable[N-1][N-1] = 500;
	g_valueTable[0][1] = -200;
	g_valueTable[1][0] = -200;
	g_valueTable[0][N-2] = -200;
	g_valueTable[1][N-1] = -200;
	g_valueTable[N-2][0] = -200;
	g_valueTable[N-1][1] = -200;
	g_valueTable[N-1][N-2] = -200;
	g_valueTable[N-2][N-1] = -200;
	g_valueTable[1][1] = -500;
	g_valueTable[1][N-2] = -500;
	g_valueTable[N-2][1] = -500;
	g_valueTable[N-2][N-2] = -500;
	//靠近边的一行赋较低的值，因为占了它对方就可能占边
	for(pos.row = 2; pos.row <= N-3; pos.row++) {
		g_valueTable[pos.row][1] = -5;
	}
	for(pos.row = 2; pos.row <= N-3; pos.row++) {
		g_valueTable[pos.row][N-2] = -5;
	}
	for(pos.col = 2; pos.col <= N-3; pos.col++) {
		g_valueTable[1][pos.col] = -5;
	}
	for(pos.col = 2; pos.col <= N-3; pos.col++) {
		g_valueTable[N-2][pos.col] = -5;
	}
	for(pos.row = 2; pos.row <= N-3; pos.row++) {
		g_valueTable[pos.row][0] = 10;
	}
	for(pos.row = 2; pos.row <= N-3; pos.row++) {
		g_valueTable[pos.row][N-1] = 10;
	}
	for(pos.col = 2; pos.col <= N-3; pos.col++) {
		g_valueTable[0][pos.col] = 10;
	}
	for(pos.col = 2; pos.col <= N-3; pos.col++) {
		g_valueTable[N-1][pos.col] = 10;
	}
}

int ValueWeight(Spos pos,int side) {
	if(g_board[pos.row][pos.col] == side) {
		return 1;
	} else if(g_board[pos.row][pos.col] == 0) {
		return 0;
	} else {
		return -1;
	}
}

void ChangeValue(int side) {
	if(g_board[0][0] == side) {
		g_valueTable[1][1] = 1;
		g_valueTable[1][0] = 10;
		g_valueTable[0][1] = 10;
	}
	if(g_board[N-1][N-1] == side) {
		g_valueTable[N-2][N-2] = 1;
		g_valueTable[N-1][N-2] = 10;
		g_valueTable[N-2][N-1] = 10;
	}
	if(g_board[0][N-1] == side) {
		g_valueTable[1][N-2] = 1;
		g_valueTable[0][N-2] = 10;
		g_valueTable[1][N-1] = 10;
	}
	if(g_board[N-1][0] == side) {
		g_valueTable[N-2][1] = 1;
		g_valueTable[N-1][1] = 10;
		g_valueTable[N-2][0] = 10;
	}
}

int Stabilizer(int side) {
	Spos pos;
	int egdeNum,i,j,insideNum;
	//判断内部稳定子时需要基于周围已存在的稳定子，故开数组记录
	int Stablizer[N][N];
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			Stablizer[i][j] = 0;
		}
	}
	//从四个角计算只在边上的稳定子
	egdeNum = 0;
	pos.row = 0;
	pos.col = 0;
	while(pos.row >= 0 && pos.row <= N-1 && pos.col >= 0 && pos.col <= N-1 &&
		g_board[pos.row][pos.col] == side) {
			egdeNum++;
			Stablizer[pos.row][pos.col] = 1;
			pos.row++;
	}
	//这句同以后相似的四句是因为当同色子占满一列或一行个数会被重复计算，所以减去重复的
	if(pos.row == N) egdeNum -= 16;
	pos.row = 0;
	pos.col = 0;
	if(g_board[pos.row][pos.col] == side) egdeNum--;
	while(pos.row >= 0 && pos.row <= N-1 && pos.col >= 0 && pos.col <= N-1 &&
		g_board[pos.row][pos.col] == side) {
			egdeNum++;
			Stablizer[pos.row][pos.col] = 1;
			pos.col++;
	}
	if(pos.col == N) egdeNum -= 16;

	pos.row = N-1;
	pos.col = N-1;
	while(pos.row >= 0 && pos.row <= N-1 && pos.col >= 0 && pos.col <= N-1 &&
		g_board[pos.row][pos.col] == side) {
			egdeNum++;
			Stablizer[pos.row][pos.col] = 1;
			pos.row--;
	}
	if(pos.row == -1) egdeNum -= 16;
	pos.row = N-1;
	pos.col = N-1;
	if(g_board[pos.row][pos.col] == side) egdeNum--;
	while(pos.row >= 0 && pos.row <= N-1 && pos.col >= 0 && pos.col <= N-1 &&
		g_board[pos.row][pos.col] == side) {
			egdeNum++;
			Stablizer[pos.row][pos.col] = 1;
			pos.col--;
	}
	if(pos.col == -1) egdeNum -= 16;

	pos.row = 0;
	pos.col = N-1;
	while(pos.row >= 0 && pos.row <= N-1 && pos.col >= 0 && pos.col <= N-1 &&
		g_board[pos.row][pos.col] == side) {
			egdeNum++;
			Stablizer[pos.row][pos.col] = 1;
			pos.row++;
	}
	pos.row = 0;
	pos.col = N-1;
	if(g_board[pos.row][pos.col] == side) egdeNum--;
	while(pos.row >= 0 && pos.row <= N-1 && pos.col >= 0 && pos.col <= N-1 &&
		g_board[pos.row][pos.col] == side) {
			egdeNum++;
			Stablizer[pos.row][pos.col] = 1;
			pos.col--;
	}

	pos.row = N-1;
	pos.col = 0;
	while(pos.row >= 0 && pos.row <= N-1 && pos.col >= 0 && pos.col <= N-1 &&
		g_board[pos.row][pos.col] == side) {
			egdeNum++;
			Stablizer[pos.row][pos.col] = 1;
			pos.row--;
	}
	pos.row = N-1;
	pos.col = 0;
	if(g_board[pos.row][pos.col] == side) egdeNum--;
	while(pos.row >= 0 && pos.row <= N-1 && pos.col >= 0 && pos.col <= N-1 &&
		g_board[pos.row][pos.col] == side) {
			egdeNum++;
			Stablizer[pos.row][pos.col] = 1;
			pos.col++;
	}
	//从四个角分别计算从边上延伸进去的稳定子
	insideNum = 0;
	insideNum = TopLeftStable(insideNum,side,Stablizer);
    insideNum = BottomLeftStable(insideNum,side,Stablizer);
    insideNum = TopRightStable(insideNum,side,Stablizer);
    insideNum = BottomRightStable(insideNum,side,Stablizer);
	return (int)(egdeNum + 0.5*insideNum);
}

int TopLeftStable(int insideNum,int side,int Stablizer[][N]) {
	Spos pos;
	int i,j;
	if(g_board[0][0] == side) {
		for(i = 1; i < 8; i++) {
			pos.row = i;
			pos.col = i;
			//如果对角线上的棋不是稳定子的话，那这颗棋行延伸和列延伸下去的同色子也不是稳定子，所以直接退出
			if(g_board[pos.row][pos.col] != side || Stablizer[pos.row-1][pos.col] != 1 || Stablizer[pos.row][pos.col-1] != 1
				||(Stablizer[pos.row+1][pos.col-1] != 1 && Stablizer[pos.row-1][pos.col+1] != 1)) {
					break;
			}
			insideNum++;
			Stablizer[pos.row][pos.col] = 1;
			for(j = pos.row + 1; j <= N - 1 - i; j++) {
				if(g_board[j][pos.col] != side || Stablizer[j+1][pos.col-1] != 1) {
					break;
				}
				insideNum++;
				Stablizer[j][pos.col] = 1;
			}
			if(j == N - 1 - i) insideNum -= N - 2 * i;
			for(j = pos.col + 1; j <= N - 1 - i; j++) {
				if(g_board[pos.row][j] != side || Stablizer[pos.row-1][j+1] != 1) {
					break;
				}
				insideNum++;
				Stablizer[pos.row][j] = 1;
			}
			if(j == N - 1 - i) insideNum -= N - 2 * i;
		}
	}
	return insideNum;
}

int BottomLeftStable(int insideNum,int side,int Stablizer[][N]) {
	Spos pos;
	int i,j;
	if(g_board[N-1][0] == side) {
		for(i = 1; i < 8; i++) {
			pos.row = N - 1 - i;
			pos.col = i;
			if(g_board[pos.row][pos.col] != side || Stablizer[pos.row+1][pos.col] != 1 || Stablizer[pos.row][pos.col-1] != 1
				||(Stablizer[pos.row-1][pos.col-1] != 1 && Stablizer[pos.row+1][pos.col+1] != 1)) {
					break;
			}
			insideNum++;
			Stablizer[pos.row][pos.col] = 1;
			for(j = pos.row - 1; j >= i; j--) {
				if(g_board[j][pos.col] != side || Stablizer[j+1][pos.col-1] != 1) {
					break;
				}
				insideNum++;
				Stablizer[j][pos.col] = 1;
			}
			for(j = pos.col + 1; j <= N - 1 - i; j++) {
				if(g_board[pos.row][j] != side || Stablizer[pos.row+1][j+1] != 1) {
					break;
				}
				insideNum++;
				Stablizer[pos.row][j] = 1;
			}
		}
	}
	return insideNum;
}

int TopRightStable(int insideNum,int side,int Stablizer[][N]) {
	Spos pos;
	int i,j;
	if(g_board[0][N-1] == side) {
		for(i = 1; i < 8; i++) {
			pos.row = i;
			pos.col = N - 1 - i;
			if(g_board[pos.row][pos.col] != side || Stablizer[pos.row-1][pos.col] != 1 || Stablizer[pos.row][pos.col+1] != 1
				||(Stablizer[pos.row-1][pos.col-1] != 1 && Stablizer[pos.row+1][pos.col+1] != 1)) {
					break;
			}
			insideNum++;
			Stablizer[pos.row][pos.col] = 1;
			for(j = pos.row + 1; j <= N - 1 - i; j++) {
				if(g_board[j][pos.col] != side || Stablizer[j+1][pos.col+1] != 1) {
					break;
				}
				insideNum++;
				Stablizer[j][pos.col] = 1;
			}
			for(j = pos.col - 1; j >= i; j--) {
				if(g_board[pos.row][j] != side || Stablizer[pos.row-1][j+1] != 1) {
					break;
				}
				insideNum++;
				Stablizer[pos.row][j] = 1;
			}
		}
	}
	return insideNum;
}

int BottomRightStable(int insideNum,int side,int Stablizer[][N]) {
	Spos pos;
	int i,j;
	if(g_board[N-1][N-1] == side) {
		for(i = 1; i < 8; i++) {
			pos.row = N - 1 - i;
			pos.col = N - 1 - i;
			if(g_board[pos.row][pos.col] != side || Stablizer[pos.row+1][pos.col] != 1 || Stablizer[pos.row][pos.col+1] != 1
				||(Stablizer[pos.row+1][pos.col-1] != 1 && Stablizer[pos.row-1][pos.col+1] != 1)) {
					break;
			}
			insideNum++;
			Stablizer[pos.row][pos.col] = 1;
			for(j = pos.row - 1; j >= i; j--) {
				if(g_board[j][pos.col] != side || Stablizer[j+1][pos.col+1] != 1) {
					break;
				}
				insideNum++;
				Stablizer[j][pos.col] = 1;
			}
			if(j == i) insideNum -= N - 2 * i;
			for(j = pos.col - 1; j >= i; j--) {
				if(g_board[pos.row][j] != side || Stablizer[pos.row+1][j+1] != 1) {
					break;
				}
				insideNum++;
				Stablizer[pos.row][j] = 1;
			}
			if(j == i) insideNum -= N - 2 * i;
		}
	}
	return insideNum;
}

int IsFalseStable(Spos pos,int side) {
	Sdirect direct[8] = {{1,1},{1,0},{1,-1},{0,1},{0,-1},{-1,1},{-1,0},{-1,-1}};
	int i,nRow,nCol;
	for(i = 0; i < 8; i++) {
		nRow = pos.row + direct[i].x;
		nCol = pos.col + direct[i].y;
		while(nRow >= 0 && nRow <= N-1 && nCol >= 0 && nCol <= N-1 &&
			g_board[nRow][nCol] == side){
				nRow += direct[i].x;
				nCol += direct[i].y;
		}
		if(nRow >= 0 && nRow <= N-1 && nCol >= 0 && nCol <= N-1 &&
			g_board[nRow][nCol] == 0) {
				return 0;
		}
	}
	return 1;
}