#pragma once

#include "dependency.h"

using Vec2 = RoninEngine::Runtime::Vec2;

namespace RoninEngine::AIPathFinder {
class NavMesh;
class Neuron;

enum NavStatus { Undefined, Fail, Closed, Opened };

enum NavAlgorithm { AStar };

enum NavMethodRule { NavigationIntelegency, PlusMethod, SquareMethod, CrossMethod };

struct Disposition {
    int x;
    int y;
};

struct NavResult {
    NavStatus status;
    NavAlgorithm algorithm;
    Neuron *firstNeuron;
    Neuron *lastNeuron;
    std::list<Neuron *> *RelativePaths;
    NavMesh *map;
};

struct Neuron_Struct {
    std::uint8_t flagType;
    std::uint32_t h;
    std::uint32_t CostType;
    std::uint16_t disposition;
};

//TODO: Class Neuron required optimization
class Neuron {
    friend class NavMesh;

#ifdef _DEBUG
    decltype(Neuron_Struct::CostType) *p_cost;
    decltype(Neuron_Struct::h) *p_h;
    decltype(Neuron_Struct::flagType) *p_flag;
#endif
    void *_data;
    bool m_lock;

   public:
    std::uint16_t x, y;

    Neuron();
    Neuron(const Disposition &p);
    Neuron(const int &x, const int &y);

    inline const bool locked();
    inline void lock(const bool state);

    inline std::uint8_t &flag();
    inline std::uint32_t &cost();
    inline std::uint32_t  &heuristic();

    inline const int weight();
    inline const std::uint32_t total();
    inline const bool empty();
};
class NavMesh {
    Neuron *segments;
    void *pmemory;

   public:
    std::size_t widthSpace, heightSpace;
    Vec2 worldScale;

    explicit NavMesh(std::size_t width, std::size_t height);

    ~NavMesh();

    void clear();
    void clear(bool clearLockedData);
    void randomGenerate();

    Neuron *neuron(Disposition point);
    Neuron *neuron(const Vec2 &vector2);
    Neuron *neuron(const Vec2 &vector2, Disposition &outPoint);
    Neuron *neuron(const int &x, const int &y);

    NavResult find(NavMethodRule method, Neuron *firstNeuron, Neuron *lastNeuron, std::list<Neuron *> *result,
                   NavAlgorithm algorithm);

    std::list<Vec2> findSpaces(const int &x, const int &y, int radiusInBlocks);
    std::list<Vec2> findSpaces(const Neuron *neuron, int radiusInBlocks);
    std::list<Vec2> findSpaces(const Vec2 &destination, float radiusInBlocks);

    const Vec2 PointToWorldPosition(Disposition point);
    const Vec2 PointToWorldPosition(Neuron *path);
    const Vec2 PointToWorldPosition(const int &x, const int &y);
};

class AlgorithmUtils {
   public:
    ///Определяет дистанцию точки от A до точки B
    ///Используется формула Пифагора "(a^2) + (b^2) = c^2"
    ///\par lhs Первоначальная точка
    ///\par rhs Конечная точка
    ///\return Сумма
    static inline int DistancePhf(Neuron *lhs, Neuron *rhs);

    ///Определяет дистанцию точки от A до точки B
    ///\par lhs Первоначальная точка
    ///\par rhs Конечная точка
    ///\return Сумма
    static inline int DistanceManht(Neuron *lhs, Neuron *rhs);

    /// Определяет, минимальную стоимость
    static auto GetMinCostPath(std::list<Neuron *> *paths) -> decltype(std::begin(*paths));

    /// Определяет, функцию пойска по направлениям. Таких как: left, up, right,
    static void AvailPoints(NavMesh *map, NavMethodRule method, Neuron *arrange, Neuron *target, std::list<Neuron *> *pathTo,
                            std::size_t maxCount = -1, int filterFlag = 0x80000000);
};
}  // namespace RoninEngine::AIPathFinder
