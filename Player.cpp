#include "stdafx.h"
#include "Player.h"
#include "GameWorld.h"
#include "Attack.h"
#include "GameObjectContainer.h"
#include "Collision.h"
#include "ItemDatabase.h"
#include "Item.h"
#include "SkillProjectile.h"

Player::Player(GameWorld* owner, const Vec2& position) : GameObject(owner, "Player")
{
	SetPosition(position);
	m_inventory = std::make_unique<Inventory>(50);

	// ★追加: アイテム使用時のコールバック設定
	m_inventory->SetItemUsedCallback([this](const ItemEffect& effect) {
		ApplyItemEffect(effect);
	});

	m_itemMenuUI = std::make_unique<ItemMenuUI>(m_inventory.get(), this);
	m_itemMenuUI->SetMenuStateCallback([this](bool isOpen) {
		if (GetOwner())
		{
			GetOwner()->SetPaused(isOpen);
		}
	});

	InitializeSkills();
	m_comboTimer.start();
	m_specialCooldown.start();
}

void Player::UpdateUI()
{
	// メニューの更新のみ
	if (m_itemMenuUI)
	{
		m_itemMenuUI->Update();
	}
}

void Player::Update()
{
	if (m_itemMenuUI && m_itemMenuUI->IsOpen())
	{
		return;
	}

	Vec2 dir{};

	if (KeyW.pressed())
	{
		dir.y = -1;
	}

	if (KeyA.pressed())
	{
		dir.x = -1;
	}

	if (KeyS.pressed())
	{
		dir.y = 1;
	}

	if (KeyD.pressed())
	{
		dir.x = 1;
	}

	// 移動速度にステータス効果を適用
	double currentMoveSpeed = MOVE_SPEED;
	for (const auto& effect : m_statusEffects)
	{
		if (effect.type == ItemEffect::Type::SpeedBoost)
		{
			currentMoveSpeed += effect.value;
		}
	}

	if (!dir.isZero())
	{
		SetPosition(GetPosition() + dir.normalized() * currentMoveSpeed * Scene::DeltaTime());
	}

	// 攻撃入力処理
	HandleAttackInput();
	
	// スキル入力処理
	HandleSkillInput();

	if (m_delay.sF() >= 0.5)
	{
		m_delay.reset();
		m_state = State::NORMAL;
	}

	// 自然MP回復（毎秒5MP回復）
	static double lastMpRegenTime = 0.0;
	double currentTime = Time::GetSec();
	if (currentTime - lastMpRegenTime >= 1.0) // 1秒ごと
	{
		RestoreMP(5);
		lastMpRegenTime = currentTime;
	}

	// ステータス効果の更新
	UpdateStatusEffects();
	
	// コンボシステム更新
	UpdateCombos();
	
	// アイテム取得通知の更新
	UpdatePickupNotifications();

	// インベントリ入力処理
	HandleInventoryInput();

	// インベントリUI更新
	if (m_itemMenuUI)
	{
		m_itemMenuUI->Update();
	}

	// アイテム取得判定
	MyCircle player_collision = GetCollision();
	GameObjectContainer items = GetOwner()->GetGameObjects(player_collision);
	for (size_t i = 0; i < items.GetCount(); i++) {
		GameObject* obj = items.GetGameObject(i);
		if (obj->GetTag() == "Item" && !obj->IsDestroy()) {
			// 新しいアイテムシステムを使用
			Item* item = dynamic_cast<Item*>(obj);
			if (item && item->GetItemData())
			{
				const ItemData* itemData = item->GetItemData();
				if (AddItem(itemData, 1))
				{
					m_score += 10;
					obj->Destroy();
					
					// 拾取エフェクト音（TODO: サウンドシステム実装時）
					// 拾取通知表示
					ShowItemPickupNotification(itemData);
				}
			}
			else
			{
				// 従来の処理（後方互換性）
				m_score += 10;
				obj->Destroy();
			}
		}
	}
}

void Player::Draw() const
{
	switch (m_state)
	{
	case State::NORMAL:
		m_texture_normal.drawAt(GetPosition());
		break;
	case State::ATTACK:
		m_texture_attack.drawAt(GetPosition());
		break;
	}

	// UI描画
	DrawStatusBar();
	DrawStatusEffects();
	DrawAttackUI();
	DrawPickupNotifications();
	DrawSkillUI();

	// インベントリUI描画
	if (m_itemMenuUI)
	{
		m_itemMenuUI->Draw();
	}
}

bool Player::AddItem(int itemId, int quantity)
{
	if (!m_inventory) return false;
	
	const ItemData* itemData = ItemDatabase::GetInstance().GetItemData(itemId);
	if (!itemData) return false;
	
	return AddItem(itemData, quantity);
}

bool Player::AddItem(const ItemData* itemData, int quantity)
{
	if (!m_inventory || !itemData) return false;
	
	int addedQuantity = m_inventory->AddItem(itemData, quantity);
	return addedQuantity > 0;
}

void Player::ApplyItemEffect(const ItemEffect& effect)
{
	switch (effect.type)
	{
	case ItemEffect::Type::HealHP:
		Heal(effect.value);
		break;
		
	case ItemEffect::Type::HealMP:
		RestoreMP(effect.value);
		break;
		
	case ItemEffect::Type::AttackBoost:
	case ItemEffect::Type::DefenseBoost:
	case ItemEffect::Type::SpeedBoost:
		// 持続効果として追加
		if (effect.duration > 0.0)
		{
			StatusEffect statusEffect;
			statusEffect.type = effect.type;
			statusEffect.value = effect.value;
			statusEffect.duration = effect.duration;
			statusEffect.remainingTime = effect.duration;
			m_statusEffects.push_back(statusEffect);
		}
		break;
	}
}

void Player::Heal(int amount)
{
	m_hp += amount;
	if (m_hp > m_maxHp) m_hp = m_maxHp;
}

void Player::RestoreMP(int amount)
{
	m_mp += amount;
	if (m_mp > m_maxMp) m_mp = m_maxMp;
}

void Player::GainExp(int amount)
{
	m_exp += amount;
	
	while (m_exp >= m_expToNextLevel)
	{
		m_exp -= m_expToNextLevel;
		LevelUp();
	}
}

void Player::LevelUp()
{
	m_level++;
	m_maxHp += HP_PER_LEVEL;
	m_hp = m_maxHp;  // HP全回復
	m_maxMp += MP_PER_LEVEL;
	m_mp = m_maxMp;  // MP全回復
	m_baseAttack += ATK_PER_LEVEL;
	
	// 次のレベルまでの経験値を計算
	m_expToNextLevel = static_cast<int>(100 * m_level * 1.2);
	
	// レベルアップエフェクト（簡易版）
	Print << U"LEVEL UP! Lv." << m_level;
}

int Player::GetAttack() const
{
	int totalAttack = m_baseAttack;
	
	// ステータス効果から攻撃力ボーナスを加算
	for (const auto& effect : m_statusEffects)
	{
		if (effect.type == ItemEffect::Type::AttackBoost)
		{
			totalAttack += effect.value;
		}
	}
	
	return totalAttack;
}

void Player::InitializeSkills()
{
	m_skills[0] = std::make_unique<Skill>(SkillType::Fireball);
	m_skills[1] = std::make_unique<Skill>(SkillType::IceSpike);
	m_skills[2] = std::make_unique<Skill>(SkillType::Teleport);
	m_skills[3] = std::make_unique<Skill>(SkillType::AreaAttack);
}

void Player::HandleSkillInput()
{
	if (KeyQ.down()) UseSkill(0);
	if (KeyE.down()) UseSkill(1);
	if (KeyR.down()) UseSkill(2);
	if (KeyF.down()) UseSkill(3);
}

void Player::UseSkill(int index)
{
	if (index < 0 || index >= 4) return;

	Skill* skill = m_skills[index].get();
	if (!skill || !skill->CanUse(m_mp)) return;

	const SkillData& data = skill->GetData();

	// MP消費
	m_mp -= data.manaCost;
	skill->Use();

	int skillDamage = static_cast<int>(GetAttack() * data.damageMultiplier);

	switch (data.type)
	{
	case SkillType::Fireball:
	case SkillType::IceSpike:
	{
		// マウス方向に発射
		Vec2 direction = (Cursor::PosF() - GetPosition()).normalized();
		GetOwner()->Registar(new SkillProjectile(
			GetOwner(), GetPosition(), direction,
			skillDamage, data.type, data.color
		));
		break;
	}

	case SkillType::Teleport:
	{
		// マウス方向に150ピクセル移動
		Vec2 direction = (Cursor::PosF() - GetPosition()).normalized();
		SetPosition(GetPosition() + direction * 150.0);
		break;
	}

	case SkillType::AreaAttack:
	{
		// 周囲の敵全てにダメージ
		MyCircle areaRange(GetPosition(), 200.0);
		GameObjectContainer enemies = GetOwner()->GetGameObjects(areaRange);
		for (size_t i = 0; i < enemies.GetCount(); ++i)
		{
			GameObject* obj = enemies.GetGameObject(i);
			if (obj->GetTag() == "Enemy")
			{
				obj->Damage(skillDamage);
			}
		}
		break;
	}
	}
}

void Player::DrawSkillUI() const
{
	// Register fonts once
	static bool fontsRegistered = false;
	if (!fontsRegistered)
	{
		FontAsset::Register(U"SkillCD", 20);
		FontAsset::Register(U"SkillIcon", 32);
		FontAsset::Register(U"SkillKey", 14);
		fontsRegistered = true;
	}
	
	constexpr int iconSize = 60;
	constexpr int spacing = 10;
	int startX = Scene::Width() / 2 - (iconSize * 4 + spacing * 3) / 2;
	int y = Scene::Height() - iconSize - 20;

	const std::array<s3d::String, 4> keys = { U"Q", U"E", U"R", U"F" };

	for (int i = 0; i < 4; ++i)
	{
		int x = startX + i * (iconSize + spacing);
		Rect iconRect(x, y, iconSize, iconSize);

		Skill* skill = m_skills[i].get();
		if (!skill) continue;

		const SkillData& data = skill->GetData();

		// 背景
		ColorF bgColor = skill->CanUse(m_mp) ? ColorF(0.2, 0.2, 0.3, 0.9) : ColorF(0.1, 0.1, 0.15, 0.9);
		iconRect.draw(bgColor);
		iconRect.drawFrame(2, data.color);

		// クールダウンオーバーレイ
		if (!skill->IsReady())
		{
			double ratio = 1.0 - skill->GetCooldownRatio();
			Rect cooldownOverlay(x, static_cast<int>(y + iconSize * ratio), iconSize, static_cast<int>(iconSize * (1.0 - ratio)));
			cooldownOverlay.draw(ColorF(0.0, 0.0, 0.0, 0.7));

			FontAsset(U"SkillCD")(U"{:.1f}"_fmt((data.cooldown - skill->GetCooldownRatio() * data.cooldown)))
				.drawAt(iconRect.center(), Palette::White);
		}

		// アイコン
		FontAsset(U"SkillIcon")(data.iconText).drawAt(iconRect.center(), data.color);

		// キー表示
		FontAsset(U"SkillKey")(keys[i]).draw(x + 5, y + 5, Palette::Yellow);

		// MPコスト
		ColorF mpColor = m_mp >= data.manaCost ? ColorF(0.3, 0.7, 1.0) : ColorF(1.0, 0.3, 0.3);
		FontAsset(U"SkillKey")(U"{}"_fmt(data.manaCost))
			.draw(x + iconSize - 25, y + iconSize - 20, mpColor);
	}
}

void Player::attack()
{
	// MP消費チェック
	const int attackCost = 5;
	if (m_mp < attackCost)
	{
		// MP不足の場合はエフェクトを表示
		return;
	}
	
	m_state = State::ATTACK;
	m_delay.restart();
	
	// MP消費
	m_mp -= attackCost;
	if (m_mp < 0) m_mp = 0;
	
	// コンボカウント増加
	m_comboCount++;
	m_comboTimer.restart();

	// 攻撃力にステータス効果とコンボを適用
	int attackPower = GetAttack();  // 基本攻撃力 + ステータス効果
	
	// コンボボーナス（最大+50%）
	int comboBonus = s3d::Min(m_comboCount * 5, 50);
	attackPower += comboBonus;

	// 強化された攻撃オブジェクトを生成
	GetOwner()->Registar(new Attack(GetOwner(), this));
}

void Player::performSpecialAttack()
{
	if (m_mp < 15) return; // MP不足
	
	m_mp -= 15;
	m_specialCooldown.restart();
	
	// コンボ数に応じて攻撃タイプを変更
	AttackType attackType;
	int damage = 120;
	
	switch (m_comboCount % 4)
	{
	case 0: attackType = AttackType::WHIRLWIND; break;
	case 1: attackType = AttackType::SHOCKWAVE; damage += 20; break;
	case 2: attackType = AttackType::LIGHTNING; damage += 30; break;
	case 3: attackType = AttackType::FIRE_BLAST; damage += 40; break;
	default: attackType = AttackType::WHIRLWIND; break;
	}
	
	GetOwner()->Registar(new Attack(GetOwner(), this, attackType, damage));
}

void Player::UpdateStatusEffects()
{
	double deltaTime = Scene::DeltaTime();
	
	for (auto it = m_statusEffects.begin(); it != m_statusEffects.end();)
	{
		it->remainingTime -= deltaTime;
		if (it->remainingTime <= 0.0)
		{
			it = m_statusEffects.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void Player::DrawStatusBar() const
{
	// Register fonts once
	static bool fontsRegistered = false;
	if (!fontsRegistered)
	{
		FontAsset::Register(U"StatusFont", 16);
		FontAsset::Register(U"StatusFontLarge", 18);
		fontsRegistered = true;
	}
	
	const int barWidth = 200;
	const int barHeight = 20;
	const int barX = 20;
	const int barY = 60;

	// HPバー
	Rect hpBarBG(barX, barY, barWidth, barHeight);
	hpBarBG.draw(ColorF(0.3, 0.3, 0.3));
	
	double hpRatio = static_cast<double>(m_hp) / m_maxHp;
	Rect hpBar(barX, barY, static_cast<int>(barWidth * hpRatio), barHeight);
	
	// HPバーの色をHP残量に応じて変更
	ColorF hpColor;
	if (hpRatio > 0.6) hpColor = ColorF(0.2, 0.8, 0.2);      // 緑（健康）
	else if (hpRatio > 0.3) hpColor = ColorF(0.9, 0.7, 0.2); // オレンジ（注意）
	else hpColor = ColorF(0.8, 0.2, 0.2);   // 赤（危険）
	
	hpBar.draw(hpColor);
	hpBarBG.drawFrame(2, ColorF(0.7, 0.7, 0.7));

	// HPテキスト
	FontAsset(U"StatusFont")(U"HP: {}/{}"_fmt(m_hp, m_maxHp))
		.draw(barX, barY - 20, Palette::White);

	// MPバー
	Rect mpBarBG(barX, barY + 30, barWidth, barHeight);
	mpBarBG.draw(ColorF(0.3, 0.3, 0.3));
	
	double mpRatio = static_cast<double>(m_mp) / m_maxMp;
	Rect mpBar(barX, barY + 30, static_cast<int>(barWidth * mpRatio), barHeight);
	
	// MPバーの色とエフェクト
	ColorF mpColor = ColorF(0.2, 0.4, 1.0);
	mpBar.draw(mpColor);
	
	// MP回復時のエフェクト
	static double lastMP = m_mp;
	static Stopwatch mpRegenEffect;
	if (m_mp > lastMP) 
	{
		mpRegenEffect.restart();
		lastMP = m_mp;
	}
	else 
	{
		lastMP = m_mp;
	}
	
	// MP回復のキラキラエフェクト
	if (mpRegenEffect.sF() < 1.0)
	{
		double effectAlpha = 1.0 - mpRegenEffect.sF();
		for (int i = 0; i < 5; ++i)
		{
			Vec2 sparklePos = Vec2(barX + Random(barWidth), barY + 30 + Random(barHeight));
			Circle(sparklePos, 2).draw(ColorF(0.8, 0.8, 1.0, effectAlpha));
		}
	}
	
	mpBarBG.drawFrame(2, ColorF(0.7, 0.7, 0.7));

	// MPテキスト
	FontAsset(U"StatusFont")(U"MP: {}/{}"_fmt(m_mp, m_maxMp))
		.draw(barX, barY + 10, Palette::White);

	// スコアと所持金
	FontAsset(U"StatusFont")(U"Score: {}"_fmt(m_score)).draw(barX, barY + 60, Palette::White);
	FontAsset(U"StatusFont")(U"Money: {} G"_fmt(m_money)).draw(barX, barY + 80, ColorF(1.0, 0.8, 0.0));
	
	// レベル・経験値バー表示
	FontAsset(U"StatusFontLarge")(U"Lv.{}"_fmt(m_level)).draw(barX, barY + 100, Palette::Yellow);
	
	// 経験値バー
	Rect expBarBg(barX, barY + 125, barWidth, 15);
	expBarBg.draw(ColorF(0.2, 0.2, 0.3, 0.8));
	
	double expRatio = static_cast<double>(m_exp) / m_expToNextLevel;
	Rect expBar(barX, barY + 125, static_cast<int>(barWidth * expRatio), 15);
	expBar.draw(ColorF(0.3, 0.8, 1.0));
	
	FontAsset(U"StatusFont")(U"{}/{}"_fmt(m_exp, m_expToNextLevel))
		.drawAt(barX + barWidth / 2, barY + 132, Palette::White);
}

void Player::DrawStatusEffects() const
{
	if (m_statusEffects.empty()) return;

	int effectX = 250;
	int effectY = 60;
	
	for (size_t i = 0; i < m_statusEffects.size(); ++i)
	{
		const auto& effect = m_statusEffects[i];
		
		// エフェクトアイコンの背景
		Rect effectIcon(effectX + i * 40, effectY, 35, 35);
		
		ColorF iconColor;
		String effectName;
		
		switch (effect.type)
		{
		case ItemEffect::Type::AttackBoost:
			iconColor = ColorF(1.0, 0.5, 0.5);
			effectName = U"ATK";
			break;
		case ItemEffect::Type::DefenseBoost:
			iconColor = ColorF(0.5, 0.5, 1.0);
			effectName = U"DEF";
			break;
		case ItemEffect::Type::SpeedBoost:
			iconColor = ColorF(0.5, 1.0, 0.5);
			effectName = U"SPD";
			break;
		default:
			iconColor = ColorF(0.7, 0.7, 0.7);
			effectName = U"???";
			break;
		}
		
		effectIcon.draw(iconColor);
		effectIcon.drawFrame(1, ColorF(0.9, 0.9, 0.9));
		
		FontAsset::Register(U"EffectFont", 10);
		FontAsset(U"EffectFont")(effectName).drawAt(effectIcon.center(), Palette::White);
		
		// 残り時間バー
		double timeRatio = effect.remainingTime / effect.duration;
		Rect timeBar(effectIcon.x, effectIcon.y + effectIcon.h + 2, 
			static_cast<int>(effectIcon.w * timeRatio), 3);
		timeBar.draw(ColorF(1.0, 1.0, 0.0));
	}
}

void Player::HandleInventoryInput()
{
	// Iキーでインベントリ開閉
	if (KeyI.down())
	{
		if (m_itemMenuUI)
		{
			bool willBeOpen = !m_itemMenuUI->IsOpen();

			m_itemMenuUI->ToggleMenu();

			if (GetOwner()) {
				GetOwner()->SetPaused(willBeOpen);
			}
		}
	}
	
	// 数字キー1-9でクイック使用（ホットバー機能）
	if (m_itemMenuUI && m_itemMenuUI->IsOpen())
	{
		return; // メニュー開いている時はホットバーを無効化
	}
	
	for (int i = 1; i <= 9; ++i)
	{
		// Siv3Dの正しいキー入力方法に修正
		bool keyPressed = false;
		switch(i) {
			case 1: keyPressed = Key1.down(); break;
			case 2: keyPressed = Key2.down(); break;
			case 3: keyPressed = Key3.down(); break;
			case 4: keyPressed = Key4.down(); break;
			case 5: keyPressed = Key5.down(); break;
			case 6: keyPressed = Key6.down(); break;
			case 7: keyPressed = Key7.down(); break;
			case 8: keyPressed = Key8.down(); break;
			case 9: keyPressed = Key9.down(); break;
		}
		
		if (keyPressed)
		{
			if (m_inventory && m_inventory->GetItemCount() > static_cast<size_t>(i - 1))
			{
				InventoryItem* item = m_inventory->GetItem(i - 1);
				if (item && item->itemData && item->itemData->usable)
				{
					// アイテム効果を適用
					ApplyItemEffect(item->itemData->effect);
					
					// 消耗品の場合は使用
					if (item->itemData->type == ItemType::Consumable)
					{
						m_inventory->UseItem(i - 1);
					}
				}
			}
		}
	}
}

void Player::HandleAttackInput()
{
	// 通常攻撃（Spaceキー）
	if (KeySpace.down())
	{
		attack();
	}
	
	// 特殊攻撃（Shiftキー）
	if (KeyShift.down() && m_specialCooldown.sF() >= 3.0) // 3秒のクールダウン
	{
		performSpecialAttack();
	}
	
	// Tキーでの特殊攻撃選択
	if (KeyT.down() && m_mp >= 35) // 炎の爆発
	{
		m_mp -= 35;
		GetOwner()->Registar(new Attack(GetOwner(), this, AttackType::FIRE_BLAST, 250));
	}
}

void Player::UpdateCombos()
{
	// コンボタイマーが3秒経過したらリセット
	if (m_comboTimer.sF() >= 3.0)
	{
		m_comboCount = 0;
	}
}

void Player::DrawAttackUI() const
{
	// 攻撃関連のUI表示
	const int uiX = Scene::Width() - 300;
	const int uiY = 100;
	
	FontAsset::Register(U"AttackUI", 14);
	
	// コンボ表示
	if (m_comboCount > 0)
	{
		ColorF comboColor = ColorF(1.0, 0.8, 0.2);
		FontAsset(U"AttackUI")(U"コンボ: {} Hits!"_fmt(m_comboCount))
			.draw(uiX, uiY, comboColor);
	}
	
	// 特殊攻撃クールダウン表示
	if (m_specialCooldown.sF() < 3.0)
	{
		double cooldownRemaining = 3.0 - m_specialCooldown.sF();
		FontAsset(U"AttackUI")(U"特殊攻撃: {:.1f}s"_fmt(cooldownRemaining))
			.draw(uiX, uiY + 25, ColorF(0.8, 0.8, 0.8));
	}
	else
	{
		FontAsset(U"AttackUI")(U"特殊攻撃: Ready!")
			.draw(uiX, uiY + 25, ColorF(0.2, 1.0, 0.2));
	}
	
	// 操作説明
	FontAsset::Register(U"ControlsUI", 12);
	FontAsset(U"ControlsUI")(U"Space: 通常攻撃").draw(uiX, uiY + 50, ColorF(0.7, 0.7, 0.7));
	FontAsset(U"ControlsUI")(U"Shift: 特殊攻撃").draw(uiX, uiY + 65, ColorF(0.7, 0.7, 0.7));
	FontAsset(U"ControlsUI")(U"T: 炎爆発 (35MP)").draw(uiX, uiY + 80, ColorF(1.0, 0.6, 0.3));
	FontAsset(U"ControlsUI")(U"Q/E/R/F: スキル").draw(uiX, uiY + 95, ColorF(0.7, 0.9, 1.0));
}

void Player::ShowItemPickupNotification(const ItemData* itemData)
{
	if (!itemData) return;
	
	ItemPickupNotification notification;
	notification.itemName = itemData->name;
	notification.itemColor = itemData->GetRarityColor();
	notification.showTime = s3d::Time::GetSec();
	notification.alpha = 1.0;
	
	m_pickupNotifications.push_back(notification);
	
	// 通知が5個を超えたら古いものを削除
	if (m_pickupNotifications.size() > 5)
	{
		m_pickupNotifications.erase(m_pickupNotifications.begin());
	}
}

void Player::UpdatePickupNotifications()
{
	double currentTime = s3d::Time::GetSec();
	
	for (auto it = m_pickupNotifications.begin(); it != m_pickupNotifications.end();)
	{
		double elapsedTime = currentTime - it->showTime;
		
		if (elapsedTime > 3.0) // 3秒で削除
		{
			it = m_pickupNotifications.erase(it);
		}
		else
		{
			// フェードアウト処理
			if (elapsedTime > 2.0)
			{
				it->alpha = 1.0 - (elapsedTime - 2.0); // 2秒後からフェード開始
			}
			++it;
		}
	}
}

void Player::DrawPickupNotifications() const
{
	if (m_pickupNotifications.empty()) return;
	
	s3d::FontAsset::Register(U"NotificationFont", 18);
	
	// 画面上部中央に表示
	int startY = 50;
	
	for (size_t i = 0; i < m_pickupNotifications.size(); ++i)
	{
		const auto& notification = m_pickupNotifications[i];
		
		s3d::String displayText = U"取得: " + notification.itemName;
		s3d::ColorF textColor = notification.itemColor;
		textColor.a = notification.alpha;
		
		// 背景
		s3d::RectF backgroundRectF = s3d::FontAsset(U"NotificationFont")(displayText).region();
		s3d::Rect backgroundRect(
			static_cast<int>(s3d::Scene::Center().x - backgroundRectF.w / 2), 
			startY + static_cast<int>(i) * 30, 
			static_cast<int>(backgroundRectF.w), 
			static_cast<int>(backgroundRectF.h)
		);
		backgroundRect = backgroundRect.stretched(10, 5);
		
		s3d::ColorF bgColor = s3d::ColorF(0.0, 0.0, 0.0, notification.alpha * 0.7);
		backgroundRect.draw(bgColor);
		backgroundRect.drawFrame(1, textColor);
		
		// テキスト
		s3d::FontAsset(U"NotificationFont")(displayText)
			.drawAt(s3d::Scene::Center().x, startY + static_cast<int>(i) * 30 + 15, textColor);
	}
}
