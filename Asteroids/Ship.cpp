#include "Ship.h"
#include "Graphics.h"
#include "Maths.h"
#include "ImmediateMode.h"
#include "ImmediateModeVertex.h"
#include "FontEngine.h"
#include <algorithm>
#define MAXLIFE 3
#define DELAY 150

Ship::Ship() :
	accelerationControl_(0.0f),
	rotationControl_(0.0f),
	velocity_(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	forward_(XMFLOAT3(0.0f, 1.0f, 0.0f)),
	rotation_(0.0f),//**TODO: Candidate for crash
	maxLife_(MAXLIFE),
	delay_(DELAY),
	shipstate(ShipState::INIT)
{

	axis[0] = { 0.0f, -5.0f, 0.0f, 0xffffffff };
	axis[1] = { 0.0f, 10.0f, 0.0f, 0xffffffff };
	axis[2] = { -5.0f, 0.0f, 0.0f, 0xffffffff };
	axis[3] = { 5.0f, 0.0f, 0.0f, 0xffffffff };
	axis[4] = { 0.0f, 10.0f, 0.0f, 0xffffffff };
	axis[5] = { -5.0f, 5.0f, 0.0f, 0xffffffff };
	axis[6] = { 0.0f, 10.0f, 0.0f, 0xffffffff };
	axis[7] = { 5.0f, 5.0f, 0.0f, 0xffffffff };

	//to draw in red color
	axis1[0] = { 0.0f, -5.0f, 0.0f, 0xFF0000FF };
	axis1[1] = {0.0f, 10.0f, 0.0f, 0xFF0000FF };
	axis1[2] = {-5.0f, 0.0f, 0.0f, 0xFF0000FF };
	axis1[3] = {5.0f, 0.0f, 0.0f, 0xFF0000FF };
	axis1[4] = {0.0f, 10.0f, 0.0f, 0xFF0000FF };
	axis1[5] = {-5.0f, 5.0f, 0.0f, 0xFF0000FF };
	axis1[6] = {0.0f, 10.0f, 0.0f, 0xFF0000FF };
	axis1[7] = {5.0f, 5.0f, 0.0f, 0xFF0000FF };
	
}

void Ship::SetControlInput(float acceleration,
	float rotation)
{
	accelerationControl_ = acceleration;
	rotationControl_ = rotation;
}

void Ship::Update(System *system)
{
	const float RATE_OF_ROTATION = 0.1f;
	const float MAX_SPEED = 2.0f;
	const float VELOCITY_TWEEN = 0.05f;

	rotation_ = Maths::WrapModulo(rotation_ + rotationControl_ * RATE_OF_ROTATION,
		Maths::TWO_PI);

	XMMATRIX rotationMatrix = XMMatrixRotationZ(rotation_);
	XMVECTOR newForward = XMVector3TransformNormal(XMVectorSet(0.f, 1.0f, 0.0f, 0.0f), rotationMatrix);
	newForward = XMVector3Normalize(newForward);
	XMStoreFloat3(&forward_, newForward);

	XMVECTOR idealVelocity = XMVectorScale(XMLoadFloat3(&forward_), accelerationControl_ * MAX_SPEED);
	XMVECTOR newVelocity = XMVectorLerp(XMLoadFloat3(&velocity_), idealVelocity, VELOCITY_TWEEN);
	XMStoreFloat3(&velocity_, newVelocity);

	XMVECTOR position = GetPosition();
	position = XMVectorAdd(position, XMLoadFloat3(&velocity_));
	SetPosition(position);

	switch (shipstate)
	{
		case ShipState::INIT:
			if (delay_ == 0)
				shipstate = ShipState::NORMAL;
			else delay_--;
			break;
		case ShipState::NORMAL:
			//further improvement
			break;
		case ShipState::POWER:
			auto now = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - powermodetime_);
			if (duration.count() > 10)
				SetState(ShipState::NORMAL);
			break;
	}

}

bool draw = true;



void Ship::Render(Graphics *graphics)
{/*  =: Remove this decalration into constructor because its a fix value and should not be reinitialize again and again
	ImmediateModeVertex axis[8] =
	{
		{0.0f, -5.0f, 0.0f, 0xffffffff}, {0.0f, 10.0f, 0.0f, 0xffffffff},
		{-5.0f, 0.0f, 0.0f, 0xffffffff}, {5.0f, 0.0f, 0.0f, 0xffffffff},
		{0.0f, 10.0f, 0.0f, 0xffffffff}, {-5.0f, 5.0f, 0.0f, 0xffffffff},
		{0.0f, 10.0f, 0.0f, 0xffffffff}, {5.0f, 5.0f, 0.0f, 0xffffffff},
	};*/
	//this is for blink/initial stage effect
	if (delay_ % 10 == 0)
	{
		draw = !draw;
	}
	if (draw || shipstate>=ShipState::NORMAL)
	{
		XMMATRIX rotationMatrix = XMMatrixRotationZ(rotation_);

		XMVECTOR position = GetPosition();
		XMMATRIX translationMatrix = XMMatrixTranslation(
			XMVectorGetX(position),
			XMVectorGetY(position),
			XMVectorGetZ(position));

		XMMATRIX shipTransform = rotationMatrix * translationMatrix;

		ImmediateMode* immediateGraphics = graphics->GetImmediateMode();

		immediateGraphics->SetModelMatrix(shipTransform);
		if(ShipState::POWER == shipstate)
			immediateGraphics->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
			&axis1[0],
			8);
		else
			immediateGraphics->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
				&axis[0],
				8);
		immediateGraphics->SetModelMatrix(XMMatrixIdentity());
	}
	RenderLifesOfPLayer(graphics);

	if (ShipState::POWER == shipstate)
	{
		FontEngine* fontengine = graphics->GetFontEngine();

		std::string mode = "Power mode is active";
		fontengine->DrawTextA(mode,250, 0, 255, FontEngine::FONT_TYPE_MEDIUM);
	}
}

bool Ship::ReduceLife()
{
	shipstate = ShipState::INIT;
	delay_ = DELAY;
	return --maxLife_ ? false : true;
}

XMVECTOR Ship::GetForwardVector() const
{
	return XMLoadFloat3(&forward_);
}

XMVECTOR Ship::GetVelocity() const
{
	return XMLoadFloat3(&velocity_);
}

float Ship::GetRotation()
{
	return rotation_;
}

void Ship::Reset()
{
	accelerationControl_ = 0.0f;
	rotationControl_ = 0.0f;

	velocity_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
	forward_ = XMFLOAT3(0.0f, 1.0f, 0.0f);
	rotation_ = 0.0f;
	delay_ = DELAY;

	SetPosition(XMVectorZero());
}

void Ship::SetState(ShipState state)
{
	if (ShipState::INIT == state)
		delay_ = DELAY;
	else if (ShipState::POWER == state)
		powermodetime_ = std::chrono::high_resolution_clock::now();
	shipstate = state;
}

ShipState Ship::GetState()
{
	return shipstate;
}

void Ship::RenderLifesOfPLayer(Graphics* graphics) const
{

	for (int i = 0; i < maxLife_; ++i) {

		XMMATRIX rotationMatrix = XMMatrixRotationZ(0);

		XMFLOAT3 pos(350. - (i * 30), 280., 0.);
		XMVECTOR position = XMLoadFloat3(&pos);
		XMMATRIX translationMatrix = XMMatrixTranslation(
			XMVectorGetX(position),
			XMVectorGetY(position),
			XMVectorGetZ(position));

		ImmediateMode* immediateGraphics = graphics->GetImmediateMode();

		immediateGraphics->SetModelMatrix(translationMatrix);
		immediateGraphics->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
			&axis[0],
			8);
		immediateGraphics->SetModelMatrix(XMMatrixIdentity());

	}
}