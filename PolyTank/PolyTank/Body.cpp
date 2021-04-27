#include "Body.h"

using namespace DirectX;

Body::Body(std::unique_ptr<ConvexShape>&& pShape, DirectX::FXMVECTOR initPos, DirectX::FXMVECTOR initRot) :
	pShape(std::move(pShape)) {
	
	XMStoreFloat3(&position, initPos);
	XMStoreFloat4(&rotation, initRot);
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
}

void Body::setRotation(DirectX::FXMVECTOR newRot) {
	XMStoreFloat4(&rotation, newRot);
}

void Body::move(DirectX::FXMVECTOR translation) {
	XMStoreFloat3(&position, XMLoadFloat3(&position) + translation);
}

ConvexShape* Body::getShape() {
	return pShape.get();
}

bool Body::checkCollision(const Body& other, DirectX::XMVECTOR* pResolution) const {

	auto pThisTransformed = pShape->transform(
		XMMatrixRotationQuaternion(XMLoadFloat4(&rotation)) *
		XMMatrixTranslationFromVector(XMLoadFloat3(&position)));

	auto pOtherTransformed = other.pShape->transform(
		XMMatrixRotationQuaternion(XMLoadFloat4(&other.rotation)) *
		XMMatrixTranslationFromVector(XMLoadFloat3(&other.position)));

	return pThisTransformed->checkIntersection(pOtherTransformed.get(), pResolution);
}

