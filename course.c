#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "wincon.h"

#define WHITE 1
#define RED 2

#define DEFAULT 0 
#define QUEEN 1 
#define DEAD 2 

#define LEFT_BOARD_CENTER ((con_width() / 2) + 1) // +- �������� ����� ������������ ������ ���� �������
#define TOP_BOARD_CENTER  ((con_height() / 2) + 2) // +- �������� ����� ������������ �������� ���� �������

#define LEFT_LIMIT		  ((con_width() / 2) - 32) // ��������� ������ �����
#define RIGHT_LIMIT		  ((con_width() / 2) + 26) // ��������� ������ ������
#define TOP_LIMIT		  ((con_height() / 2) - 15) // ��������� ������ ������
#define BOT_LIMIT		  ((con_height() / 2) + 15) // ��������� ������ �����

/* ���������� ��� ������� move_cursor() */
int sel_check_left_pos = 0;
int sel_check_top_pos = 0;
int on;
int checkers_number = -1; // �� ��������� = -1
int whose_move = 1; // ����, ����� ������, ��� ��� (1 - ��� �����, 2 - ��� �������)
int cycle_end; // ���� ��� ���������� �����
int deleted_checker_left; //���������� ��������� ����� (���� ����� �����)
int deleted_checker_top;
int eat_more = 0; // ���� ���� ���������� ������ ���
int need_to_beat = 0; // ���� = 1, �� ����������� ����� ������ �����
int white_check_num = 12; // ��������� ����� ����� �����
int red_check_num = 12; // ��������� ����� ������� �����
int end = 0; // ���� ��� ������ � ������� ����
int is_load = 0; // ����, ����� ������, �������� ����� ���� ��� ������������ ������
int game_status = 0; // 1 - �������-�������, 2 - �������-���������, 3 - ����-����
int complexity = 0; // ��������� ���� (1 - �����, 2 - ���������, 3 - ������)

int white_moves_num = 0; // ���������� ��������� ����� �� ���� ��� ������� ��������
int red_moves_num = 0;

int cells_locations[64][2]; // ���������� ������ ������

struct table // ��� ������� ��������
{
	char str[70];
	int value;
};

typedef struct
{
	int checker_num; // ����� ������������� �����
	int color; // ���� �����
	int new_left; // ����� ����
	int new_top; // ������ �����
}Move;

typedef struct
{
	int location[1][2]; // ���������� ����� (������ ������ - ������ �����, ������ - ������)
	int status; // 0 - �������, 1 - �����, 2 - �����
}checker;
checker white_checkers[12];
checker red_checkers[12];

checker white_copy[12]; // ����� ����� ��� ������������� ���� ����������
checker red_copy[12];

typedef struct
{
	checker whites[12];
	checker reds[12];
}BOARD;

typedef struct
{
	int checker_num; // ����� ������������� �����
	int color; // ���� �����
	int coord[4]; // ������ � ����� ����������
}MOVE;

typedef struct
{
	BOARD board;
	MOVE move;
	int value;
}NODE;

// ��������, ���� �� ��� (���������� 1 - ���� ����, ����� 0)
int no_move()
{
	int left;
	int top;

	if (whose_move == WHITE) // ���� ��� �����
	{
		for (int i = 0; i < 12; i++)
		{
			if (white_checkers[i].status == DEFAULT) // �� �����
			{
				left = white_checkers[i].location[0][0]; // ���������� ������ �����
				top = white_checkers[i].location[0][1];

				if (is_there_a_checker(left + 8, top - 4) == 0 && (left + 8) < RIGHT_LIMIT + 3 && (top - 4) > TOP_LIMIT) {
					return 1;
				}
				if (is_there_a_checker(left - 8, top - 4) == 0 && (left - 8) > LEFT_LIMIT + 1 && (top - 4) > TOP_LIMIT) {
					return 1;
				}
			}
			else if (white_checkers[i].status == QUEEN) // �����
			{
				left = white_checkers[i].location[0][0]; // ���������� ������ �����
				top = white_checkers[i].location[0][1];

				if (is_there_a_checker(left + 8, top - 4) == 0 && (left + 8) < RIGHT_LIMIT + 3 && (top - 4) > TOP_LIMIT) {
					return 1;
				}
				if (is_there_a_checker(left - 8, top - 4) == 0 && (left - 8) > LEFT_LIMIT + 1 && (top - 4) > TOP_LIMIT) {
					return 1;
				}
				if (is_there_a_checker(left + 8, top + 4) == RED && (left + 8) < RIGHT_LIMIT + 3 && (top + 4) < BOT_LIMIT + 2) {
					return 1;
				}
				if (is_there_a_checker(left - 8, top + 4) == RED && (left - 8) > LEFT_LIMIT + 1 && (top + 4) < BOT_LIMIT + 2) {
					return 1;
				}
			}
		}
	}
	else if (whose_move == RED) // ���� ��� �������
	{
		for (int i = 0; i < 12; i++)
		{
			if (red_checkers[i].status == DEFAULT) // �� �����
			{
				left = red_checkers[i].location[0][0]; // ���������� ������ �����
				top = red_checkers[i].location[0][1];

				if (is_there_a_checker(left + 8, top + 4) == 0 && (left + 8) < RIGHT_LIMIT + 3 && (top + 4) < BOT_LIMIT + 2) {
					return 1;
				}
				if (is_there_a_checker(left - 8, top - 4) == 0 && (left - 8) > LEFT_LIMIT + 1 && (top + 4) < BOT_LIMIT + 2) {
					return 1;
				}
			}
			else if (red_checkers[i].status == QUEEN) // �����
			{
				left = red_checkers[i].location[0][0]; // ���������� ������ �����
				top = red_checkers[i].location[0][1];

				if (is_there_a_checker(left + 8, top - 4) == 0 && (left + 8) < RIGHT_LIMIT + 3 && (top - 4) > TOP_LIMIT) {
					return 1;
				}
				if (is_there_a_checker(left - 8, top - 4) == 0 && (left - 8) > LEFT_LIMIT + 1 && (top - 4) > TOP_LIMIT) {
					return 1;
				}
				if (is_there_a_checker(left + 8, top + 4) == RED && (left + 8) < RIGHT_LIMIT + 3 && (top + 4) < BOT_LIMIT + 2) {
					return 1;
				}
				if (is_there_a_checker(left - 8, top + 4) == RED && (left - 8) > LEFT_LIMIT + 1 && (top + 4) < BOT_LIMIT + 2) {
					return 1;
				}
			}
		}
	}

	return 0;
}

// ��������, ����������� �� ���� ����
int necessary_to_beat()
{
	int left;
	int top;

	if (whose_move == WHITE) // ���� ����� �����
	{
		for (int i = 0; i < 12; i++)
		{
			if (white_checkers[i].status == DEFAULT) // �� �����
			{
				left = white_checkers[i].location[0][0]; // ���������� ������ �����
				top = white_checkers[i].location[0][1];
				if (is_there_a_checker(left + 8, top + 4) == RED && (left + 16) < RIGHT_LIMIT + 3 && (top + 8) < BOT_LIMIT + 2) {
					if (is_there_a_checker(left + 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left + 8, top - 4) == RED && (left + 16) < RIGHT_LIMIT + 3 && (top - 8) > TOP_LIMIT) {
					if (is_there_a_checker(left + 16, top - 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left - 8, top + 4) == RED && (left - 16) > LEFT_LIMIT + 1 && (top + 8) < BOT_LIMIT + 2) {
					if (is_there_a_checker(left - 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left - 8, top - 4) == RED && (left - 16) > LEFT_LIMIT + 1 && (top - 8) > TOP_LIMIT) {
					if (is_there_a_checker(left - 16, top - 8) == 0) {
						return 1;
					}
				}
			}
			else if (white_checkers[i].status == QUEEN) // �����
			{
				left = white_checkers[i].location[0][0]; // ���������� ������ �����
				top = white_checkers[i].location[0][1];

				int left_change = 8;
				int top_change = 4;

				int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
				while (1)
				{
					for (int i = 8, j = 4; i <= left_change; i += 8, j = +4)
					{
						if (is_there_a_checker(left + i, top + j) == WHITE && !flag1) {
							flag1 = 1;
						}
						if (is_there_a_checker(left + i, top - j) == WHITE && !flag2) {
							flag2 = 1;
						}
						if (is_there_a_checker(left - i, top + j) == WHITE && !flag3) {
							flag3 = 1;
						}
						if (is_there_a_checker(left - i, top - j) == WHITE && !flag4) {
							flag4 = 1;
						}
					}
					if (!flag1) {
						if (is_there_a_checker(left + left_change, top + top_change) == RED && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_there_a_checker(left + left_change + 8, top + top_change + 4) == 0) {
								return 1;
							}
							else {
								flag1 = 1;
							}
						}
					}
					if (!flag2) {
						if (is_there_a_checker(left + left_change, top - top_change) == RED && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_there_a_checker(left + left_change + 8, top - top_change - 4) == 0) {
								return 1;
							}
							else {
								flag2 = 1;
							}
						}
					}
					if (!flag3) {
						if (is_there_a_checker(left - left_change, top + top_change) == RED && (left - left_change - 8) > LEFT_LIMIT + 1 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_there_a_checker(left - left_change - 8, top + top_change + 4) == 0) {
								return 1;
							}
							else {
								flag3 = 1;
							}
						}
					}
					if (!flag4) {
						if (is_there_a_checker(left - left_change, top - top_change) == RED && (left - left_change - 8) > LEFT_LIMIT + 1 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_there_a_checker(left - left_change - 8, top - top_change - 4) == 0) {
								return 1;
							}
							else {
								flag4 = 1;
							}
						}
					}

					left_change += 8;
					top_change += 4;

					if (((left + left_change) > RIGHT_LIMIT + 3) && ((left - left_change) < LEFT_LIMIT + 1) && ((top + top_change) > BOT_LIMIT + 2) && ((top - top_change) < TOP_LIMIT))
					{
						break;
					}
				}
			}
		}
	}
	else if (whose_move == RED) // ���� ����� �������
	{
		for (int i = 0; i < 12; i++)
		{
			if (red_checkers[i].status == DEFAULT) // �� �����
			{
				left = red_checkers[i].location[0][0];
				top = red_checkers[i].location[0][1];
				if (is_there_a_checker(left + 8, top + 4) == WHITE && (left + 16) < RIGHT_LIMIT + 3 && (top + 8) < BOT_LIMIT + 2) {
					if (is_there_a_checker(left + 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left + 8, top - 4) == WHITE && (left + 16) < RIGHT_LIMIT + 3 && (top - 8) > TOP_LIMIT) {
					if (is_there_a_checker(left + 16, top - 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left - 8, top + 4) == WHITE && (left - 16) > LEFT_LIMIT + 1 && (top + 8) < BOT_LIMIT + 2) {
					if (is_there_a_checker(left - 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left - 8, top - 4) == WHITE && (left - 16) > LEFT_LIMIT + 1 && (top - 8) > TOP_LIMIT) {
					if (is_there_a_checker(left - 16, top - 8) == 0) {
						return 1;
					}
				}
			}
			else if (red_checkers[i].status == QUEEN) // �����
			{
				left = red_checkers[i].location[0][0]; // ���������� ������ �����
				top = red_checkers[i].location[0][1];

				int left_change = 8;
				int top_change = 4;
				int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
				while (1)
				{
					for (int i = 8, j = 4; i <= left_change; i += 8, j = +4)
					{
						if (is_there_a_checker(left + i, top + j) == RED && !flag1) {
							flag1 = 1;
						}
						if (is_there_a_checker(left + i, top - j) == RED && !flag2) {
							flag2 = 1;
						}
						if (is_there_a_checker(left - i, top + j) == RED && !flag3) {
							flag3 = 1;
						}
						if (is_there_a_checker(left - i, top - j) == RED && !flag4) {
							flag4 = 1;
						}
					}
					if (!flag1) {
						if (is_there_a_checker(left + left_change, top + top_change) == WHITE && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_there_a_checker(left + left_change + 8, top + top_change + 4) == 0) {
								return 1;
							}
						}
					}
					if (!flag2) {
						if (is_there_a_checker(left + left_change, top - top_change) == WHITE && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_there_a_checker(left + left_change + 8, top - top_change - 4) == 0) {
								return 1;
							}
						}
					}
					if (!flag3) {
						if (is_there_a_checker(left - left_change, top + top_change) == WHITE && (left - left_change - 8) > LEFT_LIMIT + 1 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_there_a_checker(left - left_change - 8, top + top_change + 4) == 0) {
								return 1;
							}
						}
					}
					if (!flag4) {
						if (is_there_a_checker(left - left_change, top - top_change) == WHITE && (left - left_change - 8) > LEFT_LIMIT + 1 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_there_a_checker(left - left_change - 8, top - top_change - 4) == 0) {
								return 1;
							}
						}
					}

					left_change += 8;
					top_change += 4;

					if ((left + left_change) > RIGHT_LIMIT + 3 && (left - left_change) < LEFT_LIMIT + 1 && (top + top_change) > BOT_LIMIT + 2 && (top - top_change) < TOP_LIMIT)
					{
						break;
					}
				}
			}
		}
	}
	return 0;
}

// ���� ����� ����� �� ����� �����, �� ��� ���������� ������
void became_a_queen(int top, int color)
{
	if (color) // ������� �����
	{
		if (top == BOT_LIMIT) {
			red_checkers[checkers_number].status = 1;
		}
	}
	else // ����� �����
	{
		if (top == TOP_LIMIT + 2) {
			white_checkers[checkers_number].status = 1;
		}
	}
	int a = con_height() / 2;
}

// ��������, ����� �� ������ ����� (���������� 1, ���� �����, ����� 0)
int can_eat_more(int left, int top)
{
	int status = -1;
	if (whose_move == WHITE) // ���� ��� �����
	{
		for (int i = 0; i < 12; i++)
		{
			if (white_checkers[i].location[0][0] == left && white_checkers[i].location[0][1] == top)
			{
				if (white_checkers[i].status == 0) {
					status = 0;
					break;
				}
				else if (white_checkers[i].status == 1) {
					status = 1;
					break;
				}
			}
		}

		if (status == DEFAULT) // �� �����
		{
			for (int i = 0; i < 12; i++)
			{
				if (is_there_a_checker(left + 8, top + 4) == RED && (left + 16) < RIGHT_LIMIT + 3 && (top + 8) < BOT_LIMIT + 2) {
					if (is_there_a_checker(left + 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left + 8, top - 4) == RED && (left + 16) < RIGHT_LIMIT + 3 && (top - 8) > TOP_LIMIT) {
					if (is_there_a_checker(left + 16, top - 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left - 8, top + 4) == RED && (left - 16) > LEFT_LIMIT + 1 && (top + 8) < BOT_LIMIT + 2) {
					if (is_there_a_checker(left - 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left - 8, top - 4) == RED && (left - 16) > LEFT_LIMIT + 1 && (top - 8) > TOP_LIMIT) {
					if (is_there_a_checker(left - 16, top - 8) == 0) {
						return 1;
					}
				}
			}
		}
		else if (status == 1) // �����
		{
			int left_change = 8;
			int top_change = 4;
			int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
			while (1)
			{
				for (int i = 8, j = 4; i <= left_change; i += 8, j = +4)
				{
					if (is_there_a_checker(left + i, top + j) == 1 && !flag1) {
						flag1 = 1;
					}
					if (is_there_a_checker(left + i, top - j) == 1 && !flag2) {
						flag2 = 1;
					}
					if (is_there_a_checker(left - i, top + j) == 1 && !flag3) {
						flag3 = 1;
					}
					if (is_there_a_checker(left - i, top - j) == 1 && !flag4) {
						flag4 = 1;
					}
				}
				if (!flag1) {
					if (is_there_a_checker(left + left_change, top + top_change) == 2 && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top + top_change + 4) < BOT_LIMIT + 2) {
						if (is_there_a_checker(left + left_change + 8, top + top_change + 4) == 0) {
							return 1;
						}
						else {
							flag1 = 1;
						}
					}
				}
				if (!flag2) {
					if (is_there_a_checker(left + left_change, top - top_change) == 2 && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top - top_change - 4) > TOP_LIMIT) {
						if (is_there_a_checker(left + left_change + 8, top - top_change - 4) == 0) {
							return 1;
						}
						else {
							flag2 = 1;
						}
					}
				}
				if (!flag3) {
					if (is_there_a_checker(left - left_change, top + top_change) == 2 && (left - left_change - 8) > LEFT_LIMIT + 1 && (top + top_change + 4) < BOT_LIMIT + 2) {
						if (is_there_a_checker(left - left_change - 8, top + top_change + 4) == 0) {
							return 1;
						}
						else {
							flag3 = 1;
						}
					}
				}
				if (!flag4) {
					if (is_there_a_checker(left - left_change, top - top_change) == 2 && (left - left_change - 8) > LEFT_LIMIT + 1 && (top - top_change - 4) > TOP_LIMIT) {
						if (is_there_a_checker(left - left_change - 8, top - top_change - 4) == 0) {
							return 1;
						}
						else {
							flag4 = 1;
						}
					}
				}

				left_change += 8;
				top_change += 4;

				if (((left + left_change) > RIGHT_LIMIT + 3) && ((left - left_change) < LEFT_LIMIT + 1) && ((top + top_change) > BOT_LIMIT + 2) && ((top - top_change) < TOP_LIMIT))
				{
					break;
				}
			}
		}
	}
	else if (whose_move == 2) // ���� ��� �������
	{
		for (int i = 0; i < 12; i++)
		{
			if (red_checkers[i].location[0][0] == left && red_checkers[i].location[0][1] == top)
			{
				if (red_checkers[i].status == 0) {
					status = 0;
					break;
				}
				else if (red_checkers[i].status == 1) {
					status = 1;
					break;
				}
			}
		}

		if (status == 0) // �� �����
		{
			for (int i = 0; i < 12; i++)
			{
				if (is_there_a_checker(left + 8, top + 4) == 1 && (left + 16) < RIGHT_LIMIT + 3 && (top + 8) < BOT_LIMIT + 2) {
					if (is_there_a_checker(left + 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left + 8, top - 4) == 1 && (left + 16) < RIGHT_LIMIT + 3 && (top - 8) > TOP_LIMIT) {
					if (is_there_a_checker(left + 16, top - 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left - 8, top + 4) == 1 && (left - 16) > LEFT_LIMIT + 1 && (top + 8) < BOT_LIMIT + 2) {
					if (is_there_a_checker(left - 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_there_a_checker(left - 8, top - 4) == 1 && (left - 16) > LEFT_LIMIT + 1 && (top - 8) > TOP_LIMIT) {
					if (is_there_a_checker(left - 16, top - 8) == 0) {
						return 1;
					}
				}
			}
		}
		else if (status == 1) // �����
		{
			int left_change = 8;
			int top_change = 4;
			int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
			while (1)
			{
				for (int i = 8, j = 4; i <= left_change; i += 8, j = +4)
				{
					if (is_there_a_checker(left + i, top + j) == 2 && !flag1) {
						flag1 = 1;
					}
					if (is_there_a_checker(left + i, top - j) == 2 && !flag2) {
						flag2 = 1;
					}
					if (is_there_a_checker(left - i, top + j) == 2 && !flag3) {
						flag3 = 1;
					}
					if (is_there_a_checker(left - i, top - j) == 2 && !flag4) {
						flag4 = 1;
					}
				}
				if (!flag1) {
					if (is_there_a_checker(left + left_change, top + top_change) == 1 && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top + top_change + 4) < BOT_LIMIT + 2) {
						if (is_there_a_checker(left + left_change + 8, top + top_change + 4) == 0) {
							return 1;
						}
						else {
							flag1 = 1;
						}
					}
				}
				if (!flag2) {
					if (is_there_a_checker(left + left_change, top - top_change) == 1 && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top - top_change - 4) > TOP_LIMIT) {
						if (is_there_a_checker(left + left_change + 8, top - top_change - 4) == 0) {
							return 1;
						}
						else {
							flag2 = 1;
						}
					}
				}
				if (!flag3) {
					if (is_there_a_checker(left - left_change, top + top_change) == 1 && (left - left_change - 8) > LEFT_LIMIT + 1 && (top + top_change + 4) < BOT_LIMIT + 2) {
						if (is_there_a_checker(left - left_change - 8, top + top_change + 4) == 0) {
							return 1;
						}
						else {
							flag3 = 1;
						}
					}
				}
				if (!flag4) {
					if (is_there_a_checker(left - left_change, top - top_change) == 1 && (left - left_change - 8) > LEFT_LIMIT + 1 && (top - top_change - 4) > TOP_LIMIT) {
						if (is_there_a_checker(left - left_change - 8, top - top_change - 4) == 0) {
							return 1;
						}
						else {
							flag4 = 1;
						}
					}
				}

				left_change += 8;
				top_change += 4;

				if ((left + left_change) > RIGHT_LIMIT + 3 && (left - left_change) < LEFT_LIMIT + 1 && (top + top_change) > BOT_LIMIT + 2 && (top - top_change) < TOP_LIMIT)
				{
					break;
				}
			}
		}
	}

	return 0;
}

// �������� ���������� �����
void change_check_lokation(int color, int new_left, int new_top)
{
	if (color)
	{
		red_checkers[checkers_number].location[0][0] = new_left;
		red_checkers[checkers_number].location[0][1] = new_top;
	}
	else
	{
		white_checkers[checkers_number].location[0][0] = new_left;
		white_checkers[checkers_number].location[0][1] = new_top;
	}
	checkers_number = -1;
}

// ������� ���
void step(int old_left, int old_top, int new_left, int new_top, int check_color)
{
	gotoxy(old_left, old_top);
	con_set_color(NULL, CON_CLR_BLACK);
	printf("   ");
	gotoxy(new_left, new_top);
	if (check_color) { // ������� �����
		if (red_checkers[checkers_number].status == 0) {
			became_a_queen(new_top, check_color);
		}
		con_set_color(NULL, CON_CLR_RED);
		if (red_checkers[checkers_number].status == 0) { // �� �����
			printf("   ");
		}
		else if (red_checkers[checkers_number].status == 1) { // �����
			printf("���");
		}
	}
	else { // ����� �����
		if (white_checkers[checkers_number].status == 0) {
			became_a_queen(new_top, check_color);
		}
		con_set_color(NULL, CON_CLR_WHITE);
		if (white_checkers[checkers_number].status == 0) { // �� �����
			printf("   ");
		}
		else if (white_checkers[checkers_number].status == 1) { // �����
			printf("���");
		}
	}

	change_check_lokation(check_color, new_left, new_top); // ������� ���������� �����

	if (whose_move == WHITE) { // ���� ��� ������� �����, �� ������� ����� ����� ++
		white_moves_num++;
	}
	else if (whose_move == RED) { // ���� ��� ������� ������, �� ������� ����� ������ ++
		red_moves_num++;
	}
}

// ����� � ������� ��������� �����
void find_and_delete_check(int old_left, int old_top, int new_left, int new_top)
{
	if (whose_move == 1) // ���� ��� �����, ���� ����� ������� �����	
	{
		while (1)
		{
			if (new_left > old_left) { // ���� ����� ������ ��������� ������ ������
				new_left -= 8; // ���������� �����
			}
			else {
				new_left += 8;
			}

			if (new_top > old_top) { // ���� ����� ������ ��������� ���� ������
				new_top -= 4; // ���������� �����
			}
			else {
				new_top += 4;
			}

			if (new_left == old_left || new_top == old_top) // ���� ��������� � ������, ������ ����������� ���
			{
				break;
			}

			for (int i = 0; i < 12; i++)
			{
				if (red_checkers[i].location[0][0] == new_left && red_checkers[i].location[0][1] == new_top && red_checkers[i].status != 2)
				{
					red_check_num--;
					red_checkers[i].status = 2;
					gotoxy(new_left, new_top);
					con_set_color(NULL, CON_CLR_BLACK);
					printf("   "); // ������� ��������� ����� � �����
					return;
				}
			}
		}
	}
	else if (whose_move == 2) // ���� ��� �������
	{
		while (1)
		{
			if (new_left > old_left) { // ���� ����� ������ ��������� ������ ������
				new_left -= 8; // ���������� �����
			}
			else {
				new_left += 8;
			}

			if (new_top > old_top) { // ���� ����� ������ ��������� ���� ������
				new_top -= 4; // ���������� �����
			}
			else {
				new_top += 4;
			}

			if (new_left == old_left || new_top == old_top) // ���� ��������� � ������, ������ ����������� ���
			{
				break;
			}

			for (int i = 0; i < 12; i++)
			{
				if (white_checkers[i].location[0][0] == new_left && white_checkers[i].location[0][1] == new_top && white_checkers[i].status != 2)
				{
					white_check_num--;
					white_checkers[i].status = 2;
					gotoxy(new_left, new_top);
					con_set_color(NULL, CON_CLR_BLACK);
					printf("   "); // ������� ��������� ����� � �����
					return;
				}
			}
		}
	}
}

// ����������, ����� �� ������ ����� (���������� 2, ���� �����, 1, ���� ������ ������� ���, ����� 0)
int is_it_possible_eat(int old_left, int old_top, int new_left, int new_top, int stat)
{
	int flag = 0; // ����, ��� �������� ���������� ����� �� (new_left, new_top)
	if (whose_move == 2) // ���� ��� �������
	{
		while (1)
		{
			if (new_left > old_left) { // ���� ����� ������ ��������� ������ ������
				new_left -= 8; // ���������� �����
			}
			else {
				new_left += 8;
			}

			if (new_top > old_top) { // ���� ����� ������ ��������� ���� ������
				new_top -= 4; // ���������� �����
			}
			else {
				new_top += 4;
			}

			if (new_left == old_left || new_top == old_top) // ���� ��������� � ������, ������ ����������� ���
			{
				break;
			}

			for (int i = 0; i < 12; i++)
			{
				if (red_checkers[i].location[0][0] == new_left && red_checkers[i].location[0][1] == new_top && red_checkers[i].status != 2) // ���� �� ���� ���� ���� �����
				{
					return 0;
				}
				else if (white_checkers[i].location[0][0] == new_left && white_checkers[i].location[0][1] == new_top && white_checkers[i].status != 2) // ���� �� ���� ���� ����� �����
				{
					flag++;
					break;
				}
			}

			if (flag > 1) { // ���� �� ���� ������ ����� �����
				return 0;
			}
		}
	}
	else if (whose_move == 1) // ���� ��� �����
	{
		while (1)
		{
			if (new_left > old_left) { // ���� ����� ������ ��������� ������ ������
				new_left -= 8; // ���������� �����
			}
			else {
				new_left += 8;
			}

			if (new_top > old_top) { // ���� ����� ������ ��������� ���� ������
				new_top -= 4; // ���������� �����
			}
			else {
				new_top += 4;
			}

			if (new_left == old_left || new_top == old_top) // ���� ��������� � ������, ������ ����������� ���
			{
				break;
			}

			for (int i = 0; i < 12; i++)
			{
				if (white_checkers[i].location[0][0] == new_left && white_checkers[i].location[0][1] == new_top && white_checkers[i].status != 2) // ���� �� ���� ���� ���� �����
				{
					return 0;
				}
				else if (red_checkers[i].location[0][0] == new_left && red_checkers[i].location[0][1] == new_top && red_checkers[i].status != 2) // ���� �� ���� ���� ����� �����
				{
					flag++;
					break;
				}
			}

			if (flag > 1) { // ���� �� ���� ������ ����� �����
				return 0;
			}
		}
	}

	if (flag == 1) { // ���� �� ���� ������ ���� ����� �����
		return 2;
	}
	else { // ���� �� ���� ��� �����
		if (!stat) { // ���� �� �����
			return 0;
		}
		else { // ���� �����
			return 1;
		}
	}
}

// �������� �� ������ ���
int is_action_permitted(int old_left, int old_top, int new_left, int new_top)
{
	int flag = 0;
	for (int i = 0; i < 12; i++)
	{
		// ���� � ������, ���� �������������� �����, ���� ������ �����
		if ((white_checkers[i].location[0][0] == new_left && white_checkers[i].location[0][1] == new_top && white_checkers[i].status != 2) ||
			(red_checkers[i].location[0][0] == new_left && red_checkers[i].location[0][1] == new_top && red_checkers[i].status != 2))
		{
			return 0;
		}

		if (white_checkers[i].location[0][0] == old_left && white_checkers[i].location[0][1] == old_top) // ������� �����, ������� ��������� � ������ ������
		{
			if (white_checkers[i].status == 0) // �� �����
			{
				if ((new_top - old_top) != 0 && get_cell_color(new_left - 2, new_top - 1) == 1) {
					if (abs(new_left - old_left) / abs(new_top - old_top) == 2) {
						if ((abs(new_left - old_left) % 8) == 0 && (new_left - old_left) != 0) // ������������ ������
						{
							if ((old_top - new_top) == 4) // ������� ���
							{
								return 1;
							}
							else if (abs(old_top - new_top) == 8) // ������� ������ ������ �����
							{
								if (is_it_possible_eat(old_left, old_top, new_left, new_top, 0) == 2) { // ���� �������� ������ ������ �����
									return 2;
								}
								else {
									return 0;
								}
							}
						}
					}
				}
				else { // ���� �������� ������� �� �������������� ������
					return 0;
				}
			}
			else // �����
			{
				if ((new_top - old_top) != 0 && get_cell_color(new_left - 2, new_top - 1) == 1) {
					if (abs(new_left - old_left) / abs(new_top - old_top) == 2) { // ������������ ������
						if ((abs(new_left - old_left) % 8) == 0 && (new_left - old_left) != 0)
						{
							if ((abs(new_top - old_top) % 4) == 0)
							{
								if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 2) { // ���� �������� ������ ������ ����� 
									return 2;
								}
								else if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 1) { // ���� �������� ������� ���
									return 1;
								}
								else if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 0) {
									return 0;
								}
							}
						}
					}
				}
				else { // ���� �������� ������� �� �������������� ������ 
					return 0;
				}
			}
		}
		else if (red_checkers[i].location[0][0] == old_left && red_checkers[i].location[0][1] == old_top)
		{
			if (red_checkers[i].status == 0) // �� �����
			{
				if ((new_top - old_top) != 0 && get_cell_color(new_left - 2, new_top - 1) == 1) {
					if (abs(new_left - old_left) / abs(new_top - old_top) == 2) { // ������������ ������
						if ((abs(new_left - old_left) % 8) == 0 && (new_left - old_left) != 0) // ������������ ������
						{
							if ((new_top - old_top) == 4) // ������� ���
							{
								return 1;
							}
							else if (abs(new_top - old_top) == 8) // ������� ������ ������ �����
							{
								if (is_it_possible_eat(old_left, old_top, new_left, new_top, 0) == 2) { // ���� �������� ������ ������ �����
									return 2;
								}
								else {
									return 0;
								}
							}
						}
					}
				}
				else { // ���� �������� ������� �� �������������� ������
					return 0;
				}
			}
			else // �����
			{
				if ((new_top - old_top) != 0 && get_cell_color(new_left - 2, new_top - 1) == 1) {
					if (abs(new_left - old_left) / abs(new_top - old_top) == 2) { // ������������ ������
						if ((abs(new_left - old_left) % 8) == 0 && (new_left - old_left) != 0)
						{
							if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 2) { // ���� �������� ������ ������ ����� 
								return 2;
							}
							else if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 1) { // ���� �������� ������� ���
								return 1;
							}
							else if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 0) {
								return 0;
							}
						}
					}
				}
				else { // ���� �������� ������� �� �������������� ������ 
					return 0;
				}
			}
		}
	}

	return 0;
}

// ������, ���� �� � ������ ������ ����� (���������� 1, ���� ���� ����� �����, 2 - ���� �������, ����� 0)
int is_there_a_checker(int left, int top)
{
	for (int i = 0; i < 12; i++)
	{
		if (white_checkers[i].location[0][0] == left && white_checkers[i].location[0][1] == top && white_checkers[i].status != 2)
		{
			return 1;
		}
		else if (red_checkers[i].location[0][0] == left && red_checkers[i].location[0][1] == top && red_checkers[i].status != 2)
		{
			return 2;
		}
	}
	return 0;
}

// ��������/��������� ����������� ������ (���� ��������� �����)
void steady_cursor(int left, int top, int on_off)
{
	gotoxy(left, top);
	show_cursor(0);

	if (on_off)
	{
		on = 1;
		con_set_color(NULL, CON_CLR_GREEN);
		printf("       ");
		gotoxy(left, top + 1);
		printf("  ");
		gotoxy(left + 5, top + 1);
		printf("  ");
		gotoxy(left, top + 2);
		printf("       ");
	}
	else
	{
		on = 0;
		con_set_color(NULL, CON_CLR_BLACK);
		gotoxy(left, top);
		printf("       ");
		gotoxy(left, top + 1);
		printf("  ");
		gotoxy(left + 5, top + 1);
		printf("  ");
		gotoxy(left, top + 2);
		printf("       ");
	}
}

// �������� ������
void blinking_cursor(int left, int top, int color)
{
	gotoxy(left, top);
	show_cursor(0);

	con_set_color(NULL, CON_CLR_GREEN);
	printf("       ");
	gotoxy(left, top + 1);
	printf("  ");
	gotoxy(left + 5, top + 1);
	printf("  ");
	gotoxy(left, top + 2);
	printf("       ");

	if (color == 1) {
		con_set_color(NULL, CON_CLR_BLACK);
	}
	else {
		con_set_color(NULL, CON_CLR_GRAY);
	}
	if (key_is_pressed()) { // ����� �� ���� �������� ��� ������� ������������ �������
		pause(30);
		gotoxy(left, top);
		printf("       ");
		gotoxy(left, top + 1);
		printf("  ");
		gotoxy(left + 5, top + 1);
		printf("  ");
		gotoxy(left, top + 2);
		printf("       ");
		return;
	}
	pause(350);
	gotoxy(left, top);
	printf("       ");
	gotoxy(left, top + 1);
	printf("  ");
	gotoxy(left + 5, top + 1);
	printf("  ");
	gotoxy(left, top + 2);
	printf("       ");
	if (key_is_pressed()) { // ����� �� ���� �������� ��� ������� ������������ �������
		return;
	}
	pause(350);
}

// �������� ���������� ������
void get_cells_locations()
{
	int left_edge = (con_width() / 2) - 31;
	int top_edge = (con_height() / 2) + 14;

	int left = left_edge;
	int top = top_edge;

	int flag = 7;
	for (int i = 0; i < 64; i++)
	{
		cells_locations[i][0] = left;
		cells_locations[i][1] = top;

		if (i == flag)
		{
			left += 8;
			top = top_edge;
			flag += 8;
		}
		else
		{
			top -= 4;
		}
	}
}

// �������� ���� � ����� ����� � ������ ������ (���������� 1, ���� ����� �������, ����� 0)
int get_check_color(int left, int top)
{
	for (int i = 0; i < 12; i++)
	{
		if (white_checkers[i].location[0][0] == left && white_checkers[i].location[0][1] == top && white_checkers[i].status != 2)
		{
			checkers_number = i;
			return 0;
		}
		else if (red_checkers[i].location[0][0] == left && red_checkers[i].location[0][1] == top && red_checkers[i].status != 2)
		{
			checkers_number = i;
			return 1;
		}
	}
}

// �������� ���� ������ (���������� 1, ���� ������ ������, 0 - �����)
int get_cell_color(int left, int top)
{
	for (int i = 0; i < 64; i++)
	{
		if (cells_locations[i][0] == left && cells_locations[i][1] == top)
		{
			if (i < 8)
			{
				if ((i % 2) == 0)
					return 1;
				else
					return 0;
			}
			else if (i > 7 && i < 16)
			{
				if ((i % 2) == 0)
					return 0;
				else
					return 1;
			}
			else if (i > 15 && i < 24)
			{
				if ((i % 2) == 0)
					return 1;
				else
					return 0;
			}
			else if (i > 23 && i < 32)
			{
				if ((i % 2) == 0)
					return 0;
				else
					return 1;
			}
			else if (i > 31 && i < 40)
			{
				if ((i % 2) == 0)
					return 1;
				else
					return 0;
			}
			else if (i > 39 && i < 48)
			{
				if ((i % 2) == 0)
					return 0;
				else
					return 1;
			}
			else if (i > 47 && i < 56)
			{
				if ((i % 2) == 0)
					return 1;
				else
					return 0;
			}
			else if (i > 55 && i < 64)
			{
				if ((i % 2) == 0)
					return 0;
				else
					return 1;
			}
		}
	}
}

// ��������� �����
void build_board()
{
	int left = (con_width() / 2) - 32;
	int top = (con_height() / 2) - 15;

	con_set_color(NULL, CON_CLR_YELLOW_LIGHT);
	clrscr();

	con_draw_lock(); // ������������� ���������

	gotoxy(left, top);
	con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW);
	printf("*****************************************************************");
	top++;

	for (int count = 0; count < 8; count++) // ���������� ����� � �����
	{
		for (int i = 0; i < 3; i++) // ���������� �������� ��� ���������� ������ ����
		{
			left = (con_width() / 2) - 32; // ���������� ������ � ������ ���� �����
			if ((count % 2) == 0) // ���� ����� ������� ������, �� ������ ������ ������ ���� �����
			{
				for (int j = 0; j < 4; j++) // ���������� ������ � ����� ���� / 2 (����� �������� ����� �� ��� ������)
				{
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ������� ������
					if (j != 0) {
						printf(" ");
					}
					else {
						printf("*");
					}
					left++; // ����������� ������ �� ���� ������ ������
					gotoxy(left, top);
					con_set_color(NULL, CON_CLR_GRAY); // NULL - �.�. ������� �� ������������
					printf("       ");
					left += 7; // ����������� ������ �� ���� ������ ������
					con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ������� ������
					printf(" ");
					left++; // ����������� ������ �� ���� ������ ������  |���������� ����� ������|

					if (i != 1) // ���� ������ ��������� �� � ������ ������, ������ ����������� ����� ������
					{
						con_set_color(NULL, CON_CLR_BLACK); // NULL - �.�. ������� �� ������������
						printf("       ");
						left += 7; // ����������� ������ �� ���� ������ ������
					}
					else // ���� ������ ��������� � ������ ������
					{
						con_set_color(NULL, CON_CLR_BLACK); // NULL - �.�. ������� �� ������������
						printf("  ");
						left += 2; // ����������� ������ �� ��� ������ ������
						int flag = 0; // ���� ���� = 1, ������ �� ���� ����� ����� �����
						for (int n = 0; n < 12; n++) // ���������� �� ���� ������
						{
							if (red_checkers[n].location[0][0] == left && red_checkers[n].location[0][1] == top) // ���� �� ���� ����� ������ ���������� ������� �����
							{
								flag = 1;
								if (red_checkers[n].status == 0) // ���� ��� ����� - �� �����
								{
									gotoxy(left, top);
									con_set_color(NULL, CON_CLR_RED); // NULL - �.�. ������� �� ������������
									printf("   ");
								}
								else if (red_checkers[n].status == 1) // ���� ��� ����� - �����
								{
									gotoxy(left, top);
									con_set_color(CON_CLR_BLACK, CON_CLR_RED);
									printf("���");
								}
								left += 3; // ����������� ������ �� ��� ������ ������
							}
							else if (white_checkers[n].location[0][0] == left && white_checkers[n].location[0][1] == top) // ���� �� ���� ����� ������ ���������� ����� �����
							{
								flag = 1;
								if (white_checkers[n].status == 0) // ���� ��� ����� - �� �����
								{
									gotoxy(left, top);
									con_set_color(NULL, CON_CLR_WHITE); // NULL - �.�. ������� �� ������������
									printf("   ");
								}
								else if (white_checkers[n].status == 1) // ���� ��� ����� - �����
								{
									gotoxy(left, top);
									con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
									printf("���");
								}
								left += 3; // ����������� ������ �� ��� ������ ������
							}
						}
						if (flag) // ���� � ������ ���� �������� �����
						{
							con_set_color(NULL, CON_CLR_BLACK); // NULL - �.�. ������� �� ������������
							printf("  ");
							left += 2; //����������� ������ �� ��� ������ ������
						}
						else
						{
							con_set_color(NULL, CON_CLR_BLACK); // NULL - �.�. ������� �� ������������
							printf("     ");
							left += 5; //����������� ������ �� ���� ������ ������
						}
					}
					if (j == 3) {
						con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ������ ������� �����
						printf("*");
						top++; // ����������� ������ �� ���� ������ ����
					}
				}
			}
			else // ���� ����� ������� ��������, �� ������ ������ ������ ���� ������
			{
				for (int j = 0; j < 4; j++) // ���������� ������ � ����� ���� / 2 (����� �������� ����� �� ��� ������)
				{
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ������� ������
					if (j != 0) {
						printf(" ");
					}
					else {
						printf("*");
					}
					left++; // ����������� ������ �� ���� ������ ������
					if (i != 1) // ���� ������ ��������� �� � ������ ������, ������ ����������� ����� ������
					{
						con_set_color(NULL, CON_CLR_BLACK); // NULL - �.�. ������� �� ������������
						printf("       ");
						left += 7; // ����������� ������ �� ���� ������ ������
						con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ������� ������
						printf(" ");
						left++; // ����������� ������ �� ���� ������ ������
					}
					else // ���� ������ ��������� � ������ ������
					{
						con_set_color(NULL, CON_CLR_BLACK); // NULL - �.�. ������� �� ������������
						printf("  ");
						left += 2; // ����������� ������ �� ��� ������ ������
						int flag = 0; // ���� ���� = 1, ������ �� ���� ����� ����� �����
						for (int n = 0; n < 12; n++) // ���������� �� ���� ������
						{
							if (red_checkers[n].location[0][0] == left && red_checkers[n].location[0][1] == top) // ���� �� ���� ����� ������ ���������� ������� �����
							{
								flag = 1;
								if (red_checkers[n].status == 0) // ���� ��� ����� - �� �����
								{
									gotoxy(left, top);
									con_set_color(NULL, CON_CLR_RED); // NULL - �.�. ������� �� ������������
									printf("   ");
								}
								else if (red_checkers[n].status == 1) // ���� ��� ����� - �����
								{
									gotoxy(left, top);
									con_set_color(CON_CLR_BLACK, CON_CLR_RED);
									printf("���");
								}
								left += 3; // ����������� ������ �� ��� ������ ������
							}
							else if (white_checkers[n].location[0][0] == left && white_checkers[n].location[0][1] == top) // ���� �� ���� ����� ������ ���������� ����� �����
							{
								flag = 1;
								if (white_checkers[n].status == 0) // ���� ��� ����� - �� �����
								{
									gotoxy(left, top);
									con_set_color(NULL, CON_CLR_WHITE); // NULL - �.�. ������� �� ������������
									printf("   ");
								}
								else if (white_checkers[n].status == 1) // ���� ��� ����� - �����
								{
									gotoxy(left, top);
									con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
									printf("���");
								}
								left += 3; // ����������� ������ �� ��� ������ ������
							}
						}
						if (flag) // ���� � ������ ���� �������� �����
						{
							con_set_color(NULL, CON_CLR_BLACK); // NULL - �.�. ������� �� ������������
							printf("  ");
							left += 2; //����������� ������ �� ��� ������ ������
						}
						else
						{
							con_set_color(NULL, CON_CLR_BLACK); // NULL - �.�. ������� �� ������������
							printf("     ");
							left += 5; //����������� ������ �� ���� ������ ������
						}
						con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ������ ������� �����
						printf(" ");
						left++; // ����������� ������ �� ���� ������ ������
					}
					con_set_color(NULL, CON_CLR_GRAY); // NULL - �.�. ������� �� ������������
					printf("       ");
					left += 7; // ����������� ������ �� ���� ������ ������
					if (j == 3) {
						con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ������ ������� �����
						printf("*");
						top++; // ����������� ������ �� ���� ������ ����
					}
				}
			}
		}
		left = (con_width() / 2) - 32;
		gotoxy(left, top);
		con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW);
		if (count != 7) {
			printf("*                                                               *"); // ������� ����� ��������
		}
		else {
			printf("*****************************************************************");
		}
		top++; // ����������� ������ �� ���� ������ ����
	}

	con_draw_release(); // �������������� ��������� (������� �� �����)
}

// ��������� ����
void save_the_game()
{
	char filename[30];

	int left = 60;
	int top = 15;

	// ������� ������
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	clrscr();
	gotoxy(left, top);

	printf("������� �������� ����������: ");
	top += 2;
	left += 3;
	gotoxy(left, top);
	scanf("%s", &filename);

	FILE* f = fopen(filename, "wb");

	if (f == NULL)
	{
		left = 60;
		top = 15;
		clrscr();
		gotoxy(left, top);
		printf("�� ������� ��������� ����.");
		Sleep(1000);
		return 0;
	}

	if (strstr(filename, "."))
	{
		left = 60;
		top = 15;
		clrscr();
		gotoxy(left, top);
		printf("����������� ������������ ������.");
		Sleep(1000);
		return 0;
	}

	fwrite(&sel_check_left_pos, sizeof(int), 1, f);
	fwrite(&sel_check_top_pos, sizeof(int), 1, f);
	fwrite(&on, sizeof(int), 1, f);
	fwrite(&checkers_number, sizeof(int), 1, f);
	fwrite(&whose_move, sizeof(int), 1, f);
	fwrite(&cycle_end, sizeof(int), 1, f);
	fwrite(&deleted_checker_left, sizeof(int), 1, f);
	fwrite(&deleted_checker_top, sizeof(int), 1, f);
	fwrite(&eat_more, sizeof(int), 1, f);
	fwrite(&need_to_beat, sizeof(int), 1, f);
	fwrite(&white_check_num, sizeof(int), 1, f);
	fwrite(&red_check_num, sizeof(int), 1, f);
	fwrite(white_checkers, sizeof(checker), 12, f);
	fwrite(red_checkers, sizeof(checker), 12, f);
	fwrite(&game_status, sizeof(int), 1, f);
	fwrite(&complexity, sizeof(int), 1, f);
	fwrite(&white_moves_num, sizeof(int), 1, f);
	fwrite(&red_moves_num, sizeof(int), 1, f);

	fclose(f);

	left = 66;
	top = 15;
	clrscr();
	gotoxy(left, top);
	printf("���� ���������");
	Sleep(1000);
}

// ��������� ����
int load_the_game()
{
	char filename[30];
	int left = 60;
	int top = 15;

	// ������� ������
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	clrscr();
	gotoxy(left, top);

	printf("������� �������� ����������� ����: ");
	top += 2;
	left += 5;
	gotoxy(left, top);
	scanf("%s", &filename);

	FILE* f = fopen(filename, "rb");

	if (f == NULL)
	{
		left = 60;
		top = 15;
		clrscr();
		gotoxy(left, top);
		printf("��������� ���������� �� �������.");
		Sleep(1000);
		return 0;
	}

	fread(&sel_check_left_pos, sizeof(int), 1, f);
	fread(&sel_check_top_pos, sizeof(int), 1, f);
	fread(&on, sizeof(int), 1, f);
	fread(&checkers_number, sizeof(int), 1, f);
	fread(&whose_move, sizeof(int), 1, f);
	fread(&cycle_end, sizeof(int), 1, f);
	fread(&deleted_checker_left, sizeof(int), 1, f);
	fread(&deleted_checker_top, sizeof(int), 1, f);
	fread(&eat_more, sizeof(int), 1, f);
	fread(&need_to_beat, sizeof(int), 1, f);
	fread(&white_check_num, sizeof(int), 1, f);
	fread(&red_check_num, sizeof(int), 1, f);
	fread(white_checkers, sizeof(checker), 12, f);
	fread(red_checkers, sizeof(checker), 12, f);
	fread(&game_status, sizeof(int), 1, f);
	fread(&complexity, sizeof(int), 1, f);
	fread(&white_moves_num, sizeof(int), 1, f);
	fread(&red_moves_num, sizeof(int), 1, f);

	fclose(f);
	return 1;
}

// ������� �������
void reference()
{
	// ������� ������
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	clrscr();
	gotoxy(0, 0);

	printf("								�����.\n");
	printf("				������� ���� :\n");
	printf("		1) ���� ������ �� ����� 8�8 ������, ������ �� ������ �������.\n");
	printf("		2) ����� � ������ ���� �������� ������ ��� ���� � ������ �������.\n");
	printf("		3) ���� ����� ������������ ���������� ����� � ����� ������������.\n");
	printf("		4) ������� ����� ����� ������ �����.\n");
	printf("		5) ������� ����� ����� �������(\"����\", \"����\") �����.\n");
	printf("		6) ����� ����� �� ����� ����� ����� � ����� �������.\n");
	printf("		7) ����������� ���, � ���� �� �������� �����, ���� �����.\n");
	printf("		8) ����� ��������� � ���� ����� ���(����� ��������������� ��� : ���� ����� ������ ������� ������ ��\n");
	printf("		���� ���).\n");
	printf("		9) ���� �����������.\n");
	printf("		10) ����� ������������ � �����, ��������� �������(��� �����) ��� ������(��� ������) ����� �����.\n");
	printf("		11) ���� ����� �� ����� ��� �������� ����� �������� ����, �� ��� ������������ � ����� �\n");
	printf("		��������� ���(���� ��� ��������) ��������� ��� ��� �����.\n\n\n\n");

	printf("				����������:\n");
	printf("		����� ������ � �������� �����.����� ������� ����� � �� ������� �����(�������� ������ �����\n");
	printf("		������ ���� ���� � ������ �����������), ����������� �����(��������), ������� ����� � ����\n");
	printf("		�������, �������� ������.\n");
	printf("		���� ��� ���� ����� ����������, ��������, ������ ��� ���� � �� �� ����� ��������� ���(�������\n");
	printf("		Enter), ���� ���������� \"����\" � ������� ����� � ��������� ������ � ������ ������.");

	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_ESC || code == KEY_ENTER)
		{
			break;
		}
	}
}

// ��� ������� esc ����������� ����
void esc_menu()
{
	int left = 65;
	int top = 15;

	// ������� ������
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	clrscr();

	con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*   ���������� ����   *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*   ��������� ����    *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*   � ������� ����    *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*        �����        *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	left = 65;
	top = 17;
	int position = 0;
	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_UP) // ���� ��� ������� �����
		{
			// �� ������� � �������� ������ (���� ��� ��������)
			if (position > 0)
			{
				switch (position)
				{
				case 1:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   ��������� ����    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   ���������� ����   *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				case 2:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   � ������� ����    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   ��������� ����    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				case 3:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*        �����        *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   � ������� ����    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				}
				position--;
			}
		}
		else if (code == KEY_DOWN) // ���� ������� ����
		{
			// �� ������� � ������� ������ (���� ��� ��������)
			if (position < 3)
			{
				switch (position)
				{
				case 2:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   � ������� ����    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*        �����        *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				case 1:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   ��������� ����    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   � ������� ����    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				case 0:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   ���������� ����   *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   ��������� ����    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				}
				position++;
			}
		}
		else if (code == KEY_ESC) // ESC - �����
		{
			build_board();
			// ������� �����
			return;
		}
		else if (code == KEY_ENTER) // ������ ������ Enter
		{
			if (position == 3) { // ������ ��������� ����� - ��� "�����"
				exit(10);
			}
			if (position == 0) {
				build_board();
				// ������� �����
				return;
			}
			if (position == 1) {
				save_the_game();
				esc_menu();
				// ���������
				return;
			}
			if (position == 2) {
				end = 1;
				cycle_end = 1;
				// � ������� ����
			}
			break;
		}
	}
}

// ����������� ������� �� �����
void move_cursor(int left, int top)
{
	gotoxy(left, top);

	while (1)
	{
		if (cycle_end) {
			break;
		}

		int color = get_cell_color(left, top); // �������� ���� ������, � ������� ������ ������
		if (left != sel_check_left_pos || top != sel_check_top_pos) { // ���� �� ������ ������ �� ������, � ������� ��� ��������� �����, ��� ��������� ������
			blinking_cursor(left, top, color);
		}
		if (key_is_pressed()) // ���� ������ �������
		{
			int code = key_pressed_code(); // �������� ��� ������� �������
			switch (code)
			{
			case KEY_ESC:
				esc_menu();
				// ���������� ����
				break;
			case KEY_UP:
				if (top - 4 > TOP_LIMIT) { // ���� �� ������� �� ������ �����
					top -= 4; // ����������� ����� �� 1 ������
					move_cursor(left, top);
				}
				break;
			case KEY_DOWN:
				if (top + 4 < BOT_LIMIT) { // ���� �� ������� �� ������ �����
					top += 4; // ��������� ���� �� 1 ������
					move_cursor(left, top);
				}
				break;
			case KEY_LEFT:
				if (left - 8 > LEFT_LIMIT) { // ���� �� ������� �� ������ �����
					left -= 8; // ��������� ����� �� 1 ������
					move_cursor(left, top);
				}
				break;
			case KEY_RIGHT:
				if (left + 8 < RIGHT_LIMIT) { // ���� �� ������� �� ������ �����
					left += 8; // ��������� ������ �� 1 ������
					move_cursor(left, top);
				}
				break;
			case KEY_ENTER:
				if (eat_more) {
					eat_more = 0;
					steady_cursor(sel_check_left_pos, sel_check_top_pos, 0);
					sel_check_left_pos = 0;
					sel_check_top_pos = 0;
					cycle_end = 1;
				}
				break;
			case KEY_SPACE: // ����� ������ 
				if (is_there_a_checker(left + 2, top + 1) == whose_move && eat_more == 0) // ���� � ������ ������ ���� ����� (���������/��������� �����)
				{
					if (!on) { // ���� ����� �� ���������
						steady_cursor(left, top, 1); // �������� ����������� ������
						sel_check_left_pos = left; // ���������� ���������� ����������� �������
						sel_check_top_pos = top;
					}
					else if (on)
					{
						if (left == sel_check_left_pos && top == sel_check_top_pos) { // ���� ������ ����� ��� ���������
							steady_cursor(left, top, 0); // ��������� ����������� ������
							sel_check_left_pos = 0; // �������� ���������� ����������� �������
							sel_check_top_pos = 0;
						}
						else { // ��� ����������� �����, ����������� ������
							steady_cursor(sel_check_left_pos, sel_check_top_pos, 0); // ���������� ������ � ������ ����� ��������� �����
							steady_cursor(left, top, 1); // �������� ����������� ������ � ����� ������
							sel_check_left_pos = left; // ���������� ���������� ����������� �������
							sel_check_top_pos = top;
						}
					}
				}
				else if (is_there_a_checker(left + 2, top + 1) == 0)
				{
					if (is_action_permitted(sel_check_left_pos + 2, sel_check_top_pos + 1, left + 2, top + 1) == 1 && eat_more == 0 && need_to_beat == 0) // ���� ������ ��� ��������
					{
						steady_cursor(sel_check_left_pos, sel_check_top_pos, 0); // ��������� ����������� ������
						on = 0; // �������� ���� � ��������� ������������ �������
						step(sel_check_left_pos + 2, sel_check_top_pos + 1, left + 2, top + 1, get_check_color(sel_check_left_pos + 2, sel_check_top_pos + 1), checkers_number); // ������������ ����� � ����� ������
						sel_check_left_pos = 0; // �������� ���������� ����������� �������
						sel_check_top_pos = 0;
						cycle_end = 1;
					}
					else if (is_action_permitted(sel_check_left_pos + 2, sel_check_top_pos + 1, left + 2, top + 1) == 2) // ���� ������� ������
					{
						steady_cursor(sel_check_left_pos, sel_check_top_pos, 0); // ��������� ����������� ������
						on = 0; // �������� ���� � ��������� ������������ �������
						step(sel_check_left_pos + 2, sel_check_top_pos + 1, left + 2, top + 1, get_check_color(sel_check_left_pos + 2, sel_check_top_pos + 1)); // ������������ ����� � ����� ������
						find_and_delete_check(sel_check_left_pos + 2, sel_check_top_pos + 1, left + 2, top + 1); // ����� � ������� ��������� �����
						if (can_eat_more(left + 2, top + 1)) // ���� ����� ������ ���
						{
							eat_more = 1;
							sel_check_left_pos = left;
							sel_check_top_pos = top;
							steady_cursor(sel_check_left_pos, sel_check_top_pos, 1);
							move_cursor(left, top);
						}
						else {
							sel_check_left_pos = 0; // �������� ���������� ����������� �������
							sel_check_top_pos = 0;
							eat_more = 0;
							cycle_end = 1;
						}
					}
				}
				else if (is_there_a_checker(left + 2, top + 1) != whose_move) // ���� �������� ��������� ����� �����
				{
				}

				while (key_is_pressed())
					key_pressed_code();
				break;
			}
		}
	}
}

// ��������� ��������� �������� ������
void get_start_values()
{
	int left_edge = (con_width() / 2) - 21; // ��������� �������� ������� ������ ������ ������������ ������ ���� �������
	int top_edge = (con_height() / 2) - 13; // ��������� �������� ������� ������ ������ ������������ �������� ���� �������

	int left = left_edge;
	int top = top_edge;

	for (int i = 11; i > -1; i--) // ������� �����
	{
		red_checkers[i].status = 0;

		// ����������� ��������� ���������� ������� ������
		red_checkers[i].location[0][0] = left;
		red_checkers[i].location[0][1] = top;

		left += 16; // ��������� � ��������� ������ ������
		if (i == 8) { // ����� ������ ������
			left = left_edge - 8; // ������ ������ ������ ��������� ������
			top += 4;
		}
		else if (i == 4) { // ����� ������ ������
			left = left_edge; // ������ ������ ������ ��������� ������
			top += 4;
		}
	}

	left = left_edge - 8;
	top = top_edge + 28;
	for (int i = 0; i < 12; i++) // ����� �����
	{
		white_checkers[i].status = 0;

		// ����������� ��������� ���������� ����� ������
		white_checkers[i].location[0][0] = left;
		white_checkers[i].location[0][1] = top;

		left += 16; // ��������� � ��������� ������ ������
		if (i == 3) // ����� 6 ������
		{
			left = left_edge; // ������ ������ ������ ��������� ������
			top -= 4;
		}
		else if (i == 7) { // ����� 7 ������
			left = left_edge - 8; // ������ ������ ������ ��������� ������
			top -= 4;
		}
	}
}

// ��� ����������� ����� ������ ����� �������
void my_pause()
{
	key_pressed_code();
	return;
}

// ���������� ������ �����
void white_win()
{
	int top = 25;
	int left = 61;

	gotoxy(left, top);
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	printf("*****************************");

	top++;
	gotoxy(left, top);
	printf("*      �������� �����       *");

	top++;
	gotoxy(left, top);
	printf("*****************************");

	while (!key_pressed_code()) { // ����� �� ������� ����� �������
	}
}

// ���������� ������ �������
void red_win()
{
	int top = 25;
	int left = 61;

	gotoxy(left, top);
	con_set_color(CON_CLR_WHITE, CON_CLR_RED); // ���� ������, ���� �������
	printf("*****************************");

	top++;
	gotoxy(left, top);
	printf("*     �������� �������      *");

	top++;
	gotoxy(left, top);
	printf("*****************************");

	while (!key_pressed_code()) { // ����� �� ������� ����� �������
	}
}

// �������� ������� ��������
void high_score_table_update(int whose_win)
{
	// ������� ������
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	clrscr();
	gotoxy(60, 15);

	char username[50];

	printf("������� ��� ������������: ");
	scanf("%s", &username);

	FILE* f = fopen("table.txt", "a");
	fwrite("\n", sizeof(char), 1, f);
	fwrite(username, sizeof(char), strlen(username), f);
	fwrite(" | ", sizeof(char), 3, f);

	if (whose_win == WHITE)
	{
		fprintf(f, "%d", white_moves_num);
	}
	else if (whose_win == RED)
	{
		fprintf(f, "%d", red_moves_num);
	}

	fclose(f);
}

// ���� �������� � ���������
void human_vs_human()
{
	white_moves_num = 0;
	red_moves_num = 0;
	white_check_num = 12;
	red_check_num = 12;
	if (!is_load) { // ���� �������� ����� ����
		get_start_values(); // ��������� ��������� �������� ������
	}
	build_board(); // ��������� �����
	get_cells_locations(); // �������� ���������� ������

	while (1)
	{
		if (end) // ����� ����
		{
			end = 0;
			break;
		}
		cycle_end = 0; // �������� ���� ��� ����� � ������� move_cursor()

		need_to_beat = necessary_to_beat(); // ��������, ����������� �� ����

		if (white_check_num == 0 || (whose_move == WHITE && !need_to_beat && !no_move()))
		{
			// ������ �������
			red_win();
			high_score_table_update(WHITE);
			break;
		}
		else if (red_check_num == 0 || (whose_move == RED && !need_to_beat && !no_move()))
		{
			// ������ �����
			white_win();
			high_score_table_update(RED);
			break;
		}

		move_cursor(LEFT_BOARD_CENTER, TOP_BOARD_CENTER);

		if (whose_move == RED) { // ���� ��� ��� �������
			whose_move = WHITE; // ��������� ��� �����
		}
		else if (whose_move == WHITE) { // ���� ��� ��� �����
			whose_move = RED; // ��������� ��� �������
		}
	}
}

// ������� ��������� ����� (��� ���� � �����������)
BOARD checker_delete(BOARD board, int left, int top, int color)
{
	if (color == 1) // ����� �����
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.whites[i].location[0][0] == left && board.whites[i].location[0][1] == top && board.whites[i].status != DEAD)
			{
				board.whites[i].status = DEAD;
				break;
			}
		}
	}
	else if (color == 2) // ������� �����
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.reds[i].location[0][0] == left && board.reds[i].location[0][1] == top && board.reds[i].status != DEAD)
			{
				board.reds[i].status = DEAD;
				break;
			}
		}
	}

	return board;
}

// ������, ���� �� � ������ ������ ����� (���������� 1, ���� ���� ����� �����, 2 - ���� �������, ����� 0) (��� ���� � �����������)
int is_here_a_checker(BOARD board, int left, int top)
{
	for (int i = 0; i < 12; i++)
	{
		if (board.whites[i].location[0][0] == left && board.whites[i].location[0][1] == top && board.whites[i].status != 2)
		{
			return 1;
		}
		else if (board.reds[i].location[0][0] == left && board.reds[i].location[0][1] == top && board.reds[i].status != 2)
		{
			return 2;
		}
	}
	return 0;
}

// ����� � ������� ��������� ����� (��� ���� � �����������)
NODE find_and_delete(NODE node, int who_move, int old_left, int old_top, int new_left, int new_top)
{
	int nl, nt, ol, ot;
	nl = new_left; nt = new_top; ol = old_left; ot = old_top;
	if (who_move == 1) // ���� ��� �����, ���� ����� ������� �����	
	{
		while (1)
		{
			if (nl > ol) { // ���� ����� ������ ��������� ������ ������
				nl -= 8; // ���������� �����
			}
			else {
				nl += 8;
			}

			if (nt > ot) { // ���� ����� ������ ��������� ���� ������
				nt -= 4; // ���������� �����
			}
			else {
				nt += 4;
			}

			if (nl == ol || nt == ot) // ���� ��������� � ������, ������ ����������� ���
			{
				break;
			}

			for (int i = 0; i < 12; i++)
			{
				if (node.board.reds[i].location[0][0] == nl && node.board.reds[i].location[0][1] == nt && node.board.reds[i].status != 2)
				{
					node.board.reds[i].status = 2;
					return node;
				}
			}
		}
	}
	else if (who_move == 2) // ���� ��� �������
	{
		while (1)
		{
			if (nl > ol) { // ���� ����� ������ ��������� ������ ������
				nl -= 8; // ���������� �����
			}
			else {
				nl += 8;
			}

			if (nt > ot) { // ���� ����� ������ ��������� ���� ������
				nt -= 4; // ���������� �����
			}
			else {
				nt += 4;
			}

			if (nl == ol || nt == ot) // ���� ��������� � ������, ������ ����������� ���
			{
				break;
			}

			for (int i = 0; i < 12; i++)
			{
				if (node.board.whites[i].location[0][0] == nl && node.board.whites[i].location[0][1] == nt && node.board.whites[i].status != 2)
				{
					node.board.whites[i].status = 2;
					return node;
				}
			}
		}
	}
}

// ��������, ����� �� ������ ��� (���������� 1, ���� �����, ����� 0) (��� ���� � �����������)
MOVE more_eat(BOARD board, int who_move, int left, int top)
{
	MOVE result;
	result.coord[0] = 0; result.coord[1] = 0;
	result.coord[2] = 0; result.coord[3] = 0;

	if (who_move == 0) // ���� ��� �����
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.whites[i].location[0][0] == left && board.whites[i].location[0][1] == top)
			{
				if (board.whites[i].status == 0) {
					if (is_here_a_checker(board, left + 8, top + 4) == RED && (left + 16) < RIGHT_LIMIT + 3 && (top + 8) < BOT_LIMIT + 2) {
						if (is_here_a_checker(board, left + 16, top + 8) == 0) {
							result.coord[0] = left; result.coord[1] = top;
							result.coord[2] = left + 16; result.coord[3] = top + 8;
							return result;
						}
					}
					if (is_here_a_checker(board, left + 8, top - 4) == RED && (left + 16) < RIGHT_LIMIT + 3 && (top - 8) > TOP_LIMIT) {
						if (is_here_a_checker(board, left + 16, top - 8) == 0) {
							result.coord[0] = left; result.coord[1] = top;
							result.coord[2] = left + 16; result.coord[3] = top - 8;
							return result;
						}
					}
					if (is_here_a_checker(board, left - 8, top + 4) == RED && (left - 16) > LEFT_LIMIT + 1 && (top + 8) < BOT_LIMIT + 2) {
						if (is_here_a_checker(board, left - 16, top + 8) == 0) {
							result.coord[0] = left; result.coord[1] = top;
							result.coord[2] = left - 16; result.coord[3] = top + 8;
							return result;
						}
					}
					if (is_here_a_checker(board, left - 8, top - 4) == RED && (left - 16) > LEFT_LIMIT + 1 && (top - 8) > TOP_LIMIT) {
						if (is_here_a_checker(board, left - 16, top - 8) == 0) {
							result.coord[0] = left; result.coord[1] = top;
							result.coord[2] = left - 16; result.coord[3] = top - 8;
							return result;
						}
					}
					break;
				}
				else if (board.whites[i].status == 1) {
					int left_change = 8;
					int top_change = 4;
					int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
					while (1)
					{
						for (int i = 8, j = 4; i <= left_change; i += 8, j = +4)
						{
							if (is_here_a_checker(board, left + i, top + j) == 1 && !flag1) {
								flag1 = 1;
							}
							if (is_here_a_checker(board, left + i, top - j) == 1 && !flag2) {
								flag2 = 1;
							}
							if (is_here_a_checker(board, left - i, top + j) == 1 && !flag3) {
								flag3 = 1;
							}
							if (is_here_a_checker(board, left - i, top - j) == 1 && !flag4) {
								flag4 = 1;
							}
						}
						if (!flag1) {
							if (is_here_a_checker(board, left + left_change, top + top_change) == 2 && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top + top_change + 4) < BOT_LIMIT + 2) {
								if (is_here_a_checker(board, left + left_change + 8, top + top_change + 4) == 0) {
									result.coord[0] = left; result.coord[1] = top;
									result.coord[2] = left + left_change + 8; result.coord[3] = top + top_change + 4;
									return result;
								}
								else {
									flag1 = 1;
								}
							}
						}
						if (!flag2) {
							if (is_here_a_checker(board, left + left_change, top - top_change) == 2 && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top - top_change - 4) > TOP_LIMIT) {
								if (is_here_a_checker(board, left + left_change + 8, top - top_change - 4) == 0) {
									result.coord[0] = left; result.coord[1] = top;
									result.coord[2] = left + left_change + 8; result.coord[3] = top - top_change - 4;
									return result;
								}
								else {
									flag2 = 1;
								}
							}
						}
						if (!flag3) {
							if (is_here_a_checker(board, left - left_change, top + top_change) == 2 && (left - left_change - 8) > LEFT_LIMIT + 1 && (top + top_change + 4) < BOT_LIMIT + 2) {
								if (is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) {
									result.coord[0] = left; result.coord[1] = top;
									result.coord[2] = left - left_change - 8; result.coord[3] = top + top_change + 4;
									return result;
								}
								else {
									flag3 = 1;
								}
							}
						}
						if (!flag4) {
							if (is_here_a_checker(board, left - left_change, top - top_change) == 2 && (left - left_change - 8) > LEFT_LIMIT + 1 && (top - top_change - 4) > TOP_LIMIT) {
								if (is_here_a_checker(board, left - left_change - 8, top - top_change - 4) == 0) {
									result.coord[0] = left; result.coord[1] = top;
									result.coord[2] = left - left_change - 8; result.coord[3] = top - top_change - 4;
									return result;
								}
								else {
									flag4 = 1;
								}
							}
						}

						left_change += 8;
						top_change += 4;

						if (((left + left_change) > RIGHT_LIMIT + 3) && ((left - left_change) < LEFT_LIMIT + 1) && ((top + top_change) > BOT_LIMIT + 2) && ((top - top_change) < TOP_LIMIT))
						{
							break;
						}
					}
					break;
				}
			}
		}
	}
	else if (who_move == 1) // ���� ��� �������
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.reds[i].location[0][0] == left && board.reds[i].location[0][1] == top)
			{
				if (board.reds[i].status == 0) {
					if (is_here_a_checker(board, left + 8, top + 4) == 1 && (left + 16) < RIGHT_LIMIT + 3 && (top + 8) < BOT_LIMIT + 2) {
						if (is_here_a_checker(board, left + 16, top + 8) == 0) {
							result.coord[0] = left; result.coord[1] = top;
							result.coord[2] = left + 16; result.coord[3] = top + 8;
							return result;
						}
					}
					if (is_here_a_checker(board, left + 8, top - 4) == 1 && (left + 16) < RIGHT_LIMIT + 3 && (top - 8) > TOP_LIMIT) {
						if (is_here_a_checker(board, left + 16, top - 8) == 0) {
							result.coord[0] = left; result.coord[1] = top;
							result.coord[2] = left + 16; result.coord[3] = top - 8;
							return result;
						}
					}
					if (is_here_a_checker(board, left - 8, top + 4) == 1 && (left - 16) > LEFT_LIMIT + 1 && (top + 8) < BOT_LIMIT + 2) {
						if (is_here_a_checker(board, left - 16, top + 8) == 0) {
							result.coord[0] = left; result.coord[1] = top;
							result.coord[2] = left - 16; result.coord[3] = top + 8;
							return result;
						}
					}
					if (is_here_a_checker(board, left - 8, top - 4) == 1 && (left - 16) > LEFT_LIMIT + 1 && (top - 8) > TOP_LIMIT) {
						if (is_here_a_checker(board, left - 16, top - 8) == 0) {
							result.coord[0] = left; result.coord[1] = top;
							result.coord[2] = left - 16; result.coord[3] = top - 8;
							return result;
						}
					}
					break;
				}
				else if (board.reds[i].status == 1) {
					int left_change = 8;
					int top_change = 4;
					int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
					while (1)
					{
						for (int i = 8, j = 4; i <= left_change; i += 8, j = +4)
						{
							if (is_here_a_checker(board, left + i, top + j) == 2 && !flag1) {
								flag1 = 1;
							}
							if (is_here_a_checker(board, left + i, top - j) == 2 && !flag2) {
								flag2 = 1;
							}
							if (is_here_a_checker(board, left - i, top + j) == 2 && !flag3) {
								flag3 = 1;
							}
							if (is_here_a_checker(board, left - i, top - j) == 2 && !flag4) {
								flag4 = 1;
							}
						}
						if (!flag1) {
							if (is_here_a_checker(board, left + left_change, top + top_change) == 1 && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top + top_change + 4) < BOT_LIMIT + 2) {
								if (is_here_a_checker(board, left + left_change + 8, top + top_change + 4) == 0) {
									result.coord[0] = left; result.coord[1] = top;
									result.coord[2] = left + left_change + 8; result.coord[3] = top + top_change + 4;
									return result;
								}
								else {
									flag1 = 1;
								}
							}
						}
						if (!flag2) {
							if (is_here_a_checker(board, left + left_change, top - top_change) == 1 && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top - top_change - 4) > TOP_LIMIT) {
								if (is_here_a_checker(board, left + left_change + 8, top - top_change - 4) == 0) {
									result.coord[0] = left; result.coord[1] = top;
									result.coord[2] = left + left_change + 8; result.coord[3] = top - top_change - 4;
									return result;
								}
								else {
									flag2 = 1;
								}
							}
						}
						if (!flag3) {
							if (is_here_a_checker(board, left - left_change, top + top_change) == 1 && (left - left_change - 8) > LEFT_LIMIT + 1 && (top + top_change + 4) < BOT_LIMIT + 2) {
								if (is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) {
									result.coord[0] = left; result.coord[1] = top;
									result.coord[2] = left - left_change - 8; result.coord[3] = top + top_change + 4;
									return result;
								}
								else {
									flag3 = 1;
								}
							}
						}
						if (!flag4) {
							if (is_here_a_checker(board, left - left_change, top - top_change) == 1 && (left - left_change - 8) > LEFT_LIMIT + 1 && (top - top_change - 4) > TOP_LIMIT) {
								if (is_here_a_checker(board, left - left_change - 8, top - top_change - 4) == 0) {
									result.coord[0] = left; result.coord[1] = top;
									result.coord[2] = left - left_change - 8; result.coord[3] = top - top_change - 4;
									return result;
								}
								else {
									flag4 = 1;
								}
							}
						}

						left_change += 8;
						top_change += 4;

						if ((left + left_change) > RIGHT_LIMIT + 3 && (left - left_change) < LEFT_LIMIT + 1 && (top + top_change) > BOT_LIMIT + 2 && (top - top_change) < TOP_LIMIT)
						{
							break;
						}
					}
					break;
				}
			}
		}
	}

	return result;
}

// ��������, ����������� �� ���� ���� (��� ���� � �����������)
int must_beat(BOARD board, int color)
{
	int left;
	int top;

	if (color == 0) // ���� ����� �����
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.whites[i].status == DEFAULT) // �� �����
			{
				left = board.whites[i].location[0][0]; // ���������� ������ �����
				top = board.whites[i].location[0][1];

				if (is_here_a_checker(board, left + 8, top + 4) == RED && (left + 16) < RIGHT_LIMIT + 3 && (top + 8) < BOT_LIMIT + 2) {
					if (is_here_a_checker(board, left + 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_here_a_checker(board, left + 8, top - 4) == RED && (left + 16) < RIGHT_LIMIT + 3 && (top - 8) > TOP_LIMIT) {
					if (is_here_a_checker(board, left + 16, top - 8) == 0) {
						return 1;
					}
				}
				if (is_here_a_checker(board, left - 8, top + 4) == RED && (left - 16) > LEFT_LIMIT + 1 && (top + 8) < BOT_LIMIT + 2) {
					if (is_here_a_checker(board, left - 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_here_a_checker(board, left - 8, top - 4) == RED && (left - 16) > LEFT_LIMIT + 1 && (top - 8) > TOP_LIMIT) {
					if (is_here_a_checker(board, left - 16, top - 8) == 0) {
						return 1;
					}
				}
			}
			else if (board.whites[i].status == QUEEN) // �����
			{
				left = board.whites[i].location[0][0]; // ���������� ������ �����
				top = board.whites[i].location[0][1];

				int left_change = 8;
				int top_change = 4;

				int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
				while (1)
				{
					if (!flag1) {
						if (is_here_a_checker(board, left + left_change, top + top_change) == RED && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_here_a_checker(board, left + left_change + 8, top + top_change + 4) == 0) {
								return 1;
							}
							else {
								flag1 = 1;
							}
						}
					}
					if (!flag2) {
						if (is_here_a_checker(board, left + left_change, top - top_change) == RED && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_here_a_checker(board, left + left_change + 8, top - top_change - 4) == 0) {
								return 1;
							}
							else {
								flag2 = 1;
							}
						}
					}
					if (!flag3) {
						if (is_here_a_checker(board, left - left_change, top + top_change) == RED && (left - left_change - 8) > LEFT_LIMIT + 1 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) {
								return 1;
							}
							else {
								flag3 = 1;
							}
						}
					}
					if (!flag4) {
						if (is_here_a_checker(board, left - left_change, top - top_change) == RED && (left - left_change - 8) > LEFT_LIMIT + 1 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_here_a_checker(board, left - left_change - 8, top - top_change - 4) == 0) {
								return 1;
							}
							else {
								flag4 = 1;
							}
						}
					}

					left_change += 8;
					top_change += 4;

					if (((left + left_change) > RIGHT_LIMIT + 3) && ((left - left_change) < LEFT_LIMIT + 1) && ((top + top_change) > BOT_LIMIT + 2) && ((top - top_change) < TOP_LIMIT))
					{
						break;
					}
				}
			}
		}
	}
	else if (color == 1) // ���� ����� �������
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.reds[i].status == DEFAULT) // �� �����
			{
				left = board.reds[i].location[0][0];
				top = board.reds[i].location[0][1];
				if (is_here_a_checker(board, left + 8, top + 4) == WHITE && (left + 16) < RIGHT_LIMIT + 3 && (top + 8) < BOT_LIMIT + 2) {
					if (is_here_a_checker(board, left + 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_here_a_checker(board, left + 8, top - 4) == WHITE && (left + 16) < RIGHT_LIMIT + 3 && (top - 8) > TOP_LIMIT) {
					if (is_here_a_checker(board, left + 16, top - 8) == 0) {
						return 1;
					}
				}
				if (is_here_a_checker(board, left - 8, top + 4) == WHITE && (left - 16) > LEFT_LIMIT + 1 && (top + 8) < BOT_LIMIT + 2) {
					if (is_here_a_checker(board, left - 16, top + 8) == 0) {
						return 1;
					}
				}
				if (is_here_a_checker(board, left - 8, top - 4) == WHITE && (left - 16) > LEFT_LIMIT + 1 && (top - 8) > TOP_LIMIT) {
					if (is_here_a_checker(board, left - 16, top - 8) == 0) {
						return 1;
					}
				}
			}
			else if (board.reds[i].status == QUEEN) // �����
			{
				left = board.reds[i].location[0][0]; // ���������� ������ �����
				top = board.reds[i].location[0][1];

				int left_change = 8;
				int top_change = 4;
				int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
				while (1)
				{
					if (!flag1) {
						if (is_here_a_checker(board, left + left_change, top + top_change) == WHITE && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_here_a_checker(board, left + left_change + 8, top + top_change + 4) == 0) {
								return 1;
							}
							else {
								flag1 = 1;
							}
						}
					}
					if (!flag2) {
						if (is_here_a_checker(board, left + left_change, top - top_change) == WHITE && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_here_a_checker(board, left + left_change + 8, top - top_change - 4) == 0) {
								return 1;
							}
							else {
								flag2 = 1;
							}
						}
					}
					if (!flag3) {
						if (is_here_a_checker(board, left - left_change, top + top_change) == WHITE && (left - left_change - 8) > LEFT_LIMIT + 1 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) {
								return 1;
							}
							else {
								flag3 = 1;
							}
						}
					}
					if (!flag4) {
						if (is_here_a_checker(board, left - left_change, top - top_change) == WHITE && (left - left_change - 8) > LEFT_LIMIT + 1 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_here_a_checker(board, left - left_change - 8, top - top_change - 4) == 0) {
								return 1;
							}
							else {
								flag4 = 1;
							}
						}
					}

					left_change += 8;
					top_change += 4;

					if ((left + left_change) > RIGHT_LIMIT + 3 && (left - left_change) < LEFT_LIMIT + 1 && (top + top_change) > BOT_LIMIT + 2 && (top - top_change) < TOP_LIMIT)
					{
						break;
					}
				}
			}
		}
	}
	return 0;
}

// �������� ���������� �����
void update_checker_coord(NODE move)
{
	if (move.move.color == 0)
	{
		white_checkers[move.move.checker_num].location[0][0] = move.move.coord[2];
		white_checkers[move.move.checker_num].location[0][1] = move.move.coord[3];
	}
	else if (move.move.color == 1)
	{
		red_checkers[move.move.checker_num].location[0][0] = move.move.coord[2];
		red_checkers[move.move.checker_num].location[0][1] = move.move.coord[3];
	}
}

// ������� ����� �����
BOARD board_copy(BOARD board)
{
	BOARD result;
	for (int i = 0; i < 12; i++)
	{
		result.whites[i] = board.whites[i];
		result.reds[i] = board.reds[i];
	}

	return result;
}

// ��������, ������� �� ����� (���������� 1 - ���� �����, 2 - ���� ����� �����, ����� 0) (��� ���� � �����������)
int check_hit(BOARD board, MOVE move)
{
	int left_change = 8;
	int top_change = 4;

	if (move.color == 1) // ����� �����
	{
		int left = move.coord[0];
		int top = move.coord[1];

		if (move.coord[2] > move.coord[0]) // ���� ����� ������ ������ ������
		{
			if (move.coord[3] > move.coord[1]) // ���� ����� ������ ���� ������
			{
				while (left + left_change < move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.reds[i].location[0][0] == left + left_change && board.reds[i].location[0][1] == top + top_change) {
							if (board.reds[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // ���� ����� ������������� �����
									if (left + left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left + left_change - 8, top + top_change + 4) == 0) { // ���� ����� ������ ������ ������
										return 2;
									}
									if (left + left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left + left_change + 8, top + top_change + 4) == 0) { // ���� ������ ������ ������ ������
										return 2;
									}
								}
								return 1; // ���� ����� ������� �����
							}
							else if (board.reds[i].status == 1) { // ���� ����� �����
								return 2;
							}
						}
					}
					left_change += 8;
					top_change += 4;
				}
			}
			else // ���� ����� ������ ���� ������
			{
				while (left + left_change < move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.reds[i].location[0][0] == left + left_change && board.reds[i].location[0][1] == top - top_change) {
							if (board.reds[i].status == 0) {
								if (top - top_change + 4 == BOT_LIMIT) { // ���� ����� ������������� �����
									if (left + left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left + left_change - 8, top - top_change + 4) == 0) { // ���� ����� ������ ������ ������
										return 2;
									}
									if (left + left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left + left_change + 8, top - top_change + 4) == 0) { // ���� ������ ������ ������ ������
										return 2;
									}
								}
								return 1;
							}
							else if (board.reds[i].status == 1) {
								return 2;
							}
						}
					}
					left_change += 8;
					top_change += 4;
				}
			}
		}
		else // ���� ����� ������ ����� ������
		{
			int left = move.coord[0];
			int top = move.coord[1];

			if (move.coord[3] > move.coord[1]) // ���� ����� ������ ���� ������
			{
				while (left - left_change > move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.reds[i].location[0][0] == left - left_change && board.reds[i].location[0][1] == top + top_change) {
							if (board.reds[i].status == 0) { // �� �����
								if (top + top_change + 4 == BOT_LIMIT) { // ���� ����� ������������� �����
									if (left - left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) { // ���� ����� ������ ������ ������
										return 2;
									}
									if (left - left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left - left_change + 8, top + top_change + 4) == 0) { // ���� ������ ������ ������ ������
										return 2;
									}
								}
								return 1;
							}
							else if (board.reds[i].status == 1) { // �����
								return 2;
							}
						}
					}
					left_change += 8;
					top_change += 4;
				}
			}
			else // ���� ����� ������ ���� ������
			{
				while (left - left_change > move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.reds[i].location[0][0] == left - left_change && board.reds[i].location[0][1] == top - top_change) {
							if (board.reds[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // ���� ����� ������������� �����
									if (left - left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left - left_change - 8, top - top_change + 4) == 0) { // ���� ����� ������ ������ ������
										return 2;
									}
									if (left - left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left - left_change + 8, top - top_change + 4) == 0) { // ���� ������ ������ ������ ������
										return 2;
									}
								}
								return 1;
							}
							else if (board.reds[i].status == 1) {
								return 2;
							}
						}
					}
					left_change += 8;
					top_change += 4;
				}
			}
		}
	}
	else if (move.color == 2) // ������� �����
	{
		int left = move.coord[0];
		int top = move.coord[1];

		if (move.coord[2] > move.coord[0]) // ���� ����� ������ ������ ������
		{
			if (move.coord[3] > move.coord[1]) // ���� ����� ������ ���� ������
			{
				while (left + left_change < move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.whites[i].location[0][0] == left + left_change && board.whites[i].location[0][1] == top + top_change) {
							if (board.whites[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // ���� ����� ������������� �����
									if (left + left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left + left_change - 8, top + top_change + 4) == 0) { // ���� ����� ������ ������ ������
										return 2;
									}
									if (left + left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left + left_change + 8, top + top_change + 4) == 0) { // ���� ������ ������ ������ ������
										return 2;
									}
								}
								return 1;
							}
							else if (board.whites[i].status == 1) {
								return 2;
							}
						}
					}
					left_change += 8;
					top_change += 4;
				}
			}
			else // ���� ����� ������ ���� ������
			{
				while (left + left_change < move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.whites[i].location[0][0] == left + left_change && board.whites[i].location[0][1] == top - top_change) {
							if (board.whites[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // ���� ����� ������������� �����
									if (left + left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left + left_change - 8, top - top_change + 4) == 0) { // ���� ����� ������ ������ ������
										return 2;
									}
									if (left + left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left + left_change + 8, top - top_change + 4) == 0) { // ���� ������ ������ ������ ������
										return 2;
									}
								}
								return 1;
							}
							else if (board.whites[i].status == 1) {
								return 2;
							}
						}
					}
					left_change += 8;
					top_change += 4;
				}
			}
		}
		else // ���� ����� ������ ����� ������
		{
			int left = move.coord[0];
			int top = move.coord[1];

			if (move.coord[3] > move.coord[1]) // ���� ����� ������ ���� ������
			{
				while (left - left_change > move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.whites[i].location[0][0] == left - left_change && board.whites[i].location[0][1] == top + top_change) {
							if (board.whites[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // ���� ����� ������������� �����
									if (left - left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) { // ���� ����� ������ ������ ������
										return 2;
									}
									if (left - left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left - left_change + 8, top + top_change + 4) == 0) { // ���� ������ ������ ������ ������
										return 2;
									}
								}
								return 1;
							}
							else if (board.whites[i].status == 1) {
								return 2;
							}
						}
					}
					left_change += 8;
					top_change += 4;
				}
			}
			else // ���� ����� ������ ���� ������
			{
				while (left - left_change > move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.whites[i].location[0][0] == left - left_change && board.whites[i].location[0][1] == top - top_change) {
							if (board.whites[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // ���� ����� ������������� �����
									if (left - left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) { // ���� ����� ������ ������ ������
										return 2;
									}
									if (left - left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left - left_change + 8, top + top_change + 4) == 0) { // ���� ������ ������ ������ ������
										return 2;
									}
								}
								return 1;
							}
							else if (board.whites[i].status == 1) {
								return 2;
							}
						}
					}
					left_change += 8;
					top_change += 4;
				}
			}
		}
	}
	return 0;
}

int now_size = 0;
// ������������� ���� ��� �����
NODE* find_whites_moves(BOARD board, int beat)
{
	NODE* result = NULL;
	int size = 0;

	if (beat) // ���� ����������� ���� ����
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.whites[i].status == 0) // �� �����
			{
				int left = board.whites[i].location[0][0];
				int top = board.whites[i].location[0][1];

				if (is_here_a_checker(board, left + 8, top + 4) == RED && (left + 16) < RIGHT_LIMIT + 3 && (top + 8) < BOT_LIMIT + 2) {
					if (is_here_a_checker(board, left + 16, top + 8) == 0) {
						size++;
						result = (NODE*)realloc(result, size * sizeof(NODE));
						(result + size - 1)->board = board_copy(board);
						(result + size - 1)->board = checker_delete(board, left + 8, top + 4, RED);
						result[size - 1].board.whites[i].location[0][0] = left + 16;
						result[size - 1].board.whites[i].location[0][1] = top + 8;
						(result + size - 1)->move.checker_num = i;
						(result + size - 1)->move.color = 0;
						(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
						(result + size - 1)->move.coord[2] = left + 16; (result + size - 1)->move.coord[3] = top + 8;
						MOVE more_move = more_eat(result[size - 1].board, 0, left + 16, top + 8);
						while (more_move.coord[0] != 0) // ���� ����� ������ ���
						{
							result[size - 1] = find_and_delete(result[size - 1], 1, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
							(result + size - 1)->move.coord[2] = more_move.coord[2];
							(result + size - 1)->move.coord[3] = more_move.coord[3];
							result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
							result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
							more_move = more_eat(result[size - 1].board, 0, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
						}
					}
				}
				if (is_here_a_checker(board, left + 8, top - 4) == RED && (left + 16) < RIGHT_LIMIT + 3 && (top - 8) > TOP_LIMIT) {
					if (is_here_a_checker(board, left + 16, top - 8) == 0) {
						size++;
						result = (NODE*)realloc(result, size * sizeof(NODE));
						(result + size - 1)->board = board_copy(board);
						(result + size - 1)->board = checker_delete(board, left + 8, top - 4, RED);
						result[size - 1].board.whites[i].location[0][0] = left + 16;
						result[size - 1].board.whites[i].location[0][1] = top - 8;
						(result + size - 1)->move.checker_num = i;
						(result + size - 1)->move.color = 0;
						(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
						(result + size - 1)->move.coord[2] = left + 16; (result + size - 1)->move.coord[3] = top - 8;
						MOVE more_move = more_eat(result[size - 1].board, 0, left + 16, top - 8);
						while (more_move.coord[0] != 0) // ���� ����� ������ ���
						{
							result[size - 1] = find_and_delete(result[size - 1], 1, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
							(result + size - 1)->move.coord[2] = more_move.coord[2];
							(result + size - 1)->move.coord[3] = more_move.coord[3];
							result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
							result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
							more_move = more_eat(result[size - 1].board, 0, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
						}
					}
				}
				if (is_here_a_checker(board, left - 8, top + 4) == RED && (left - 16) > LEFT_LIMIT + 1 && (top + 8) < BOT_LIMIT + 2) {
					if (is_here_a_checker(board, left - 16, top + 8) == 0) {
						size++;
						result = (NODE*)realloc(result, size * sizeof(NODE));
						(result + size - 1)->board = board_copy(board);
						(result + size - 1)->board = checker_delete(board, left - 8, top + 4, RED);
						result[size - 1].board.whites[i].location[0][0] = left - 16;
						result[size - 1].board.whites[i].location[0][1] = top + 8;
						(result + size - 1)->move.checker_num = i;
						(result + size - 1)->move.color = 0;
						(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
						(result + size - 1)->move.coord[2] = left - 16; (result + size - 1)->move.coord[3] = top + 8;
						MOVE more_move = more_eat(result[size - 1].board, 0, left - 16, top + 8);
						while (more_move.coord[0] != 0) // ���� ����� ������ ���
						{
							result[size - 1] = find_and_delete(result[size - 1], 1, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
							(result + size - 1)->move.coord[2] = more_move.coord[2];
							(result + size - 1)->move.coord[3] = more_move.coord[3];
							result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
							result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
							more_move = more_eat(result[size - 1].board, 0, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
						}
					}
				}
				if (is_here_a_checker(board, left - 8, top - 4) == RED && (left - 16) > LEFT_LIMIT + 1 && (top - 8) > TOP_LIMIT) {
					if (is_here_a_checker(board, left - 16, top - 8) == 0) {
						size++;
						result = (NODE*)realloc(result, size * sizeof(NODE));
						(result + size - 1)->board = board_copy(board);
						(result + size - 1)->board = checker_delete(board, left - 8, top - 4, RED);
						result[size - 1].board.whites[i].location[0][0] = left - 16;
						result[size - 1].board.whites[i].location[0][1] = top - 8;
						(result + size - 1)->move.checker_num = i;
						(result + size - 1)->move.color = 0;
						(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
						(result + size - 1)->move.coord[2] = left - 16; (result + size - 1)->move.coord[3] = top - 8;
						MOVE more_move = more_eat(result[size - 1].board, 0, left - 16, top - 8);
						while (more_move.coord[0] != 0) // ���� ����� ������ ���
						{
							result[size - 1] = find_and_delete(result[size - 1], 1, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
							(result + size - 1)->move.coord[2] = more_move.coord[2];
							(result + size - 1)->move.coord[3] = more_move.coord[3];
							result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
							result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
							more_move = more_eat(result[size - 1].board, 0, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
						}
					}
				}
			}
			else if (board.whites[i].status == 1) // �����
			{
				int left = board.whites[i].location[0][0];
				int top = board.whites[i].location[0][1];

				int left_change = 8;
				int top_change = 4;
				int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
				while (1)
				{
					if (!flag1)
					{
						if (is_here_a_checker(board, left + left_change, top + top_change) == RED && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_here_a_checker(board, left + left_change + 8, top + top_change + 4) == 0) {
								size++;
								result = (NODE*)realloc(result, size * sizeof(NODE));
								(result + size - 1)->board = board_copy(board);
								(result + size - 1)->board = checker_delete(board, left + left_change, top + top_change, RED);
								result[size - 1].board.whites[i].location[0][0] = left + left_change + 8;
								result[size - 1].board.whites[i].location[0][1] = top + top_change + 4;
								(result + size - 1)->move.checker_num = i;
								(result + size - 1)->move.color = 0;
								(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
								(result + size - 1)->move.coord[2] = left + left_change + 8; (result + size - 1)->move.coord[3] = top + top_change + 4;
								MOVE more_move = more_eat(result[size - 1].board, 0, left + left_change + 8, top + top_change + 4);
								while (more_move.coord[0] != 0) // ���� ����� ������ ���
								{
									result[size - 1] = find_and_delete(result[size - 1], 1, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
									(result + size - 1)->move.coord[2] = more_move.coord[2];
									(result + size - 1)->move.coord[3] = more_move.coord[3];
									result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
									result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
									more_move = more_eat(result[size - 1].board, 0, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
								}
							}
							else {
								flag1 = 1;
							}
						}
					}
					if (!flag2)
					{
						if (is_here_a_checker(board, left + left_change, top - top_change) == RED && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_here_a_checker(board, left + left_change + 8, top - top_change - 4) == 0) {
								size++;
								result = (NODE*)realloc(result, size * sizeof(NODE));
								(result + size - 1)->board = board_copy(board);
								(result + size - 1)->board = checker_delete(board, left + left_change, top - top_change, RED);
								result[size - 1].board.whites[i].location[0][0] = left + left_change + 8;
								result[size - 1].board.whites[i].location[0][1] = top - top_change - 4;
								(result + size - 1)->move.checker_num = i;
								(result + size - 1)->move.color = 0;
								(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
								(result + size - 1)->move.coord[2] = left + left_change + 8; (result + size - 1)->move.coord[3] = top - top_change - 4;
								MOVE more_move = more_eat(result[size - 1].board, 0, left + left_change + 8, top - top_change - 4);
								while (more_move.coord[0] != 0) // ���� ����� ������ ���
								{
									result[size - 1] = find_and_delete(result[size - 1], 1, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
									(result + size - 1)->move.coord[2] = more_move.coord[2];
									(result + size - 1)->move.coord[3] = more_move.coord[3];
									result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
									result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
									more_move = more_eat(result[size - 1].board, 0, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
								}
							}
							else {
								flag2 = 1;
							}
						}
					}
					if (!flag3)
					{
						if (is_here_a_checker(board, left - left_change, top + top_change) == RED && (left - left_change - 8) > LEFT_LIMIT + 1 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) {
								size++;
								result = (NODE*)realloc(result, size * sizeof(NODE));
								(result + size - 1)->board = board_copy(board);
								(result + size - 1)->board = checker_delete(board, left - left_change, top + top_change, RED);
								result[size - 1].board.whites[i].location[0][0] = left - left_change - 8;
								result[size - 1].board.whites[i].location[0][1] = top + top_change + 4;
								(result + size - 1)->move.checker_num = i;
								(result + size - 1)->move.color = 0;
								(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
								(result + size - 1)->move.coord[2] = left - left_change - 8; (result + size - 1)->move.coord[3] = top + top_change + 4;
								MOVE more_move = more_eat(result[size - 1].board, 0, left - left_change - 8, top + top_change + 4);
								while (more_move.coord[0] != 0) // ���� ����� ������ ���
								{
									result[size - 1] = find_and_delete(result[size - 1], 1, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
									(result + size - 1)->move.coord[2] = more_move.coord[2];
									(result + size - 1)->move.coord[3] = more_move.coord[3];
									result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
									result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
									more_move = more_eat(result[size - 1].board, 0, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
								}
							}
							else {
								flag3 = 1;
							}
						}
					}
					if (!flag4)
					{
						if (is_here_a_checker(board, left - left_change, top - top_change) == RED && (left - left_change - 8) > LEFT_LIMIT + 1 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_here_a_checker(board, left - left_change - 8, top - top_change - 4) == 0) {
								size++;
								result = (NODE*)realloc(result, size * sizeof(NODE));
								(result + size - 1)->board = board_copy(board);
								(result + size - 1)->board = checker_delete(board, left - left_change, top - top_change, RED);
								result[size - 1].board.whites[i].location[0][0] = left - left_change - 8;
								result[size - 1].board.whites[i].location[0][1] = top - top_change - 4;
								(result + size - 1)->move.checker_num = i;
								(result + size - 1)->move.color = 0;
								(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
								(result + size - 1)->move.coord[2] = left - left_change - 8; (result + size - 1)->move.coord[3] = top - top_change - 4;
								MOVE more_move = more_eat(result[size - 1].board, 0, left - left_change - 8, top - top_change - 4);
								while (more_move.coord[0] != 0) // ���� ����� ������ ���
								{
									result[size - 1] = find_and_delete(result[size - 1], 1, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
									(result + size - 1)->move.coord[2] = more_move.coord[2];
									(result + size - 1)->move.coord[3] = more_move.coord[3];
									result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
									result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
									more_move = more_eat(result[size - 1].board, 0, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
								}
							}
							else {
								flag4 = 1;
							}
						}
					}

					left_change += 8;
					top_change += 4;

					if ((left + left_change) > RIGHT_LIMIT + 3 && (left - left_change) < LEFT_LIMIT + 1 && (top + top_change) > BOT_LIMIT + 2 && (top - top_change) < TOP_LIMIT)
					{
						break;
					}
				}
			}
		}
	}
	else // ������� ���
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.whites[i].status == 0) // �� �����
			{
				int left = board.whites[i].location[0][0];
				int top = board.whites[i].location[0][1];

				if (is_here_a_checker(board, left + 8, top - 4) == 0 && (left + 8) < RIGHT_LIMIT + 3 && (top - 4) > TOP_LIMIT) {
					size++;
					result = (NODE*)realloc(result, size * sizeof(NODE));
					(result + size - 1)->board = board_copy(board);
					result[size - 1].board.whites[i].location[0][0] = left + 8;
					result[size - 1].board.whites[i].location[0][1] = top - 4;
					(result + size - 1)->move.checker_num = i;
					(result + size - 1)->move.color = 0;
					(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
					(result + size - 1)->move.coord[2] = left + 8; (result + size - 1)->move.coord[3] = top - 4;
				}
				if (is_here_a_checker(board, left - 8, top - 4) == 0 && (left - 8) > LEFT_LIMIT + 1 && (top - 4) > TOP_LIMIT) {
					size++;
					result = (NODE*)realloc(result, size * sizeof(NODE));
					(result + size - 1)->board = board_copy(board);
					result[size - 1].board.whites[i].location[0][0] = left - 8;
					result[size - 1].board.whites[i].location[0][1] = top - 4;
					(result + size - 1)->move.checker_num = i;
					(result + size - 1)->move.color = 0;
					(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
					(result + size - 1)->move.coord[2] = left - 8; (result + size - 1)->move.coord[3] = top - 4;
				}
			}
			else if (board.whites[i].status == 1) // �����
			{
				int left = board.whites[i].location[0][0];
				int top = board.whites[i].location[0][1];

				int left_change = 8;
				int top_change = 4;
				int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
				while (1)
				{
					if (!flag1)
					{
						if (is_here_a_checker(board, left + left_change, top + top_change) == 0 && (left + left_change) < RIGHT_LIMIT + 3 && (top + top_change) < BOT_LIMIT + 2) {
							size++;
							result = (NODE*)realloc(result, size * sizeof(NODE));
							(result + size - 1)->board = board_copy(board);
							result[size - 1].board.whites[i].location[0][0] = left + left_change;
							result[size - 1].board.whites[i].location[0][1] = top + top_change;
							(result + size - 1)->move.checker_num = i;
							(result + size - 1)->move.color = 0;
							(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
							(result + size - 1)->move.coord[2] = left + left_change; (result + size - 1)->move.coord[3] = top + top_change;
						}
						else {
							flag1 = 1;
						}
					}
					if (!flag2)
					{
						if (is_here_a_checker(board, left + left_change, top - top_change) == 0 && (left + left_change) < RIGHT_LIMIT + 3 && (top - top_change) > TOP_LIMIT) {
							size++;
							result = (NODE*)realloc(result, size * sizeof(NODE));
							(result + size - 1)->board = board_copy(board);
							result[size - 1].board.whites[i].location[0][0] = left + left_change;
							result[size - 1].board.whites[i].location[0][1] = top - top_change;
							(result + size - 1)->move.checker_num = i;
							(result + size - 1)->move.color = 0;
							(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
							(result + size - 1)->move.coord[2] = left + left_change; (result + size - 1)->move.coord[3] = top - top_change;
						}
						else {
							flag2 = 1;
						}
					}
					if (!flag3)
					{
						if (is_here_a_checker(board, left - left_change, top + top_change) == 1 && (left - left_change) > LEFT_LIMIT + 1 && (top + top_change) < BOT_LIMIT + 2) {
							size++;
							result = (NODE*)realloc(result, size * sizeof(NODE));
							(result + size - 1)->board = board_copy(board);
							result[size - 1].board.whites[i].location[0][0] = left - left_change;
							result[size - 1].board.whites[i].location[0][1] = top + top_change;
							(result + size - 1)->move.checker_num = i;
							(result + size - 1)->move.color = 0;
							(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
							(result + size - 1)->move.coord[2] = left - left_change; (result + size - 1)->move.coord[3] = top + top_change;
						}
						else {
							flag3 = 1;
						}
					}
					if (!flag4)
					{
						if (is_here_a_checker(board, left - left_change, top - top_change) == 0 && (left - left_change) > LEFT_LIMIT + 1 && (top - top_change) > TOP_LIMIT) {
							size++;
							result = (NODE*)realloc(result, size * sizeof(NODE));
							(result + size - 1)->board = board_copy(board);
							result[size - 1].board.whites[i].location[0][0] = left - left_change;
							result[size - 1].board.whites[i].location[0][1] = top - top_change;
							(result + size - 1)->move.checker_num = i;
							(result + size - 1)->move.color = 0;
							(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
							(result + size - 1)->move.coord[2] = left - left_change; (result + size - 1)->move.coord[3] = top - top_change;
						}
						else {
							flag4 = 1;
						}
					}

					left_change += 8;
					top_change += 4;

					if ((left + left_change) > RIGHT_LIMIT + 3 && (left - left_change) < LEFT_LIMIT + 1 && (top + top_change) > BOT_LIMIT + 2 && (top - top_change) < TOP_LIMIT)
					{
						break;
					}
				}
			}
		}
	}

	now_size = size;
	return result;
}

// ������������� ���� ��� �������
NODE* find_reds_moves(BOARD board, int beat)
{
	NODE* result = NULL;
	int size = 0;

	if (beat) // ���� ����������� ���� ����
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.reds[i].status == 0) // �� �����
			{
				int left = board.reds[i].location[0][0];
				int top = board.reds[i].location[0][1];

				if (is_here_a_checker(board, left + 8, top + 4) == WHITE && (left + 16) < RIGHT_LIMIT + 3 && (top + 8) < BOT_LIMIT + 2) {
					if (is_here_a_checker(board, left + 16, top + 8) == 0) {
						size++;
						result = (NODE*)realloc(result, size * sizeof(NODE));
						(result + size - 1)->board = board_copy(board);
						(result + size - 1)->board = checker_delete(board, left + 8, top + 4, WHITE);
						result[size - 1].board.reds[i].location[0][0] = left + 16;
						result[size - 1].board.reds[i].location[0][1] = top + 8;
						(result + size - 1)->move.checker_num = i;
						(result + size - 1)->move.color = 1;
						(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
						(result + size - 1)->move.coord[2] = left + 16; (result + size - 1)->move.coord[3] = top + 8;
						MOVE more_move = more_eat(result[size - 1].board, 1, left + 16, top + 8);
						while (more_move.coord[0] != 0) // ���� ����� ������ ���
						{
							result[size - 1] = find_and_delete(result[size - 1], 2, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
							(result + size - 1)->move.coord[2] = more_move.coord[2];
							(result + size - 1)->move.coord[3] = more_move.coord[3];
							result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
							result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
							more_move = more_eat(result[size - 1].board, 1, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
						}
					}
				}
				if (is_here_a_checker(board, left + 8, top - 4) == WHITE && (left + 16) < RIGHT_LIMIT + 3 && (top - 8) > TOP_LIMIT) {
					if (is_here_a_checker(board, left + 16, top - 8) == 0) {
						size++;
						result = (NODE*)realloc(result, size * sizeof(NODE));
						(result + size - 1)->board = board_copy(board);
						(result + size - 1)->board = checker_delete(board, left + 8, top - 4, WHITE);
						result[size - 1].board.reds[i].location[0][0] = left + 16;
						result[size - 1].board.reds[i].location[0][1] = top - 8;
						(result + size - 1)->move.checker_num = i;
						(result + size - 1)->move.color = 1;
						(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
						(result + size - 1)->move.coord[2] = left + 16; (result + size - 1)->move.coord[3] = top - 8;
						MOVE more_move = more_eat(result[size - 1].board, 1, left + 16, top + 8);
						while (more_move.coord[0] != 0) // ���� ����� ������ ���
						{
							result[size - 1] = find_and_delete(result[size - 1], 2, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
							(result + size - 1)->move.coord[2] = more_move.coord[2];
							(result + size - 1)->move.coord[3] = more_move.coord[3];
							result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
							result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
							more_move = more_eat(result[size - 1].board, 1, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
						}
					}
				}
				if (is_here_a_checker(board, left - 8, top + 4) == WHITE && (left - 16) > LEFT_LIMIT + 1 && (top + 8) < BOT_LIMIT + 2) {
					if (is_here_a_checker(board, left - 16, top + 8) == 0) {
						size++;
						result = (NODE*)realloc(result, size * sizeof(NODE));
						(result + size - 1)->board = board_copy(board);
						(result + size - 1)->board = checker_delete(board, left - 8, top + 4, WHITE);
						result[size - 1].board.reds[i].location[0][0] = left - 16;
						result[size - 1].board.reds[i].location[0][1] = top + 8;
						(result + size - 1)->move.checker_num = i;
						(result + size - 1)->move.color = 1;
						(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
						(result + size - 1)->move.coord[2] = left - 16; (result + size - 1)->move.coord[3] = top + 8;
						MOVE more_move = more_eat(result[size - 1].board, 1, left - 16, top + 8);
						while (more_move.coord[0] != 0) // ���� ����� ������ ���
						{
							result[size - 1] = find_and_delete(result[size - 1], 2, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
							(result + size - 1)->move.coord[2] = more_move.coord[2];
							(result + size - 1)->move.coord[3] = more_move.coord[3];
							result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
							result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
							more_move = more_eat(result[size - 1].board, 1, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
						}
					}
				}
				if (is_here_a_checker(board, left - 8, top - 4) == WHITE && (left - 16) > LEFT_LIMIT + 1 && (top - 8) > TOP_LIMIT) {
					if (is_here_a_checker(board, left - 16, top - 8) == 0) {
						size++;
						result = (NODE*)realloc(result, size * sizeof(NODE));
						(result + size - 1)->board = board_copy(board);
						(result + size - 1)->board = checker_delete(board, left - 8, top - 4, WHITE);
						result[size - 1].board.reds[i].location[0][0] = left - 16;
						result[size - 1].board.reds[i].location[0][1] = top - 8;
						(result + size - 1)->move.checker_num = i;
						(result + size - 1)->move.color = 1;
						(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
						(result + size - 1)->move.coord[2] = left - 16; (result + size - 1)->move.coord[3] = top - 8;
						MOVE more_move = more_eat(result[size - 1].board, 1, left + 16, top + 8);
						while (more_move.coord[0] != 0) // ���� ����� ������ ���
						{
							result[size - 1] = find_and_delete(result[size - 1], 2, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
							(result + size - 1)->move.coord[2] = more_move.coord[2];
							(result + size - 1)->move.coord[3] = more_move.coord[3];
							result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
							result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
							more_move = more_eat(result[size - 1].board, 1, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
						}
					}
				}
			}
			else if (board.reds[i].status == 1) // �����
			{
				int left = board.reds[i].location[0][0];
				int top = board.reds[i].location[0][1];

				int left_change = 8;
				int top_change = 4;
				int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
				while (1)
				{
					if (!flag1)
					{
						if (is_here_a_checker(board, left + left_change, top + top_change) == WHITE && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_here_a_checker(board, left + left_change + 8, top + top_change + 4) == 0) {
								size++;
								result = (NODE*)realloc(result, size * sizeof(NODE));
								(result + size - 1)->board = board_copy(board);
								(result + size - 1)->board = checker_delete(board, left + left_change, top + top_change, WHITE);
								result[size - 1].board.reds[i].location[0][0] = left + left_change + 8;
								result[size - 1].board.reds[i].location[0][1] = top + top_change + 4;
								(result + size - 1)->move.checker_num = i;
								(result + size - 1)->move.color = 1;
								(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
								(result + size - 1)->move.coord[2] = left + left_change + 8; (result + size - 1)->move.coord[3] = top + top_change + 4;
								MOVE more_move = more_eat(result[size - 1].board, 1, left + 16, top + 8);
								while (more_move.coord[0] != 0) // ���� ����� ������ ���
								{
									result[size - 1] = find_and_delete(result[size - 1], 2, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
									(result + size - 1)->move.coord[2] = more_move.coord[2];
									(result + size - 1)->move.coord[3] = more_move.coord[3];
									result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
									result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
									more_move = more_eat(result[size - 1].board, 1, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
								}
							}
							else {
								flag1 = 1;
							}
						}
					}
					if (!flag2)
					{
						if (is_here_a_checker(board, left + left_change, top - top_change) == WHITE && (left + left_change + 8) < RIGHT_LIMIT + 3 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_here_a_checker(board, left + left_change + 8, top - top_change - 4) == 0) {
								size++;
								result = (NODE*)realloc(result, size * sizeof(NODE));
								(result + size - 1)->board = board_copy(board);
								(result + size - 1)->board = checker_delete(board, left + left_change, top - top_change, WHITE);
								result[size - 1].board.reds[i].location[0][0] = left + left_change + 8;
								result[size - 1].board.reds[i].location[0][1] = top - top_change - 4;
								(result + size - 1)->move.checker_num = i;
								(result + size - 1)->move.color = 1;
								(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
								(result + size - 1)->move.coord[2] = left + left_change + 8; (result + size - 1)->move.coord[3] = top - top_change - 4;
								MOVE more_move = more_eat(result[size - 1].board, 1, left + 16, top + 8);
								while (more_move.coord[0] != 0) // ���� ����� ������ ���
								{
									result[size - 1] = find_and_delete(result[size - 1], 2, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
									(result + size - 1)->move.coord[2] = more_move.coord[2];
									(result + size - 1)->move.coord[3] = more_move.coord[3];
									result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
									result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
									more_move = more_eat(result[size - 1].board, 1, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
								}
							}
							else {
								flag2 = 1;
							}
						}
					}
					if (!flag3)
					{
						if (is_here_a_checker(board, left - left_change, top + top_change) == WHITE && (left - left_change - 8) > LEFT_LIMIT + 1 && (top + top_change + 4) < BOT_LIMIT + 2) {
							if (is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) {
								size++;
								result = (NODE*)realloc(result, size * sizeof(NODE));
								(result + size - 1)->board = board_copy(board);
								(result + size - 1)->board = checker_delete(board, left - left_change, top + top_change, WHITE);
								result[size - 1].board.reds[i].location[0][0] = left - left_change - 8;
								result[size - 1].board.reds[i].location[0][1] = top + top_change + 4;
								(result + size - 1)->move.checker_num = i;
								(result + size - 1)->move.color = 1;
								(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
								(result + size - 1)->move.coord[2] = left - left_change - 8; (result + size - 1)->move.coord[3] = top + top_change + 4;
								MOVE more_move = more_eat(result[size - 1].board, 1, left + 16, top + 8);
								while (more_move.coord[0] != 0) // ���� ����� ������ ���
								{
									result[size - 1] = find_and_delete(result[size - 1], 2, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
									(result + size - 1)->move.coord[2] = more_move.coord[2];
									(result + size - 1)->move.coord[3] = more_move.coord[3];
									result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
									result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
									more_move = more_eat(result[size - 1].board, 1, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
								}
							}
							else {
								flag3 = 1;
							}
						}
					}
					if (!flag4)
					{
						if (is_here_a_checker(board, left - left_change, top - top_change) == WHITE && (left - left_change - 8) > LEFT_LIMIT + 1 && (top - top_change - 4) > TOP_LIMIT) {
							if (is_here_a_checker(board, left - left_change - 8, top - top_change - 4) == 0) {
								size++;
								result = (NODE*)realloc(result, size * sizeof(NODE));
								(result + size - 1)->board = board_copy(board);
								(result + size - 1)->board = checker_delete(board, left - left_change, top - top_change, WHITE);
								result[size - 1].board.reds[i].location[0][0] = left - left_change - 8;
								result[size - 1].board.reds[i].location[0][1] = top - top_change - 4;
								(result + size - 1)->move.checker_num = i;
								(result + size - 1)->move.color = 1;
								(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
								(result + size - 1)->move.coord[2] = left - left_change - 8; (result + size - 1)->move.coord[3] = top - top_change - 4;
								MOVE more_move = more_eat(result[size - 1].board, 1, left + 16, top + 8);
								while (more_move.coord[0] != 0) // ���� ����� ������ ���
								{
									result[size - 1] = find_and_delete(result[size - 1], 2, more_move.coord[0], more_move.coord[1], more_move.coord[2], more_move.coord[3]);
									(result + size - 1)->move.coord[2] = more_move.coord[2];
									(result + size - 1)->move.coord[3] = more_move.coord[3];
									result[size - 1].board.reds[i].location[0][0] = more_move.coord[2];
									result[size - 1].board.reds[i].location[0][1] = more_move.coord[3];
									more_move = more_eat(result[size - 1].board, 1, result[size - 1].board.reds[i].location[0][0], result[size - 1].board.reds[i].location[0][1]);
								}
							}
							else {
								flag4 = 1;
							}
						}
					}

					left_change += 8;
					top_change += 4;

					if ((left + left_change) > RIGHT_LIMIT + 3 && (left - left_change) < LEFT_LIMIT + 1 && (top + top_change) > BOT_LIMIT + 2 && (top - top_change) < TOP_LIMIT)
					{
						break;
					}
				}
			}
		}
	}
	else // ������� ���
	{
		for (int i = 0; i < 12; i++)
		{
			int left = board.reds[i].location[0][0];
			int top = board.reds[i].location[0][1];

			if (board.reds[i].status == 0) // �� �����
			{

				if (is_here_a_checker(board, left + 8, top + 4) == 0 && (left + 8) < RIGHT_LIMIT + 3 && (top + 4) < BOT_LIMIT + 2) {
					size++;
					result = (NODE*)realloc(result, size * sizeof(NODE));
					(result + size - 1)->board = board_copy(board);
					result[size - 1].board.reds[i].location[0][0] = left + 8;
					result[size - 1].board.reds[i].location[0][1] = top + 4;
					(result + size - 1)->move.checker_num = i;
					(result + size - 1)->move.color = 1;
					(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
					(result + size - 1)->move.coord[2] = left + 8; (result + size - 1)->move.coord[3] = top + 4;
				}
				if (is_here_a_checker(board, left - 8, top + 4) == 0 && (left - 8) > LEFT_LIMIT + 1 && (top + 4) < BOT_LIMIT + 2) {
					size++;
					result = (NODE*)realloc(result, size * sizeof(NODE));
					(result + size - 1)->board = board_copy(board);
					result[size - 1].board.reds[i].location[0][0] = left - 8;
					result[size - 1].board.reds[i].location[0][1] = top + 4;
					(result + size - 1)->move.checker_num = i;
					(result + size - 1)->move.color = 1;
					(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
					(result + size - 1)->move.coord[2] = left - 8; (result + size - 1)->move.coord[3] = top + 4;
				}
			}
			else if (board.reds[i].status == 1) // �����
			{
				int left = board.reds[i].location[0][0];
				int top = board.reds[i].location[0][1];

				int left_change = 8;
				int top_change = 4;
				int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
				while (1)
				{
					if (!flag1)
					{
						if (is_here_a_checker(board, left + left_change, top + top_change) == 0 && (left + left_change) < RIGHT_LIMIT + 3 && (top + top_change) < BOT_LIMIT + 2) {
							size++;
							result = (NODE*)realloc(result, size * sizeof(NODE));
							(result + size - 1)->board = board_copy(board);
							result[size - 1].board.reds[i].location[0][0] = left + left_change;
							result[size - 1].board.reds[i].location[0][1] = top + top_change;
							(result + size - 1)->move.checker_num = i;
							(result + size - 1)->move.color = 1;
							(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
							(result + size - 1)->move.coord[2] = left + left_change; (result + size - 1)->move.coord[3] = top + top_change;
						}
						else {
							flag1 = 1;
						}
					}
					if (!flag2)
					{
						if (is_here_a_checker(board, left + left_change, top - top_change) == 0 && (left + left_change) < RIGHT_LIMIT + 3 && (top - top_change) > TOP_LIMIT) {
							size++;
							result = (NODE*)realloc(result, size * sizeof(NODE));
							(result + size - 1)->board = board_copy(board);
							result[size - 1].board.reds[i].location[0][0] = left + left_change;
							result[size - 1].board.reds[i].location[0][1] = top - top_change;
							(result + size - 1)->move.checker_num = i;
							(result + size - 1)->move.color = 1;
							(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
							(result + size - 1)->move.coord[2] = left + left_change; (result + size - 1)->move.coord[3] = top - top_change;
						}
						else {
							flag2 = 1;
						}
					}
					if (!flag3)
					{
						if (is_here_a_checker(board, left - left_change, top + top_change) == 1 && (left - left_change) > LEFT_LIMIT + 1 && (top + top_change) < BOT_LIMIT + 2) {
							size++;
							result = (NODE*)realloc(result, size * sizeof(NODE));
							(result + size - 1)->board = board_copy(board);
							result[size - 1].board.reds[i].location[0][0] = left - left_change;
							result[size - 1].board.reds[i].location[0][1] = top + top_change;
							(result + size - 1)->move.checker_num = i;
							(result + size - 1)->move.color = 1;
							(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
							(result + size - 1)->move.coord[2] = left - left_change; (result + size - 1)->move.coord[3] = top + top_change;
						}
						else {
							flag3 = 1;
						}
					}
					if (!flag4)
					{
						if (is_here_a_checker(board, left - left_change, top - top_change) == 0 && (left - left_change) > LEFT_LIMIT + 1 && (top - top_change) > TOP_LIMIT) {
							size++;
							result = (NODE*)realloc(result, size * sizeof(NODE));
							(result + size - 1)->board = board_copy(board);
							result[size - 1].board.reds[i].location[0][0] = left - left_change;
							result[size - 1].board.reds[i].location[0][1] = top - top_change;
							(result + size - 1)->move.checker_num = i;
							(result + size - 1)->move.color = 1;
							(result + size - 1)->move.coord[0] = left; (result + size - 1)->move.coord[1] = top;
							(result + size - 1)->move.coord[2] = left - left_change; (result + size - 1)->move.coord[3] = top - top_change;
						}
						else {
							flag4 = 1;
						}
					}

					left_change += 8;
					top_change += 4;

					if ((left + left_change) > RIGHT_LIMIT + 3 && (left - left_change) < LEFT_LIMIT + 1 && (top + top_change) > BOT_LIMIT + 2 && (top - top_change) < TOP_LIMIT)
					{
						break;
					}
				}
			}
		}
	}

	now_size = size;
	return result;
}

// ���������� ��������� ���� ����������
int calculate_evaluate(BOARD board)
{
	int result = 0;

	int white_default_num = white_check_num; // ���������� ������� ����� �����
	int red_default_num = red_check_num; // ���������� ������� ������� �����
	int white_queens_num = 0; // ���������� ����� �����
	int red_queens_num = 0; // ���������� ������� �����

			/*��������� ����� ���� ����������*/
	int white_default_num_res = 0; // ���������� ������� ����� �����
	int red_default_num_res = 0; // ���������� ������� ������� �����
	int white_queens_num_res = 0; // ���������� ����� �����
	int red_queens_num_res = 0; // ���������� ������� �����

	for (int i = 0; i < 12; i++)
	{
		if (white_checkers[i].status == QUEEN) {
			white_queens_num++;
		}

		if (red_checkers[i].status == QUEEN) {
			red_queens_num++;
		}
	}

	for (int i = 0; i < 12; i++)
	{
		if (board.whites[i].status == DEFAULT) {
			white_default_num_res++;
		}
		else if (board.whites[i].status == QUEEN) {
			white_queens_num_res++;
		}

		if (board.reds[i].status == DEFAULT) {
			red_default_num_res++;
		}
		else if (board.reds[i].status == QUEEN) {
			red_queens_num_res++;
		}
	}

	if (whose_move == WHITE)
	{
		result += (red_default_num_res - red_default_num) * 1000;
		result += (red_queens_num_res - red_queens_num) * 25000;
		result -= (white_default_num_res - white_default_num) * 1000;
		result -= (white_queens_num_res - white_queens_num) * 25000;
	}
	else if (whose_move == RED)
	{
		result += (white_default_num_res - white_default_num) * 1000;
		result += (white_queens_num_res - white_queens_num) * 25000;
		result -= (red_default_num_res - red_default_num) * 1000;
		result -= (red_queens_num_res - red_queens_num) * 25000;
	}

	return result;
}

// ���������� ������ ��� �������
int red_evaluate_calc(BOARD board)
{
	int result = 0;
	int mine = 0;
	int opp = 0;

	for (int i = 0; i < 12; i++) // ������������ ���-�� ����������� � ����� �����
	{
		if (board.reds[i].status != DEAD) {
			mine++;

			int left = board.reds[i].location[0][0];
			int top = board.reds[i].location[0][1];

			if (board.reds[i].location[0][1] == 12 || board.reds[i].location[0][0] == 46 || board.reds[i].location[0][1] == 40 || board.reds[i].location[0][0] == 102) {
				result += 7;
				continue;
			}

			if (is_here_a_checker(board, left - 8, top + 4) == WHITE && is_here_a_checker(board, left + 8, top - 4) == 0) {
				result -= 3;
			}
			if (is_here_a_checker(board, left + 8, top + 4) == WHITE && is_here_a_checker(board, left - 8, top - 4) == 0) {
				result -= 3;
			}
			if (is_here_a_checker(board, left - 8, top - 4) == WHITE && is_here_a_checker(board, left + 8, top + 4) == 0) {
				result -= 3;
			}
			if (is_here_a_checker(board, left + 8, top - 4) == WHITE && is_here_a_checker(board, left - 8, top + 4) == 0) {
				result -= 3;
			}
			if (left - 16 < 46 || left + 16 > 102) {
				continue;
			}

			if (is_here_a_checker(board, left - 8, top + 4) == WHITE && is_here_a_checker(board, left - 16, top + 8) == 0 && (left - 16) >= 46 && (top + 8) <= 40) {
				result += 6;
			}
			if (is_here_a_checker(board, left + 8, top + 4) == WHITE && is_here_a_checker(board, left + 16, top + 8) == 0 && (left + 16) <= 102 && (top + 8) <= 40) {
				result += 6;
			}
			if (is_here_a_checker(board, left + 8, top - 4) == WHITE && is_here_a_checker(board, left + 16, top + 8) == 0 && (left + 16) <= 102 && (top - 8) >= 46) {
				result += 6;
			}
			if (is_here_a_checker(board, left - 8, top - 4) == WHITE && is_here_a_checker(board, left + 16, top + 8) == 0 && (left - 16) >= 46 && (top - 8) >= 12) {
				result += 6;
			}
		}
		if (board.whites[i].status != DEAD) {
			opp++;
		}
	}

	return result + (mine - opp) * 1000;
}

// ���������� ������ ��� �����
int white_evaluate_calc(BOARD board)
{
	int result = 0;
	int mine = 0;
	int opp = 0;

	for (int i = 0; i < 12; i++) // ������������ ���-�� ����������� � ����� �����
	{
		if (board.whites[i].status != DEAD) {
			mine++;

			int left = board.reds[i].location[0][0];
			int top = board.reds[i].location[0][1];

			if (board.reds[i].location[0][1] == 12 || board.reds[i].location[0][0] == 46 || board.reds[i].location[0][1] == 40 || board.reds[i].location[0][0] == 102) {
				result += 7;
				continue;
			}

			if (is_here_a_checker(board, left - 8, top + 4) == RED && is_here_a_checker(board, left + 8, top - 4) == 0) {
				result -= 3;
			}
			if (is_here_a_checker(board, left + 8, top + 4) == RED && is_here_a_checker(board, left - 8, top - 4) == 0) {
				result -= 3;
			}
			if (is_here_a_checker(board, left - 8, top - 4) == RED && is_here_a_checker(board, left + 8, top + 4) == 0) {
				result -= 3;
			}
			if (is_here_a_checker(board, left + 8, top - 4) == RED && is_here_a_checker(board, left - 8, top + 4) == 0) {
				result -= 3;
			}
			if (left - 16 < 46 || left + 16 > 102) {
				continue;
			}

			if (is_here_a_checker(board, left - 8, top + 4) == RED && is_here_a_checker(board, left - 16, top + 8) == 0 && (left - 16) >= 46 && (top + 8) <= 40) {
				result += 6;
			}
			if (is_here_a_checker(board, left + 8, top + 4) == RED && is_here_a_checker(board, left + 16, top + 8) == 0 && (left + 16) <= 102 && (top + 8) <= 40) {
				result += 6;
			}
			if (is_here_a_checker(board, left + 8, top - 4) == RED && is_here_a_checker(board, left + 16, top + 8) == 0 && (left + 16) <= 102 && (top - 8) >= 46) {
				result += 6;
			}
			if (is_here_a_checker(board, left - 8, top - 4) == RED && is_here_a_checker(board, left + 16, top + 8) == 0 && (left - 16) >= 46 && (top - 8) >= 12) {
				result += 6;
			}
		}
		if (board.reds[i].status != DEAD) {
			opp++;
		}
	}

	return result + (mine - opp) * 1000;
}

// ������� ����� ��� �������
int red_minimax(BOARD board, int depth, int alpha, int beta, int maximizing_player)
{
	if (depth == 0) {
		return red_evaluate_calc(board); // �������
	}

	// ����������� �����()
	BOARD current_board = board_copy(board);
	NODE* moves = NULL;

	if (maximizing_player == 1)
	{
		int max_eval = -2147483646;
		int ev;
		// sizeof(NODE) = 316
		if (must_beat(board, 1)) { // ���� ����������� ���� ����
			moves = find_reds_moves(board, 1); // ����� ��� ������ ����
		}
		else {
			moves = find_reds_moves(board, 0); // ����� ��� ��������� ����
		}
		int size = now_size;

		for (int i = 0; i < size; i++)
		{
			ev = red_minimax(moves->board, depth - 1, alpha, beta, 0);
			if (ev > max_eval) {
				max_eval = ev;
			}
			if (ev > alpha) {
				alpha = ev;
			}
			if (beta <= alpha) {
				break;
			}
		}
		return max_eval;
	}
	else if (maximizing_player == 0)
	{
		int min_eval = 2147483647;
		int ev;
		if (must_beat(board, 0)) { // ���� ����������� ���� ����
			moves = find_whites_moves(board, 1); // ����� ��� ������ ����
		}
		else {
			moves = find_whites_moves(board, 0); // ����� ��� ��������� ����
		}
		int size = now_size;

		for (int i = 0; i < size; i++)
		{
			ev = red_minimax(moves->board, depth - 1, alpha, beta, 1);
			if (ev < min_eval) {
				min_eval = ev;
			}
			if (ev < beta) {
				beta = ev;
			}
			if (beta <= alpha) {
				break;
			}
		}
		return min_eval;
	}
}

// ������� ����� ��� �����
int white_minimax(BOARD board, int depth, int alpha, int beta, int maximizing_player)
{
	if (depth == 0) {
		return white_evaluate_calc(board); // �������
	}

	// ����������� �����()
	BOARD current_board = board_copy(board);
	NODE* moves = NULL;

	if (maximizing_player == 1)
	{
		int max_eval = -2147483646;
		int ev;
		// sizeof(NODE) = 316
		if (must_beat(board, 0)) { // ���� ����������� ���� ����
			moves = find_whites_moves(board, 1); // ����� ��� ������ ����
		}
		else {
			moves = find_whites_moves(board, 0); // ����� ��� ��������� ����
		}
		int size = now_size;

		for (int i = 0; i < size; i++)
		{
			ev = white_minimax(moves->board, depth - 1, alpha, beta, 0);
			if (ev > max_eval) {
				max_eval = ev;
			}
			if (ev > alpha) {
				alpha = ev;
			}
			if (beta <= alpha) {
				break;
			}
		}
		return max_eval;
	}
	else if (maximizing_player == 0)
	{
		int min_eval = 2147483647;
		int ev;
		if (must_beat(board, 1)) { // ���� ����������� ���� ����
			moves = find_reds_moves(board, 1); // ����� ��� ������ ����
		}
		else {
			moves = find_reds_moves(board, 0); // ����� ��� ��������� ����
		}
		int size = now_size;

		for (int i = 0; i < size; i++)
		{
			ev = white_minimax(moves->board, depth - 1, alpha, beta, 1);
			if (ev < min_eval) {
				min_eval = ev;
			}
			if (ev < beta) {
				beta = ev;
			}
			if (beta <= alpha) {
				break;
			}
		}
		return min_eval;
	}
}

// ������� ����� �������� �����
BOARD new_board()
{
	BOARD result;

	for (int i = 0; i < 12; i++)
	{
		result.whites[i] = white_checkers[i];
		result.reds[i] = red_checkers[i];
	}

	return result;
}

// ���������������� �����
BOARD init_board()
{
	BOARD result;

	if (!is_load) {
		get_start_values();
	}

	for (int i = 0; i < 12; i++)
	{
		result.whites[i] = white_checkers[i];
		result.reds[i] = red_checkers[i];
	}

	return result;
}

// ��������� ������ � ����� � ���� (��� ���� � �����������)
void revive_the_board(BOARD board)
{
	for (int i = 0; i < 12; i++)
	{
		if (board.reds[i].status == 2 && board.reds[i].status != red_checkers[i].status) {
			red_checkers[i].status = 2;
			red_check_num--;
			gotoxy(red_checkers[i].location[0][0], red_checkers[i].location[0][1]);
			con_set_color(NULL, CON_CLR_BLACK);
			printf("   "); // ������� ��������� ����� � �����
		}
		if (board.whites[i].status == 2 && board.whites[i].status != white_checkers[i].status) {
			white_checkers[i].status = 2;
			white_check_num--;
			gotoxy(white_checkers[i].location[0][0], white_checkers[i].location[0][1]);
			con_set_color(NULL, CON_CLR_BLACK);
			printf("   "); // ������� ��������� ����� � �����
		}
	}
}

// ��� ����������
void computer_move(BOARD board, int white_mode, int red_mode)
{
	NODE* moves = NULL;

	if (whose_move == WHITE)
	{
		// �������, ���� �� ����
		if (must_beat(board, 0) == 1) {
			moves = find_whites_moves(board, 1);
		}
		else {
			moves = find_whites_moves(board, 0);
		}
	}
	else if (whose_move == RED)
	{
		// �������, ���� �� ����
		if (must_beat(board, 1) == 1) {
			moves = find_reds_moves(board, 1);
		}
		else {
			moves = find_reds_moves(board, 0);
		}
	}
	int size = now_size;

	if (whose_move == WHITE)
	{
		if (white_mode == 1)
		{
			for (int i = 0; i < size; i++) // ��������� ����
			{
				(moves + i)->value = white_minimax((moves + i)->board, 0, 2147483647, -2147483647, 0);
			}
		}
		else if (white_mode == 2)
		{
			if (red_mode == 3)
			{
				for (int i = 0; i < size; i++) // ��������� ����
				{
					(moves + i)->value = white_minimax((moves + i)->board, 2, 2147483647, -2147483647, 0);
				}
			}
			else
			{
				for (int i = 0; i < size; i++) // ��������� ����
				{
					(moves + i)->value = white_minimax((moves + i)->board, 1, 2147483647, -2147483647, 0);
				}
			}
		}
		else if (white_mode == 3)
		{
			for (int i = 0; i < size; i++) // ��������� ����
			{
				(moves + i)->value = white_minimax((moves + i)->board, 5, 2147483647, -2147483647, 0);
			}
		}
	}
	else if (whose_move == RED)
	{
		if (red_mode == 1)
		{
			for (int i = 0; i < size; i++) // ��������� ����
			{
				(moves + i)->value = red_minimax((moves + i)->board, 0, 2147483647, -2147483647, 0);
			}
		}
		else if (red_mode == 2)
		{
			for (int i = 0; i < size; i++) // ��������� ����
			{
				(moves + i)->value = red_minimax((moves + i)->board, 1, 2147483647, -2147483647, 0);
			}
		}
		else if (red_mode == 3)
		{
			if (white_mode == 1)
			{
				for (int i = 0; i < size; i++) // ��������� ����
				{
					(moves + i)->value = red_minimax((moves + i)->board, 5, 2147483647, -2147483647, 0);
				}
			}
			else
			{
				for (int i = 0; i < size; i++) // ��������� ����
				{
					(moves + i)->value = red_minimax((moves + i)->board, 4, 2147483647, -2147483647, 0);
				}
			}
		}
	}

	int evaluate = -2147483647;
	int num = -1;
	for (int i = 0; i < size; i++) // �������� ��� � ������ �������
	{
		if ((moves + i)->value > evaluate)
		{
			evaluate = (moves + i)->value;
			num = i;
		}
	}

	if (num > -1) {
		checkers_number = (moves + num)->move.checker_num;
		step((moves + num)->move.coord[0], (moves + num)->move.coord[1], (moves + num)->move.coord[2], (moves + num)->move.coord[3], (moves + num)->move.color);
		revive_the_board((moves + num)->board);
		update_checker_coord(moves[num]);
	}
}

// �������-����
void hum_vs_comp(int mode)
{
	white_moves_num = 0;
	red_moves_num = 0;
	int white_lose = 0;
	int red_lose = 0;
	white_check_num = 12;
	red_check_num = 12;
	BOARD board = init_board();

	get_cells_locations(); // �������� ���������� ������
	whose_move = WHITE;
	build_board(); // ��������� �����

	while (1)
	{
		if (end) // ����� ����
		{
			end = 0;
			break;
		}

		need_to_beat = necessary_to_beat(); // ��������, ����������� �� ����
		cycle_end = 0; // �������� ���� ��� ����� � ������� move_cursor()

		if (red_check_num == 0 || (whose_move == RED && !need_to_beat && !no_move()))
		{
			white_win(); // ������ �����
			// ������ � ������� ��������
			break;
		}
		else if (white_check_num == 0 || (whose_move == WHITE && !need_to_beat && !no_move()))
		{
			red_win(); // ������ �������
			// ������ � ������� ��������
			break;
		}

		if (whose_move == WHITE) { // ���� ����� �������
			move_cursor(LEFT_BOARD_CENTER, TOP_BOARD_CENTER);
		}
		else if (whose_move == RED) {
			if (mode == 1) {
				computer_move(board, -1, 1);
			}
			else if (mode == 2) {
				computer_move(board, -1, 2);
			}
			else if (mode == 3) {
				computer_move(board, -1, 3);
			}

		}

		board = new_board(board);

		if (whose_move == RED) { // ���� ��� ��� ����������
			whose_move = WHITE; // ��������� ��� ��������
		}
		else if (whose_move == WHITE) { // ���� ��� ��� ��������
			whose_move = RED; // ��������� ��� ����������
		}
	}
}

// ���� ���������� c �����������
void computer_vs_computer(int white_mode, int red_mode)
{
	/*mode - 1-�����, 2-���������, 3-������*/
	LARGE_INTEGER freq, time1, time2;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&time1);

	white_moves_num = 0;
	red_moves_num = 0;
	int white_lose = 0;
	int red_lose = 0;
	white_check_num = 12;
	red_check_num = 12;
	BOARD board = init_board();

	get_cells_locations(); // �������� ���������� ������
	whose_move = WHITE;
	build_board(); // ��������� �����

	while (1)
	{
		if (end) // ����� ����
		{
			end = 0;
			break;
		}

		need_to_beat = necessary_to_beat(); // ��������, ����������� �� ����

		if (red_check_num == 0 || (whose_move == RED && !need_to_beat && !no_move()))
		{
			QueryPerformanceCounter(&time2);
			time2.QuadPart -= time1.QuadPart;
			double span = (double)time2.QuadPart / freq.QuadPart;
			white_win(); // ������ �����
			// ������ � ������� ��������
			break;
		}
		else if (white_check_num == 0 || (whose_move == WHITE && !need_to_beat && !no_move()))
		{
			QueryPerformanceCounter(&time2);
			time2.QuadPart -= time1.QuadPart;
			double span = (double)time2.QuadPart / freq.QuadPart;
			red_win(); // ������ �������
			// ������ � ������� ��������
			break;
		}

		computer_move(board, white_mode, red_mode);

		board = new_board();

		if (whose_move == RED) { // ���� ��� ��� ����������
			whose_move = WHITE; // ��������� ��� ��������
		}
		else if (whose_move == WHITE) { // ���� ��� ��� ��������
			whose_move = RED; // ��������� ��� ����������
		}
	}
}

// ���������� ���� ������ ���������
void draw_choose_difficulty_menu()
{
	int left = 60;
	int top = 9;

	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	clrscr();

	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("-----------------------------");
	top++;
	gotoxy(left, top);
	printf("|      ��������� ����       |");
	top++;
	gotoxy(left, top);
	printf("-----------------------------");

	left = 65;
	top = 15;
	con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*      �����      *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*    ���������    *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*     ������      *");
	top++;
	gotoxy(left, top);
	printf("*******************");
}

// ���� ������ ��������� ��� ���� ������
void difficulty_menu_comp()
{
	int white_mode = 0;
	int red_mode = 0;

	int left = 45;
	int top = 9;

	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	clrscr();

	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("-----------------------------------------------------------------");
	top++;
	gotoxy(left, top);
	printf("|        ��������� �����        |       ��������� �������       |");
	top++;
	gotoxy(left, top);
	printf("-----------------------------------------------------------------");

	left = 50;
	top = 15;
	con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*      �����      *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*    ���������    *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*     ������      *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	left = 85;
	top = 15;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*      �����      *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*    ���������    *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*     ������      *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	int flag = 1; // 1-���������� ��������� �����; 2-���������� ��������� �������
	int preparedness = 0;

	left = 50;
	top = 17;
	int position = 0;
	while (1)
	{
		if (flag == 1) {
			left = 50;
		}
		else if (flag == 2) {
			left = 85;
		}

		int code = key_pressed_code();
		if (code == KEY_UP) // ���� ��� ������� �����
		{
			// �� ������� � �������� ������ (���� ��� ��������)
			if (position > 0)
			{
				switch (position)
				{
				case 1:
					if (flag == 1)
					{
						if (white_mode == 1)
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else if (white_mode == 2)
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
					}
					else if (flag == 2)
					{
						if (red_mode == 1)
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else if (red_mode == 2)
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
					}
					break;
				case 2:
					if (flag == 1)
					{
						if (white_mode == 2)
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else if (white_mode == 3)
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
					}
					else if (flag == 2)
					{
						if (red_mode == 2)
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else if (red_mode == 3)
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else
						{
							top -= 2;
							gotoxy(left, top);
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
					}
					break;
				}
				position--;
			}
		}
		else if (code == KEY_DOWN) // ���� ������� ����
		{
			// �� ������� � ������� ������ (���� ��� ��������)
			if (position < 2)
			{
				switch (position)
				{
				case 1:
					if (flag == 1)
					{
						if (white_mode == 2)
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else if (white_mode == 3)
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
					}
					else if (flag == 2)
					{
						if (red_mode == 2)
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else if (red_mode == 3)
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     ������      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
					}
					break;
				case 0:
					if (flag == 1)
					{
						if (white_mode == 1)
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else if (white_mode == 2)
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
					}
					else if (flag == 2)
					{
						if (red_mode == 1)
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else if (red_mode == 2)
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
						else
						{
							top -= 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      �����      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    ���������    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
						}
					}
					break;
				}
				position++;
			}
		}
		else if (code == KEY_LEFT) // ������� �����
		{
			if (flag == 2)
			{
				flag = 1;
				switch (position)
				{
				case 0:
					top -= 2;
					if (red_mode == 1) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					}
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*      �����      *");
					top++;
					gotoxy(left, top);
					printf("*******************");

					left = 50;
					top -= 2;
					if (white_mode == 1) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					}
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*      �����      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				case 1:
					top -= 2;
					if (red_mode == 2) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					}
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*    ���������    *");
					top++;
					gotoxy(left, top);
					printf("*******************");

					left = 50;
					top -= 2;
					if (white_mode == 2) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					}
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*    ���������    *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				case 2:
					top -= 2;
					gotoxy(left, top);
					if (red_mode == 3) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					}
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*     ������      *");
					top++;
					gotoxy(left, top);
					printf("*******************");

					left = 50;
					top -= 2;
					gotoxy(left, top);
					if (white_mode == 3) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					}
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*     ������      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				}
			}
		}
		else if (code == KEY_RIGHT) // ������� ������
		{
			if (flag == 1)
			{
				flag = 2;
				switch (position)
				{
				case 0:
					top -= 2;
					if (white_mode == 1) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					}
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*      �����      *");
					top++;
					gotoxy(left, top);
					printf("*******************");

					left = 85;
					top -= 2;
					if (red_mode == 1) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					}
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*      �����      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				case 1:
					top -= 2;
					if (white_mode == 2) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					}
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*    ���������    *");
					top++;
					gotoxy(left, top);
					printf("*******************");

					left = 85;
					top -= 2;
					if (red_mode == 2) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					}
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*    ���������    *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				case 2:
					top -= 2;
					gotoxy(left, top);
					if (white_mode == 3) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					}
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*     ������      *");
					top++;
					gotoxy(left, top);
					printf("*******************");

					left = 85;
					top -= 2;
					gotoxy(left, top);
					if (red_mode == 3) {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
					}
					else {
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					}
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*     ������      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				}
			}
		}
		else if (code == KEY_ESC) // ESC - �����
		{
			return;
		}
		else if (code == KEY_ENTER) // ������ ������ Enter
		{
			switch (position)
			{
			case 0:
				if (flag == 1)
				{
					if (white_mode == 0) // ���� ������ �� �������
					{
						white_mode = 1;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 1) // ���� ��� ����� ��� ������ ����� "�����"
					{
						white_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 2) // ���� ��� ����� ��� ������ ����� "���������"
					{
						white_mode = 1;
						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 3) // ���� ��� ����� ��� ������ ����� "������"
					{
						white_mode = 1;
						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
				}
				else if (flag == 2)
				{
					if (red_mode == 0) // ���� ������ �� �������
					{
						red_mode = 1;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 1) // ���� ��� ����� ��� ������ ����� "�����"
					{
						red_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 2) // ���� ��� ����� ��� ������ ����� "���������"
					{
						red_mode = 1;
						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 3) // ���� ��� ����� ��� ������ ����� "������"
					{
						red_mode = 1;
						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
				}
				break;
			case 1:
				if (flag == 1)
				{
					if (white_mode == 0) // ���� ������ �� �������
					{
						white_mode = 2;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 1) // ���� ��� ����� ��� ������ ����� "�����"
					{
						white_mode = 2;
						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 2) // ���� ��� ����� ��� ������ ����� "���������"
					{
						white_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 3) // ���� ��� ����� ��� ������ ����� "������"
					{
						white_mode = 2;
						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
				}
				else if (flag == 2)
				{
					if (red_mode == 0) // ���� ������ �� �������
					{
						red_mode = 2;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 1) // ���� ��� ����� ��� ������ ����� "�����"
					{
						red_mode = 2;
						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 2) // ���� ��� ����� ��� ������ ����� "���������"
					{
						red_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 3) // ���� ��� ����� ��� ������ ����� "������"
					{
						red_mode = 2;
						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
				}
				break;
			case 2:
				if (flag == 1)
				{
					if (white_mode == 0) // ���� ������ �� �������
					{
						white_mode = 3;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 1) // ���� ��� ����� ��� ������ ����� "�����"
					{
						white_mode = 3;
						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 2) // ���� ��� ����� ��� ������ ����� "���������"
					{
						white_mode = 3;
						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 3) // ���� ��� ����� ��� ������ ����� "������"
					{
						white_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
				}
				else if (flag == 2)
				{
					if (red_mode == 0) // ���� ������ �� �������
					{
						red_mode = 3;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 1) // ���� ��� ����� ��� ������ ����� "�����"
					{
						red_mode = 3;
						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      �����      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 2) // ���� ��� ����� ��� ������ ����� "���������"
					{
						red_mode = 3;
						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    ���������    *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 3) // ���� ��� ����� ��� ������ ����� "������"
					{
						red_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     ������      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
				}
				break;
			}

			if (preparedness == 2)
			{
				computer_vs_computer(white_mode, red_mode);
				return; // ������ ����
			}
		}
	}
}

// ���� ������ ���������
void choose_difficulty_menu()
{
	draw_choose_difficulty_menu();

	int left = 65;
	int top = 17;
	int position = 0;
	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_UP) // ���� ��� ������� �����
		{
			// �� ������� � �������� ������ (���� ��� ��������)
			if (position > 0)
			{
				switch (position)
				{
				case 1:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*    ���������    *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*      �����      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				case 2:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*     ������      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*    ���������    *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				}
				position--;
			}
		}
		else if (code == KEY_DOWN) // ���� ������� ����
		{
			// �� ������� � ������� ������ (���� ��� ��������)
			if (position < 2)
			{
				switch (position)
				{
				case 1:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*    ���������    *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*     ������      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				case 0:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*      �����      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*    ���������    *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				}
				position++;
			}
		}
		else if (code == KEY_ESC) // ESC - �����
		{
			return;
		}
		else if (code == KEY_ENTER) // ������ ������ Enter
		{
			if (position == 0) {
				complexity = 1;
				hum_vs_comp(complexity);
			}
			else if (position == 1) {
				complexity = 2;
				hum_vs_comp(complexity);
			}
			else if (position == 2) {
				complexity = 3;
				hum_vs_comp(complexity);
			}
			return;
		}
	}
}

// ���������� ������� ����
void draw_main_menu()
{
	int left = 65;
	int top = 15;

	// ������� ������
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	clrscr();

	con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("*  ��������� ����  *");
	top++;
	gotoxy(left, top);
	printf("********************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("*    ����� ����    *");
	top++;
	gotoxy(left, top);
	printf("********************");

	top += 2;
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("* ������� �������� *");
	top++;
	gotoxy(left, top);
	printf("********************");

	top += 2;
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("*     �������      *");
	top++;
	gotoxy(left, top);
	printf("********************");

	top += 2;
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("*   � ���������    *");
	top++;
	gotoxy(left, top);
	printf("********************");

	top += 2;
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("*      �����       *");
	top++;
	gotoxy(left, top);
	printf("********************");
}

// ���������� ���� ����� ����
void draw_new_game_menu()
{
	int left = 65;
	int top = 15;

	// ������� ������
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	clrscr();

	con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*   �������-�������   *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*  �������-���������  *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("* ���������-��������� *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*        �����        *");
	top++;
	gotoxy(left, top);
	printf("***********************");
}

// �����������, ���� ������� "����� ����"
void new_game_menu()
{
	draw_new_game_menu(); // ���������� ���� ����� ����

	int left = 65;
	int top = 17;
	int position = 0;
	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_UP) // ���� ��� ������� �����
		{
			// �� ������� � �������� ������ (���� ��� ��������)
			if (position > 0)
			{
				switch (position)
				{
				case 1:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*  �������-���������  *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   �������-�������   *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				case 2:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("* ���������-��������� *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*  �������-���������  *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				case 3:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*        �����        *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("* ���������-��������� *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				}
				position--;
			}
		}
		else if (code == KEY_DOWN) // ���� ������� ����
		{
			// �� ������� � ������� ������ (���� ��� ��������)
			if (position < 3)
			{
				switch (position)
				{
				case 2:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("* ���������-��������� *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*        �����        *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				case 1:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*  �������-���������  *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("* ���������-��������� *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				case 0:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   �������-�������   *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*  �������-���������  *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				}
				position++;
			}
		}
		else if (code == KEY_ESC) // ESC - �����
		{
			//draw_main_menu();
			return;
		}
		else if (code == KEY_ENTER) // ������ ������ Enter
		{
			if (position == 3) { // ������ ��������� ����� - ��� "�����"
				draw_main_menu();
				return;
			}
			if (position == 0) {
				game_status = 1;
				human_vs_human();
				return;
				// �������-�������();
			}
			if (position == 1) {
				game_status = 2;
				choose_difficulty_menu();
				// �������-���������();
			}
			if (position == 2) {
				game_status = 3;
				difficulty_menu_comp();
				// ���������-���������();
			}
			left = 65;
			top = 17;
			draw_new_game_menu();
			position = 0;
		}
	}
}

// ������� ���������� ������ ������� (����� "��������")
void bubbleSort(struct table* table, int size)
{
	// ��� ���� ���������
	for (int i = 0; i < size - 1; i++)
	{
		for (int j = (size - 1); j > i; j--) // ��� ���� ��������� ����� i-���
		{
			if ((table + j - 1)->value > (table + j)->value) // ���� ������� ������� ������ �����������
			{
				struct table temp = *(table + j - 1); // ������ �� �������
				*(table + j - 1) = *(table + j);
				*(table + j) = temp;
			}
		}
	}
}

// �������� ������� ��������
void high_score_table()
{
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	clrscr();
	gotoxy(0, 0);

	struct table sort_table[200];

	char filename[] = "table.txt";
	char ch;
	char str[70]; // ��� �������� ����� ������ �� ������� ��������
	char ch_value[10]; // ��� �������� ������� � ���� ������ 

	FILE* f = fopen(filename, "r");
	if (f == NULL) {
		printf("�� ������� ������� ����.\n");
		exit(1);
	}

	int k = 0; int j;
	// ��������� ���� � �������� �������� � ��������� ��������� ��� ����������
	while (fgets((sort_table + k)->str, 70, f) != NULL)
	{
		j = 0;
		while ((sort_table + k)->str[j] != '\n' && (sort_table + k)->str[j] != '\0')
		{
			j++;
		}
		if ((sort_table + k)->str[j] == '\n') // �������� '\n' �� '\0' � ����� ������
		{
			(sort_table + k)->str[j] = '\0';
		}

		j = 0;
		while ((sort_table + k)->str[j] != '|') { // ���������� � ������ ��� ������������ � ��������� � �������
			j++;
		}

		j++;
		if ((sort_table + k)->str[j] != ' ')
		{
			printf("���� � �������� �������� �������� �������.");
			return;
		}

		j++;
		int a = 0;
		while ((sort_table + k)->str[j] != EOF && (sort_table + k)->str[j] != '\0') // ��������� ������
		{
			switch ((sort_table + k)->str[j])
			{
			case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
				ch_value[a] = (sort_table + k)->str[j];
				a++;
				j++;
				break;
			default:
				printf("���� � �������� �������� �������� �������.");
				return;
			}
		}
		(sort_table + k)->value = atoi(ch_value);
		k++;
		memset(ch_value, '\0', a);
		a = 0;
	}

	bubbleSort(sort_table, k); // ���������� ������� ��������
	fclose(f);

	f = fopen(filename, "w");
	if (f == NULL) {
		printf("�� ������� ������� ����.\n");
		exit(1);
	}

	for (int i = 0; i < k; i++)
	{
		fwrite(sort_table[i].str, sizeof(char), strlen(sort_table[i].str), f);
		if (i != k) {
			fwrite("\n", sizeof(char), 1, f);
		}
	}

	fclose(f);

	f = fopen(filename, "r");
	if (f == NULL) {
		printf("�� ������� ������� ����.\n");
		exit(1);
	}

	printf("����� | ��� ������ | ���������� ����� ��� ������\n\n");
	int i = 1;
	while (fgets(str, 70, f) != NULL)
	{
		j = 0;
		while (str[j] != '|') {
			j++;
		}

		j++;
		if (str[j] != ' ')
		{
			printf("���� � �������� �������� �������� �������.");
			return;
		}

		j++;
		while (str[j] != '\n' && str[j] != '\0')
		{
			switch (str[j])
			{
			case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
				j++;
				break;
			default:
				printf("���� � �������� �������� �������� �������.");
				return;
			}
		}

		printf(" %d) %s", i, str);
		i++;
	}
	fclose(f);

	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_ESC || code == KEY_ENTER)
		{
			break;
		}
	}
}

// ������� ������ "� ���������" 
void about_the_program()
{
	// ������� ������
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // ���� ������, ���� �������
	clrscr();
	gotoxy(0, 0);

	printf("		������: \n");
	printf("	1. ������� ����� ���������; ��. 4831001/90002; 2020�.; ����� ����� ��������; �����; ������.\n");
	printf("	2. ��������� ��������� ���������; ��. 4831001/90002; 2020�.; ����� ����� ��������; �����; ������.\n");

	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_ESC || code == KEY_ENTER)
		{
			break;
		}
	}
}

// ��������� �������� ����
void main_menu()
{
	draw_main_menu(); // ���������� ����

	int left = 65;
	int top = 17;
	int position = 0;
	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_UP) // ���� ��� ������� �����
		{
			// �� ������� � �������� ������ (���� ��� ��������)
			if (position > 0)
			{
				switch (position)
				{
				case 1:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*    ����� ����    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*  ��������� ����  *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				case 2:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("* ������� �������� *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*    ����� ����    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				case 3:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*     �������      *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("* ������� �������� *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				case 4:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*   � ���������    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*     �������      *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				case 5:
					top -= 2;
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*      �����       *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*   � ���������    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				}
				position--;
			}
		}
		else if (code == KEY_DOWN) // ���� ������� ����
		{
			// �� ������� � ������� ������ (���� ��� ��������)
			if (position < 5)
			{
				switch (position)
				{
				case 4:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*   � ���������    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*      �����       *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				case 3:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*     �������      *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*   � ���������    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				case 2:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("* ������� �������� *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*     �������      *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				case 1:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*    ����� ����    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("* ������� �������� *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				case 0:
					top -= 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*  ��������� ����  *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*    ����� ����    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				}
				position++;
			}
		}
		else if (code == KEY_ESC) // ESC - �����
		{
			return;
		}
		else if (code == KEY_ENTER) // ������ ������ Enter
		{
			if (position == 5) { // ������ ��������� ����� - ��� �����
				return;
			}
			else if (position == 0) { // ��������� ����
				is_load = 1;
				if (load_the_game() == 1);
				{
					build_board();
					if (game_status == 1)
					{
						human_vs_human();
					}
					else if (game_status == 2)
					{
						hum_vs_comp(complexity);
					}
				}
			}
			else if (position == 1) { // ����� ����
				is_load = 0;
				new_game_menu();
			}
			else if (position == 2) { // ������� ��������
				high_score_table();
			}
			else if (position == 3) // ������� 
			{
				reference();
			}
			else if (position == 4) // � ��������� 
			{
				about_the_program();
			}
			draw_main_menu();
			position = 0; // ��� ������ �� ���� ����� ����, ��������� ������ �����
			top = 17;
			left = 65;
		}
	}
}

int main()
{
	// ���������������� �������, ���������� ������
	con_init(300, 100);
	show_cursor(0);
	system("mode con cols=150 lines=50"); // ����������� ������ ������� (150�50)

	main_menu();

	return 0;
}