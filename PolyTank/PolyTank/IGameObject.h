#pragma once
class IGameObject
{
public:
	virtual ~IGameObject() = default;

	virtual void update(float dt) = 0;
};

