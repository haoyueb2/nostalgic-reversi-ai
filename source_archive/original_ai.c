#include "ai.h"

/*
 * YOUR CODE BEGIN
 * 你的代码开始
 */
 
/* 
 * You can define your own struct and variable here
 * 你可以在这里定义你自己的结构体和变量
 */
 
const int DI[8][2] = { {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1} };

/*
 * 你可以在这里初始化你的AI
 */
void initAI(int me)
{

}

struct Position findValidPos(const char board[BOARD_SIZE][BOARD_SIZE], int flag)
{
	int row, col, curRow, curCol, curFlag, k, phrase;
	
	struct Position pos = {-1, -1};
	
	for (row = 0; row < BOARD_SIZE; row++)
		for (col = 0; col < BOARD_SIZE; col++)
			if (board[row][col] == 0)
			{
				for (k = 0; k < 8; k++)
				{
					curRow = row + DI[k][0];
					curCol = col + DI[k][1];
					phrase = 0;
					
					while(curRow >= 0 && curRow < BOARD_SIZE && curCol >= 0 && curCol < BOARD_SIZE)
					{
						curFlag = board[curRow][curCol];
						
						if (curFlag == 0) break;
						
						if (phrase == 0)
						{
							if (curFlag != flag)
								phrase = 1;
							else
								break;
						}
						else
						{
							if (curFlag == flag)
							{
								pos.x = row;
								pos.y = col;
								return pos;
							}
						}
						
						curRow += DI[k][0];
						curCol += DI[k][1];
					}
				}
			}
	
	return pos;
}

/*
 * 轮到你落子。
 * 棋盘上0表示空白，1表示黑棋，2表示白旗
 * me表示你所代表的棋子(1或2) 
 * 你需要返回一个结构体Position，在x属性和y属性填上你想要落子的位置。 
 */
struct Position aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{
    /*
     * TODO：在这里写下你的AI。 
     * 这里有一个示例AI，它只会寻找第一个可下的位置进行落子。 
     */
    struct Position preferedPos = findValidPos(board, me);
    
    return preferedPos;
}

/*
 * 你的代码结束 
 */