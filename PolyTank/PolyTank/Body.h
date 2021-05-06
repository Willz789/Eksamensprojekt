#pragma once

#include "ConvexShape.h"
#include <memory>

class Body {
public:
	Body(std::unique_ptr<ConvexShape>&& pShape, DirectX::FXMVECTOR initPos, DirectX::FXMVECTOR initRot);
	virtual ~Body() = default;

	DirectX::XMVECTOR getPosition() const;
	DirectX::XMVECTOR getRotation() const;
	DirectX::XMMATRIX getTransform() const;

	void setPosition(DirectX::FXMVECTOR newPos);
	void setRotation(DirectX::FXMVECTOR newRot);
	
	virtual float getInvMass() const = 0;
	virtual void move(DirectX::FXMVECTOR translation);

	ConvexShape* getShape();

	void updateWorldShape();
	const AABB& getBoundingBox() const;
	bool checkCollision(const Body& other, DirectX::XMVECTOR* pResolution) const;

protected:
	std::unique_ptr<ConvexShape> pShape;
	std::unique_ptr<TransformedShape> pWorldShape;
	AABB boundingBox;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 rotation;
};

