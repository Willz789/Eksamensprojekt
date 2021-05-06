#include "TransformedShape.h"

#include <algorithm>
#include <functional>
#include <cassert>
#include <iomanip>

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
	
	EPATriangle(std::vector<XMFLOAT3>& vertices, size_t a, size_t b, size_t c) :
		vertices(vertices),
		indices{ a, b, c } {

		XMVECTOR va = XMLoadFloat3(&vertices[a]);
		XMVECTOR vb = XMLoadFloat3(&vertices[b]);
		XMVECTOR vc = XMLoadFloat3(&vertices[c]);

		XMVECTOR vn = XMVector3Normalize(XMVector3Cross(vb - va, vc - va));
		XMStoreFloat3(&n, vn);
		dist = XMVectorGetX(XMVector3Dot(va, vn));

		if (dist + 0.000001f < 0.0f) {
			std::swap(b, c);
			XMStoreFloat3(&n, -vn);
			dist = -dist;
		}
	}

	EPATriangle& operator=(const EPATriangle& other) {
		a = other.a;
		b = other.b;
		c = other.c;

		n = other.n;
		dist = other.dist;

		return *this;
	}

	XMVECTOR operator[](size_t idx) {
		return XMLoadFloat3(&vertices[indices[idx]]);
	}

	XMVECTOR normal() {
		return XMLoadFloat3(&n);
	}

	union {
		size_t indices[3];

		struct {
			size_t a;
			size_t b;
			size_t c;
		};
	};

	std::vector<XMFLOAT3>& vertices;
	XMFLOAT3 n;
	float dist;
};

struct EPAEdge {
	EPAEdge(size_t a, size_t b) :
		a(a),
		b(b) {}

	bool operator==(const EPAEdge& e) {
		return (a == e.b) && (b == e.a);
	}

	size_t a, b;
};

inline void handleEdges(EPATriangle& t, std::vector<EPAEdge>& freeEdges) {
	for (size_t i = 0; i < 3; i++) { // 3 edges in a triangle
		EPAEdge edge(t.indices[i], t.indices[(i + 1) % 3]);

		bool edgeWasRemoved = false;
		for (auto edgeit = freeEdges.begin(); edgeit != freeEdges.end(); edgeit++) {
			// edge was already in list ->
			// it was shared with another removed triangle ->
			// it should be removed
			if (*edgeit == edge) {
				freeEdges.erase(edgeit);
				edgeWasRemoved = true;
				break;
			}
		}

		if (!edgeWasRemoved) {
			// the sharing triangle is still here ->
			// the edge needs to be handled
			freeEdges.push_back(edge);
		}
	}
}

struct EPAPolyhedron {
public:
	EPAPolyhedron(const Simplex& simplex) {
		vertices.resize(4);

		XMStoreFloat3(&vertices[0], simplex[0]);
		XMStoreFloat3(&vertices[1], simplex[1]);
		XMStoreFloat3(&vertices[2], simplex[2]);
		XMStoreFloat3(&vertices[3], simplex[3]);

		triangles.emplace_back(vertices, 3, 2, 1); // ABC
		triangles.emplace_back(vertices, 3, 1, 0); // ACD
		triangles.emplace_back(vertices, 3, 0, 2); // ADB
		triangles.emplace_back(vertices, 2, 0, 1); // BDC

		assertValid();
	}

	void assertValid() {
		for (EPATriangle& t : triangles) {
			XMVECTOR a = t[0];
			XMVECTOR b = t[1];
			XMVECTOR c = t[2];

			XMVECTOR abc = XMVector3Cross(b - a, c - a);

			float tempA = XMVectorGetX(XMVector3Dot(abc, a));
			float tempB = XMVectorGetX(XMVector3Dot(abc, b));
			float tempC = XMVectorGetX(XMVector3Dot(abc, c));

			/*if (tempA < 0.0f || tempB < 0.0f || tempC < 0.0f) 
			{
				printDebug();
			}*/

			assert(XMVectorGetX(XMVector3Dot(abc, a)) >= -0.00001f);
			assert(XMVectorGetX(XMVector3Dot(abc, b)) >= -0.00001f);
			assert(XMVectorGetX(XMVector3Dot(abc, c)) >= -0.00001f);
		}
	}

	void printDebug() {

		std::cout << std::fixed << std::setprecision(5);

		for (EPATriangle& t : triangles) {
			std::cout << "Polygon(";
			std::cout << "(" << vertices[t.a].x << ", " << vertices[t.a].y << ", " << vertices[t.a].z << "), ";
			std::cout << "(" << vertices[t.b].x << ", " << vertices[t.b].y << ", " << vertices[t.b].z << "), ";
			std::cout << "(" << vertices[t.c].x << ", " << vertices[t.c].y << ", " << vertices[t.c].z << ")";
			std::cout << "),\n";
		}
	}

	void addVertex(XMVECTOR v) {
		XMStoreFloat3(&vertices.emplace_back(), v);

		std::vector<EPAEdge> freeEdges;
		triangles.erase(std::remove_if(triangles.begin(), triangles.end(), [&](EPATriangle& t) -> bool {
			if (sameDir(t.normal(), v-t[0])) {
				handleEdges(t, freeEdges);
				return true;
			}
			return false;
		}), triangles.end());

		for (EPAEdge& e : freeEdges) {
			triangles.emplace_back(vertices, e.a, e.b, vertices.size() - 1);
		}
	}


	EPATriangle& closestFace() {
		EPATriangle* pClosest = nullptr;
		float minDist = std::numeric_limits<float>::infinity();

		for (EPATriangle& t : triangles) {
			if (t.dist < minDist) {
				minDist = t.dist;
				pClosest = &t;
			}
		}

		return *pClosest;
	}

	std::vector<XMFLOAT3> vertices;
	std::vector<EPATriangle> triangles;
};

inline XMVECTOR EPA(const MinkowskiDiff& diff, const Simplex& gjkOutput) {
	EPAPolyhedron polyhedron(gjkOutput);

	//size_t n = 0;
	while (true) {
		//std::cout << "iteration: " << n << "\n";
		//polyhedron.printDebug();
		
		EPATriangle& closest = polyhedron.closestFace();
		XMVECTOR dir = closest.normal();
		XMVECTOR p = diff.support(dir);


		if (XMVectorGetX(XMVector3Dot(p, dir)) - closest.dist < 0.00001f) {
			return dir * XMVector3Dot(p, dir); // projection of p onto normal
		}
		
		polyhedron.addVertex(p);
		//n++;
		//polyhedron.assertValid();
	}

	assert(false);
	return XMVectorZero();
}

bool TransformedShape::checkIntersection(const TransformedShape* pOther, DirectX::XMVECTOR* pResolution) const {
	MinkowskiDiff diff = MinkowskiDiff(this, pOther);
	Simplex simplex;
	
	if (GJK(diff, simplex)) {
		*pResolution = EPA(diff, simplex);
		return true;
	}

	return false;
}
