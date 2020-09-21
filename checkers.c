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

#define LEFT_BOARD_CENTER ((con_width() / 2) + 1) // +- ñåðåäèíà äîñêè îòíîñèòåëüíî ëåâîãî êðàÿ êîíñîëè
#define TOP_BOARD_CENTER  ((con_height() / 2) + 2) // +- ñåðåäèíà äîñêè îòíîñèòåëüíî âåðõíåãî êðàÿ êîíñîëè

#define LEFT_LIMIT		  ((con_width() / 2) - 32) // ïîñëåäíÿÿ êëåòêà ñëåâà
#define RIGHT_LIMIT		  ((con_width() / 2) + 26) // ïîñëåäíÿÿ êëåòêà ñïðàâà
#define TOP_LIMIT		  ((con_height() / 2) - 15) // ïîñëåäíÿÿ êëåòêà ñâåðõó
#define BOT_LIMIT		  ((con_height() / 2) + 15) // ïîñëåäíÿÿ êëåòêà ñíèçó

/* Ïåðåìåííûå äëÿ ôóíêöèè move_cursor() */
int sel_check_left_pos = 0;
int sel_check_top_pos = 0;
int on;
int checkers_number = -1; // ïî óìîë÷àíèþ = -1
int whose_move = 1; // ôëàã, ÷òîáû ïîíÿòü, ÷åé õîä (1 - õîä áåëûõ, 2 - õîä êðàñíûõ)
int cycle_end; // ôëàã äëÿ çàâåðøåíèÿ öèêëà
int deleted_checker_left; //êîîðäèíàòû óäàëÿåìîé øàøêè (åñëè øàøêó ñúåëè)
int deleted_checker_top;
int eat_more = 0; // åñëè åñòü âîçìîæíîñü ñúåñòü åùå
int need_to_beat = 0; // åñëè = 1, òî îáÿçàòåëüíî íóæíî ñúåñòü øàøêó
int white_check_num = 12; // êîë÷åñòâî æèâûõ áåëûõ øàøåê
int red_check_num = 12; // êîë÷åñòâî æèâûõ êðàñíûõ øàøåê
int end = 0; // ôëàã äëÿ âûõîäà â ãëàâíîå ìåíþ
int is_load = 0; // ôëàã, ÷òîáû ïîíÿòü, íà÷àëàñü íîâàÿ èãðà èëè ïðîäîëæàåòñÿ ñòàðàÿ
int game_status = 0; // 1 - ÷åëîâåê-÷åëîâåê, 2 - ÷åëîâåê-êîìïüþòåð, 3 - êîìï-êîìï
int complexity = 0; // ñëîæíîñòü èãðû (1 - ëåãêî, 2 - íîðìàëüíî, 3 - ñëîæíî)

int white_moves_num = 0; // êîëè÷åñòâî ñäåëàííûõ õîäîâ çà èãðó äëÿ òàáëèöû ðåêîðäîâ
int red_moves_num = 0;

int cells_locations[64][2]; // êîîðäèíàòû êàæäîé êëåòêè

struct table // äëÿ òàáëèöû ðåêîðäîâ
{
	char str[70];
	int value;
};

typedef struct
{
	int checker_num; // íîìåð ïåðåäâèãàåìîé øàøêè
	int color; // öâåò øàøêè
	int new_left; // íîâûå êîîð
	int new_top; // äèíàòû øàøêè
}Move;

typedef struct
{
	int location[1][2]; // êîîðäèíàòû øàøêè (ïåðâàÿ ÿ÷åéêà - îòñòóï ñëåâà, âòîðàÿ - ñâåðõó)
	int status; // 0 - îáû÷íûé, 1 - äàìêà, 2 - ìåðòâ
}checker;
checker white_checkers[12];
checker red_checkers[12];

checker white_copy[12]; // êîïèè øàøåê äëÿ ãåíåðèðîâàíèÿ õîäà êîìïüþòåðà
checker red_copy[12];

typedef struct
{
	checker whites[12];
	checker reds[12];
}BOARD;

typedef struct
{
	int checker_num; // íîìåð ïåðåäâèãàåìîé øàøêè
	int color; // öâåò øàøêè
	int coord[4]; // ñòàðûå è íîâûå êîîðäèíàòû
}MOVE;

typedef struct
{
	BOARD board;
	MOVE move;
	int value;
}NODE;

// ïðîâåðêà, åñòü ëè õîä (âîçâðàùàåò 1 - åñëè åñòü, èíà÷å 0)
int no_move()
{
	int left;
	int top;

	if (whose_move == WHITE) // åñëè õîä áåëûõ
	{
		for (int i = 0; i < 12; i++)
		{
			if (white_checkers[i].status == DEFAULT) // íå äàìêà
			{
				left = white_checkers[i].location[0][0]; // êîîðäèíàòû äàííîé øàøêè
				top = white_checkers[i].location[0][1];

				if (is_there_a_checker(left + 8, top - 4) == 0 && (left + 8) < RIGHT_LIMIT + 3 && (top - 4) > TOP_LIMIT) {
					return 1;
				}
				if (is_there_a_checker(left - 8, top - 4) == 0 && (left - 8) > LEFT_LIMIT + 1 && (top - 4) > TOP_LIMIT) {
					return 1;
				}
			}
			else if (white_checkers[i].status == QUEEN) // äàìêà
			{
				left = white_checkers[i].location[0][0]; // êîîðäèíàòû äàííîé øàøêè
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
	else if (whose_move == RED) // åñëè õîä êðàñíûõ
	{
		for (int i = 0; i < 12; i++)
		{
			if (red_checkers[i].status == DEFAULT) // íå äàìêà
			{
				left = red_checkers[i].location[0][0]; // êîîðäèíàòû äàííîé øàøêè
				top = red_checkers[i].location[0][1];

				if (is_there_a_checker(left + 8, top + 4) == 0 && (left + 8) < RIGHT_LIMIT + 3 && (top + 4) < BOT_LIMIT + 2) {
					return 1;
				}
				if (is_there_a_checker(left - 8, top - 4) == 0 && (left - 8) > LEFT_LIMIT + 1 && (top + 4) < BOT_LIMIT + 2) {
					return 1;
				}
			}
			else if (red_checkers[i].status == QUEEN) // äàìêà
			{
				left = red_checkers[i].location[0][0]; // êîîðäèíàòû äàííîé øàøêè
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

// ïðîâåðêà, îáÿçàòåëüíî ëè íàäî åñòü
int necessary_to_beat()
{
	int left;
	int top;

	if (whose_move == WHITE) // åñëè õîäÿò áåëûå
	{
		for (int i = 0; i < 12; i++)
		{
			if (white_checkers[i].status == DEFAULT) // íå äàìêà
			{
				left = white_checkers[i].location[0][0]; // êîîðäèíàòû äàííîé øàøêè
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
			else if (white_checkers[i].status == QUEEN) // äàìêà
			{
				left = white_checkers[i].location[0][0]; // êîîðäèíàòû äàííîé øàøêè
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
	else if (whose_move == RED) // åñëè õîäÿò êðàñíûå
	{
		for (int i = 0; i < 12; i++)
		{
			if (red_checkers[i].status == DEFAULT) // íå äàìêà
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
			else if (red_checkers[i].status == QUEEN) // äàìêà
			{
				left = red_checkers[i].location[0][0]; // êîîðäèíàòû äàííîé øàøêè
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

// åñëè øàøêà äîøëà äî êîíöà äîñêè, òî îíà ñòàíîâèòñÿ äàìêîé
void became_a_queen(int top, int color)
{
	if (color) // êðàñíàÿ øàøêà
	{
		if (top == BOT_LIMIT) {
			red_checkers[checkers_number].status = 1;
		}
	}
	else // áåëàÿ øàøêà
	{
		if (top == TOP_LIMIT + 2) {
			white_checkers[checkers_number].status = 1;
		}
	}
	int a = con_height() / 2;
}

// ïðîâåðêà, ìîæíî ëè ñúåñòü øàøêó (âîçâðàùàåò 1, åñëè ìîæíî, èíà÷å 0)
int can_eat_more(int left, int top)
{
	int status = -1;
	if (whose_move == WHITE) // åñëè õîä áåëûõ
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

		if (status == DEFAULT) // íå äàìêà
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
		else if (status == 1) // äàìêà
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
	else if (whose_move == 2) // åñëè õîä êðàñíûõ
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

		if (status == 0) // íå äàìêà
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
		else if (status == 1) // äàìêà
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

// ïîìåíÿòü êîîðäèíàòû øàøêè
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

// ñäåëàòü õîä
void step(int old_left, int old_top, int new_left, int new_top, int check_color)
{
	gotoxy(old_left, old_top);
	con_set_color(NULL, CON_CLR_BLACK);
	printf("   ");
	gotoxy(new_left, new_top);
	if (check_color) { // êðàñíàÿ øàøêà
		if (red_checkers[checkers_number].status == 0) {
			became_a_queen(new_top, check_color);
		}
		con_set_color(NULL, CON_CLR_RED);
		if (red_checkers[checkers_number].status == 0) { // íå äàìêà
			printf("   ");
		}
		else if (red_checkers[checkers_number].status == 1) { // äàìêà
			printf("¦¤¦");
		}
	}
	else { // áåëàÿ øàøêà
		if (white_checkers[checkers_number].status == 0) {
			became_a_queen(new_top, check_color);
		}
		con_set_color(NULL, CON_CLR_WHITE);
		if (white_checkers[checkers_number].status == 0) { // íå äàìêà
			printf("   ");
		}
		else if (white_checkers[checkers_number].status == 1) { // äàìêà
			printf("¦¤¦");
		}
	}

	change_check_lokation(check_color, new_left, new_top); // ñìåíèòü êîîðäèíàòû øàøêè

	if (whose_move == WHITE) { // åñëè õîä ñäåëàëè áåëûå, òî ñ÷åò÷èê õîäîâ áåëûõ ++
		white_moves_num++;
	}
	else if (whose_move == RED) { // åñëè õîä ñäåëàëè ÷åðíûå, òî ñ÷åò÷èê õîäîâ ÷åðíûõ ++
		red_moves_num++;
	}
}

// íàéòè è óäàëèòü ñúåäåííóþ øàøêó
void find_and_delete_check(int old_left, int old_top, int new_left, int new_top)
{
	if (whose_move == 1) // åñëè õîä áåëûõ, èùåì ñðåäè êðàñíûõ øàøåê	
	{
		while (1)
		{
			if (new_left > old_left) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ ïðàâåå ñòàðîé
				new_left -= 8; // ñäâèãàåìñÿ âëåâî
			}
			else {
				new_left += 8;
			}

			if (new_top > old_top) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ íèæå ñòàðîé
				new_top -= 4; // ñäâèãàåìñÿ ââåðõ
			}
			else {
				new_top += 4;
			}

			if (new_left == old_left || new_top == old_top) // åñëè âåðíóëèñü â êëåòêó, îòêóäà ñîâåðøàåòñÿ õîä
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
					printf("   "); // ñòèðàåì ñúåäåííóþ øàøêó ñ äîñêè
					return;
				}
			}
		}
	}
	else if (whose_move == 2) // åñëè õîä êðàñíûõ
	{
		while (1)
		{
			if (new_left > old_left) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ ïðàâåå ñòàðîé
				new_left -= 8; // ñäâèãàåìñÿ âëåâî
			}
			else {
				new_left += 8;
			}

			if (new_top > old_top) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ íèæå ñòàðîé
				new_top -= 4; // ñäâèãàåìñÿ ââåðõ
			}
			else {
				new_top += 4;
			}

			if (new_left == old_left || new_top == old_top) // åñëè âåðíóëèñü â êëåòêó, îòêóäà ñîâåðøàåòñÿ õîä
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
					printf("   "); // ñòèðàåì ñúåäåííóþ øàøêó ñ äîñêè
					return;
				}
			}
		}
	}
}

// îïðåäåëÿåò, ìîæíî ëè ñúåñòü øàøêó (âîçâðàùàåò 2, åñëè ìîæíî, 1, åñëè äåëàåì ïðîñòîé õîä, èíà÷å 0)
int is_it_possible_eat(int old_left, int old_top, int new_left, int new_top, int stat)
{
	int flag = 0; // ôëàã, äëÿ ïîäñ÷åòà êîëè÷åñòâà øàøîê äî (new_left, new_top)
	if (whose_move == 2) // åñëè õîä êðàñíûõ
	{
		while (1)
		{
			if (new_left > old_left) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ ïðàâåå ñòàðîé
				new_left -= 8; // ñäâèãàåìñÿ âëåâî
			}
			else {
				new_left += 8;
			}

			if (new_top > old_top) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ íèæå ñòàðîé
				new_top -= 4; // ñäâèãàåìñÿ ââåðõ
			}
			else {
				new_top += 4;
			}

			if (new_left == old_left || new_top == old_top) // åñëè âåðíóëèñü â êëåòêó, îòêóäà ñîâåðøàåòñÿ õîä
			{
				break;
			}

			for (int i = 0; i < 12; i++)
			{
				if (red_checkers[i].location[0][0] == new_left && red_checkers[i].location[0][1] == new_top && red_checkers[i].status != 2) // åñëè ïî ïóòè åñòü ñâîÿ øàøêà
				{
					return 0;
				}
				else if (white_checkers[i].location[0][0] == new_left && white_checkers[i].location[0][1] == new_top && white_checkers[i].status != 2) // åñëè ïî ïóòè åòñü ÷óæàÿ øàøêà
				{
					flag++;
					break;
				}
			}

			if (flag > 1) { // åñëè ïî ïóòè áîëüøå îäíîé øàøêè
				return 0;
			}
		}
	}
	else if (whose_move == 1) // åñëè õîä áåëûõ
	{
		while (1)
		{
			if (new_left > old_left) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ ïðàâåå ñòàðîé
				new_left -= 8; // ñäâèãàåìñÿ âëåâî
			}
			else {
				new_left += 8;
			}

			if (new_top > old_top) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ íèæå ñòàðîé
				new_top -= 4; // ñäâèãàåìñÿ ââåðõ
			}
			else {
				new_top += 4;
			}

			if (new_left == old_left || new_top == old_top) // åñëè âåðíóëèñü â êëåòêó, îòêóäà ñîâåðøàåòñÿ õîä
			{
				break;
			}

			for (int i = 0; i < 12; i++)
			{
				if (white_checkers[i].location[0][0] == new_left && white_checkers[i].location[0][1] == new_top && white_checkers[i].status != 2) // åñëè ïî ïóòè åñòü ñâîÿ øàøêà
				{
					return 0;
				}
				else if (red_checkers[i].location[0][0] == new_left && red_checkers[i].location[0][1] == new_top && red_checkers[i].status != 2) // åñëè ïî ïóòè åñòü ÷óæàÿ øàøêà
				{
					flag++;
					break;
				}
			}

			if (flag > 1) { // åñëè ïî ïóòè áîëüøå îäíîé øàøêè
				return 0;
			}
		}
	}

	if (flag == 1) { // åñëè ïî ïóòè òîëüêî îäíà ÷óæàÿ øàøêà
		return 2;
	}
	else { // åñëè ïî ïóòè íåò øàøåê
		if (!stat) { // åñëè íå äàìêà
			return 0;
		}
		else { // åñëè äàìêà
			return 1;
		}
	}
}

// âîçìîæåí ëè äàííûé õîä
int is_action_permitted(int old_left, int old_top, int new_left, int new_top)
{
	int flag = 0;
	for (int i = 0; i < 12; i++)
	{
		// åñëè â êëåòêå, êóäà ïåðåñòàâëÿåòñÿ øàøêà, åñòü äðóãàÿ øàøêà
		if ((white_checkers[i].location[0][0] == new_left && white_checkers[i].location[0][1] == new_top && white_checkers[i].status != 2) ||
			(red_checkers[i].location[0][0] == new_left && red_checkers[i].location[0][1] == new_top && red_checkers[i].status != 2))
		{
			return 0;
		}

		if (white_checkers[i].location[0][0] == old_left && white_checkers[i].location[0][1] == old_top) // íàõîäèì øàøêó, êîòîðàÿ íàõîäèòñÿ â äàííîé êëåòêå
		{
			if (white_checkers[i].status == 0) // íå äàìêà
			{
				if ((new_top - old_top) != 0 && get_cell_color(new_left - 2, new_top - 1) == 1) {
					if (abs(new_left - old_left) / abs(new_top - old_top) == 2) {
						if ((abs(new_left - old_left) % 8) == 0 && (new_left - old_left) != 0) // äèàãîíàëüíàÿ êëåòêà
						{
							if ((old_top - new_top) == 4) // ïðîñòîé õîä
							{
								return 1;
							}
							else if (abs(old_top - new_top) == 8) // ïîïûòêà ñúåñòü äðóãóþ øàøêó
							{
								if (is_it_possible_eat(old_left, old_top, new_left, new_top, 0) == 2) { // åñëè âîçìîæíî ñúåñòü äðóãóþ øàøêó
									return 2;
								}
								else {
									return 0;
								}
							}
						}
					}
				}
				else { // åñëè ïûòàåìñÿ ïåðåéòè íà íåäèàãîíàëüíóþ êëåòêó
					return 0;
				}
			}
			else // äàìêà
			{
				if ((new_top - old_top) != 0 && get_cell_color(new_left - 2, new_top - 1) == 1) {
					if (abs(new_left - old_left) / abs(new_top - old_top) == 2) { // äèàãîíàëüíàÿ êëåòêà
						if ((abs(new_left - old_left) % 8) == 0 && (new_left - old_left) != 0)
						{
							if ((abs(new_top - old_top) % 4) == 0)
							{
								if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 2) { // åñëè âîçìîæíî ñúåñòü äðóãóþ øàøêó 
									return 2;
								}
								else if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 1) { // åñëè âîçìîæíî ñäåëàòü õîä
									return 1;
								}
								else if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 0) {
									return 0;
								}
							}
						}
					}
				}
				else { // åñëè ïûòàåìñÿ ïåðåéòè íà íåäèàãîíàëüíóþ êëåòêó 
					return 0;
				}
			}
		}
		else if (red_checkers[i].location[0][0] == old_left && red_checkers[i].location[0][1] == old_top)
		{
			if (red_checkers[i].status == 0) // íå äàìêà
			{
				if ((new_top - old_top) != 0 && get_cell_color(new_left - 2, new_top - 1) == 1) {
					if (abs(new_left - old_left) / abs(new_top - old_top) == 2) { // äèàãîíàëüíàÿ êëåòêà
						if ((abs(new_left - old_left) % 8) == 0 && (new_left - old_left) != 0) // äèàãîíàëüíàÿ êëåòêà
						{
							if ((new_top - old_top) == 4) // ïðîñòîé õîä
							{
								return 1;
							}
							else if (abs(new_top - old_top) == 8) // ïîïûòêà ñúåñòü äðóãóþ øàøêó
							{
								if (is_it_possible_eat(old_left, old_top, new_left, new_top, 0) == 2) { // åñëè âîçìîæíî ñúåñòü äðóãóþ øàøêó
									return 2;
								}
								else {
									return 0;
								}
							}
						}
					}
				}
				else { // åñëè ïûòàåìñÿ ïåðåéòè íà íåäèàãîíàëüíóþ êëåòêó
					return 0;
				}
			}
			else // äàìêà
			{
				if ((new_top - old_top) != 0 && get_cell_color(new_left - 2, new_top - 1) == 1) {
					if (abs(new_left - old_left) / abs(new_top - old_top) == 2) { // äèàãîíàëüíàÿ êëåòêà
						if ((abs(new_left - old_left) % 8) == 0 && (new_left - old_left) != 0)
						{
							if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 2) { // åñëè âîçìîæíî ñúåñòü äðóãóþ øàøêó 
								return 2;
							}
							else if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 1) { // åñëè âîçìîæíî ñäåëàòü õîä
								return 1;
							}
							else if (is_it_possible_eat(old_left, old_top, new_left, new_top, 1) == 0) {
								return 0;
							}
						}
					}
				}
				else { // åñëè ïûòàåìñÿ ïåðåéòè íà íåäèàãîíàëüíóþ êëåòêó 
					return 0;
				}
			}
		}
	}

	return 0;
}

// óçíàòü, åñòü ëè â äàííîé êëåòêå øàøêà (âîçâðàùàåò 1, åñëè åñòü áåëàÿ øàøêà, 2 - åñëè êðàñíàÿ, èíà÷å 0)
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

// âêëþ÷èòü/âûêëþ÷èòü ñòàòè÷åñêèé êóðñîð (åñëè çàõâàòèëè øàøêó)
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

// ìèãàþùèé êóðñîð
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
	if (key_is_pressed()) { // ÷òîáû íå áûëî çàäåðæêè ïðè áûñòðîì ïåðåäâèæåíèè êóðñîðà
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
	if (key_is_pressed()) { // ÷òîáû íå áûëî çàäåðæêè ïðè áûñòðîì ïåðåäâèæåíèè êóðñîðà
		return;
	}
	pause(350);
}

// ïîëó÷èòü êîîðäèíàòû êëåòîê
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

// ïîëó÷èòü öâåò è íîìåð øàøêè â äàííîé êëåòêå (âîçâðàùàåò 1, åñëè øàøêà êðàñíàÿ, èíà÷å 0)
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

// ïîëó÷èòü öâåò êëåòêè (âîçâðàùàåò 1, åñëè ÷åðíàÿ êëåòêà, 0 - áåëàÿ)
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

// ïîñòðîèòü äîñêó
void build_board()
{
	int left = (con_width() / 2) - 32;
	int top = (con_height() / 2) - 15;

	con_set_color(NULL, CON_CLR_YELLOW_LIGHT);
	clrscr();

	con_draw_lock(); // çàáëîêèðîâàòü îòðèñîâêó

	gotoxy(left, top);
	con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW);
	printf("*****************************************************************");
	top++;

	for (int count = 0; count < 8; count++) // êîëè÷åñòâî ñòðîê â äîñêå
	{
		for (int i = 0; i < 3; i++) // êîëè÷åñòâî ñòîëáöîâ äëÿ ïîñòðîåíèÿ îäíîãî ðÿäà
		{
			left = (con_width() / 2) - 32; // âîçâðàùàåì êóðñîð ê ëåâîìó êðàþ äîñêè
			if ((count % 2) == 0) // åñëè íîìåð ñòîëáöà ÷åòíûé, òî ïåðâàÿ êëåòêà äîëæíà áûòü áåëîé
			{
				for (int j = 0; j < 4; j++) // êîëè÷åñòâî êëåòîê â îäíîì ðÿäó / 2 (áóäåì ðèñîâàòü ñðàçó ïî äâå êëåòêè)
				{
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ãðàíèöà êëåòêè
					if (j != 0) {
						printf(" ");
					}
					else {
						printf("*");
					}
					left++; // ïåðåìåñòèëè êóðñîð íà îäíó êëåòêó âïðàâî
					gotoxy(left, top);
					con_set_color(NULL, CON_CLR_GRAY); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
					printf("       ");
					left += 7; // ïåðåìåñòèëè êóðñîð íà ñåìü êëåòîê âïðàâî
					con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ãðàíèöà êëåòêè
					printf(" ");
					left++; // ïåðåìåñòèëè êóðñîð íà îäíó êëåòêó âïðàâî  |ÄÎÐÈÑÎÂÀËÈ ÁÅËÓÞ ÊËÅÒÊÓ|

					if (i != 1) // åñëè êóðñîð íàõîäèòñÿ íå â öåíòðå êëåòêè, ïðîñòî çàêðàøèâàåì ÷àñòü êëåòêè
					{
						con_set_color(NULL, CON_CLR_BLACK); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
						printf("       ");
						left += 7; // ïåðåìåñòèëè êóðñîð íà ñåìü êëåòîê âïðàâî
					}
					else // åñëè êóðñîð íàõîäèòñÿ â öåíòðå êëåòêè
					{
						con_set_color(NULL, CON_CLR_BLACK); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
						printf("  ");
						left += 2; // ïåðåìåñòèëè êóðñîð íà äâå êëåòêè âïðàâî
						int flag = 0; // åñëè ôëàã = 1, çíà÷èò íà ýòîì ìåñòå ñòîèò øàøêà
						for (int n = 0; n < 12; n++) // ïðîõîäèìñÿ ïî âñåì øàøêàì
						{
							if (red_checkers[n].location[0][0] == left && red_checkers[n].location[0][1] == top) // åñëè íà ýòîì ìåñòå äîëæíà íàõîäèòüñÿ êðàñíàÿ øàøêà
							{
								flag = 1;
								if (red_checkers[n].status == 0) // åñëè ýòà øàøêà - íå äàìêà
								{
									gotoxy(left, top);
									con_set_color(NULL, CON_CLR_RED); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
									printf("   ");
								}
								else if (red_checkers[n].status == 1) // åñëè ýòà øàøêà - äàìêà
								{
									gotoxy(left, top);
									con_set_color(CON_CLR_BLACK, CON_CLR_RED);
									printf("¦¤¦");
								}
								left += 3; // ïåðåìåñòèëè êóðñîð íà òðè êëåòêè âïðàâî
							}
							else if (white_checkers[n].location[0][0] == left && white_checkers[n].location[0][1] == top) // åñëè íà ýòîì ìåñòå äîëæíà íàõîäèòüñÿ áåëàÿ øàøêà
							{
								flag = 1;
								if (white_checkers[n].status == 0) // åñëè ýòà øàøêà - íå äàìêà
								{
									gotoxy(left, top);
									con_set_color(NULL, CON_CLR_WHITE); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
									printf("   ");
								}
								else if (white_checkers[n].status == 1) // åñëè ýòà øàøêà - äàìêà
								{
									gotoxy(left, top);
									con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
									printf("¦¤¦");
								}
								left += 3; // ïåðåìåñòèëè êóðñîð íà òðè êëåòêè âïðàâî
							}
						}
						if (flag) // åñëè â êëåòêó áûëà ïîìåùåíà øàøêà
						{
							con_set_color(NULL, CON_CLR_BLACK); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
							printf("  ");
							left += 2; //ïåðåìåñòèëè êóðñîð íà äâå êëåòêè âïðàâî
						}
						else
						{
							con_set_color(NULL, CON_CLR_BLACK); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
							printf("     ");
							left += 5; //ïåðåìåñòèëè êóðñîð íà ïÿòü êëåòîê âïðàâî
						}
					}
					if (j == 3) {
						con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ïðàâàÿ ãðàíèöà äîñêè
						printf("*");
						top++; // ïåðåìåñòèëè êóðñîð íà îäíó êëåòêó âíèç
					}
				}
			}
			else // åñëè íîìåð ñòîëáöà íå÷åòíûé, òî ïåðâàÿ êëåòêà äîëæíà áûòü ÷åðíîé
			{
				for (int j = 0; j < 4; j++) // êîëè÷åñòâî êëåòîê â îäíîì ðÿäó / 2 (áóäåì ðèñîâàòü ñðàçó ïî äâå êëåòêè)
				{
					gotoxy(left, top);
					con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ãðàíèöà êëåòêè
					if (j != 0) {
						printf(" ");
					}
					else {
						printf("*");
					}
					left++; // ïåðåìåñòèëè êóðñîð íà îäíó êëåòêó âïðàâî
					if (i != 1) // åñëè êóðñîð íàõîäèòñÿ íå â öåíòðå êëåòêè, ïðîñòî çàêðàøèâàåì ÷àñòü êëåòêè
					{
						con_set_color(NULL, CON_CLR_BLACK); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
						printf("       ");
						left += 7; // ïåðåìåñòèëè êóðñîð íà ñåìü êëåòîê âïðàâî
						con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ãðàíèöà êëåòêè
						printf(" ");
						left++; // ïåðåìåñòèëè êóðñîð íà îäíó êëåòêó âïðàâî
					}
					else // åñëè êóðñîð íàõîäèòñÿ â öåíòðå êëåòêè
					{
						con_set_color(NULL, CON_CLR_BLACK); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
						printf("  ");
						left += 2; // ïåðåìåñòèëè êóðñîð íà äâå êëåòêè âïðàâî
						int flag = 0; // åñëè ôëàã = 1, çíà÷èò íà ýòîì ìåñòå ñòîèò øàøêà
						for (int n = 0; n < 12; n++) // ïðîõîäèìñÿ ïî âñåì øàøêàì
						{
							if (red_checkers[n].location[0][0] == left && red_checkers[n].location[0][1] == top) // åñëè íà ýòîì ìåñòå äîëæíà íàõîäèòüñÿ êðàñíàÿ øàøêà
							{
								flag = 1;
								if (red_checkers[n].status == 0) // åñëè ýòà øàøêà - íå äàìêà
								{
									gotoxy(left, top);
									con_set_color(NULL, CON_CLR_RED); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
									printf("   ");
								}
								else if (red_checkers[n].status == 1) // åñëè ýòà øàøêà - äàìêà
								{
									gotoxy(left, top);
									con_set_color(CON_CLR_BLACK, CON_CLR_RED);
									printf("¦¤¦");
								}
								left += 3; // ïåðåìåñòèëè êóðñîð íà òðè êëåòêè âïðàâî
							}
							else if (white_checkers[n].location[0][0] == left && white_checkers[n].location[0][1] == top) // åñëè íà ýòîì ìåñòå äîëæíà íàõîäèòüñÿ áåëàÿ øàøêà
							{
								flag = 1;
								if (white_checkers[n].status == 0) // åñëè ýòà øàøêà - íå äàìêà
								{
									gotoxy(left, top);
									con_set_color(NULL, CON_CLR_WHITE); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
									printf("   ");
								}
								else if (white_checkers[n].status == 1) // åñëè ýòà øàøêà - äàìêà
								{
									gotoxy(left, top);
									con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
									printf("¦¤¦");
								}
								left += 3; // ïåðåìåñòèëè êóðñîð íà òðè êëåòêè âïðàâî
							}
						}
						if (flag) // åñëè â êëåòêó áûëà ïîìåùåíà øàøêà
						{
							con_set_color(NULL, CON_CLR_BLACK); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
							printf("  ");
							left += 2; //ïåðåìåñòèëè êóðñîð íà äâå êëåòêè âïðàâî
						}
						else
						{
							con_set_color(NULL, CON_CLR_BLACK); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
							printf("     ");
							left += 5; //ïåðåìåñòèëè êóðñîð íà ïÿòü êëåòîê âïðàâî
						}
						con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ïðàâàÿ ãðàíèöà äîñêè
						printf(" ");
						left++; // ïåðåìåñòèëè êóðñîð íà îäíó êëåòêó âïðàâî
					}
					con_set_color(NULL, CON_CLR_GRAY); // NULL - ò.ê. ñèìâîëû íå èñïîëüçóþòñÿ
					printf("       ");
					left += 7; // ïåðåìåñòèëè êóðñîð íà ñåìü êëåòîê âïðàâî
					if (j == 3) {
						con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW); // ïðàâàÿ ãðàíèöà äîñêè
						printf("*");
						top++; // ïåðåìåñòèëè êóðñîð íà îäíó êëåòêó âíèç
					}
				}
			}
		}
		left = (con_width() / 2) - 32;
		gotoxy(left, top);
		con_set_color(CON_CLR_BLACK, CON_CLR_YELLOW);
		if (count != 7) {
			printf("*                                                               *"); // ãðàíèöà ìåæäó ñòðîêàìè
		}
		else {
			printf("*****************************************************************");
		}
		top++; // ïåðåìåñòèëè êóðñîð íà îäíó êëåòêó âíèç
	}

	con_draw_release(); // ðàçáëîêèðîâàòü îòðèñîâêó (âûâåñòè íà ýêðàí)
}

// ñîõðàíèòü èãðó
void save_the_game()
{
	char filename[30];

	int left = 60;
	int top = 15;

	// Î÷èñòêà ýêðàíà
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	clrscr();
	gotoxy(left, top);

	printf("Ââåäèòå íàçâàíèå ñîõðàíåíèÿ: ");
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
		printf("Íå óäàëîñü ñîõðàíèòü èãðó.");
		Sleep(1000);
		return 0;
	}

	if (strstr(filename, "."))
	{
		left = 60;
		top = 15;
		clrscr();
		gotoxy(left, top);
		printf("Èñïîëüçîâàí íåäîïóñòèìûé ñèìâîë.");
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
	printf("Èãðà ñîõðàíåíà");
	Sleep(1000);
}

// çàãðóçèòü èãðó
int load_the_game()
{
	char filename[30];
	int left = 60;
	int top = 15;

	// Î÷èñòêà ýêðàíà
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	clrscr();
	gotoxy(left, top);

	printf("Ââåäèòå íàçâàíèå ñîõðàíåííîé èãðû: ");
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
		printf("Óêàçàííîå ñîõðàíåíèå íå íàéäåíî.");
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

// îòêðûòü ñïðàâêó
void reference()
{
	// Î÷èñòêà ýêðàíà
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	clrscr();
	gotoxy(0, 0);

	printf("								Øàøêè.\n");
	printf("				Ïðàâèëà èãðû :\n");
	printf("		1) Èãðà âåä¸òñÿ íà äîñêå 8õ8 êëåòîê, òîëüêî íà ÷åðíûõ ÿ÷åéêàõ.\n");
	printf("		2) Øàøêè â íà÷àëå èãðû çàíèìàþò ïåðâûå òðè ðÿäà ñ êàæäûé ñòîðîíû.\n");
	printf("		3) Áèòü ìîæíî ïðîèçâîëüíîå êîëè÷åñòâî øàøåê â ëþáûõ íàïðàâëåíèÿõ.\n");
	printf("		4) Ïðîñòûå øàøêè õîäÿò òîëüêî âïåð¸ä.\n");
	printf("		5) Ïðîñòàÿ øàøêà ìîæåò ñðóáèòü(\"áèòü\", \"åñòü\") íàçàä.\n");
	printf("		6) Äàìêà õîäèò íà ëþáîå ÷èñëî ïîëåé â ëþáóþ ñòîðîíó.\n");
	printf("		7) Ïðîèãðûâàåò òîò, ó êîãî íå îñòàåòñÿ ôèãóð, ëèáî õîäîâ.\n");
	printf("		8) Øàøêà ñíèìàåòñÿ ñ ïîëÿ ïîñëå áîÿ(ìîæíî ïåðåôðàçèðîâàòü òàê : îäíó øàøêè íåëüçÿ ñðóáèòü äâàæäû çà\n");
	printf("		îäèí õîä).\n");
	printf("		9) Áèòü îáÿçàòåëüíî.\n");
	printf("		10) Øàøêà ïðåâðàùàåòñÿ â äàìêó, äîñòèãíóâ âîñüìîé(äëÿ áåëûõ) èëè ïåðâîé(äëÿ ÷åðíûõ) ëèíèè äîñêè.\n");
	printf("		11) Åñëè øàøêà âî âðåìÿ áîÿ ïðîõîäèò ÷åðåç äàìî÷íîå ïîëå, òî îíà ïðåâðàùàåòñÿ â äàìêó è\n");
	printf("		ñëåäóþùèå áîè(åñëè îíè âîçìîæíû) ñîâåðøàåò óæå êàê äàìêà.\n\n\n\n");

	printf("				Óïðàâëåíèå:\n");
	printf("		Øàøêè áåëîãî è êðàñíîãî öâåòà.Êîãäà ÷åëîâåê õîäèò – îí íàâîäèò ôîêóñ(ôîêóñíàÿ êëåòêà èìååò\n");
	printf("		äðóãîé öâåò ôîíà – ìèãàåò äèíàìè÷åñêè), çàõâàòûâàåò øàøêó(ïðîáåëîì), íàâîäèò ôîêóñ – êóäà\n");
	printf("		ñõîäèòü, íàæèìàåò ïðîáåë.\n");
	printf("		Åñëè ïðè ýòîì ìîæíî ïðîäîëæèòü, íàïðèìåð, ñúåñòü åùå îäíó – òî îí ìîæåò çàâåðøèòü õîä(êíîïêîé\n");
	printf("		Enter), ëèáî ïðîäîëæèòü \"åñòü\" – íàâåñòè ôîêóñ â ñëåäóþùóþ êëåòêó è íàæàòü ïðîáåë.");

	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_ESC || code == KEY_ENTER)
		{
			break;
		}
	}
}

// ïðè íàæàòèè esc îòêðûâàþòñÿ ìåíþ
void esc_menu()
{
	int left = 65;
	int top = 15;

	// Î÷èñòêà ýêðàíà
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	clrscr();

	con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*   Ïðîäîëæèòü èãðó   *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*   Ñîõðàíèòü èãðó    *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*   Â ãëàâíîå ìåíþ    *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*        Âûõîä        *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	left = 65;
	top = 17;
	int position = 0;
	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_UP) // Åñëè ýòî ñòðåëêà ââåðõ
		{
			// Òî ïåðåõîä ê âåðõíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
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
					printf("*   Ñîõðàíèòü èãðó    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   Ïðîäîëæèòü èãðó   *");
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
					printf("*   Â ãëàâíîå ìåíþ    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   Ñîõðàíèòü èãðó    *");
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
					printf("*        Âûõîä        *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   Â ãëàâíîå ìåíþ    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				}
				position--;
			}
		}
		else if (code == KEY_DOWN) // Åñëè ñòðåëêà âíèç
		{
			// Òî ïåðåõîä ê íèæíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
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
					printf("*   Â ãëàâíîå ìåíþ    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*        Âûõîä        *");
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
					printf("*   Ñîõðàíèòü èãðó    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   Â ãëàâíîå ìåíþ    *");
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
					printf("*   Ïðîäîëæèòü èãðó   *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   Ñîõðàíèòü èãðó    *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				}
				position++;
			}
		}
		else if (code == KEY_ESC) // ESC - âûõîä
		{
			build_board();
			// âåðíóòü äîñêó
			return;
		}
		else if (code == KEY_ENTER) // Íàæàòà êíîïêà Enter
		{
			if (position == 3) { // Âûáðàí ïîñëåäíèé ïóíêò - ýòî "âûõîä"
				exit(10);
			}
			if (position == 0) {
				build_board();
				// âåðíóòü äîñêó
				return;
			}
			if (position == 1) {
				save_the_game();
				esc_menu();
				// ñîõðàíèòü
				return;
			}
			if (position == 2) {
				end = 1;
				cycle_end = 1;
				// â ãëàâíîå ìåíþ
			}
			break;
		}
	}
}

// ïåðåìåùåíèå êóðñîðà ïî äîñêå
void move_cursor(int left, int top)
{
	gotoxy(left, top);

	while (1)
	{
		if (cycle_end) {
			break;
		}

		int color = get_cell_color(left, top); // ïîëó÷èòü öâåò êëåòêè, â êîòîðóþ íàâåëè êóðñîð
		if (left != sel_check_left_pos || top != sel_check_top_pos) { // åñëè ìû íàâåëè êóðñîð íà êëåòêó, â êîòîðîé óæå çàõâàòèëè øàøêó, îíà ïåðåñòàåò ìèãàòü
			blinking_cursor(left, top, color);
		}
		if (key_is_pressed()) // åñëè íàæàëè êëàâèøó
		{
			int code = key_pressed_code(); // ïîëó÷èòü êîä íàæàòîé êëàâèøè
			switch (code)
			{
			case KEY_ESC:
				esc_menu();
				// îòðèñîâàòü ìåíþ
				break;
			case KEY_UP:
				if (top - 4 > TOP_LIMIT) { // åñëè íå âûõîäèì çà ïðåäåë äîñêè
					top -= 4; // ïîäíèìàåìñÿ ââåðõ íà 1 êëåòêó
					move_cursor(left, top);
				}
				break;
			case KEY_DOWN:
				if (top + 4 < BOT_LIMIT) { // åñëè íå âûõîäèì çà ïðåäåë äîñêè
					top += 4; // ñïóñàåìñÿ âíèç íà 1 êëåòêó
					move_cursor(left, top);
				}
				break;
			case KEY_LEFT:
				if (left - 8 > LEFT_LIMIT) { // åñëè íå âûõîäèì çà ïðåäåë äîñêè
					left -= 8; // ñìåùàåìñÿ âëåâî íà 1 êëåòêó
					move_cursor(left, top);
				}
				break;
			case KEY_RIGHT:
				if (left + 8 < RIGHT_LIMIT) { // åñëè íå âûõîäèì çà ïðåäåë äîñêè
					left += 8; // ñìåùàåìñÿ âïðàâî íà 1 êëåòêó
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
			case KEY_SPACE: // íàæàò ïðîáåë 
				if (is_there_a_checker(left + 2, top + 1) == whose_move && eat_more == 0) // åñëè â äàííîé êëåòêå åñòü øàøêà (çàõâàòèòü/îòïóñòèòü øàøêó)
				{
					if (!on) { // åñëè øàøêà íå çàõâà÷åíà
						steady_cursor(left, top, 1); // âêëþ÷èòü ñòàòè÷åñêèé êóðñîð
						sel_check_left_pos = left; // çàïîìèíàåì êîîðäèíàòû âêëþ÷åííîãî êóðñîðà
						sel_check_top_pos = top;
					}
					else if (on)
					{
						if (left == sel_check_left_pos && top == sel_check_top_pos) { // åñëè äàííàÿ øàøêà óæå çàõâà÷åíà
							steady_cursor(left, top, 0); // âûêëþ÷èòü ñòàòè÷åñêèé êóðñîð
							sel_check_left_pos = 0; // îáíóëÿåì êîîðäèíàòû âêëþ÷åííîãî êóðñîðà
							sel_check_top_pos = 0;
						}
						else { // ïðè çàõâà÷åííîé øàøêå, çàõâàòûâàåì äðóãóþ
							steady_cursor(sel_check_left_pos, sel_check_top_pos, 0); // âûêëþþ÷èòü êóðñîð â êëåòêå ðàíåå âûáðàííîé øàøêè
							steady_cursor(left, top, 1); // âêëþ÷èòü ñòàòè÷åñêèé êóðñîð â íîâîé êëåòêå
							sel_check_left_pos = left; // çàïîìèíàåì êîîðäèíàòû âêëþ÷åííîãî êóðñîðà
							sel_check_top_pos = top;
						}
					}
				}
				else if (is_there_a_checker(left + 2, top + 1) == 0)
				{
					if (is_action_permitted(sel_check_left_pos + 2, sel_check_top_pos + 1, left + 2, top + 1) == 1 && eat_more == 0 && need_to_beat == 0) // åñëè äàííûé õîä âîçìîæåí
					{
						steady_cursor(sel_check_left_pos, sel_check_top_pos, 0); // âûêëþ÷èòü ñòàòè÷åñêèé êóðñîð
						on = 0; // îáíóëèòü ôëàã î âêëþ÷åíèè ñòàòè÷åñêîãî êóðñîðà
						step(sel_check_left_pos + 2, sel_check_top_pos + 1, left + 2, top + 1, get_check_color(sel_check_left_pos + 2, sel_check_top_pos + 1), checkers_number); // ïåðåðèñîâàòü øàøêó â íîâîé êëåòêå
						sel_check_left_pos = 0; // îáíóëÿåì êîîðäèíàòû âêëþ÷åííîãî êóðñîðà
						sel_check_top_pos = 0;
						cycle_end = 1;
					}
					else if (is_action_permitted(sel_check_left_pos + 2, sel_check_top_pos + 1, left + 2, top + 1) == 2) // åñëè âîçìæíî ñúåñòü
					{
						steady_cursor(sel_check_left_pos, sel_check_top_pos, 0); // âûêëþ÷èòü ñòàòè÷åñêèé êóðñîð
						on = 0; // îáíóëèòü ôëàã î âêëþ÷åíèè ñòàòè÷åñêîãî êóðñîðà
						step(sel_check_left_pos + 2, sel_check_top_pos + 1, left + 2, top + 1, get_check_color(sel_check_left_pos + 2, sel_check_top_pos + 1)); // ïåðåðèñîâàòü øàøêó â íîâîé êëåòêå
						find_and_delete_check(sel_check_left_pos + 2, sel_check_top_pos + 1, left + 2, top + 1); // íàéòè è óäàëèòü ñúåäåííóþ øàøêó
						if (can_eat_more(left + 2, top + 1)) // åñëè ìîæíî ñúåñòü åùå
						{
							eat_more = 1;
							sel_check_left_pos = left;
							sel_check_top_pos = top;
							steady_cursor(sel_check_left_pos, sel_check_top_pos, 1);
							move_cursor(left, top);
						}
						else {
							sel_check_left_pos = 0; // îáíóëÿåì êîîðäèíàòû âêëþ÷åííîãî êóðñîðà
							sel_check_top_pos = 0;
							eat_more = 0;
							cycle_end = 1;
						}
					}
				}
				else if (is_there_a_checker(left + 2, top + 1) != whose_move) // åñëè ïûòàåìñÿ çàõâàòèòü ÷óæóþ ïåøêó
				{
				}

				while (key_is_pressed())
					key_pressed_code();
				break;
			}
		}
	}
}

// ïðèñâîèòü íà÷àëüíûå çíà÷åíèÿ øàøêàì
void get_start_values()
{
	int left_edge = (con_width() / 2) - 21; // âû÷èñÿëåì ñåðåäèíó âåðõíåé ÷åðíîé êëåòêè îòíîñèòåëüíî ëåâîãî êðàÿ êîíñîëè
	int top_edge = (con_height() / 2) - 13; // âû÷èñÿëåì ñåðåäèíó âåðõíåé ÷åðíîé êëåòêè îòíîñèòåëüíî âåðõíåãî êðàÿ êîíñîëè

	int left = left_edge;
	int top = top_edge;

	for (int i = 11; i > -1; i--) // êðàñíûå øàøêè
	{
		red_checkers[i].status = 0;

		// ïðèñâàèâàåì íà÷àëüíûå êîîðäèíàòû êðàñíûì øàøêàì
		red_checkers[i].location[0][0] = left;
		red_checkers[i].location[0][1] = top;

		left += 16; // ïåðåõîäèì ê ñëåäóþùåé ÷åðíîé êëåòêå
		if (i == 8) { // êîíåö ïåðâîé ñòðîêè
			left = left_edge - 8; // ïåðâàÿ ÷åðíàÿ êëåòêà ñëåäóþùåé ñòðîêè
			top += 4;
		}
		else if (i == 4) { // êîíåö âòîðîé ñòðîêè
			left = left_edge; // ïåðâàÿ ÷åðíàÿ êëåòêà ñëåäóþùåé ñòðîêè
			top += 4;
		}
	}

	left = left_edge - 8;
	top = top_edge + 28;
	for (int i = 0; i < 12; i++) // áåëûå øàøêè
	{
		white_checkers[i].status = 0;

		// ïðèñâàèâàåì íà÷àëüíûå êîîðäèíàòû áåëûì øàøêàì
		white_checkers[i].location[0][0] = left;
		white_checkers[i].location[0][1] = top;

		left += 16; // ïåðåõîäèì ê ñëåäóþùåé ÷åðíîé êëåòêå
		if (i == 3) // êîíåö 6 ñòðîêè
		{
			left = left_edge; // ïåðâàÿ ÷åðíàÿ êëåòêà ñëåäóþùåé ñòðîêè
			top -= 4;
		}
		else if (i == 7) { // êîíåö 7 ñòðîêè
			left = left_edge - 8; // ïåðâàÿ ÷åðíàÿ êëåòêà ñëåäóþùåé ñòðîêè
			top -= 4;
		}
	}
}

// äëÿ ïðîäîëæåíèÿ íóæíî íàæàòü ëþáóþ êëàâèøó
void my_pause()
{
	key_pressed_code();
	return;
}

// îáúÿâëåíèå ïîáåäû áåëûõ
void white_win()
{
	int top = 25;
	int left = 61;

	gotoxy(left, top);
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	printf("*****************************");

	top++;
	gotoxy(left, top);
	printf("*      Ïîáåäèëè áåëûå       *");

	top++;
	gotoxy(left, top);
	printf("*****************************");

	while (!key_pressed_code()) { // ïàóçà äî íàæàòèÿ ëþáîé êëàâèøè
	}
}

// îáúÿâëåíèå ïîáåäû êðàñíûõ
void red_win()
{
	int top = 25;
	int left = 61;

	gotoxy(left, top);
	con_set_color(CON_CLR_WHITE, CON_CLR_RED); // öâåò øðèôòà, öâåò êîíñîëè
	printf("*****************************");

	top++;
	gotoxy(left, top);
	printf("*     Ïîáåäèëè êðàñíûå      *");

	top++;
	gotoxy(left, top);
	printf("*****************************");

	while (!key_pressed_code()) { // ïàóçà äî íàæàòèÿ ëþáîé êëàâèøè
	}
}

// îáíîâèòü òàáëèöó ðåêîðäîâ
void high_score_table_update(int whose_win)
{
	// Î÷èñòêà ýêðàíà
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	clrscr();
	gotoxy(60, 15);

	char username[50];

	printf("Ââåäèòå èìÿ ïîëüçîâàòåëÿ: ");
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

// èãðà ÷åëîâåêà ñ ÷åëîâåêîì
void human_vs_human()
{
	white_moves_num = 0;
	red_moves_num = 0;
	white_check_num = 12;
	red_check_num = 12;
	if (!is_load) { // åñëè íà÷àëàñü íîâàÿ èãðà
		get_start_values(); // ïðèñâîèòü íà÷àëüíûå çíà÷åíèÿ øàøêàì
	}
	build_board(); // ïîñòðîèòü äîñêó
	get_cells_locations(); // ïîëó÷èòü êîîðäèíàòû êëåòîê

	while (1)
	{
		if (end) // êîíåö èãðû
		{
			end = 0;
			break;
		}
		cycle_end = 0; // îáíóëÿåì ôëàã äëÿ öèêëà â ôóíêöèè move_cursor()

		need_to_beat = necessary_to_beat(); // ïðîâåðêà, îáÿçàòåëüíî ëè åñòü

		if (white_check_num == 0 || (whose_move == WHITE && !need_to_beat && !no_move()))
		{
			// ïîáåäà êðàñíûõ
			red_win();
			high_score_table_update(WHITE);
			break;
		}
		else if (red_check_num == 0 || (whose_move == RED && !need_to_beat && !no_move()))
		{
			// ïîáåäà áåëûõ
			white_win();
			high_score_table_update(RED);
			break;
		}

		move_cursor(LEFT_BOARD_CENTER, TOP_BOARD_CENTER);

		if (whose_move == RED) { // åñëè áûë õîä êðàñíûõ
			whose_move = WHITE; // ñëåäóþùèé õîä áåëûõ
		}
		else if (whose_move == WHITE) { // åñëè áûë õîä áåëûõ
			whose_move = RED; // ñëåäóþùèé õîä êðàñíûõ
		}
	}
}

// óäàëèòü ñúåäåííóþ øàøêó (äëÿ èãðû ñ êîìïüþòåðîì)
BOARD checker_delete(BOARD board, int left, int top, int color)
{
	if (color == 1) // áåëûå øàøêè
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
	else if (color == 2) // êðàñíûå øàøêè
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

// óçíàòü, åñòü ëè â äàííîé êëåòêå øàøêà (âîçâðàùàåò 1, åñëè åñòü áåëàÿ øàøêà, 2 - åñëè êðàñíàÿ, èíà÷å 0) (äëÿ èãðû ñ êîìïüþòåðîì)
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

// íàéòè è óäàëèòü ñúåäåííóþ øàøêó (äëÿ èãðû ñ êîìïüþòåðîì)
NODE find_and_delete(NODE node, int who_move, int old_left, int old_top, int new_left, int new_top)
{
	int nl, nt, ol, ot;
	nl = new_left; nt = new_top; ol = old_left; ot = old_top;
	if (who_move == 1) // åñëè õîä áåëûõ, èùåì ñðåäè êðàñíûõ øàøåê	
	{
		while (1)
		{
			if (nl > ol) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ ïðàâåå ñòàðîé
				nl -= 8; // ñäâèãàåìñÿ âëåâî
			}
			else {
				nl += 8;
			}

			if (nt > ot) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ íèæå ñòàðîé
				nt -= 4; // ñäâèãàåìñÿ ââåðõ
			}
			else {
				nt += 4;
			}

			if (nl == ol || nt == ot) // åñëè âåðíóëèñü â êëåòêó, îòêóäà ñîâåðøàåòñÿ õîä
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
	else if (who_move == 2) // åñëè õîä êðàñíûõ
	{
		while (1)
		{
			if (nl > ol) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ ïðàâåå ñòàðîé
				nl -= 8; // ñäâèãàåìñÿ âëåâî
			}
			else {
				nl += 8;
			}

			if (nt > ot) { // åñëè íîâàÿ êëåòêà íàõîäèòñÿ íèæå ñòàðîé
				nt -= 4; // ñäâèãàåìñÿ ââåðõ
			}
			else {
				nt += 4;
			}

			if (nl == ol || nt == ot) // åñëè âåðíóëèñü â êëåòêó, îòêóäà ñîâåðøàåòñÿ õîä
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

// ïðîâåðêà, ìîæíî ëè ñúåñòü åùå (âîçâðàùàåò 1, åñëè ìîæíî, èíà÷å 0) (äëÿ èãðû ñ êîìïüþòåðîì)
MOVE more_eat(BOARD board, int who_move, int left, int top)
{
	MOVE result;
	result.coord[0] = 0; result.coord[1] = 0;
	result.coord[2] = 0; result.coord[3] = 0;

	if (who_move == 0) // åñëè õîä áåëûõ
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
	else if (who_move == 1) // åñëè õîä êðàñíûõ
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

// ïðîâåðêà, îáÿçàòåëüíî ëè íàäî åñòü (äëÿ èãðû ñ êîìïüþòåðîì)
int must_beat(BOARD board, int color)
{
	int left;
	int top;

	if (color == 0) // åñëè õîäÿò áåëûå
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.whites[i].status == DEFAULT) // íå äàìêà
			{
				left = board.whites[i].location[0][0]; // êîîðäèíàòû äàííîé øàøêè
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
			else if (board.whites[i].status == QUEEN) // äàìêà
			{
				left = board.whites[i].location[0][0]; // êîîðäèíàòû äàííîé øàøêè
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
	else if (color == 1) // åñëè õîäÿò êðàñíûå
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.reds[i].status == DEFAULT) // íå äàìêà
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
			else if (board.reds[i].status == QUEEN) // äàìêà
			{
				left = board.reds[i].location[0][0]; // êîîðäèíàòû äàííîé øàøêè
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

// îáíîâèòü êîîðëèíàòû øàøêè
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

// ñäåëàòü êîïèþ äîñêè
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

// ïðîâåðêà, ñúåëèëè ëè øàøêó (âîçâðàùàåò 1 - åñëè ñúåëè, 2 - åñëè ñúåëè äàìêó, èíà÷å 0) (äëÿ èãðû ñ êîìïüþòåðîì)
int check_hit(BOARD board, MOVE move)
{
	int left_change = 8;
	int top_change = 4;

	if (move.color == 1) // áåëàÿ øàøêà
	{
		int left = move.coord[0];
		int top = move.coord[1];

		if (move.coord[2] > move.coord[0]) // åñëè íîâàÿ êëåòêà ïðàâåå ñòàðîé
		{
			if (move.coord[3] > move.coord[1]) // åñëè íîâàÿ êëåòêà íèæå ñòàðîé
			{
				while (left + left_change < move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.reds[i].location[0][0] == left + left_change && board.reds[i].location[0][1] == top + top_change) {
							if (board.reds[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // åñëè ñúåëè ïîòåíöèàëüíóþ äàìêó
									if (left + left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left + left_change - 8, top + top_change + 4) == 0) { // åñëè ëåâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
										return 2;
									}
									if (left + left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left + left_change + 8, top + top_change + 4) == 0) { // åñëè ïðàâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
										return 2;
									}
								}
								return 1; // åñëè ñúåëè îáû÷íóþ øàøêó
							}
							else if (board.reds[i].status == 1) { // åñëè ñúåëè äàìêó
								return 2;
							}
						}
					}
					left_change += 8;
					top_change += 4;
				}
			}
			else // åñëè íîâàÿ êëåòêà âûøå ñòàðîé
			{
				while (left + left_change < move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.reds[i].location[0][0] == left + left_change && board.reds[i].location[0][1] == top - top_change) {
							if (board.reds[i].status == 0) {
								if (top - top_change + 4 == BOT_LIMIT) { // åñëè ñúåëè ïîòåíöèàëüíóþ äàìêó
									if (left + left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left + left_change - 8, top - top_change + 4) == 0) { // åñëè ëåâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
										return 2;
									}
									if (left + left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left + left_change + 8, top - top_change + 4) == 0) { // åñëè ïðàâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
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
		else // åñëè íîâàÿ êëåòêà ëåâåå ñòàðîé
		{
			int left = move.coord[0];
			int top = move.coord[1];

			if (move.coord[3] > move.coord[1]) // åñëè íîâàÿ êëåòêà íèæå ñòàðîé
			{
				while (left - left_change > move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.reds[i].location[0][0] == left - left_change && board.reds[i].location[0][1] == top + top_change) {
							if (board.reds[i].status == 0) { // íå äàìêà
								if (top + top_change + 4 == BOT_LIMIT) { // åñëè ñúåëè ïîòåíöèàëüíóþ äàìêó
									if (left - left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) { // åñëè ëåâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
										return 2;
									}
									if (left - left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left - left_change + 8, top + top_change + 4) == 0) { // åñëè ïðàâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
										return 2;
									}
								}
								return 1;
							}
							else if (board.reds[i].status == 1) { // äàìêà
								return 2;
							}
						}
					}
					left_change += 8;
					top_change += 4;
				}
			}
			else // åñëè íîâàÿ êëåòêà âûøå ñòàðîé
			{
				while (left - left_change > move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.reds[i].location[0][0] == left - left_change && board.reds[i].location[0][1] == top - top_change) {
							if (board.reds[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // åñëè ñúåëè ïîòåíöèàëüíóþ äàìêó
									if (left - left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left - left_change - 8, top - top_change + 4) == 0) { // åñëè ëåâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
										return 2;
									}
									if (left - left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left - left_change + 8, top - top_change + 4) == 0) { // åñëè ïðàâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
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
	else if (move.color == 2) // êðàñíàÿ øàøêà
	{
		int left = move.coord[0];
		int top = move.coord[1];

		if (move.coord[2] > move.coord[0]) // åñëè íîâàÿ êëåòêà ïðàâåå ñòàðîé
		{
			if (move.coord[3] > move.coord[1]) // åñëè íîâàÿ êëåòêà íèæå ñòàðîé
			{
				while (left + left_change < move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.whites[i].location[0][0] == left + left_change && board.whites[i].location[0][1] == top + top_change) {
							if (board.whites[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // åñëè ñúåëè ïîòåíöèàëüíóþ äàìêó
									if (left + left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left + left_change - 8, top + top_change + 4) == 0) { // åñëè ëåâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
										return 2;
									}
									if (left + left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left + left_change + 8, top + top_change + 4) == 0) { // åñëè ïðàâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
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
			else // åñëè íîâàÿ êëåòêà âûøå ñòàðîé
			{
				while (left + left_change < move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.whites[i].location[0][0] == left + left_change && board.whites[i].location[0][1] == top - top_change) {
							if (board.whites[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // åñëè ñúåëè ïîòåíöèàëüíóþ äàìêó
									if (left + left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left + left_change - 8, top - top_change + 4) == 0) { // åñëè ëåâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
										return 2;
									}
									if (left + left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left + left_change + 8, top - top_change + 4) == 0) { // åñëè ïðàâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
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
		else // åñëè íîâàÿ êëåòêà ëåâåå ñòàðîé
		{
			int left = move.coord[0];
			int top = move.coord[1];

			if (move.coord[3] > move.coord[1]) // åñëè íîâàÿ êëåòêà íèæå ñòàðîé
			{
				while (left - left_change > move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.whites[i].location[0][0] == left - left_change && board.whites[i].location[0][1] == top + top_change) {
							if (board.whites[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // åñëè ñúåëè ïîòåíöèàëüíóþ äàìêó
									if (left - left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) { // åñëè ëåâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
										return 2;
									}
									if (left - left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left - left_change + 8, top + top_change + 4) == 0) { // åñëè ïðàâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
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
			else // åñëè íîâàÿ êëåòêà âûøå ñòàðîé
			{
				while (left - left_change > move.coord[2])
				{
					for (int i = 0; i < 12; i++)
					{
						if (board.whites[i].location[0][0] == left - left_change && board.whites[i].location[0][1] == top - top_change) {
							if (board.whites[i].status == 0) {
								if (top + top_change + 4 == BOT_LIMIT) { // åñëè ñúåëè ïîòåíöèàëüíóþ äàìêó
									if (left - left_change - 8 > LEFT_LIMIT + 1 && is_here_a_checker(board, left - left_change - 8, top + top_change + 4) == 0) { // åñëè ëåâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
										return 2;
									}
									if (left - left_change + 8 < RIGHT_LIMIT + 3 && is_here_a_checker(board, left - left_change + 8, top + top_change + 4) == 0) { // åñëè ïðàâàÿ íèæíÿÿ êëåòêà ïóñòàÿ
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
// ñãåíåðèðîâàòü õîäû äëÿ áåëûõ
NODE* find_whites_moves(BOARD board, int beat)
{
	NODE* result = NULL;
	int size = 0;

	if (beat) // åñëè îáÿçàòåëüíî íàäî áèòü
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.whites[i].status == 0) // íå äàìêà
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
						while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
						while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
						while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
						while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
			else if (board.whites[i].status == 1) // äàìêà
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
								while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
								while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
								while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
								while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
	else // ïðîñòîé õîä
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.whites[i].status == 0) // íå äàìêà
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
			else if (board.whites[i].status == 1) // äàìêà
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

// ñãåíåðèðîâàòü õîäû äëÿ êðàñíûõ
NODE* find_reds_moves(BOARD board, int beat)
{
	NODE* result = NULL;
	int size = 0;

	if (beat) // åñëè îáÿçàòåëüíî íàäî áèòü
	{
		for (int i = 0; i < 12; i++)
		{
			if (board.reds[i].status == 0) // íå äàìêà
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
						while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
						while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
						while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
						while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
			else if (board.reds[i].status == 1) // äàìêà
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
								while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
								while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
								while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
								while (more_move.coord[0] != 0) // åñëè ìîæíî ñúåñòü åùå
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
	else // ïðîñòîé õîä
	{
		for (int i = 0; i < 12; i++)
		{
			int left = board.reds[i].location[0][0];
			int top = board.reds[i].location[0][1];

			if (board.reds[i].status == 0) // íå äàìêà
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
			else if (board.reds[i].status == 1) // äàìêà
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

// ïîäñ÷èòàòü ðåçóëüòàò õîäà êîìïüþòåðà
int calculate_evaluate(BOARD board)
{
	int result = 0;

	int white_default_num = white_check_num; // êîëè÷åñòâî îáû÷íûõ áåëûõ øàøåê
	int red_default_num = red_check_num; // êîëè÷åñòâî îáû÷íûõ êðàñíûõ øàøåê
	int white_queens_num = 0; // êîëè÷åñòâî áåëûõ äàìîê
	int red_queens_num = 0; // êîëè÷åñòâî êðàñíûõ äàìîê

			/*Ðåçóëüòàò ïîñëå õîäà êîìïüþòåðà*/
	int white_default_num_res = 0; // êîëè÷åñòâî îáû÷íûõ áåëûõ øàøåê
	int red_default_num_res = 0; // êîëè÷åñòâî îáû÷íûõ êðàñíûõ øàøåê
	int white_queens_num_res = 0; // êîëè÷åñòâî áåëûõ äàìîê
	int red_queens_num_res = 0; // êîëè÷åñòâî êðàñíûõ äàìîê

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

// ïîäñ÷èòàòü îöåíêó äëÿ êðàñíûõ
int red_evaluate_calc(BOARD board)
{
	int result = 0;
	int mine = 0;
	int opp = 0;

	for (int i = 0; i < 12; i++) // ïîäñ÷èòûâàåì êîë-âî ñîáñòâåííûõ è ÷óæèõ øàøåê
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

// ïîäñ÷èòàòü îöåíêó äëÿ áåëûõ
int white_evaluate_calc(BOARD board)
{
	int result = 0;
	int mine = 0;
	int opp = 0;

	for (int i = 0; i < 12; i++) // ïîäñ÷èòûâàåì êîë-âî ñîáñòâåííûõ è ÷óæèõ øàøåê
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

// ïåðåáîð õîäîâ äëÿ êðàñíûõ
int red_minimax(BOARD board, int depth, int alpha, int beta, int maximizing_player)
{
	if (depth == 0) {
		return red_evaluate_calc(board); // îöåíèòü
	}

	// ñêîïèðîâàòü äîñêó()
	BOARD current_board = board_copy(board);
	NODE* moves = NULL;

	if (maximizing_player == 1)
	{
		int max_eval = -2147483646;
		int ev;
		// sizeof(NODE) = 316
		if (must_beat(board, 1)) { // åñëè îáÿçàòåëüíî íàäî áèòü
			moves = find_reds_moves(board, 1); // íàéòè âñå áüþùèå õîäû
		}
		else {
			moves = find_reds_moves(board, 0); // íàéòè âñå âîçìîæíûå õîäû
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
		if (must_beat(board, 0)) { // åñëè îáÿçàòåëüíî íàäî áèòü
			moves = find_whites_moves(board, 1); // íàéòè âñå áüþùèå õîäû
		}
		else {
			moves = find_whites_moves(board, 0); // íàéòè âñå âîçìîæíûå õîäû
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

// ïåðåáîð õîäîâ äëÿ áåëûõ
int white_minimax(BOARD board, int depth, int alpha, int beta, int maximizing_player)
{
	if (depth == 0) {
		return white_evaluate_calc(board); // îöåíèòü
	}

	// ñêîïèðîâàòü äîñêó()
	BOARD current_board = board_copy(board);
	NODE* moves = NULL;

	if (maximizing_player == 1)
	{
		int max_eval = -2147483646;
		int ev;
		// sizeof(NODE) = 316
		if (must_beat(board, 0)) { // åñëè îáÿçàòåëüíî íàäî áèòü
			moves = find_whites_moves(board, 1); // íàéòè âñå áüþùèå õîäû
		}
		else {
			moves = find_whites_moves(board, 0); // íàéòè âñå âîçìîæíûå õîäû
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
		if (must_beat(board, 1)) { // åñëè îáÿçàòåëüíî íàäî áèòü
			moves = find_reds_moves(board, 1); // íàéòè âñå áüþùèå õîäû
		}
		else {
			moves = find_reds_moves(board, 0); // íàéòè âñå âîçìîæíûå õîäû
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

// ñäåëàòü êîïèþ íûíåøíåé äîñêè
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

// èíèöèàëèçèðîâàòü äîñêó
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

// ïåðåíåñòè äàííûå ñ äîñêè â èãðó (äëÿ èãðû ñ êîìïüþòåðîì)
void revive_the_board(BOARD board)
{
	for (int i = 0; i < 12; i++)
	{
		if (board.reds[i].status == 2 && board.reds[i].status != red_checkers[i].status) {
			red_checkers[i].status = 2;
			red_check_num--;
			gotoxy(red_checkers[i].location[0][0], red_checkers[i].location[0][1]);
			con_set_color(NULL, CON_CLR_BLACK);
			printf("   "); // ñòèðàåì ñúåäåííóþ øàøêó ñ äîñêè
		}
		if (board.whites[i].status == 2 && board.whites[i].status != white_checkers[i].status) {
			white_checkers[i].status = 2;
			white_check_num--;
			gotoxy(white_checkers[i].location[0][0], white_checkers[i].location[0][1]);
			con_set_color(NULL, CON_CLR_BLACK);
			printf("   "); // ñòèðàåì ñúåäåííóþ øàøêó ñ äîñêè
		}
	}
}

// õîä êîìïüþòåðà
void computer_move(BOARD board, int white_mode, int red_mode)
{
	NODE* moves = NULL;

	if (whose_move == WHITE)
	{
		// ñìîòðèì, åñòü ëè õîäû
		if (must_beat(board, 0) == 1) {
			moves = find_whites_moves(board, 1);
		}
		else {
			moves = find_whites_moves(board, 0);
		}
	}
	else if (whose_move == RED)
	{
		// ñìîòðèì, åñòü ëè õîäû
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
			for (int i = 0; i < size; i++) // îöåíèâàåì õîäû
			{
				(moves + i)->value = white_minimax((moves + i)->board, 0, 2147483647, -2147483647, 0);
			}
		}
		else if (white_mode == 2)
		{
			if (red_mode == 3)
			{
				for (int i = 0; i < size; i++) // îöåíèâàåì õîäû
				{
					(moves + i)->value = white_minimax((moves + i)->board, 2, 2147483647, -2147483647, 0);
				}
			}
			else
			{
				for (int i = 0; i < size; i++) // îöåíèâàåì õîäû
				{
					(moves + i)->value = white_minimax((moves + i)->board, 1, 2147483647, -2147483647, 0);
				}
			}
		}
		else if (white_mode == 3)
		{
			for (int i = 0; i < size; i++) // îöåíèâàåì õîäû
			{
				(moves + i)->value = white_minimax((moves + i)->board, 5, 2147483647, -2147483647, 0);
			}
		}
	}
	else if (whose_move == RED)
	{
		if (red_mode == 1)
		{
			for (int i = 0; i < size; i++) // îöåíèâàåì õîäû
			{
				(moves + i)->value = red_minimax((moves + i)->board, 0, 2147483647, -2147483647, 0);
			}
		}
		else if (red_mode == 2)
		{
			for (int i = 0; i < size; i++) // îöåíèâàåì õîäû
			{
				(moves + i)->value = red_minimax((moves + i)->board, 1, 2147483647, -2147483647, 0);
			}
		}
		else if (red_mode == 3)
		{
			if (white_mode == 1)
			{
				for (int i = 0; i < size; i++) // îöåíèâàåì õîäû
				{
					(moves + i)->value = red_minimax((moves + i)->board, 5, 2147483647, -2147483647, 0);
				}
			}
			else
			{
				for (int i = 0; i < size; i++) // îöåíèâàåì õîäû
				{
					(moves + i)->value = red_minimax((moves + i)->board, 4, 2147483647, -2147483647, 0);
				}
			}
		}
	}

	int evaluate = -2147483647;
	int num = -1;
	for (int i = 0; i < size; i++) // âûáèðàåì õîä ñ ëó÷øåé îöåíêîé
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

// ÷åëîâåê-êîìï
void hum_vs_comp(int mode)
{
	white_moves_num = 0;
	red_moves_num = 0;
	int white_lose = 0;
	int red_lose = 0;
	white_check_num = 12;
	red_check_num = 12;
	BOARD board = init_board();

	get_cells_locations(); // ïîëó÷èòü êîîðäèíàòû êëåòîê
	whose_move = WHITE;
	build_board(); // ïîñòðîèòü äîñêó

	while (1)
	{
		if (end) // êîíåö èãðû
		{
			end = 0;
			break;
		}

		need_to_beat = necessary_to_beat(); // ïðîâåðêà, îáÿçàòåëüíî ëè åñòü
		cycle_end = 0; // îáíóëÿåì ôëàã äëÿ öèêëà â ôóíêöèè move_cursor()

		if (red_check_num == 0 || (whose_move == RED && !need_to_beat && !no_move()))
		{
			white_win(); // ïîáåäà áåëûõ
			// âíåñòè â òàáëèöó ðåêîðäîâ
			break;
		}
		else if (white_check_num == 0 || (whose_move == WHITE && !need_to_beat && !no_move()))
		{
			red_win(); // ïîáåäà êðàñíûõ
			// âíåñòè â òàáëèöó ðåêîðäîâ
			break;
		}

		if (whose_move == WHITE) { // åñëè õîäèò ÷åëîâåê
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

		if (whose_move == RED) { // åñëè áûë õîä êîìïüþòåðà
			whose_move = WHITE; // ñëåäóþùèé õîä ÷åëîâåêà
		}
		else if (whose_move == WHITE) { // åñëè áûë õîä ÷åëîâåêà
			whose_move = RED; // ñëåäóþùèé õîä êîìïüþòåðà
		}
	}
}

// èãðà êîìïüþòåðà c êîìïüþòåðîì
void computer_vs_computer(int white_mode, int red_mode)
{
	/*mode - 1-ëåãêî, 2-íîðìàëüíî, 3-ñëîæíî*/
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

	get_cells_locations(); // ïîëó÷èòü êîîðäèíàòû êëåòîê
	whose_move = WHITE;
	build_board(); // ïîñòðîèòü äîñêó

	while (1)
	{
		if (end) // êîíåö èãðû
		{
			end = 0;
			break;
		}

		need_to_beat = necessary_to_beat(); // ïðîâåðêà, îáÿçàòåëüíî ëè åñòü

		if (red_check_num == 0 || (whose_move == RED && !need_to_beat && !no_move()))
		{
			QueryPerformanceCounter(&time2);
			time2.QuadPart -= time1.QuadPart;
			double span = (double)time2.QuadPart / freq.QuadPart;
			white_win(); // ïîáåäà áåëûõ
			// âíåñòè â òàáëèöó ðåêîðäîâ
			break;
		}
		else if (white_check_num == 0 || (whose_move == WHITE && !need_to_beat && !no_move()))
		{
			QueryPerformanceCounter(&time2);
			time2.QuadPart -= time1.QuadPart;
			double span = (double)time2.QuadPart / freq.QuadPart;
			red_win(); // ïîáåäà êðàñíûõ
			// âíåñòè â òàáëèöó ðåêîðäîâ
			break;
		}

		computer_move(board, white_mode, red_mode);

		board = new_board();

		if (whose_move == RED) { // åñëè áûë õîä êîìïüþòåðà
			whose_move = WHITE; // ñëåäóþùèé õîä ÷åëîâåêà
		}
		else if (whose_move == WHITE) { // åñëè áûë õîä ÷åëîâåêà
			whose_move = RED; // ñëåäóþùèé õîä êîìïüþòåðà
		}
	}
}

// îòðèñîâàòü ìåíþ âûáîðà ñëîæíîñòè
void draw_choose_difficulty_menu()
{
	int left = 60;
	int top = 9;

	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	clrscr();

	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("-----------------------------");
	top++;
	gotoxy(left, top);
	printf("|      ÑËÎÆÍÎÑÒÜ ÈÃÐÛ       |");
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
	printf("*      Ëåãêî      *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*    Íîðìàëüíî    *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*     Ñëîæíî      *");
	top++;
	gotoxy(left, top);
	printf("*******************");
}

// ìåíþ âûáîðà ñëîæíîñòè äëÿ èãðû êîìïîâ
void difficulty_menu_comp()
{
	int white_mode = 0;
	int red_mode = 0;

	int left = 45;
	int top = 9;

	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	clrscr();

	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("-----------------------------------------------------------------");
	top++;
	gotoxy(left, top);
	printf("|        ÑËÎÆÍÎÑÒÜ ÁÅËÛÕ        |       ÑËÎÆÍÎÑÒÜ ÊÐÀÑÍÛÕ       |");
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
	printf("*      Ëåãêî      *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*    Íîðìàëüíî    *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*     Ñëîæíî      *");
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
	printf("*      Ëåãêî      *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*    Íîðìàëüíî    *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	top += 2;
	gotoxy(left, top);
	printf("*******************");
	top++;
	gotoxy(left, top);
	printf("*     Ñëîæíî      *");
	top++;
	gotoxy(left, top);
	printf("*******************");

	int flag = 1; // 1-âûáèðàåòñÿ ñëîæíîñòü áåëûõ; 2-âûáèðàåòñÿ ñëîæíîñòü êðàñíûõ
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
		if (code == KEY_UP) // Åñëè ýòî ñòðåëêà ââåðõ
		{
			// Òî ïåðåõîä ê âåðõíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      Ëåãêî      *");
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      Ëåãêî      *");
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      Ëåãêî      *");
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      Ëåãêî      *");
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      Ëåãêî      *");
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*      Ëåãêî      *");
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
							printf("*     Ñëîæíî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
							printf("*     Ñëîæíî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
							printf("*     Ñëîæíî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
							printf("*     Ñëîæíî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
							printf("*     Ñëîæíî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
							printf("*     Ñëîæíî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top -= 6;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
		else if (code == KEY_DOWN) // Åñëè ñòðåëêà âíèç
		{
			// Òî ïåðåõîä ê íèæíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     Ñëîæíî      *");
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     Ñëîæíî      *");
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     Ñëîæíî      *");
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     Ñëîæíî      *");
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     Ñëîæíî      *");
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
							printf("*    Íîðìàëüíî    *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*     Ñëîæíî      *");
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
							printf("*      Ëåãêî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
							printf("*      Ëåãêî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
							printf("*      Ëåãêî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
							printf("*      Ëåãêî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
							printf("*      Ëåãêî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
							printf("*      Ëåãêî      *");
							top++;
							gotoxy(left, top);
							printf("*******************");
							top += 2;
							con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
							gotoxy(left, top);
							printf("*******************");
							top++;
							gotoxy(left, top);
							printf("*    Íîðìàëüíî    *");
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
		else if (code == KEY_LEFT) // ñòðåëêà âëåâî
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
					printf("*      Ëåãêî      *");
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
					printf("*      Ëåãêî      *");
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
					printf("*    Íîðìàëüíî    *");
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
					printf("*    Íîðìàëüíî    *");
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
					printf("*     Ñëîæíî      *");
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
					printf("*     Ñëîæíî      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				}
			}
		}
		else if (code == KEY_RIGHT) // ñòðåëêà âïðàâî
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
					printf("*      Ëåãêî      *");
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
					printf("*      Ëåãêî      *");
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
					printf("*    Íîðìàëüíî    *");
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
					printf("*    Íîðìàëüíî    *");
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
					printf("*     Ñëîæíî      *");
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
					printf("*     Ñëîæíî      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				}
			}
		}
		else if (code == KEY_ESC) // ESC - âûõîä
		{
			return;
		}
		else if (code == KEY_ENTER) // Íàæàòà êíîïêà Enter
		{
			switch (position)
			{
			case 0:
				if (flag == 1)
				{
					if (white_mode == 0) // åñëè íè÷åãî íå âûáðàíî
					{
						white_mode = 1;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 1) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ëåãêî"
					{
						white_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 2) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "íîðìàëüíî"
					{
						white_mode = 1;
						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 3) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ñëîæíî"
					{
						white_mode = 1;
						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
				}
				else if (flag == 2)
				{
					if (red_mode == 0) // åñëè íè÷åãî íå âûáðàíî
					{
						red_mode = 1;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 1) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ëåãêî"
					{
						red_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 2) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "íîðìàëüíî"
					{
						red_mode = 1;
						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 3) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ñëîæíî"
					{
						red_mode = 1;
						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
				}
				break;
			case 1:
				if (flag == 1)
				{
					if (white_mode == 0) // åñëè íè÷åãî íå âûáðàíî
					{
						white_mode = 2;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 1) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ëåãêî"
					{
						white_mode = 2;
						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 2) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "íîðìàëüíî"
					{
						white_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 3) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ñëîæíî"
					{
						white_mode = 2;
						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
				}
				else if (flag == 2)
				{
					if (red_mode == 0) // åñëè íè÷åãî íå âûáðàíî
					{
						red_mode = 2;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 1) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ëåãêî"
					{
						red_mode = 2;
						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 2) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "íîðìàëüíî"
					{
						red_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 3) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ñëîæíî"
					{
						red_mode = 2;
						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
				}
				break;
			case 2:
				if (flag == 1)
				{
					if (white_mode == 0) // åñëè íè÷åãî íå âûáðàíî
					{
						white_mode = 3;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 1) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ëåãêî"
					{
						white_mode = 3;
						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 2) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "íîðìàëüíî"
					{
						white_mode = 3;
						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (white_mode == 3) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ñëîæíî"
					{
						white_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
				}
				else if (flag == 2)
				{
					if (red_mode == 0) // åñëè íè÷åãî íå âûáðàíî
					{
						red_mode = 3;
						preparedness++;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 1) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ëåãêî"
					{
						red_mode = 3;
						top = 15;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*      Ëåãêî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 2) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "íîðìàëüíî"
					{
						red_mode = 3;
						top = 19;
						con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*    Íîðìàëüíî    *");
						top++;
						gotoxy(left, top);
						printf("*******************");

						top = 23;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN_LIGHT);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
						top++;
						gotoxy(left, top);
						printf("*******************");
					}
					else if (red_mode == 3) // åñëè äëÿ áåëûõ óæå âûáðàí ðåæèì "ñëîæíî"
					{
						red_mode = 0;
						preparedness--;
						top -= 2;
						con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
						gotoxy(left, top);
						printf("*******************");
						top++;
						gotoxy(left, top);
						printf("*     Ñëîæíî      *");
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
				return; // íà÷àòü èãðó
			}
		}
	}
}

// ìåíþ âûáîðà ñëîæíîñòè
void choose_difficulty_menu()
{
	draw_choose_difficulty_menu();

	int left = 65;
	int top = 17;
	int position = 0;
	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_UP) // Åñëè ýòî ñòðåëêà ââåðõ
		{
			// Òî ïåðåõîä ê âåðõíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
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
					printf("*    Íîðìàëüíî    *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*      Ëåãêî      *");
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
					printf("*     Ñëîæíî      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*    Íîðìàëüíî    *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				}
				position--;
			}
		}
		else if (code == KEY_DOWN) // Åñëè ñòðåëêà âíèç
		{
			// Òî ïåðåõîä ê íèæíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
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
					printf("*    Íîðìàëüíî    *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*     Ñëîæíî      *");
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
					printf("*      Ëåãêî      *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("*******************");
					top++;
					gotoxy(left, top);
					printf("*    Íîðìàëüíî    *");
					top++;
					gotoxy(left, top);
					printf("*******************");
					break;
				}
				position++;
			}
		}
		else if (code == KEY_ESC) // ESC - âûõîä
		{
			return;
		}
		else if (code == KEY_ENTER) // Íàæàòà êíîïêà Enter
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

// îòðèñîâàòü ãëàâíîå ìåíþ
void draw_main_menu()
{
	int left = 65;
	int top = 15;

	// Î÷èñòêà ýêðàíà
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	clrscr();

	con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("*  Çàãðóçèòü èãðó  *");
	top++;
	gotoxy(left, top);
	printf("********************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("*    Íîâàÿ èãðà    *");
	top++;
	gotoxy(left, top);
	printf("********************");

	top += 2;
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("* Òàáëèöà ðåêîðäîâ *");
	top++;
	gotoxy(left, top);
	printf("********************");

	top += 2;
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("*     Ñïðàâêà      *");
	top++;
	gotoxy(left, top);
	printf("********************");

	top += 2;
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("*   Î ïðîãðàììå    *");
	top++;
	gotoxy(left, top);
	printf("********************");

	top += 2;
	gotoxy(left, top);
	printf("********************");
	top++;
	gotoxy(left, top);
	printf("*      Âûõîä       *");
	top++;
	gotoxy(left, top);
	printf("********************");
}

// îòðèñîâàòü ìåíþ íîâîé èãðû
void draw_new_game_menu()
{
	int left = 65;
	int top = 15;

	// Î÷èñòêà ýêðàíà
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	clrscr();

	con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*   ×åëîâåê-÷åëîâåê   *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE);
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*  ×åëîâåê-êîìïüþòåð  *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("* Êîìïüþòåð-êîìïüþòåð *");
	top++;
	gotoxy(left, top);
	printf("***********************");

	top += 2;
	gotoxy(left, top);
	printf("***********************");
	top++;
	gotoxy(left, top);
	printf("*        Íàçàä        *");
	top++;
	gotoxy(left, top);
	printf("***********************");
}

// îòêðûâàåòñÿ, åñëè âûáðàòü "íîâóþ èãðó"
void new_game_menu()
{
	draw_new_game_menu(); // îòðèñîâàòü ìåíþ íîâîé èãðû

	int left = 65;
	int top = 17;
	int position = 0;
	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_UP) // Åñëè ýòî ñòðåëêà ââåðõ
		{
			// Òî ïåðåõîä ê âåðõíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
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
					printf("*  ×åëîâåê-êîìïüþòåð  *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*   ×åëîâåê-÷åëîâåê   *");
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
					printf("* Êîìïüþòåð-êîìïüþòåð *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*  ×åëîâåê-êîìïüþòåð  *");
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
					printf("*        Íàçàä        *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("* Êîìïüþòåð-êîìïüþòåð *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				}
				position--;
			}
		}
		else if (code == KEY_DOWN) // Åñëè ñòðåëêà âíèç
		{
			// Òî ïåðåõîä ê íèæíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
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
					printf("* Êîìïüþòåð-êîìïüþòåð *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*        Íàçàä        *");
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
					printf("*  ×åëîâåê-êîìïüþòåð  *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("* Êîìïüþòåð-êîìïüþòåð *");
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
					printf("*   ×åëîâåê-÷åëîâåê   *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("***********************");
					top++;
					gotoxy(left, top);
					printf("*  ×åëîâåê-êîìïüþòåð  *");
					top++;
					gotoxy(left, top);
					printf("***********************");
					break;
				}
				position++;
			}
		}
		else if (code == KEY_ESC) // ESC - âûõîä
		{
			//draw_main_menu();
			return;
		}
		else if (code == KEY_ENTER) // Íàæàòà êíîïêà Enter
		{
			if (position == 3) { // Âûáðàí ïîñëåäíèé ïóíêò - ýòî "íàçàä"
				draw_main_menu();
				return;
			}
			if (position == 0) {
				game_status = 1;
				human_vs_human();
				return;
				// ÷åëîâåê-÷åëîâåê();
			}
			if (position == 1) {
				game_status = 2;
				choose_difficulty_menu();
				// ÷åëîâåê-êîìïüþòåð();
			}
			if (position == 2) {
				game_status = 3;
				difficulty_menu_comp();
				// êîìïüþòåð-êîìïüþòåð();
			}
			left = 65;
			top = 17;
			draw_new_game_menu();
			position = 0;
		}
	}
}

// ôóíêöèÿ ñîðòèðîâêè ïðÿìûì îáìåíîì (ìåòîä "ïóçûðüêà")
void bubbleSort(struct table* table, int size)
{
	// Äëÿ âñåõ ýëåìåíòîâ
	for (int i = 0; i < size - 1; i++)
	{
		for (int j = (size - 1); j > i; j--) // äëÿ âñåõ ýëåìåíòîâ ïîñëå i-îãî
		{
			if ((table + j - 1)->value > (table + j)->value) // åñëè òåêóùèé ýëåìåíò ìåíüøå ïðåäûäóùåãî
			{
				struct table temp = *(table + j - 1); // ìåíÿåì èõ ìåñòàìè
				*(table + j - 1) = *(table + j);
				*(table + j) = temp;
			}
		}
	}
}

// îòêðûòèå òàáëèöû ðåêîðäîâ
void high_score_table()
{
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	clrscr();
	gotoxy(0, 0);

	struct table sort_table[200];

	char filename[] = "table.txt";
	char ch;
	char str[70]; // äëÿ õðàíåíèÿ îäíîé ñòðîêè èç òàáëèöû ðåêîðäîâ
	char ch_value[10]; // äëÿ õðàíåíèÿ ðåêîðäà â âèäå ñòðîêè 

	FILE* f = fopen(filename, "r");
	if (f == NULL) {
		printf("Íå óäàëîñü îòêðûòü ôàéë.\n");
		exit(1);
	}

	int k = 0; int j;
	// ñ÷èòûâàåì ôàéë ñ òàáëèöåé ðåêîðäîâ â îòäåëüíûå ñòðóêòóðû äëÿ ñîðòèðîâêè
	while (fgets((sort_table + k)->str, 70, f) != NULL)
	{
		j = 0;
		while ((sort_table + k)->str[j] != '\n' && (sort_table + k)->str[j] != '\0')
		{
			j++;
		}
		if ((sort_table + k)->str[j] == '\n') // çàìåíÿåì '\n' íà '\0' â êîíöå ñòðîêè
		{
			(sort_table + k)->str[j] = '\0';
		}

		j = 0;
		while ((sort_table + k)->str[j] != '|') { // ïðîïóñêàåì â ñòðîêå èìÿ ïîëüçîâàòåëÿ è ïåðåõîäèì ê ðåêîðäó
			j++;
		}

		j++;
		if ((sort_table + k)->str[j] != ' ')
		{
			printf("Ôàéë ñ òàáëèöåé ðåêîðäîâ çàïîëíåí íåâåðíî.");
			return;
		}

		j++;
		int a = 0;
		while ((sort_table + k)->str[j] != EOF && (sort_table + k)->str[j] != '\0') // ñ÷èòûâàåì ðåêîðä
		{
			switch ((sort_table + k)->str[j])
			{
			case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
				ch_value[a] = (sort_table + k)->str[j];
				a++;
				j++;
				break;
			default:
				printf("Ôàéë ñ òàáëèöåé ðåêîðäîâ çàïîëíåí íåâåðíî.");
				return;
			}
		}
		(sort_table + k)->value = atoi(ch_value);
		k++;
		memset(ch_value, '\0', a);
		a = 0;
	}

	bubbleSort(sort_table, k); // ñîðòèðîâêà ìåòîäîì ïóçûðüêà
	fclose(f);

	f = fopen(filename, "w");
	if (f == NULL) {
		printf("Íå óäàëîñü îòêðûòü ôàéë.\n");
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
		printf("Íå óäàëîñü îòêðûòü ôàéë.\n");
		exit(1);
	}

	printf("Ìåñòî | Èìÿ èãðîêà | Êîëè÷åñòâî õîäîâ äëÿ ïîáåäû\n\n");
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
			printf("Ôàéë ñ òàáëèöåé ðåêîðäîâ çàïîëíåí íåâåðíî.");
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
				printf("Ôàéë ñ òàáëèöåé ðåêîðäîâ çàïîëíåí íåâåðíî.");
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

// îòêðûòü ðàçäåë "î ïðîãðàììå" 
void about_the_program()
{
	// Î÷èñòêà ýêðàíà
	con_set_color(CON_CLR_BLACK, CON_CLR_WHITE); // öâåò øðèôòà, öâåò êîíñîëè
	clrscr();
	gotoxy(0, 0);

	printf("		Àâòîðû: \n");
	printf("	1. Àêêóëîâ Èäðèñ Ìàëèêîâè÷; ãð. 4831001/90002; 2020ã.; ÑÏáÏÓ Ïåòðà Âåëèêîãî; ÈÊèÇÈ; ÂØÊèÇÈ.\n");
	printf("	2. Êîòëÿðîâà Àíàñòàñèÿ Ñåðãååâíà; ãð. 4831001/90002; 2020ã.; ÑÏáÏÓ Ïåòðà Âåëèêîãî; ÈÊèÇÈ; ÂØÊèÇÈ.\n");

	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_ESC || code == KEY_ENTER)
		{
			break;
		}
	}
}

// ïîääåðæêà ãëàâíîãî ìåíþ
void main_menu()
{
	draw_main_menu(); // îòðèñîâàòü ìåíþ

	int left = 65;
	int top = 17;
	int position = 0;
	while (1)
	{
		int code = key_pressed_code();
		if (code == KEY_UP) // Åñëè ýòî ñòðåëêà ââåðõ
		{
			// Òî ïåðåõîä ê âåðõíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
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
					printf("*    Íîâàÿ èãðà    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*  Çàãðóçèòü èãðó  *");
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
					printf("* Òàáëèöà ðåêîðäîâ *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*    Íîâàÿ èãðà    *");
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
					printf("*     Ñïðàâêà      *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("* Òàáëèöà ðåêîðäîâ *");
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
					printf("*   Î ïðîãðàììå    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*     Ñïðàâêà      *");
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
					printf("*      Âûõîä       *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top -= 6;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*   Î ïðîãðàììå    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				}
				position--;
			}
		}
		else if (code == KEY_DOWN) // Åñëè ñòðåëêà âíèç
		{
			// Òî ïåðåõîä ê íèæíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
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
					printf("*   Î ïðîãðàììå    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*      Âûõîä       *");
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
					printf("*     Ñïðàâêà      *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*   Î ïðîãðàììå    *");
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
					printf("* Òàáëèöà ðåêîðäîâ *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*     Ñïðàâêà      *");
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
					printf("*    Íîâàÿ èãðà    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("* Òàáëèöà ðåêîðäîâ *");
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
					printf("*  Çàãðóçèòü èãðó  *");
					top++;
					gotoxy(left, top);
					printf("********************");
					top += 2;
					con_set_color(CON_CLR_BLACK, CON_CLR_GREEN);
					gotoxy(left, top);
					printf("********************");
					top++;
					gotoxy(left, top);
					printf("*    Íîâàÿ èãðà    *");
					top++;
					gotoxy(left, top);
					printf("********************");
					break;
				}
				position++;
			}
		}
		else if (code == KEY_ESC) // ESC - âûõîä
		{
			return;
		}
		else if (code == KEY_ENTER) // Íàæàòà êíîïêà Enter
		{
			if (position == 5) { // Âûáðàí ïîñëåäíèé ïóíêò - ýòî âûõîä
				return;
			}
			else if (position == 0) { // çàãðóçèòü èãðó
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
			else if (position == 1) { // íîâàÿ èãðà
				is_load = 0;
				new_game_menu();
			}
			else if (position == 2) { // òàáëèöà ðåêîðäîâ
				high_score_table();
			}
			else if (position == 3) // ñïðàâêà 
			{
				reference();
			}
			else if (position == 4) // î ïðîãðàììå 
			{
				about_the_program();
			}
			draw_main_menu();
			position = 0; // ïðè âûõîäå èç ìåíþ íîâîé èãðû, îáâîäèòñÿ ïåðâûé ïóíêò
			top = 17;
			left = 65;
		}
	}
}

int main()
{
	// Èíèöèàëèçèðóåòñÿ êîíñîëü, ñêðûâàåòñÿ êóðñîð
	con_init(300, 100);
	show_cursor(0);
	system("mode con cols=150 lines=50"); // ìèíèìàëüíûé ðàçìåð êîíñîëè (150õ50)

	main_menu();

	return 0;
}