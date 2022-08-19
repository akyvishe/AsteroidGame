#include "Game.h"
#include "System.h"
#include "OrthoCamera.h"
#include "Background.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Ufo.h"
#include "Explosion.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Random.h"
#include "Maths.h"
#include "Bullet.h"
#include "Collision.h"
#include <algorithm>
#include "FontEngine.h"
#include "Graphics.h"
#include <string>

Game::Game() :
	camera_(0),
	background_(0),
	player_(0),
	collision_(0),
	score_(0),
	delay_(0),
	scoreCntrToActivePower_(0)
{
	camera_ = new OrthoCamera();
	camera_->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	camera_->SetFrustum(800.0f, 600.0f, -100.0f, 100.0f);
	background_ = new Background(800.0f, 600.0f);
	collision_ = new Collision();
	//bullet_ = nullptr;
	bullettime_ = std::chrono::high_resolution_clock::now();
}

Game::~Game()
{
	delete camera_;
	delete background_;
	delete player_;
	DeleteAllBullets();
	DeleteAllAsteroids();
	DeleteAllExplosions();
	if(collision_)
	delete collision_;

}

void Game::Update(System *system)
{
	UpdatePlayer(system);
	UpdateAsteroids(system);
	UpdateUfos(system);
	UpdateBullet(system);
	UpdateCollisions();
	if (scoreCntrToActivePower_ > 4 && ShipState::POWER != player_->GetState())
	{
		player_->SetState(ShipState::POWER);
		scoreCntrToActivePower_ = 0;
	}
}

void Game::RenderBackgroundOnly(Graphics *graphics)
{
	camera_->SetAsView(graphics);
	background_->Render(graphics);
}

void Game::RenderEverything(Graphics *graphics)
{
	camera_->SetAsView(graphics);

	background_->Render(graphics);

	if (player_)
	{
		player_->Render(graphics);
	}

	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
		++asteroidIt)
	{
		(*asteroidIt)->Render(graphics);
	}
	for (UfoList::const_iterator ufoIt = ufos_.begin(),
		end = ufos_.end();
		ufoIt != end;
		++ufoIt)
	{
		(*ufoIt)->Render(graphics);
	}

	for (BulletList::const_iterator bulletIt = bullets_.begin(), end = bullets_.end();
		bulletIt != end;
		++bulletIt)
	{
		(*bulletIt)->Render(graphics);
	}

	for (ExplosionList::const_iterator explosionIt = explosions_.begin(),
		end = explosions_.end();
		explosionIt != end;
		++explosionIt)
	{
		(*explosionIt)->Render(graphics);
	}

	//to draw score and highscore
	FontEngine* fontengine = graphics->GetFontEngine();

	std::string scorestr = "Score: ";
	std::string score = std::to_string(score_);
	std::string result = scorestr + score;
	fontengine->DrawTextA(result, 5, 5, 0xffffffff, FontEngine::FONT_TYPE_MEDIUM);
	//fontengine->DrawTextA()
}

int Game::GetScore() const
{
	return score_;
}

void Game::InitialiseLevel(int numAsteroids)
{
	//todo: Need exact location to reset the score
	//temporary resetting the score
	if(numAsteroids==1)
		score_ = 0;
	scoreCntrToActivePower_ = 0;
	DeleteAllAsteroids();
	DeleteAllBullets();
	DeleteAllExplosions();

	SpawnPlayer();
	delay_ = 150;
	SpawnAsteroids(numAsteroids);
	SpawnUfos(numAsteroids);
	player_->SetState(ShipState::INIT);
}

bool Game::IsLevelComplete() const
{
	return (asteroids_.empty() && explosions_.empty() && ufos_.empty());
}

bool Game::IsGameOver() const
{
	return (player_ == 0 && explosions_.empty());
}

void Game::DoCollision(GameEntity *a, GameEntity *b)
{
	//comment this code because IsAsteroid function iterating whole list to check its type, instead of this we check with its type
	/*Ship *player = static_cast<Ship *>(a == player_ ? a : (b == player_ ? b : 0));
	Bullet* bullet = static_cast<Bullet*>( IsBullet(a) ? a : (IsBullet(b) ? b : 0));
	Asteroid *asteroid = static_cast<Asteroid *>(IsAsteroid(a) ? a : (IsAsteroid(b) ? b : 0));
	Ufo *asteroid = static_cast<Ufo *>(IsAsteroid(a) ? a : (IsAsteroid(b) ? b : 0));*/

	Ship* player = static_cast<Ship *>( a->GetEntityType() == EntityType::PLAYER ? a : ((b->GetEntityType() == EntityType::PLAYER) ? b : 0));
	Bullet* bullet = static_cast<Bullet*> (a->GetEntityType() == EntityType::BULLET ? a : ((b->GetEntityType() == EntityType::BULLET) ? b : 0));
	Asteroid* asteroid = static_cast<Asteroid*>(a->GetEntityType() == EntityType::ASTEROID ? a : ((b->GetEntityType() == EntityType::ASTEROID) ? b : 0));
	Ufo* ufo = static_cast<Ufo*>(a->GetEntityType() == EntityType::UFO ? a : ((b->GetEntityType() == EntityType::UFO) ? b : 0));

	if (player && asteroid)
	{
		AsteroidHit(asteroid);
		DeletePlayer();
	}
	if (player && ufo)
	{
		DeleteUfo(ufo);
		DeletePlayer();
	}

	if (bullet && asteroid)
	{
		AsteroidHit(asteroid);
		bullet->DisableCollisions();
		bullets_.remove(bullet);
		score_++;
		scoreCntrToActivePower_++;
	}

	if (bullet && ufo)
	{
		DeleteUfo(ufo);
		bullet->DisableCollisions();
		bullets_.remove(bullet);
		score_++;
		scoreCntrToActivePower_++;
	}
}

void Game::SpawnPlayer()
{
	DeletePlayer();
	player_ = new Ship();
	player_->SetEntityType(EntityType::PLAYER);
	player_->EnableCollisions(collision_, 10.0f);
}

void Game::DeletePlayer()
{
	if (player_ && player_->ReduceLife())
	{
		delete player_;
		player_ = 0;
	}
}

void Game::UpdatePlayer(System *system)
{
	if (player_ == 0)
		return;

	Keyboard *keyboard = system->GetKeyboard();
	Mouse *mouse = system->GetMouse();

	float acceleration = 0.0f;
	if (keyboard->IsKeyHeld(VK_UP) || keyboard->IsKeyHeld('W') || mouse->IsRClicked())
	{
		acceleration = 1.0f;
	}
	else if (keyboard->IsKeyHeld(VK_DOWN) || keyboard->IsKeyHeld('S'))
	{
		acceleration = -1.0f;
	}

	float rotation = 0.0f;
	if (keyboard->IsKeyHeld(VK_RIGHT) || keyboard->IsKeyHeld('D') || mouse->IsMoveRight())
	{
		rotation = -1.0f;
	}
	else if (keyboard->IsKeyHeld(VK_LEFT) || keyboard->IsKeyHeld('A') || mouse->IsMoveLeft())
	{
		rotation = 1.0f;
	}

	player_->SetControlInput(acceleration, rotation);
	player_->Update(system);
	WrapEntity(player_);


	auto now = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - bullettime_);
	if (duration.count() > 250 && (keyboard->IsKeyPressed(VK_SPACE) || mouse->IsLClick()))
	{
		bullettime_ = now;
		//bullet will generate only if player in  normal state
		if (ShipState::NORMAL <= player_->GetState())
		{
			XMVECTOR playerForward = player_->GetForwardVector();

			XMVECTOR bulletPosition = player_->GetPosition();//+playerForward * 10.0f;

			XMVECTOR newVec = bulletPosition + playerForward * 10.0f;

			SpawnBullet(newVec, playerForward);
 
			if (ShipState::POWER == player_->GetState())
			{
				float angle = player_->GetRotation();

				XMMATRIX rotationMatrixRight = XMMatrixRotationZ(angle + 0.3f);
				XMVECTOR newRightForward = XMVector3TransformNormal(XMVectorSet(0.f, 1.0f, 0.0f, 0.0f), rotationMatrixRight);
				newRightForward = XMVector3Normalize(newRightForward);
				SpawnBullet(newVec, newRightForward);

				XMMATRIX rotationMatrixLeft = XMMatrixRotationZ(angle - 0.3f);
				XMVECTOR newLeftForward = XMVector3TransformNormal(XMVectorSet(0.f, 1.0f, 0.0f, 0.0f), rotationMatrixLeft);
				newLeftForward = XMVector3Normalize(newLeftForward);
				SpawnBullet(newVec, newLeftForward);
			}
		}
	}
}

void Game::UpdateAsteroids(System *system)
{
	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
	++asteroidIt)
	{
		(*asteroidIt)->Update(system);
		WrapEntity(*asteroidIt);
	}
}

void Game::UpdateUfos(System *system)
{
	for (UfoList::const_iterator ufoIt = ufos_.begin(),
		end = ufos_.end();
		ufoIt != end;
	++ufoIt)
	{
		(*ufoIt)->Update(system);
		WrapEntity(*ufoIt);
	}
}

void Game::UpdateBullet(System *system)
{
	for (BulletList::const_iterator bulletIt = bullets_.begin(), end = bullets_.end();
		bulletIt != end;
		++bulletIt)
	{
		(*bulletIt)->Update(system);
		if(WrapEntity(*bulletIt))
			break;
	}
}

bool Game::IsModuloReq(XMFLOAT3 &entityPosition)
{
	return (entityPosition.x < -400 || entityPosition.x > 400 ||
		entityPosition.y < -300 || entityPosition.y > 300);
}

bool Game::WrapEntity(GameEntity *entity)
{
	XMFLOAT3 entityPosition;
	XMStoreFloat3(&entityPosition, entity->GetPosition());

	if (IsModuloReq(entityPosition) && IsBullet(entity))
	{
		entity->DisableCollisions();
		bullets_.remove((Bullet*)entity);
		return true;
	}
	entityPosition.x = Maths::WrapModulo(entityPosition.x, -400.0f, 400.0f);
	entityPosition.y = Maths::WrapModulo(entityPosition.y, -300.0f, 300.0f);
	entity->SetPosition(XMLoadFloat3(&entityPosition));
	return false;
}

void Game::DeleteAllAsteroids()
{
	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
		++asteroidIt)
	{
		delete (*asteroidIt);
	}

	asteroids_.clear();
}

void Game::DeleteAllExplosions()
{
	for (ExplosionList::const_iterator explosionIt = explosions_.begin(),
		end = explosions_.end();
		explosionIt != end;
	++explosionIt)
	{
		delete (*explosionIt);
	}

	explosions_.clear();
}

void Game::SpawnBullet(XMVECTOR position, XMVECTOR direction)
{
	//DeleteBullet();
	Bullet* bullet = new Bullet(position, direction);
	bullet->SetEntityType(EntityType::BULLET);
	bullet->EnableCollisions(collision_, 3.0f);
	bullets_.push_back(bullet);
}

void Game::DeleteAllBullets()
{
	//delete bullet_;
	//bullet_ = 0;
	for (BulletList::iterator bulletIt = bullets_.begin(), end = bullets_.end();
		bulletIt != end;
		++bulletIt)
	{
		(*bulletIt)->DisableCollisions();
		delete* bulletIt;
	}
	bullets_.clear();
}

void Game::SpawnUfos(int numUfos)
{
	float halfWidth = 800.0f * 0.5f;
	float halfHeight = 600.0f * 0.5f;
	for (int i = 0; i < numUfos/2; i++)
	{
		float x = Random::GetFloat(-halfWidth, halfWidth);
		float y = Random::GetFloat(-halfHeight, halfHeight);
		XMVECTOR position = XMVectorSet(x, y, 0.0f, 0.0f);
		SpawnUfoAt(position, 3);
	}
}
void Game::SpawnUfoAt(XMVECTOR position, int size)
{
	const float MAX_UFO_SPEED = 2.0f;

	float angle = Random::GetFloat(Maths::TWO_PI);
	XMMATRIX randomRotation = XMMatrixRotationZ(angle);
	XMVECTOR velocity = XMVectorSet(0.0f, Random::GetFloat(MAX_UFO_SPEED), 0.0f, 0.0f);
	velocity = XMVector3TransformNormal(velocity, randomRotation);

	Ufo* ufo = new Ufo(position, velocity, size);
	ufo->SetEntityType(EntityType::UFO);
	ufo->EnableCollisions(collision_, size * 5.0f);
	ufos_.push_back((Ufo*)ufo);
}

void Game::SpawnAsteroids(int numAsteroids)
{
	float halfWidth = 800.0f * 0.5f;
	float halfHeight = 600.0f * 0.5f;
	for (int i = 0; i < numAsteroids; i++)
	{
		float x = Random::GetFloat(-halfWidth, halfWidth);
		float y = Random::GetFloat(-halfHeight, halfHeight);
		XMVECTOR position = XMVectorSet(x, y, 0.0f, 0.0f);
		SpawnAsteroidAt(position, 3);
	}
}

void Game::SpawnAsteroidAt(XMVECTOR position, int size)
{
	const float MAX_ASTEROID_SPEED = 1.0f;

	float angle = Random::GetFloat(Maths::TWO_PI);
	XMMATRIX randomRotation = XMMatrixRotationZ(angle);
	XMVECTOR velocity = XMVectorSet(0.0f, Random::GetFloat(MAX_ASTEROID_SPEED), 0.0f, 0.0f);
	velocity = XMVector3TransformNormal(velocity, randomRotation);

	Asteroid *asteroid = new Asteroid(position, velocity, size);
	asteroid->SetEntityType(EntityType::ASTEROID);
	asteroid->EnableCollisions(collision_, size * 5.0f);
	asteroids_.push_back(asteroid);
}

bool Game::IsAsteroid(GameEntity* entity) const
{
	return (std::find(asteroids_.begin(),
		asteroids_.end(), entity) != asteroids_.end());
}
bool Game::IsBullet(GameEntity* entity) const
{
	return (std::find(bullets_.begin(),
		bullets_.end(), entity) != bullets_.end());
}

void Game::AsteroidHit(Asteroid *asteroid)
{
	int oldSize = asteroid->GetSize();
	if (oldSize > 1)
	{
		int smallerSize = oldSize -1;
		XMVECTOR position = asteroid->GetPosition();
		SpawnAsteroidAt(position, smallerSize);
		SpawnAsteroidAt(position, smallerSize);
	}
	DeleteAsteroid(asteroid);
}

void Game::DeleteAsteroid(Asteroid *asteroid)
{
	asteroid->DisableCollisions();
	asteroids_.remove(asteroid);
	delete asteroid;
}

void Game::DeleteUfo(Ufo *ufo)
{
	ufo->DisableCollisions();
	ufos_.remove(ufo);
	delete ufo;
}

void Game::UpdateCollisions()
{
	if(ShipState::NORMAL <= player_->GetState())
		collision_->DoCollisions(this);
}
