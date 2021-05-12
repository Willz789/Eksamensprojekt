#include "Body.h"

using namespace DirectX;

Body::Body(std::unique_ptr<ConvexShape>&& pShape, DirectX::FXMVECTOR initPos, DirectX::FXMVECTOR initRot) :
	pShape(std::move(pShape)) {
	
	XMStoreFloat3(&position, initPos);
	XMStoreFloat4(&rotation, initRot);

	transformDirty = true;
	updateWorldShape();

}

XMVECTOR Body::getPosition() const {
	return XMVectorSetW(XMLoadFloat3(&position), 1.0f);
}

DirectX::XMVECTOR Body::getRotation() const {
	return XMLoadFloat4(&rotation);
}

DirectX::XMMATRIX Body::getTransform() const {
	return XMMatrixRotationQuaternion(XMLoadFloat4(&rotation)) * XMMatrixTranslationFromVector(XMLoadFloat3(&position));
}

void Body::setPosition(DirectX::FXMVECTOR newPos) {
	XMStoreFloat3(&position, newPos);
	transformDirty = true;
}

void Body::setRotation(DirectX::FXMVECTOR newRot) {
	XMStoreFloat4(&rotation, newRot);
	transformDirty = true;
}

void Body::move(DirectX::FXMVECTOR translation) {
	XMStoreFloat3(&position, XMLoadFloat3(&position) + translation);
	transformDirty = true;
}

ConvexShape* Body::getShape() {
	return pShape.get();
}

void Body::updateWorldShape() {
	if (transformDirty) {
		pWorldShape = pShape->transform(
			XMMatrixRotationQuaternion(XMLoadFloat4(&rotation)) *
			XMMatrixTranslationFromVector(XMLoadFloat3(&position)));

		boundingBox = pWorldShape->getBoundingBox();
		transformDirty = false;
	}
};

const AABB& Body::getBoundingBox() const {
	return boundingBox;
}

bool Body::checkCollision(const Body& other, DirectX::XMVECTOR* pResolution) const {
	
	return pWorldShape->checkIntersection(other.pWorldShape.get(), pResolution);
}

