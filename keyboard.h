#pragma once

//ヘッダファイルの読み込み
#include "DxLib.h"

//キーボードのヘッダファイル

//マクロ定義
#define KEY_KIND_MAX	256

//キーボード構造体

struct KEYBOARD
{
	char TempKeyState[KEY_KIND_MAX];	//入力状態を管理
	int AllKeyState[KEY_KIND_MAX];		//すべてのキーの入力時間を管理
	int OldAllKeyState[KEY_KIND_MAX];		//以前のキーの入力時間を管理
};

//外部のgフローバル関数
extern KEYBOARD keyboard;

//プロトタイプ宣言
extern VOID AllKeyUpdate(VOID);		//すべてのキーの状態を取得する
extern BOOL KeyDown(int KEY_INPUT_);		//特定のキーを押しているか
extern BOOL KeyUp(int KEY_INPUT_);			//特定のキーをあげているか
extern BOOL KeyClick(int KEY_INPUT_);		//特定のキーをクリックしたか
extern BOOL KeyDownKeep(int KEY_INPUT_,int milSec);		//特定のキーを指定のミリ秒文押し続けているか？