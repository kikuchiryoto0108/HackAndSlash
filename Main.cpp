# include <Siv3D.hpp> // Siv3D v0.6.16
#include "GameWorld.h"
#include "Player.h"
#include "EnemySpawner.h"
#include "TreasureChest.h"
#include "ItemDatabase.h"

static GameWorld world;

void Main()
{
	// モニターの解像度を取得
	const auto monitors = System::EnumerateMonitors();
	const Size monitorResolution = monitors[System::GetCurrentMonitorIndex()].displayRect.size;

	// 16:9のアスペクト比で最大サイズを計算
	int targetWidth = monitorResolution.x;
	int targetHeight = static_cast<int>(targetWidth / 16.0 * 9.0);

	// ウィンドウサイズを設定してフルスクリーン化
	Window::Resize(targetWidth, targetHeight);
	Window::SetFullscreen(true, (targetWidth, targetHeight));

	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ 0.2, 0.2, 0.7 });

	// アイテムデータベースを初期化
	ItemDatabase::GetInstance();

	world.Registar(new Player(&world, Scene::Center()));
	world.Registar(new EnemySpawner(&world, Scene::Size() * Vec2 { 0.8f, 0.2f }));
	
	// テスト用宝箱を配置
	world.Registar(new TreasureChest(&world, Scene::Center() + Vec2(-200, -200), false)); // 通常宝箱
	world.Registar(new TreasureChest(&world, Scene::Center() + Vec2(200, -200), true));   // レア宝箱
	world.Registar(new TreasureChest(&world, Scene::Center() + Vec2(-200, 200), false));  // 通常宝箱
	world.Registar(new TreasureChest(&world, Scene::Center() + Vec2(200, 200), true));    // レア宝箱
	
	// 追加の宝箱をマップ周辺に配置
	world.Registar(new TreasureChest(&world, Vec2(100, 100), false));
	world.Registar(new TreasureChest(&world, Vec2(Scene::Width() - 100, 100), true));
	world.Registar(new TreasureChest(&world, Vec2(100, Scene::Height() - 100), true));
	world.Registar(new TreasureChest(&world, Vec2(Scene::Width() - 100, Scene::Height() - 100), false));
	
	// テスト用のアイテム直接ドロップ
	ItemDropper::DropItem(&world, Scene::Center() + Vec2(-100, 0), 1, 3);  // ヒーリングポーション
	ItemDropper::DropItem(&world, Scene::Center() + Vec2(-50, 0), 2, 2);   // マナポーション
	ItemDropper::DropItem(&world, Scene::Center() + Vec2(0, 0), 401, 10);  // 金貨
	ItemDropper::DropItem(&world, Scene::Center() + Vec2(50, 0), 6, 1);    // 力の薬
	ItemDropper::DropItem(&world, Scene::Center() + Vec2(100, 0), 104, 1); // 雷鳴の槍

	// ヘルプテキスト用フォント登録
	FontAsset::Register(U"HelpFont", 16);

	while (System::Update())
	{
		world.Update();

		world.Draw();

		// ヘルプテキストを表示
		FontAsset(U"HelpFont")(U"=== 操作方法 ===").draw(20, Scene::Height() - 200, Palette::Yellow);
		FontAsset(U"HelpFont")(U"WASD: 移動").draw(20, Scene::Height() - 180, Palette::White);
		FontAsset(U"HelpFont")(U"Space: 通常攻撃（周囲）").draw(20, Scene::Height() - 160, Palette::White);
		FontAsset(U"HelpFont")(U"Shift: 特殊攻撃（コンボ連動）").draw(20, Scene::Height() - 140, Palette::White);
		FontAsset(U"HelpFont")(U"Q/E/R/T: 各種特殊攻撃").draw(20, Scene::Height() - 120, Palette::Cyan);
		FontAsset(U"HelpFont")(U"I: インベントリ開閉").draw(20, Scene::Height() - 100, Palette::Lime);
		FontAsset(U"HelpFont")(U"F: 宝箱を開く").draw(20, Scene::Height() - 80, Palette::Orange);
		FontAsset(U"HelpFont")(U"1-9: クイック使用").draw(20, Scene::Height() - 60, Palette::White);
		FontAsset(U"HelpFont")(U"Shift+クリック: 装備切替").draw(20, Scene::Height() - 40, Palette::White);
		FontAsset(U"HelpFont")(U"右クリック: アイテム使用").draw(20, Scene::Height() - 20, Palette::White);
	}
}

