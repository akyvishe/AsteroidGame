#pragma once

#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

#include <Windows.h>

enum class MouseMotion{MOVE_LEFT, MOVE_RIGHT, STEADY};

class Mouse
{
public:

	Mouse();

	void Update();

	void SetLBtnClicked();
	void SetRBtnClicked();
	void SetRBtnReleased();
	bool IsLClick();
	bool IsRClicked();
	bool IsMoveRight();
	bool IsMoveLeft();


	unsigned char GetKeyState(int virtualKey) const;

private:
	POINT lppoint_;
	MouseMotion moveDir_;
	bool isLClicked_;
	bool isRClicked_;
};

#endif // MOUSE_H_INCLUDED
