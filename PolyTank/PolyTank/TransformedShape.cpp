#include "TransformedShape.h"

#include <functional>
#include <cassert>

using namespace DirectX;

class MinkowskiDiff {
public:
	MinkowskiDiff(const TransformedShape* pShape1, const TransformedShape* pShape2) :
	pShape1(pShape1),
	pShape2(pShape2) {}
	
	inline XMVECTOR support(FXMVECTOR dir) const {
		return pShape1->support(dir) - pShape2->support(-dir);
	}

private:
	const TransformedShape* pShape1;
	const TransformedShape* pShape2;

};

struct Simplex {
	inline Simplex() {
		index = -1;
		points[0] = XMVectorZero();
	}

	inline Simplex(FXMVECTOR a) {
		index = 0;
		points[0] = a;
	}

	inline Simplex(FXMVECTOR a, FXMVECTOR b) {
		index = 1;
		points[1] = a;
		points[0] = b;
	}

	inline Simplex(FXMVECTOR a, FXMVECTOR b, FXMVECTOR c) {
		index = 2;
		points[2] = a;
		points[1] = b;
		points[0] = c;
	}

	inline Simplex(FXMVECTOR a, FXMVECTOR b, FXMVECTOR c, FXMVECTOR d) {
		index = 3;
		points[3] = a;
		points[2] = b;
		points[1] = c;
		points[0] = d;
	}

	inline XMVECTOR& operator[](size_t idx) {
		return points[idx];
	}

	inline const XMVECTOR& operator[](size_t idx) const {
		return points[idx];
	}

	inline XMVECTOR& last() {
		return points[index];
	}

	inline XMVECTOR& next() {
		index++;
		return last();
	}

	inline size_t dimensions() {
		return index + 1;
	}

	XMVECTOR points[4];
	size_t index;
};

inline bool sameDir(DirectX::FXMVECTOR v1, DirectX::FXMVECTOR v2) {
	return XMVectorGetX(XMVector3Dot(v1, v2)) > 0.0f;
}

inline bool doLine(Simplex& simplex, XMVECTOR& dir, const MinkowskiDiff& diff) {

	XMVECTOR& a = simplex[1];
	XMVECTOR& b = simplex[0];

	XMVECTOR ab = b - a;
	XMVECTOR ao = -a;
	
	if (sameDir(ab, ao)) {
		dir = XMVector3Cross(XMVector3Cross(ab, ao), ab);
	} else {
		simplex = Simplex(a);
		dir = ao;
	}
	
	return false;
}

inline bool doTriangle(Simplex& simplex, XMVECTOR& dir, const MinkowskiDiff& diff) {
	
	XMVECTOR& a = simplex[2];
	XMVECTOR& b = simplex[1];
	XMVECTOR& c = simplex[0];

	XMVECTOR ab = b - a;
	XMVECTOR ac = c - a;
	XMVECTOR ao = -a;

	XMVECTOR abc = XMVector3Cross(ab, ac);
	XMVECTOR abPerp = XMVector3Cross(ab, abc);
	XMVECTOR acPerp = XMVector3Cross(abc, ac);

	if (sameDir(acPerp, ao)) { // outside AC
		if (sameDir(ac, ao)) { // closer to AC than A
			simplex = Simplex(a, c);
			dir = XMVector3Cross(XMVector3Cross(ac, ao), ac);
		} else { // closer to A than AC
			if (sameDir(ab, ao)) { // closer to AB than A
				simplex = Simplex(a, b);
				dir = XMVector3Cross(XMVector3Cross(ab, ao), ab);
			} else { // closer to A than AB
				simplex = Simplex(a);
				dir = ao;
			}
		}

	} else { // inside AC
		if (sameDir(abPerp, ao)) { // outside AB
			if (sameDir(ab, ao)) { // closer to AB than A
				simplex = Simplex(a, b);
				dir = XMVector3Cross(XMVector3Cross(ab, ao), ab);
			} else { // closer to A than AB
				simplex = Simplex(a);
				dir = ao;
			}
		} else { // inside AB -> inside triangle
			if (sameDir(abc, ao)) { // origin is above triangle
				dir = abc;
			} else { // origin is below triangle
				simplex = Simplex(a, c, b);
				dir = -abc;
			}
		}
	}

	return false;
}

inline bool doTetrahedron(Simplex& simplex, XMVECTOR& dir, const MinkowskiDiff& diff) {
	XMVECTOR& a = simplex[3];
	XMVECTOR& b = simplex[2];
	XMVECTOR& c = simplex[1];
	XMVECTOR& d = simplex[0];
	
	XMVECTOR ab = b - a;
	XMVECTOR ac = c - a;
	XMVECTOR ad = d - a;

	XMVECTOR abc = XMVector3Cross(ab, ac);
	XMVECTOR acd = XMVector3Cross(ac, ad);
	XMVECTOR adb = XMVector3Cross(ad, ab);

	XMVECTOR ao = -a;

	if (sameDir(abc, ao)) {
		simplex = Simplex(a, b, c);
		dir = abc;
		return false;
	}

	if (sameDir(acd, ao)) {
		simplex = Simplex(a, c, d);
		dir = acd;
		return false;
	}

	if (sameDir(adb, ao)) {
		simplex = Simplex(a, d, b);
		dir = adb;
		return false;
	}

	return true;
}

inline bool nextSimplex(Simplex& simplex, XMVECTOR& dir, const MinkowskiDiff& diff) {

	switch (simplex.dimensions()) {
	case 2: return doLine(simplex, dir, diff);
	case 3: return doTriangle(simplex, dir, diff);
	case 4: return doTetrahedron(simplex, dir, diff);
	}

	assert(false);
	return false;
}

inline bool GJK(const MinkowskiDiff& diff, Simplex& simplex) {
	
	XMVECTOR dir = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	simplex = Simplex(diff.support(dir));
	
	if (!sameDir(simplex.last(), dir)) return false;

	dir = -simplex.last();

	while (true) {
		simplex.next() = diff.support(dir);
		
		if (!sameDir(simplex.last(), dir)) {
			return false;
		}

		if (nextSimplex(simplex, dir, diff)) {
			return true;
		}

	}
}

struct EPATriangle {
	size_t& operator[](size_t idx) {
		return indices[idx];
	}

	union {
		size_t indices[3];

		struct {
			size_t a;
			size_t b;
			size_t c;
		};
	};

};

class EPAPolyhedron {
public:
	EPAPolyhedron(const Simplex& simplex) {
		vertices.resize(4);

		XMStoreFloat3(&vertices[0], simplex[0]);
		XMStoreFloat3(&vertices[1], simplex[1]);
		XMStoreFloat3(&vertices[2], simplex[2]);
		XMStoreFloat3(&vertices[3], simplex[3]);

		triangles.push_back({ 3, 2, 1 }); // ABC
		triangles.push_back({ 3, 1, 0 }); // ACD
		triangles.push_back({ 3, 0, 2 }); // ADB
		triangles.push_back({ 0, 1, 2 }); // DCB
	}

private:
	std::vector<XMFLOAT3> vertices;
	std::vector<EPATriangle> triangles;

};

void EPA(const Simplex& gjkOutput, const MinkowskiDiff& diff) {
	EPAPolyhedron polyhedron(gjkOutput);


}

bool TransformedShape::checkIntersection(const TransformedShape* pOther, DirectX::XMVECTOR* pResolution) const {
	Simplex simplex;

	if (GJK(MinkowskiDiff(this, pOther), simplex)) {
		return true;
	}

	return false;
}
