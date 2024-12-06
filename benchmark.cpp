#include <iostream>
#include <chrono>
#include <random>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

#include "UnitLib/Matrix.h"
#include "UnitLib/Vector.h"
#include "UnitLib/Unit.h"
#include "UnitLib/UnitMath.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtc/matrix_inverse.hpp>


// Helper struct for test specification
struct TestSpec {
    size_t dimension;
    std::string operation; // "multiply", "determinant", "inverse"
};

template <size_t M, size_t N, typename T>
using UMatrix = Matrix<M, N, T>;

// Generate random matrices without relying on a constructor that takes iterators.
template <typename T, size_t DIM>
void generateRandomMatrices(std::vector<UMatrix<DIM, DIM, T>> &unitMatsA,
                            std::vector<UMatrix<DIM, DIM, T>> &unitMatsB,
                            std::vector<glm::mat<DIM, DIM, T>> &glmMatsA,
                            std::vector<glm::mat<DIM, DIM, T>> &glmMatsB,
                            size_t count, std::mt19937_64 &rng)
{
    std::uniform_real_distribution<T> dist(-1.0, 1.0);
    unitMatsA.reserve(count);
    unitMatsB.reserve(count);
    glmMatsA.reserve(count);
    glmMatsB.reserve(count);

    for (size_t i = 0; i < count; i++) {
        std::array<T, DIM*DIM> valsA, valsB;
        for (auto &v : valsA) v = dist(rng);
        for (auto &v : valsB) v = dist(rng);

        // Construct UnitLib matrices manually
        UMatrix<DIM, DIM, T> uA;
        for (size_t idx = 0; idx < valsA.size(); idx++) {
            uA[ get_row<DIM,DIM>(idx) ][ get_col<DIM,DIM>(idx) ] = valsA[idx];
        }

        UMatrix<DIM, DIM, T> uB;
        for (size_t idx = 0; idx < valsB.size(); idx++) {
            uB[ get_row<DIM,DIM>(idx) ][ get_col<DIM,DIM>(idx) ] = valsB[idx];
        }

        unitMatsA.push_back(uA);
        unitMatsB.push_back(uB);

        // Construct GLM matrices
        glm::mat<DIM, DIM, T> gA((T)1.0), gB((T)1.0);
        for (size_t col = 0; col < DIM; col++) {
            for (size_t row = 0; row < DIM; row++) {
                gA[col][row] = valsA[row * DIM + col];
                gB[col][row] = valsB[row * DIM + col];
            }
        }
        glmMatsA.push_back(gA);
        glmMatsB.push_back(gB);
    }
}

template <size_t DIM, typename T>
double benchmarkUnitLibOperation(const std::string &operation,
                                 const std::vector<UMatrix<DIM,DIM,T>> &unitMatsA,
                                 const std::vector<UMatrix<DIM,DIM,T>> &unitMatsB)
{
    constexpr int WARMUP = 3;
    constexpr int RUNS = 5;
    size_t N = unitMatsA.size();
    std::vector<long long> times;

    for (int r = 0; r < WARMUP + RUNS; r++) {
        auto start = std::chrono::high_resolution_clock::now();

        if (operation == "multiply") {
            UMatrix<DIM,DIM,T> sum = UMatrix<DIM,DIM,T>::Zero();
            for (size_t i = 0; i < N; i++) {
                sum += (unitMatsA[i] * unitMatsB[i]);
            }
            volatile T x = sum.At(0,0);
            (void)x;

        } else if (operation == "determinant") {
            T accum = 0;
            for (size_t i = 0; i < N; i++) {
                auto d = Det(unitMatsA[i]);
                accum += d;
            }
            volatile T x = accum;
            (void)x;

        } else if (operation == "inverse") {
            UMatrix<DIM,DIM,T> sum = UMatrix<DIM,DIM,T>::Zero();
            for (size_t i = 0; i < N; i++) {
                auto invMat = Inv(unitMatsA[i]);
                sum += invMat; 
            }
            volatile T x = sum.At(0,0);
            (void)x;
        }

        auto end = std::chrono::high_resolution_clock::now();
        long long dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        if (r >= WARMUP) {
            times.push_back(dur);
        }
    }

    double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    return avg;
}

template <size_t DIM, typename T>
double benchmarkGLMOperation(const std::string &operation,
                             const std::vector<glm::mat<DIM,DIM,T>> &glmMatsA,
                             const std::vector<glm::mat<DIM,DIM,T>> &glmMatsB)
{
    constexpr int WARMUP = 3;
    constexpr int RUNS = 5;
    size_t N = glmMatsA.size();
    std::vector<long long> times;

    for (int r = 0; r < WARMUP + RUNS; r++) {
        auto start = std::chrono::high_resolution_clock::now();

        if (operation == "multiply") {
            glm::mat<DIM,DIM,T> sum((T)0);
            for (size_t i = 0; i < N; i++) {
                sum += (glmMatsA[i] * glmMatsB[i]);
            }
            volatile T x = sum[0][0];
            (void)x;
        } else if (operation == "determinant") {
            T accum = (T)0;
            for (size_t i = 0; i < N; i++) {
                accum += glm::determinant(glmMatsA[i]);
            }
            volatile T x = accum;
            (void)x;
        } else if (operation == "inverse") {
            glm::mat<DIM,DIM,T> sum((T)0);
            for (size_t i = 0; i < N; i++) {
                auto invMat = glm::inverse(glmMatsA[i]);
                sum += invMat;
            }
            volatile T x = sum[0][0];
            (void)x;
        }

        auto end = std::chrono::high_resolution_clock::now();
        long long dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        if (r >= WARMUP) {
            times.push_back(dur);
        }
    }

    double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    return avg;
}

int main()
{
    std::ofstream results("./benchmark_results.csv");
    results << "MatrixSize,Operation,Library,ExecutionTime(us)\n";

    std::vector<TestSpec> tests = {
        {2, "multiply"}, {2, "determinant"}, {2, "inverse"},
        {3, "multiply"}, {3, "determinant"}, {3, "inverse"},
        {4, "multiply"}, {4, "determinant"}, {4, "inverse"}
    };

    constexpr size_t NUM = 100000;
    using T = double;

    std::mt19937_64 rng(12345);

    for (auto &spec : tests) {
        size_t DIM = spec.dimension;
        std::string op = spec.operation;

        if (DIM == 2) {
            std::vector<UMatrix<2,2,T>> uA,uB;
            std::vector<glm::dmat2> gA,gB;
            generateRandomMatrices<T,2>(uA,uB,gA,gB,NUM,rng);

            double unitTime = benchmarkUnitLibOperation<2,T>(op, uA, uB);
            double glmTime = benchmarkGLMOperation<2,T>(op, gA, gB);

            results << "2x2," << op << ",UnitLib," << unitTime << "\n";
            results << "2x2," << op << ",GLM," << glmTime << "\n";

        } else if (DIM == 3) {
            std::vector<UMatrix<3,3,T>> uA,uB;
            std::vector<glm::dmat3> gA,gB;
            generateRandomMatrices<T,3>(uA,uB,gA,gB,NUM,rng);

            double unitTime = benchmarkUnitLibOperation<3,T>(op, uA, uB);
            double glmTime = benchmarkGLMOperation<3,T>(op, gA, gB);

            results << "3x3," << op << ",UnitLib," << unitTime << "\n";
            results << "3x3," << op << ",GLM," << glmTime << "\n";

        } else if (DIM == 4) {
            std::vector<UMatrix<4,4,T>> uA,uB;
            std::vector<glm::dmat4> gA,gB;
            generateRandomMatrices<T,4>(uA,uB,gA,gB,NUM,rng);

            double unitTime = benchmarkUnitLibOperation<4,T>(op, uA, uB);
            double glmTime = benchmarkGLMOperation<4,T>(op, gA, gB);

            results << "4x4," << op << ",UnitLib," << unitTime << "\n";
            results << "4x4," << op << ",GLM," << glmTime << "\n";
        }
    }

    results.close();

    std::cout << "Benchmarking completed. Results in benchmark_results.csv.\n";

    return 0;
}
