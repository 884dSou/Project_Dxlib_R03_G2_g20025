
//ヘッダファイルの読み込み
#include "game.h"		//ゲーム全体のヘッダファイル
#include"keyboard.h"	//キーボードの処理
#include"FPS.h"			//FPSの処理

//構造体

//キャラクタの構造体
struct CHARCTOR
{
	int handle = -1;	//画像のハンドル（管理番号）
	char path[255];		//画像の場所（パス）
	int x;				//X位置
	int y;				//Y位置
	int width;			//幅
	int height;			//高さ
	int speed = 3;		//移動速度

	RECT coll;			//当たり判定の領域（四角）
	BOOL IsDraw = FALSE;	//画像が描画できる？
};

//動画の構造体
struct MOVIE
{
	int handle = -1;
	char path[255];

	int x;
	int y;
	int width;
	int height;

	int Volume = 255;	//ボリューム（最小）1～255（最大）
};

//音楽の構造体
struct AUDIO
{
	int handle = -1;
	char path[255];

	int Volume = -1;	//ボリューム（MIN 0～255 MAX）
	int playType = -1;
};

//シーンを管理する変数
GAME_SCEAEN GameScene;				//現在のゲームシーン
GAME_SCEAEN OldGameScene;			//前回のゲームシーン
GAME_SCEAEN NextGameScene;			//次のゲームシーン

//プレイ背景の動画
MOVIE playMovie;

//プレイヤー
CHARCTOR player;

//ゴール
CHARCTOR gaol;

//音楽
AUDIO TitleBGM;
AUDIO PlayBGM;
AUDIO EndBGM;

AUDIO playerSE;

//画面の切り替え
BOOL IsFadeOut = FALSE;		//フェードアウト
BOOL IsFadeIn = FALSE;		//フェードイン

//シーン切り替え
int fadeTimeMill = 2000;						//切り替えミリ秒
int fadeTimeMax = fadeTimeMill / 1000 * 60;		//ミリ秒をフレーム秒に変換

//フェードアウト
int fadeOutCntInit = 0;				//初期値
int fadeOutCnt = fadeOutCntInit;	//フェードアウトのカウンタ
int fadeOutCntMax = fadeTimeMax;	//フェードアウトのカウンタMAX

//フェードアウト
int fadeInCntInit = fadeTimeMax;	//初期値
int fadeInCnt = fadeInCntInit;		//フェードインのカウンタ
int fadeInCntMax = 0;				//フェードインのカウンタMAX

//プロトタイプ宣言
VOID Title(VOID);		//タイトル画面
VOID TitleProc(VOID);	//タイトル画面（処理）
VOID TitleDraw(VOID);	//タイトル画面（描画）

VOID Play(VOID);		//プレイ画面
VOID PlayProc(VOID);	//プレイ画面（処理）
VOID PlayDraw(VOID);	//プレイ画面（描画）

VOID End(VOID);			//エンド画面
VOID EndProc(VOID);		//エンド画面（処理）
VOID EndDraw(VOID);		//エンド画面（描画）

VOID Change(VOID);		//切り替え画面
VOID ChangeProc(VOID);	//切り替え画面（処理）
VOID ChangeDraw(VOID);	//切り替え画面（描画）

VOID ChangeScene(GAME_SCEAEN scene);	//シーン切り替え

VOID CollUpdatePlayer(CHARCTOR* chara);	//当たり判定の領域を更新
VOID CollUpdate(CHARCTOR* chara);	//当たり判定

BOOL OnCollrect(RECT a, RECT b);	//矩形と矩形の当たり判定

BOOL GameLoad(VOID);	//ゲームデータの読み込み
VOID GameInit(VOID);

BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType);	//音楽の読み込み

//プログラムはWinMainから始まります
//Windowsのプログラミング方法＝（WinAPI）で動いている
//DxLibは、DirectXという、ゲーミングプログラミングを簡単に使える仕組み
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)

{
	SetOutApplicationLogValidFlag(FALSE);					//log.txtを出力しない
	ChangeWindowMode(TRUE);									//ウィンドウモードに設定
	SetMainWindowText(GAME_TITLE);							//ウィンドウのタイトルの文字
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);		//ウィンドウの解像度を設定
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);					//ウィンドウの大きさを設定
	SetBackgroundColor(255, 255, 255);						//デフォルトの背景の色
	SetWindowIconID(GAME_ICON_ID);							//アイコンファイルを読込
	SetWindowStyleMode(GAME_WINDOW_BAR);					//ウィンドウバーの状態
	SetWaitVSyncFlag(TRUE);									//ディスプレイの垂直同期を有効にする
	SetAlwaysRunFlag(TRUE);									//ウィンドウをずっとアクティブにする

	//DxLibの初期化
	if (DxLib_Init() == -1)			//DXライブラリの初期化処理
	{
		return -1;					//エラーが起きたら直ちに終了
	}

	

	//ダブルバッファリング有効化
	SetDrawScreen(DX_SCREEN_BACK);


	//最初のシーンは、タイトル画面から
	GameScene = GAME_SCENE_TITLE;

	//ゲーム全体の初期化
	if (!GameLoad())
	{
		//ゲームのデータを読み込み
		DxLib_End();	//DxLib終了
		return -1;		//異常終了
	}

	//ゲームの初期化
	GameInit();

	//無限ループ
	while (1)
	{

		if (ProcessMessage() != 0) { break; }

		//画面を消去する
		if (ClearDrawScreen() != 0) { break; }

		//キーボード入力の更新
		AllKeyUpdate();

		//FPS値の更新
		FPSUpdate();

		//ESCキーで強制終了
		if (KeyClick(KEY_INPUT_ESCAPE) == TRUE) { break; }

		//以前のシーンを取得
		if (GameScene != GAME_SCEAEN_CHANGE)
		{
			OldGameScene = GameScene;
		}

		//シーンごとに処理を行う
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:
			Title();			//タイトル画面
			break;
		case GAME_SCENE_PLAY:
			Play();				//プレイ画面
			break;
		case GAME_SCENE_END:
			End();				//エンド画面
			break;
		case GAME_SCEAEN_CHANGE:
			Change();			//切り替え画面
			break;
		default:
			break;
		}

		//シーンを切り替える
		if (OldGameScene != GameScene)
		{
			//現在のシーンが切り替え画面でない時
			if (GameScene != GAME_SCEAEN_CHANGE)
			{
				NextGameScene = GameScene;	//次のシーンを保存
				GameScene = GAME_SCEAEN_CHANGE;
			}
		}


		//FPS値を描画
		FPSDraw();

		//FPS値を待つ
		FPSWait();

		ScreenFlip();		//ダブルバッファリングした画面を描画

	}

	//終わるときの処理
	DeleteGraph(playMovie.handle);
	DeleteGraph(player.handle);
	DeleteGraph(gaol.handle);

	DeleteSoundMem(TitleBGM.handle);
	DeleteSoundMem(PlayBGM.handle);
	DeleteSoundMem(EndBGM.handle);
	
	DeleteSoundMem(playerSE.handle);

	//DxLib使用の終了処理
	DxLib_End();

	return 0;				//ソフトの終了
}

/// <summary>
/// ゲームのデータを読み込み
/// </summary>
/// <returns>読み込めたらTRUE/読み込めなかったらFALSE</returns>
BOOL GameLoad()
{
	//プレイ動画の背景を読み込み
	strcpyDx(playMovie.path, ".\\movie\\playMovie.mp4");
	playMovie.handle = LoadGraph(playMovie.path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (playMovie.handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			playMovie.path,			//メッセージタイトル
			"画像読み込みエラー！",	//メッセージ本文
			MB_OK					//ボタン
		);

		return FALSE;		//エラー終了
	}

	//画像の幅と高さを取得
	GetGraphSize(playMovie.handle, &playMovie.width, &playMovie.height);

	//動画のボリューム
	playMovie.Volume = 255;

	//プレイヤーの画像を読み込み
	strcpyDx(player.path, ".\\image\\player.png");
	player.handle = LoadGraph(player.path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (player.handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			player.path,			//メッセージタイトル
			"画像読み込みエラー！",	//メッセージ本文
			MB_OK					//ボタン
		);

		return FALSE;		//エラー終了
	}

	//画像の幅と高さを取得
	GetGraphSize(player.handle, &player.width, &player.height);

	//ゴールの画像を読み込み
	strcpyDx(gaol.path, ".\\image\\gaol.png");
	gaol.handle = LoadGraph(gaol.path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (gaol.handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			gaol.path,			//メッセージタイトル
			"画像読み込みエラー！",	//メッセージ本文
			MB_OK					//ボタン
		);

		return FALSE;		//エラー終了
	}

	//画像の幅と高さを取得
	GetGraphSize(gaol.handle, &gaol.width, &gaol.height);

	//音楽を読み込む
	if (!LoadAudio(&TitleBGM, ".\\audio\\chiisanaomochabako.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&PlayBGM, ".\\audio\\natsuyasuminotanken.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&EndBGM, ".\\audio\\hiyokonokakekko.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }

	if (!LoadAudio(&playerSE, ".\\audio\\畳の上を走る.mp3", 255, DX_PLAYTYPE_BACK)) { return FALSE; }

	return TRUE;
}

/// <summary>
/// ゲームデータを初期化
/// </summary>
/// <param name=""></param>
VOID GameInit(VOID)
{
	//プレイヤーを初期化
	player.x = GAME_WIDTH / 2 - player.width / 2;
	player.y = GAME_HEIGHT / 2 - player.height / 2;
	player.speed = 500;
	player.IsDraw = TRUE;

	//当たり判定を更新する
	CollUpdatePlayer(&player);
	//ゴールを初期化
	gaol.x = GAME_WIDTH - gaol.width - 10;
	gaol.y = 10;
	gaol.speed = 500;
	gaol.IsDraw = TRUE;

	//当たり判定を更新する
	CollUpdate(&gaol);
}

//▼▼▼▼▼▼▼▼▼▼タイトル画面▼▼▼▼▼▼▼▼▼▼▼

/// <summary>
/// シーンを切り替える関数
/// </summary>
/// <param name="scene">シーン</param>
VOID ChangeScene(GAME_SCEAEN scene)
{
		GameScene = scene;
		IsFadeIn = FALSE;
		IsFadeOut = TRUE;

		return;
}

/// <summary>
/// タイトル画面
/// </summary>
VOID Title(VOID)
{
	TitleProc();		//処理
	TitleDraw();		//描画
	return;
}

/// <summary>
/// タイトル画面の処理
/// </summary>
VOID TitleProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//BGMをとめる
		StopSoundMem(TitleBGM.handle);

		//シーン切り替え
		//次のシーンの初期化をここで行うと楽

		//ゲームの初期化
		GameInit();

		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);

		return;
	}

	//BGMが流れていないとき
	if (CheckSoundMem(TitleBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(TitleBGM.handle, TitleBGM.playType);
	}

	return;
}

/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw(VOID)
{
	DrawString(0, 0, "タイトル画面", GetColor(0, 0, 0));
	return;
}

//▼▼▼▼▼▼▼▼▼▼▼プレイ画面▼▼▼▼▼▼▼▼▼▼▼
/// <summary>
/// プレイ画面
/// </summary>
VOID Play(VOID)
{
	PlayProc();		//処理
	PlayDraw();		//描画
	return;
}

/// <summary>
/// プレイ画面の処理
/// </summary>
VOID PlayProc(VOID)
{
	//if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	//{
	//	//シーン切り替え

	//	//エンド画面に切り替え
	//	ChangeScene(GAME_SCENE_END);
	//}

	//BGMが流れていないとき
	if (CheckSoundMem(PlayBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(PlayBGM.handle, PlayBGM.playType);
	}

	//プレイヤーの操作
	if (KeyDown(KEY_INPUT_W) == TRUE)
	{
		player.y -= player.speed * fps.DeltaTime;
		
		//足音が流れていないとき
		if (CheckSoundMem(playerSE.handle) == 0)
		{
			//動く時の効果音
			PlaySoundMem(playerSE.handle, playerSE.playType);
		}
	}
	if (KeyDown(KEY_INPUT_S) == TRUE)
	{
		player.y += player.speed * fps.DeltaTime;

		//足音が流れていないとき
		if (CheckSoundMem(playerSE.handle) == 0)
		{
			//動く時の効果音
			PlaySoundMem(playerSE.handle, playerSE.playType);
		}
	}
	if (KeyDown(KEY_INPUT_A) == TRUE)
	{
		player.x -= player.speed * fps.DeltaTime;

		//足音が流れていないとき
		if (CheckSoundMem(playerSE.handle) == 0)
		{
			//動く時の効果音
			PlaySoundMem(playerSE.handle, playerSE.playType);
		}
	}
	if (KeyDown(KEY_INPUT_D) == TRUE)
	{
		player.x += player.speed * fps.DeltaTime;
		
		//足音が流れていないとき
		if (CheckSoundMem(playerSE.handle) == 0)
		{
			//動く時の効果音
			PlaySoundMem(playerSE.handle, playerSE.playType);
		}
	}

	//当たり判定を更新する
	CollUpdatePlayer(&player);
	
	//当たり判定を更新する
	CollUpdate(&gaol);

	//プレイヤーがゴールに当たったとき
	if (OnCollrect(player.coll, gaol.coll) == TRUE)
	{
		//BGMをとめる
		StopSoundMem(PlayBGM.handle);

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_END);
		return;		//処理を強制終了
	}

	return;
}

/// <summary>
/// プレイ画面の描画
/// </summary>
VOID PlayDraw(VOID)
{
	//動画を描画
	if (GetMovieStateToGraph(playMovie.handle) == 0)
	{
		//再生する
		SeekMovieToGraph(playMovie.handle, 0);	//シークバーを最初に戻す
		PlayMovieToGraph(playMovie.handle);
	}
	//動画を描画(画面を引き延ばす)
	DrawExtendGraph(0, 0, GAME_WIDTH, GAME_HEIGHT, playMovie.handle, TRUE);


	//ゴールを描画
	if (gaol.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(gaol.x, gaol.y, gaol.handle, TRUE);

		//デバッグの時は、当たり判定領域を描画
		if (Game_DEBUG == TRUE)
		{
			//四角を描画
			DrawBox(gaol.coll.left, gaol.coll.top, gaol.coll.right, gaol.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
	}

	//プレイヤーを描画
	if (player.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(player.x, player.y, player.handle, TRUE);

		//デバッグの時は、当たり判定領域を描画
		if (Game_DEBUG == TRUE)
		{
			//四角を描画
			DrawBox(player.coll.left, player.coll.top, player.coll.right, player.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
	}
	
	DrawString(0, 0, "プレイ画面", GetColor(0, 0, 0));
	return;
}

//▼▼▼▼▼▼▼▼▼▼エンド画面▼▼▼▼▼▼▼▼▼▼▼
/// <summary>
/// エンド画面
/// </summary>
VOID End(VOID)
{
	EndProc();		//処理
	EndDraw();		//描画
	return;
}

/// <summary>
/// エンド画面の処理
/// </summary>
VOID EndProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//BGMをとめる
		StopSoundMem(TitleBGM.handle);

		//シーン切り替え

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}

	//BGMが流れていないとき
	if (CheckSoundMem(EndBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(EndBGM.handle, EndBGM.playType);
	}

	return;
}

/// <summary>
/// エンド画面の描画
/// </summary>
VOID EndDraw(VOID)
{
	DrawString(0, 0, "エンド画面", GetColor(0, 0, 0));
	return;
}

//▼▼▼▼▼▼▼▼▼▼切り替え画面▼▼▼▼▼▼▼▼▼▼
/// <summary>
/// 切り替え画面
/// </summary>
VOID Change(VOID)
{
	ChangeProc();		//処理
	ChangeDraw();		//描画
	return;
}

/// <summary>
/// 切り替え画面の処理
/// </summary>
VOID ChangeProc(VOID)
{
	//フェードイン
	if (IsFadeIn == TRUE)
	{
		if (fadeInCnt > fadeInCntMax)
		{
			fadeInCnt--;		//カウンタを減らす
		}
		else
		{
			//フェードイン処理が終わった

			fadeInCnt = fadeInCntInit;	//カウンタ初期化
			IsFadeIn = FALSE;			//フェードイン処理終了
		}
	}

	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		if (fadeOutCnt < fadeOutCntMax)
		{
			fadeOutCnt++;		//カウンタを減らす
		}
		else
		{
			//フェードアウト処理が終わった

			fadeOutCnt = fadeOutCntInit;	//カウンタ初期化
			IsFadeOut = FALSE;			//フェードアウト処理終了
		}
	}

	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		//フェードインしていない、フェードアウトしていないとき
		GameScene = NextGameScene;		//次のシーンに切り替え
		OldGameScene = GameScene;		//以前のゲームシーン更新
	}

	return;
}

/// <summary>
/// 切り替え画面の描画
/// </summary>
VOID ChangeDraw(VOID)
{
	switch (OldGameScene)
	{
	case GAME_SCENE_TITLE:
		TitleDraw();			//タイトル画面の描画
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();				//プレイ画面の描画
		break;
	case GAME_SCENE_END:
		EndDraw();				//エンド画面の描画
		break;
	default:
		break;
	}

	//フェードイン
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeInCnt / fadeInCntMax) * 255);
	}
	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeOutCnt / fadeOutCntMax) * 255);
	}

	//四角を描画
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0), TRUE);

	//半透明終了
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(0, 0, "切り替え画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// 当たり判定の領域更新（プレイヤー）
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdatePlayer(CHARCTOR* chara)
{
	chara->coll.left = chara->x;
	chara->coll.top = chara->y;
	chara->coll.right = chara->x + chara->width-55;
	chara->coll.bottom = chara->y + chara->height;

	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdate(CHARCTOR* chara)
{
	chara->coll.left = chara->x;
	chara->coll.top = chara->y;
	chara->coll.right = chara->x + chara->width;
	chara->coll.bottom = chara->y + chara->height;

	return;
}

/// <summary>
/// オブジェクトの衝突の確認
/// </summary>
/// <param name="a">当たり判定の領域（a）</param>
/// <param name="b">当たり判定の領域（b）</param>
/// <returns>当たったら「TRUE」当たらなかったら「FALSE」</returns>
BOOL OnCollrect(RECT a, RECT b)
{
	if (
		a.left < b.right &&
		a.right > b.left &&
		a.top < b.bottom &&
		a.bottom > b.top
		)
	{
		//当たっているとき
		return TRUE;
	}
	else
	{
		//当たってないとき
		return FALSE;
	}
}

/// <summary>
/// 音楽の読み込み
/// </summary>
/// <param name="audio">Audio構造体変数のアドレス</param>
/// <param name="path">Audioの音楽パス</param>
/// <param name="volume">ボリューム</param>
/// <param name="playType">DX_PLAYTYPE_LOOP or DX_PLAYTYPE_BACK</param>
/// <returns></returns>
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType)
{
	//音楽の読み込み
	strcpyDx(audio->path,path);
	audio->handle = LoadSoundMem(audio->path);

	if (audio->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),
			audio->path,
			"音楽読み込みエラー",
			MB_OK
		);

		return FALSE;
	}

	//その他設定
	audio->Volume = volume;
	audio->playType = playType;

	return TRUE;
}