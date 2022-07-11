#pragma once

#include "dependency.h"

// TODO: Добавить Storm алгоритм в NavMesh

namespace RoninEngine::AIPathFinder {

class NavMesh;

class Neuron;

enum NavStatus { Undefined, Locked, Closed, Opened };

// TODO: Дополнить список алгоритмов поиска путей
enum NavAlgorithm { AStar };

enum NavMethodRule { NavigationIntelegency, PlusMethod, SquareMethod, CrossMethod };

struct NavResult {
    NavStatus status;
    NavAlgorithm algorithm;
    Neuron *firstNeuron;
    Neuron *lastNeuron;
    std::list<Neuron *> RelativePaths;
    NavMesh *map;
};

struct Neuron_Struct {
    std::uint8_t flagType;
    std::uint32_t h;
    std::uint32_t CostType;
    std::uint16_t disposition;
};

struct NavMeshData {
    std::uint32_t widthSpace;
    std::uint32_t heightSpace;
    void *navmesh;
};

constexpr std::size_t NavMeshDataSizeMultiplier =
    (sizeof(Neuron_Struct::flagType) + sizeof(Neuron_Struct::CostType) + sizeof(Neuron_Struct::h));

class Neuron {
    friend class NavMesh;

#ifdef _DEBUG
    decltype(Neuron_Struct::CostType) *p_cost;
    decltype(Neuron_Struct::h) *p_h;
    decltype(Neuron_Struct::flagType) *p_flag;
#endif
   public:
    std::uint16_t x, y;

    Neuron();
    Neuron(const Runtime::Vec2Int& position);
    Neuron(const int &x, const int &y);

    inline const bool locked();
    inline void lock(const bool state);
    inline std::uint8_t &flag();
    inline std::uint32_t &cost();
    inline std::uint32_t &heuristic();

    inline const int weight();
    inline const std::uint32_t total();
    inline const bool empty();
};
class NavMesh {
    Neuron *segments;
    int segmentOffset;
    void *pmemory;

   public:
    std::size_t widthSpace, heightSpace;
    RoninEngine::Runtime::Vec2 worldScale;

    explicit NavMesh(int width, int height);

    ~NavMesh();

    void clear(bool clearLocks = false);
    void fill(bool fillLocks = false);
    void randomGenerate(int flagFilter = 0xffffff);
    void stress();

    Neuron *neuron(Runtime::Vec2Int nt);
    Neuron *neuron(const RoninEngine::Runtime::Vec2 &vector2);
    Neuron *neuron(const Runtime::Vec2 &vector2, Runtime::Vec2Int& outPoint);
    Neuron *neuron(const int &x, const int &y);

    inline const Runtime::Vec2Int getDisposition(const RoninEngine::Runtime::Vec2& worldPoint);

    void find(NavResult &navResult, NavMethodRule method, Runtime::Vec2 worldPointFirst, Runtime::Vec2 worldPointLast);

    void find(NavResult &navResult, NavMethodRule method, Runtime::Vec2Int first, Runtime::Vec2Int last);
    void find(NavResult &navResult, NavMethodRule method, Neuron *firstNeuron, Neuron *lastNeuron, NavAlgorithm algorithm);

    std::list<RoninEngine::Runtime::Vec2> findSpaces(const int &x, const int &y, int radiusInBlocks);
    std::list<RoninEngine::Runtime::Vec2> findSpaces(const Neuron *neuron, int radiusInBlocks);
    std::list<RoninEngine::Runtime::Vec2> findSpaces(const RoninEngine::Runtime::Vec2 &destination, float radiusInBlocks);

    const RoninEngine::Runtime::Vec2 PointToWorldPosition(const Runtime::Vec2Int& point);
    const RoninEngine::Runtime::Vec2 PointToWorldPosition(Neuron *path);
    const RoninEngine::Runtime::Vec2 PointToWorldPosition(const int &x, const int &y);
    void load(const NavMeshData &navmeshData);
    void save(NavMeshData *navmeshData);

    friend class Neuron;
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
                            std::size_t maxCount = -1, int filterFlag = -1);
};

}  // namespace RoninEngine::AIPathFinder
