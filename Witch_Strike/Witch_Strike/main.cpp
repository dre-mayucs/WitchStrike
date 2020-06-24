#include "DxLib.h"
#include "MayuUtility.h"
#include <math.h>
#include <time.h>

//config
const char TITLE[] = "WitchStrike";
const int WIN_WIDTH = 1920;
const int WIN_HEIGHT = 1080;

int BGM_Volume = 96;

//prototype
void game_start(void);
void game_end(void);
void game_stage(void);
bool hit_judge(int, int, int, int, int, int);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	/***********************************
	*[mode setting]
	************************************/
	ChangeWindowMode(TRUE);							//ウィンドウモード
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);	//サイズ変更不可
	SetMainWindowText(TITLE);						//タイトル設定
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);		//サイズ設定
	SetWindowSizeExtendRate(1.0);					//比率設定
	SetBackgroundColor(0, 0, 0);					//カラー

	//init DXlib rand
	if (DxLib_Init() == -1) { return -1; }
	srand(time(NULL));

	// (ダブルバッファ)描画先グラフィック領域は裏面を指定
	SetDrawScreen(DX_SCREEN_BACK);

	//サークルロゴ表示
	PlayMovie("resources\\movie\\logo.mp4", 1, DX_MOVIEPLAYTYPE_NORMAL);

	//Background music setting
	int BGM = LoadSoundMem("resources\\sounds\\Stars.ogg");
	PlaySoundMem(BGM, DX_PLAYTYPE_LOOP, true);
	ChangeVolumeSoundMem(BGM_Volume, BGM);

	/********************************
	*[scene]
	*1.start
	*2.game main loop
	*3.end
	**********************************/
	game_start();
	game_stage();
	game_end();

	// Dxライブラリ終了処理
	DxLib_End();

	// 正常終了
	return 0;
}

void game_start()
{
	bool load = false;
	int Back_ground = LoadGraph("resources\\images\\StartBG.png");
	int Back_load = LoadGraph("resources\\images\\LoadingTopics.png");
	int Load_image = LoadGraph("resources\\images\\loading_animation.png");

	int flame = 0;
	int animation_flmae = 0;

	while (true)
	{
		//window_init
		ClearDrawScreen();
		if (load == false)
		{
			DrawGraph(0, 0, Back_ground, true);
			if (CheckHitKey(KEY_INPUT_RETURN) == true)
			{
				load = true;
			}
		}
		else
		{
			DrawGraph(0, 0, Back_load, true);
			DrawRectGraph(1740, 1015, animation_flmae, 0, 180, 50, Load_image, true, false);

			animation_flmae += 180;
			if (animation_flmae == 5040 - 180)
			{
				animation_flmae = 0;
			}

			if (flame == 300) { break; }
			flame++;
		}

		ScreenFlip();	// (ダブルバッファ)裏面
		WaitTimer(20);	// 20ミリ秒待機(疑似60FPS)

		if (ProcessMessage() == -1)
		{
			break;
		}
	}
}

void game_end()
{
	int Background_image = LoadGraph("resources\\images\\Endroll.png");
	int animation_flame = 0;
	while (true)
	{
		ClearDrawScreen();	//clear

		DrawRectGraph(0, 0, 0, animation_flame, 1920, 1080, Background_image, true, false);
		animation_flame += 2;

		ScreenFlip();		// (ダブルバッファ)裏面
		WaitTimer(20);		// 20ミリ秒待機(疑似60FPS)

		if (CheckHitKey(KEY_INPUT_ESCAPE) == true || ProcessMessage() == -1 || animation_flame == 3240)
		{
			break;
		}
	}
}

void game_stage()
{
	#pragma region player
	//player image
	int player_animation_nomal = LoadGraph("resources\\images\\player_animation.png");
	int player_image_turn_on = LoadGraph("resources\\images\\player_turn_on.png");
	int player_image_turn_back = LoadGraph("resources\\images\\player_turn_back.png");

	//player bullet
	int player_bullet = LoadGraph("resources\\images\\player_bull.png");

	//playerHP
	int player_HP = 200;
	int player_boost = 0;

	//player axes
	int player_bullet_x[24] = { 0 };
	int player_bullet_y[24] = { 0 };

	int player_pos_X = 100;
	int player_pos_Y = 100;
	int player_pos_R = 196 / 2;
	int bullet_r = 10;

	const int player_size = 196;

	//player config
	const int player_speed = 20;
	short player_direction = 0;

	bool bull_mode = false;
	bool player_bull_flag[24] = { false };

	int player_animation_x = 0;
	int player_animation_y = 0;
	float animation_flame = 0.0f;
	int sprite_x = 196;
	int sprite_y = 240;
	#pragma endregion

	#pragma region enemy
	//enemy speed
	int enemy_speed[2] = {6, 6};

	//enemy image
	int nomal_enemy_image = LoadGraph("resources\\images\\enemy.png");

	//enemy bullet
	int enemy_bull = LoadGraph("resources\\images\\enemy_bull.png");

	//enemyHP
	float nomal_enemy_HP[2] =
	{
		100.0f,
		100.0f
	};

	//enemy axes
	int enemy_pos_r = 64;
	int enemy_pos_x_adjust[2] =
	{
		WIN_WIDTH + 10,
		WIN_WIDTH + 10
	};
	int nomal_enemy_pos_x[2] =
	{
		(WIN_WIDTH / 2) + rand() % (WIN_WIDTH / 2) - 150,
		(WIN_WIDTH / 2) + rand() % (WIN_WIDTH / 2) - 150
	};
	int enemy_pos_y[2] =
	{
		rand() % WIN_HEIGHT,
		rand() % WIN_HEIGHT
	};

	//enemy config
	bool nomal_enemy_flag[2] = { true, true };
	bool nomal_enemy_bullet_flag[2][3] = { false };
	int nomal_enemy_bullet_x[2][3] = { WIN_WIDTH + 50 };
	int nomal_enemy_bullet_y[2][3] = { WIN_HEIGHT + 50 };
	#pragma endregion

	#pragma region config
	int world_count = 120;
	int clear_window = LoadGraph("resources\\images\\CLEAR.png");
	int over_window = LoadGraph("resources\\images\\game_over.png");
	int effect_window = LoadGraph("resources\\images\\Effect.png");

	int score_Text = LoadGraph("resources\\images\\Score_Text.png");

	int Background_image_bill[2] =
	{
		LoadGraph("resources\\images\\BG_bill.png"),
		LoadGraph("resources\\images\\BG_bill.png")
	};
	int Background_image_star[2] =
	{
		LoadGraph("resources\\images\\BG_Star.png"),
		LoadGraph("resources\\images\\BG_Star.png")
	};

	//axes
	int Background_bill_x1 = 0;
	int Background_bill_x2 = WIN_WIDTH;
	int Background_star_x1 = 0;
	int Background_star_x2 = WIN_WIDTH;

	//background setting
	const int speed = -20;
	int count_set = 5;
	int star_count = 0;
	bool Background_scroll_flag = true;

	//keybord info
	char keys[256] = { 0 };
	char oldkeys[256] = { 0 };

	//score
	double score = 0;
	bool judge = false;
	#pragma endregion

	while (true)
	{
		for (int i = 0; i < 256; i++)
		{
			oldkeys[i] = keys[i];
		}

		GetHitKeyStateAll(keys);	//get key state
		ClearDrawScreen();			//clear

		#pragma region Player_Operation
		if (keys[KEY_INPUT_W] == true && player_pos_Y > 0)
		{
			player_pos_Y -= player_speed;
			player_direction = NOMAL;
		}
		if (keys[KEY_INPUT_S] == true && player_pos_Y < WIN_HEIGHT - player_size)
		{
			player_pos_Y += player_speed;
			player_direction = NOMAL;
		}
		if (keys[KEY_INPUT_A] == true && player_pos_X > 0)
		{
			player_pos_X -= player_speed;
			player_direction = LEFT;
		}
		if (keys[KEY_INPUT_D] == true && player_pos_X < WIN_WIDTH - player_size)
		{
			player_pos_X += player_speed;
			player_direction = RIGHT;
		}
		if (WASD_ALL_KEY_OFF)
		{
			player_direction = NOMAL;
		}

		//bullet boost config
		if (player_boost <= 150 && player_boost >= 0)
		{
			player_boost++;
		}
		else if (player_boost <= 0 && keys[KEY_INPUT_SPACE] == false)
		{
			player_boost++;
		}

		//player single shot
		if (BULLET_ONE_SHOT && bull_mode == false && player_HP > 0)
		{
			if (player_bull_flag[0] == false)
			{
				player_bullet_x[0] = player_pos_X + player_pos_R;
				player_bullet_y[0] = player_pos_Y + player_pos_R;
				player_bull_flag[0] = true;
			}
		}
		if (player_bullet_x[0] >= WIN_WIDTH + 20 && bull_mode == false && player_bull_flag[0] == true)
		{
			player_bull_flag[0] = false;
		}
		
		//player multi shot
		if (BULL_MULTI_SHOT && bull_mode == true && player_boost > 0 && player_HP > 0)
		{
			for (int i = 0; i < 16; i++)
			{
				if (player_bull_flag[i] == false)
				{
					player_bullet_x[i] = player_pos_X + player_pos_R;
					player_bullet_y[i] = player_pos_Y + player_pos_R;
					player_bull_flag[i] = true;
					break;
				}
				else if (player_bullet_x[i] > WIN_WIDTH + 20)
				{
					player_bull_flag[i] = false;
				}
			}
		}
		if (BULL_MULTI_SHOT && bull_mode == true && player_boost > 0)
		{
			player_boost -= 2;
		}

		//bull mode
		if (keys[KEY_INPUT_E] == true && oldkeys[KEY_INPUT_E] != true)
		{
			bull_mode = !bull_mode;
		}
		#pragma endregion

		#pragma region enemy_Operation
		for (int i = 0; i < 2; i++)
		{
			for (int n = 0; n < 3; n++)
			{
				if (nomal_enemy_bullet_flag[i][n] == false && nomal_enemy_flag[i] == true)
				{
					nomal_enemy_bullet_x[i][n] = enemy_pos_x_adjust[i] + 64;
					nomal_enemy_bullet_y[i][n] = enemy_pos_y[i] + 64;
					nomal_enemy_bullet_flag[i][n] = true;
				}
				else if (nomal_enemy_bullet_flag[i][n] == true && nomal_enemy_bullet_x[i][n] < -150)
				{
					nomal_enemy_bullet_flag[i][n] = false;
				}
			}
		}
		#pragma endregion

		#pragma region Hit_Judge
		//player -> enemy judge -> boss judge
		for (int i = 0; i < 24; i++)
		{
			for (int n = 0; n < 2; n++)
			{
				judge = hit_judge(player_bullet_x[i], enemy_pos_x_adjust[n], player_bullet_y[i], enemy_pos_y[n], bullet_r, enemy_pos_r);

				if (judge == true && nomal_enemy_HP[n] >= 0 && player_bull_flag[i] == true && bull_mode == false)
				{
					nomal_enemy_HP[n] -= 5;
					score += 1050;
				}
				else if (judge == true && nomal_enemy_HP[n] >= 0 && player_bull_flag[i] == true && bull_mode == true)
				{
					nomal_enemy_HP[n] -= 0.5f;
					score += 550;
				}
				else if (nomal_enemy_HP[n] <= 0)
				{
					nomal_enemy_flag[n] = false;
				}
			}
		}

		//enemy -> player judge
		for (int i = 0; i < 2; i++)
		{
			for (int n = 0; n < 3; n++)
			{
				judge = hit_judge(nomal_enemy_bullet_x[i][n], player_pos_X, nomal_enemy_bullet_y[i][n], player_pos_Y, bullet_r, player_pos_R);

				if (judge == true && player_HP >= 0 && nomal_enemy_flag[i] == true)
				{
					player_HP -= 1;
				}
			}
		}
		#pragma endregion

		#pragma region Background_process
		if (Background_scroll_flag == true)
		{
			if (star_count != count_set)
			{
				Background_bill_x1 += speed;
				Background_bill_x2 += speed;
				star_count++;
			}
			else if (star_count == count_set)
			{
				Background_bill_x1 += speed;
				Background_bill_x2 += speed;
				Background_star_x1--;
				Background_star_x2--;
				star_count = 0;
			}

			//反転
			if (Background_bill_x1 <= -WIN_WIDTH)
			{
				Background_bill_x1 = WIN_WIDTH;
			}
			if (Background_bill_x2 <= -WIN_WIDTH)
			{
				Background_bill_x2 = WIN_WIDTH;
			}

			if (Background_star_x1 <= -WIN_WIDTH)
			{
				Background_star_x1 = WIN_WIDTH;
			}
			if (Background_star_x2 <= -WIN_WIDTH)
			{
				Background_star_x2 = WIN_WIDTH;
			}
		}
		#pragma endregion

		#pragma region Background_draw_process
		DrawGraph(Background_bill_x1, 0, Background_image_bill[0], true);
		DrawGraph(Background_bill_x2, 0, Background_image_bill[1], true);

		//star
		DrawGraph(Background_star_x1, 0, Background_image_star[0], true);
		DrawGraph(Background_star_x2, 0, Background_image_star[1], true);

		//score
		DrawGraph(0, 0, score_Text, true);
		SetFontSize(30);
		SetFontThickness(5);
		DrawFormatString(150, 80, WHITE, "%.lf", score);
		#pragma endregion

		#pragma region HP_gage
		DrawBox(80, 10, 80 + player_HP, 30, RED, true);
		DrawBox(130, 45, 130 + player_boost, 65, YELLO, true);
		#pragma endregion

		#pragma region Bullet_draw
		//player
		if (bull_mode == false)
		{
			player_bullet_x[0] += 80;
			DrawGraph(player_bullet_x[0], player_bullet_y[0], player_bullet, true);
		}
		else
		{
			for (int i = 0; i < 16; i++)
			{
				if (player_bull_flag[i] == true)
				{
					player_bullet_x[i] += 80;
					DrawGraph(player_bullet_x[i], player_bullet_y[i], player_bullet, true);
				}
			}
		}

		//enemy
		for (int i = 0; i < 2; i++)
		{
			for (int n = 0; n < 3; n++)
			{
				if (nomal_enemy_bullet_flag[i][n] == true && nomal_enemy_flag[i] == true)
				{
					nomal_enemy_bullet_x[i][n] -= 40;
					DrawGraph(nomal_enemy_bullet_x[i][n], nomal_enemy_bullet_y[i][n], enemy_bull, true);
				}
			}
		}
		#pragma endregion

		#pragma region draw_enemy

		for (int i = 0; i < 2; i++)
		{
			if (nomal_enemy_flag[i] == true)
			{
				if (enemy_pos_y[i] < 0 && enemy_speed[i] != 6)
				{
					enemy_speed[i] = 6;
				}
				else if (enemy_pos_y[i] > WIN_HEIGHT - 128 && enemy_speed[i] != -6)
				{
					enemy_speed[i] = -6;
				}
				enemy_pos_y[i] += enemy_speed[i];

				if (enemy_pos_x_adjust[i] >= nomal_enemy_pos_x[i])
				{
					enemy_pos_x_adjust[i] -= 6;
				}
				DrawBox(enemy_pos_x_adjust[i], enemy_pos_y[i] - 15, enemy_pos_x_adjust[i] + nomal_enemy_HP[i], enemy_pos_y[i] - 5, RED, true);
				DrawGraph(enemy_pos_x_adjust[i], enemy_pos_y[i], nomal_enemy_image, true);
			}
		}
		#pragma endregion
		
		#pragma region draw_player
		if (player_direction == NOMAL && player_HP >= 0)
		{
			DrawRectGraph(player_pos_X, player_pos_Y, player_animation_x, player_animation_y, sprite_x, sprite_y, player_animation_nomal, true, false);
			if (player_animation_x != 1764 - 196)
			{
				animation_flame += 0.5f;
				if (animation_flame == 2.0f)
				{
					player_animation_x += 196;
					animation_flame = 0.0f;
				}
			}
			else
			{
				player_animation_x = 0;
			}
		}
		else if (player_direction == RIGHT && player_HP >= 0)
		{
			DrawGraph(player_pos_X, player_pos_Y, player_image_turn_on, true);
		}
		else if (player_direction == LEFT && player_HP >= 0)
		{
			DrawGraph(player_pos_X, player_pos_Y, player_image_turn_back , true);
		}
		#pragma endregion
		
		#pragma region world_config
		if (nomal_enemy_flag[0] == false && nomal_enemy_flag[1] == false)
		{
			DrawGraph(0, 0, clear_window, true);
			world_count--;
		}
		if (player_HP <= 0)
		{
			DrawGraph(0, 0, effect_window, true);
			DrawGraph(0, 0, over_window, true);
			world_count--;
		}
		#pragma endregion

		ScreenFlip();	// (ダブルバッファ)裏面
		WaitTimer(20);	// 20ミリ秒待機(疑似60FPS)

		if (CheckHitKey(KEY_INPUT_ESCAPE) == true || ProcessMessage() == -1 || world_count == 0)
		{
			break;
		}
	}
}

int collision, collision1, collision2;
bool hit_judge(int x1, int x2, int y1, int y2, int r1, int r2)
{
	collision1 = (x1 + r1) - (x2 + r2);
	collision2 = (y1 + r1) - (y2 + r2);
	collision = sqrt((collision1 * collision1) + (collision2 * collision2));

	if (collision <= r1 + r2) { return true; }
	else { return false; }
}