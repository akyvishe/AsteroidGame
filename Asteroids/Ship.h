#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "GameEntity.h"
#include "ImmediateModeVertex.h"
#include <chrono>

class Graphics;

enum class ShipState { INIT, NORMAL, POWER };

class Ship : public GameEntity
{
public:
	Ship();

	void SetControlInput(float acceleration,
		float rotation);

	void Update(System *system);
	void Render(Graphics *graphics);
	//this function returns true when all lifes equals to 0
	bool ReduceLife();

	XMVECTOR GetForwardVector() const;
	XMVECTOR GetVelocity() const;
	float GetRotation();

	void Reset();
	void SetState(ShipState shipstate);
	ShipState GetState();

private:
	void RenderLifesOfPLayer(Graphics* graphics) const;

	float accelerationControl_;
	float rotationControl_;

	XMFLOAT3 velocity_;
	XMFLOAT3 forward_;
	float rotation_;
	int maxLife_;
	int delay_;
	ShipState shipstate;
	ImmediateModeVertex axis[8];
	ImmediateModeVertex axis1[8];//to change color of player while running in power mode

	std::chrono::time_point<std::chrono::high_resolution_clock> powermodetime_;

};

#endif // SHIP_H_INCLUDED
