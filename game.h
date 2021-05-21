#pragma once

//ゲーム全体のヘッダファイル

//ヘッダーファイルの読み込み
#include"DxLib.h"		//DxLibを使うのに必要

//マクロ定義
#define GAME_TITLE "ゲームタイトル"
#define GAME_WIDTH 1280
#define GAME_HEIGHT 720
#define GAME_COLOR 32

#define GAME_ICON_ID	333

#define GAME_WINDOW_BAR		0

#define Game_DEBUG TRUE

//列挙型
enum GAME_SCEAEN {
	GAME_SCENE_TITLE,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
	GAME_SCEAEN_CHANGE
};		//ゲームのシーン