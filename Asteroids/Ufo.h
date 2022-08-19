#ifndef UFO_H_INCLUDED
#define UFO_H_INCLUDED

#include "GameEntity.h"

class Ufo : public GameEntity
{
public:
	Ufo(XMVECTOR position,
		XMVECTOR velocity,
		int size);

	void Update(System *system);
	void Render(Graphics *graphics) const;

private:

	XMFLOAT3 velocity_;
	XMFLOAT3 axis_;
	float angle_;
	float angularSpeed_;
	int size_;

};

#endif // UFO_H_INCLUDED
