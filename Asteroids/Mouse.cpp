#include "Keyboard.h"
#include "Mouse.h"

Mouse::Mouse():
	moveDir_(MouseMotion::STEADY),
	isLClicked_(false),
	isRClicked_(false)
{
	SetCursorPos(0, 10);
	GetCursorPos(&lppoint_);	
}

void Mouse::Update()
{
	POINT lp;
	bool b = GetCursorPos(&lp);
	if (b) {
		if (lp.x < lppoint_.x)
			moveDir_ = MouseMotion::MOVE_LEFT;
		else if (lp.x > lppoint_.x)
			moveDir_ = MouseMotion::MOVE_RIGHT;
		else
			moveDir_ = MouseMotion::STEADY;
		lppoint_ = lp;
	}

}

bool Mouse::IsMoveRight()
{
	return (moveDir_ == MouseMotion::MOVE_RIGHT) ? true : false;
}

bool Mouse::IsMoveLeft()
{
	return (moveDir_ == MouseMotion::MOVE_LEFT) ? true : false;
}
void Mouse::SetLBtnClicked()
{
	isLClicked_ = true;
}
void Mouse::SetRBtnClicked()
{
	isRClicked_ = true;
}
void Mouse::SetRBtnReleased()
{
	isRClicked_ = false;
}
bool Mouse::IsLClick() {

	if (isLClicked_)
	{
		isLClicked_ = false;
		return true;
	}
	return isLClicked_;
}

bool Mouse::IsRClicked()
{
	return isRClicked_;
}





