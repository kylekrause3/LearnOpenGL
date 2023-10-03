#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

class Spline {
public:
    enum SplineType {
        CatmullRomSpline,
        HermiteSpline
    };

    Spline(std::vector<glm::vec3> & points, SplineType type) {
        this->type = type;
        for (int i = 0; i < points.size(); i++) {
            controlPoints.push_back(points[i]);
        }
    }

    bool AddControlPoint(const glm::vec3& point) {
        if (point != controlPoints.back()) {
            controlPoints.push_back(point);
            return true;
        }
        return false;
    }

    void ChangeControlPoint(int index, const glm::vec3& axis, float amount) {
		this->controlPoints[index] += axis * amount;
	}

    std::vector<glm::vec3> GetControlPoints() {
        return controlPoints;
    }

    glm::vec3 Evaluate(float t) {
        assert(controlPoints.size() >= 4);

        if (type == CatmullRomSpline) {
            return EvaluateCatmullRom(t);
        }
        else {
            return EvaluateHermite(t);
        }
    }
private:
    SplineType type;

    std::vector<glm::vec3> controlPoints;


    glm::vec3 EvaluateCatmullRom(float t) {
        assert(controlPoints.size() >= 4);

        int segmentCount = controlPoints.size() - 1;
        float segmentLength = 1.0f / segmentCount;
        int segmentIndex = std::min(static_cast<int>(t / segmentLength), segmentCount - 1);

        float localT = (t - segmentIndex * segmentLength) / segmentLength;

        // Create ghost points at the start and end
        glm::vec3 ghostStart = 2.0f * controlPoints[0] - controlPoints[1];
        glm::vec3 ghostEnd = 2.0f * controlPoints.back() - controlPoints[controlPoints.size() - 2];

        // Select the appropriate control points for the current segment
        glm::vec3 p0 = segmentIndex == 0 ? ghostStart : controlPoints[segmentIndex - 1];
        glm::vec3 p1 = controlPoints[segmentIndex];
        glm::vec3 p2 = controlPoints[segmentIndex + 1];
        glm::vec3 p3 = segmentIndex == (segmentCount - 1) ? ghostEnd : controlPoints[segmentIndex + 2];

        return CatmullRom(p0, p1, p2, p3, localT);
    }

    glm::vec3 CatmullRom(const glm::vec3 & p0, const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3, float t /* between 0 and 1 */, float alpha = .5f /* between 0 and 1 */) {
        float t0 = 0.0f;
        float t1 = GetT(t0, alpha, p0, p1);
        float t2 = GetT(t1, alpha, p1, p2);
        float t3 = GetT(t2, alpha, p2, p3);
        t = lerp(t1, t2, t);
        glm::vec3 A1 = (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1;
        glm::vec3 A2 = (t2 - t) / (t2 - t1) * p1 + (t - t1) / (t2 - t1) * p2;
        glm::vec3 A3 = (t3 - t) / (t3 - t2) * p2 + (t - t2) / (t3 - t2) * p3;
        glm::vec3 B1 = (t2 - t) / (t2 - t0) * A1 + (t - t0) / (t2 - t0) * A2;
        glm::vec3 B2 = (t3 - t) / (t3 - t1) * A2 + (t - t1) / (t3 - t1) * A3;
        glm::vec3 C = (t2 - t) / (t2 - t1) * B1 + (t - t1) / (t2 - t1) * B2;
        return C;
    }

    

    glm::vec3 EvaluateHermite(float t) {
        assert((controlPoints.size() % 2) == 0);

        int segmentCount = controlPoints.size() / 2 - 1;
        float segmentLength = 1.0f / segmentCount;
        int segmentIndex = std::min(static_cast<int>(t / segmentLength), segmentCount - 1);

        float localT = (t - segmentIndex * segmentLength) / segmentLength;

        // Select the appropriate control points and tangents for the current segment
        glm::vec3 p0 = controlPoints[segmentIndex * 2];
        glm::vec3 p1 = controlPoints[segmentIndex * 2 + 2];
        glm::vec3 t0 = controlPoints[segmentIndex * 2 + 1];
        glm::vec3 t1 = controlPoints[segmentIndex * 2 + 3];

        return Hermite(p0, t0, p1, t1, localT);
    }

    glm::vec3 Hermite(const glm::vec3& p0, const glm::vec3& t0, const glm::vec3& p1, const glm::vec3& t1, float t) {
        float t2 = t * t;
        float t3 = t2 * t;

        float h00 = (2.0f * t3) - (3.0f * t2) + 1.0f;
        float h10 = t3 - (2.0f * t2) + t;
        float h01 = (-2.0f * t3) + (3.0f * t2);
        float h11 = t3 - t2;

        return (p0 * h00) + (t0 * h10) + (p1 * h01) + (t1 * h11);
    }

    
    float lerp(float a, float b, float t) {
        return a + ((b - a) * t);
    }
    float GetT(float t, float alpha, const glm::vec3& p0, const glm::vec3& p1)
    {
        auto d = p1 - p0;
        float a = glm::dot(d, d); // Dot product
        float b = pow(a, alpha * .5f);
        return (b + t);
    }

};

/*

glm::vec3 Hermite(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& tangents, float t) {
        assert(points.size() == 2);
        assert(tangents.size() == 2);
        const glm::vec3& p0 = points[0];
        const glm::vec3& p1 = points[1];
        const glm::vec3& t0 = tangents[0];
        const glm::vec3& t1 = tangents[1];

        float t2 = t * t;
        float t3 = t2 * t;

        float h00 = (2.0f * t3) - (3.0f * t2) + 1.0f;
        float h10 = t3 - (2.0f * t2) + t;
        float h01 = (-2.0f * t3) + (3.0f * t2);
        float h11 = t3 - t2;

        return (p0 * h00) + (t0 * h10) + (p1 * h01) + (t1 * h11);
    }

*/