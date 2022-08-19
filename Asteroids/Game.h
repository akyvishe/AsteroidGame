#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <DirectXMath.h>
#include <list>
#include <chrono>

using namespace DirectX;

class OrthoCamera;
class Background;
class Ship;
class Bullet;
class Asteroid;
class Ufo;
class Explosion;
class Collision;
class System;
class Graphics;
class GameEntity;

class Game
{
public:
	Game();
	~Game();

	void Update(System *system);
	void RenderBackgroundOnly(Graphics *graphics);
	void RenderEverything(Graphics *graphics);
	int GetScore() const;

	void InitialiseLevel(int numAsteroids);
	bool IsLevelComplete() const;
	bool IsGameOver() const;

	void DoCollision(GameEntity *a, GameEntity *b);

private:
	Game(const Game &);
	void operator=(const Game &);

	typedef std::list<Asteroid *> AsteroidList;
	typedef std::list<Ufo *> UfoList;
	typedef std::list<Explosion *> ExplosionList;
	typedef std::list<Bullet *> BulletList;

	void SpawnPlayer();
	void DeletePlayer();

	void UpdatePlayer(System *system);
	void UpdateAsteroids(System *system);
	void UpdateUfos(System* system);
	void UpdateBullet(System *system);
	bool WrapEntity(GameEntity *entity) ;
	bool IsModuloReq(XMFLOAT3& entityPosition);

	void DeleteAllAsteroids();
	void DeleteAllExplosions();

	void SpawnBullet(XMVECTOR position, XMVECTOR direction);
	void DeleteAllBullets();

	void SpawnAsteroids(int numAsteroids);
	void SpawnUfos(int numUfos);
	void SpawnUfoAt(XMVECTOR position, int size);
	void SpawnAsteroidAt(XMVECTOR position, int size);
	bool IsAsteroid(GameEntity* entity) const;
	bool IsBullet(GameEntity* entity) const;
	void AsteroidHit(Asteroid *asteroid);
	void DeleteAsteroid(Asteroid *asteroid);

	void DeleteUfo(Ufo* ufo);

	void UpdateCollisions();

	int score_;
	int delay_;
	int scoreCntrToActivePower_;

	OrthoCamera *camera_;

	Background *background_;
	Ship *player_;
	BulletList bullets_;
	AsteroidList asteroids_;
	UfoList ufos_;
	ExplosionList explosions_;

	Collision *collision_;
	std::chrono::time_point<std::chrono::high_resolution_clock> bullettime_;
};

#endif // GAME_H_INCLUDED
