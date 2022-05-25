#pragma once

#include "Mathf.h"

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
    list<Neuron *> *RelativePaths;
    NavMesh *map;
};

struct Neuron_Struct {
    uint8_t flagType;
    uint32_t h;
    uint32_t CostType;
    uint16_t disposition;
};

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
    decltype(Neuron_Struct::disposition) x, y;

    Neuron();
    Neuron(const Disposition &p);
    Neuron(const int &x, const int &y);

    inline const bool locked();
    inline void lock(const bool state);

    inline decltype(Neuron_Struct::flagType) &flag();
    inline decltype(Neuron_Struct::CostType) &cost();
    inline decltype(Neuron_Struct::h) &heuristic();

    inline const int weight();
    inline const uint32_t total();
    inline const bool empty();
};
class NavMesh {
    Neuron *segments;
    void *pmemory;

   public:
    size_t widthSpace, heightSpace;
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

    NavResult find(NavMethodRule method, Neuron *firstNeuron, Neuron *lastNeuron, list<Neuron *> *result,
                   NavAlgorithm algorithm);

    list<Vec2> findSpaces(const int &x, const int &y, int radiusInBlocks);
    list<Vec2> findSpaces(const Neuron *neuron, int radiusInBlocks);
    list<Vec2> findSpaces(const Vec2 &destination, float radiusInBlocks);

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
    static auto GetMinCostPath(list<Neuron *> *paths) -> decltype(std::begin(*paths));

    /// Определяет, функцию пойска по направлениям. Таких как: left, up, right,
    static void AvailPoints(NavMesh *map, NavMethodRule method, Neuron *arrange, Neuron *target, list<Neuron *> *pathTo,
                            std::size_t maxCount = -1, int filterFlag = 0x80000000);
};
}  // namespace RoninEngine::AIPathFinder
