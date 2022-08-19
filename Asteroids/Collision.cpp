#include "Collision.h"
#include "Collider.h"
#include "Game.h"
#include "GameEntity.h"
#include <functional>

Collision::Collision(): playerCollider_(nullptr)
{
}

Collision::~Collision()
{
	for (ColliderList::iterator colliderIt = bulletColliders_.begin(), end = bulletColliders_.end();
		colliderIt != end;
		++colliderIt)
	{
		delete* colliderIt;
	}
	for (ColliderList::iterator colliderIt = astroidColliders_.begin(), end = astroidColliders_.end();
		colliderIt != end;
		++colliderIt)
	{
		delete* colliderIt;
	}
	for (ColliderList::iterator colliderIt = ufoColliders_.begin(), end = ufoColliders_.end();
		colliderIt != end;
		++colliderIt)
	{
		delete* colliderIt;
	}
	if(playerCollider_)
	{
		delete playerCollider_;
	}
}

Collider *Collision::CreateCollider(GameEntity *entity)
{
	Collider *collider = new Collider();

	collider->position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	collider->radius = 0.0f;
	collider->entity = entity;
	collider->enabled = true;
	EntityType etype = entity->GetEntityType();
	//colliders_.push_back(collider);

	//Bullet is frequently creating so checking bullet condition 1st
	if (EntityType::BULLET == etype)
		bulletColliders_.push_back(collider);
	else if (EntityType::ASTEROID == etype)
		astroidColliders_.push_back(collider);
	else if (EntityType::UFO == etype)
		ufoColliders_.push_back(collider);
	else if (EntityType::PLAYER == etype)
		playerCollider_ = collider;

	return collider;
}

void Collision::DestroyCollider(Collider *collider)
{
	EntityType etype = collider->entity->GetEntityType();
	if (EntityType::BULLET == etype)
		bulletColliders_.remove_if(std::bind1st((std::equal_to<Collider*>()), collider));
	else if (EntityType::ASTEROID == etype)
		astroidColliders_.remove_if(std::bind1st((std::equal_to<Collider*>()), collider));
	else if (EntityType::UFO == etype)
		ufoColliders_.remove_if(std::bind1st((std::equal_to<Collider*>()), collider));
	else if (EntityType::PLAYER == etype)
		playerCollider_ = nullptr;
	//colliders_.remove_if(std::bind1st((std::equal_to<Collider *>()), collider));
	delete collider;
	collider = nullptr;
}

void Collision::UpdateColliderPosition(Collider *collider, const XMFLOAT3 &position)
{
	collider->position = position;
}

void Collision::UpdateColliderRadius(Collider *collider, float radius)
{
	collider->radius = radius;
}

void Collision::EnableCollider(Collider *collider)
{
	collider->enabled = true;
}

void Collision::DisableCollider(Collider *collider)
{
	collider->enabled = false;
}

void Collision::DoCollisions(Game *game) const
{
	//to check asteroid is colliding with player or not
	for (ColliderList::const_iterator colliderAIt = astroidColliders_.begin(), end = astroidColliders_.end();
		colliderAIt != end;
		++colliderAIt)
	{
		Collider* colliderA = *colliderAIt;
		Collider* colliderB = playerCollider_;
		if (CollisionTest(colliderA, colliderB))
		{
			game->DoCollision(colliderA->entity, colliderB->entity);
			return;
		}
	}
	//to check ufo is colliding with player or not
	for (ColliderList::const_iterator colliderAIt = ufoColliders_.begin(), end = ufoColliders_.end();
		colliderAIt != end;
		++colliderAIt)
	{
		Collider* colliderA = *colliderAIt;
		Collider* colliderB = playerCollider_;
		if (CollisionTest(colliderA, colliderB))
		{
			game->DoCollision(colliderA->entity, colliderB->entity);
			return;
		}
	}

	//to check bullet and asteroid and ufos
	bool flag = false;
	for (ColliderList::const_iterator colliderAIt = bulletColliders_.begin(), end = bulletColliders_.end();
		colliderAIt != end;
		++colliderAIt)
	{
		//each bullet collision check with asteroids
		for (ColliderList::const_iterator colliderBIt = astroidColliders_.begin(), end2 = astroidColliders_.end();
			colliderBIt != end2; 
			++colliderBIt)
		{
			Collider* colliderA = *colliderAIt;
			Collider* colliderB = *colliderBIt;
			if (CollisionTest(colliderA, colliderB))
			{
				game->DoCollision(colliderA->entity, colliderB->entity);
				flag = true;
				break;
			}
		}
		if (flag)
			break;
		//each bullet collision check with ufos
		for (ColliderList::const_iterator colliderBIt = ufoColliders_.begin(), end2 = ufoColliders_.end();
			colliderBIt != end2;
			++colliderBIt)
		{
			Collider* colliderA = *colliderAIt;
			Collider* colliderB = *colliderBIt;
			if (CollisionTest(colliderA, colliderB))
			{
				game->DoCollision(colliderA->entity, colliderB->entity);
				flag = true;
				break;
			}
		}
		if (flag)
			break;
	}

	
	/*
	* OLD default code
	bool flag = false;
	for (ColliderList::const_iterator colliderAIt = colliders_.begin(), end = colliders_.end();
		colliderAIt != end;
		++colliderAIt)
	{
		ColliderList::const_iterator colliderBIt = colliderAIt;
		for (++colliderBIt; colliderBIt != end; ++colliderBIt)
		{
			Collider *colliderA = *colliderAIt;
			Collider *colliderB = *colliderBIt;
			if (CollisionTest(colliderA, colliderB))
			{
				game->DoCollision(colliderA->entity, colliderB->entity);
				flag = true;
				break;
			}
		}
		if (flag)
			break;
	}*/

}

bool Collision::CollisionTest(Collider *a, Collider *b)
{
	//check pointer before using it
	if (a && a->enabled == false)
		return false;
	if (b && b->enabled == false)
		return false;

	XMVECTOR diff = XMVectorSubtract(XMLoadFloat3(&a->position), XMLoadFloat3(&b->position));
	float distance = XMVectorGetX(XMVector3Length(diff));
	if (distance < (a->radius + b->radius))
	{
		return true;
	}

	return false;
}
