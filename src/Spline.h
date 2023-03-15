#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>

#include <glm/glm.hpp>

class Spline {
public:
    static glm::vec3 CatmullRom(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float t /* between 0 and 1 */, float alpha = .5f /* between 0 and 1 */) {
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
        glm::vec3 C  = (t2 - t) / (t2 - t1) * B1 + (t - t1) / (t2 - t1) * B2;
        return C;
    }
    static glm::vec3 Hermite(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& t0, const glm::vec3& t1, float t) {
        float t2 = t * t;
        float t3 = t2 * t;

        float h00 = (2.0f * t3) - (3.0f * t2) + 1.0f;
        float h10 = t3 - (2.0f * t2) + t;
        float h01 = (-2.0f * t3) + (3.0f * t2);
        float h11 = t3 - t2;

        return (p0 * h00) + (t0 * h10) + (p1 * h01) + (t1 * h11);
    }
    static glm::vec3 BSpline(const std::vector<glm::vec3>& controlPoints, float t, int degree = 3) {
        assert(degree >= 1 && "Degree must be greater than or equal to 1");

        int n = controlPoints.size() - 1;
        std::vector<float> knots = generateUniformKnots(n, degree);

        int span = findSpan(n, degree, t, knots);
        return deBoor(controlPoints, t, span, degree, knots);
    }
private:
    static float lerp(float a, float b, float t) {
        return a + ((b - a) * t);
    }
    static float GetT(float t, float alpha, const glm::vec3& p0, const glm::vec3& p1)
    {
        auto d = p1 - p0;
        float a = glm::dot(d, d); // Dot product
        float b = pow(a, alpha * .5f);
        return (b + t);
    }


    static std::vector<float> generateUniformKnots(int n, int degree) {
        int numKnots = n + degree + 1;
        std::vector<float> knots(numKnots);
        for (int i = 0; i < numKnots; ++i) {
            if (i <= degree) {
                knots[i] = 0.0f;
            }
            else if (i > degree && i < n) {
                knots[i] = static_cast<float>(i - degree) / static_cast<float>(n - degree + 1);
            }
            else {
                knots[i] = 1.0f;
            }
        }
        return knots;
    }

    static int findSpan(int n, int degree, float t, const std::vector<float>& knots) {
        int low = degree;
        int high = n + 1;
        int mid = (low + high) / 2;

        while (t < knots[mid] || t >= knots[mid + 1]) {
            if (t < knots[mid]) {
                high = mid;
            }
            else {
                low = mid;
            }
            mid = (low + high) / 2;
        }

        return mid;
    }

    static glm::vec3 deBoor(const std::vector<glm::vec3>& controlPoints, float t, int span, int degree, const std::vector<float>& knots) {
        std::vector<glm::vec3> points(degree + 1);
        for (int i = 0; i <= degree; ++i) {
            points[i] = controlPoints[span - degree + i];
        }

        for (int r = 1; r <= degree; ++r) {
            for (int i = degree; i >= r; --i) {
                float alpha = (t - knots[span - degree + i]) / (knots[i + span + 1] - knots[span - degree + i]);
                points[i] = points[i] * (1.0f - alpha) + points[i - 1] * alpha;
            }
        }

        return points[degree];
    }

};