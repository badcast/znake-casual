#include "NavMesh.h"

#include <algorithm>

#include "framework.h"

namespace RoninEngine::AIPathFinder {
NavMesh *clnavmesh = nullptr;

Neuron::Neuron() : Neuron(0, 0) {}
Neuron::Neuron(const Vec2Int &position) : Neuron(position.x, position.y) {}
Neuron::Neuron(const int &x, const int &y) {
    this->x = x;
    this->y = y;

#ifdef _DEBUG
    this->p_flag = nullptr;
    this->p_h = nullptr;
    this->p_cost = nullptr;
#endif  // _DEBUG
}

#define MEMORY_DATA                                                                                          \
    (((std::uint8_t *)clnavmesh->pmemory) + ((x * clnavmesh->heightSpace + y) * NavMeshDataSizeMultiplier) + \
     clnavmesh->segmentOffset)

const bool Neuron::locked() {
    auto divide = std::div(x * clnavmesh->heightSpace + y, 8);
    auto pointer = (reinterpret_cast<std::uint8_t *>(clnavmesh->pmemory)) + divide.quot;
    bool result = (*pointer) & (1 << divide.rem);
    return result;
}

void Neuron::lock(const bool state) {
    auto divide = std::div(x * clnavmesh->heightSpace + y, 8);
    auto &&pointer = (reinterpret_cast<std::uint8_t *>(clnavmesh->pmemory)) + divide.quot;
    divide.quot = (1 << divide.rem);
    (*pointer) ^= (*pointer) & (divide.quot);
    (*pointer) |= divide.quot * (state == true);
}

decltype(Neuron_Struct::flagType) &Neuron::flag() { return *MEMORY_DATA; }

decltype(Neuron_Struct::CostType) &Neuron::cost() {
    return *(decltype(Neuron_Struct::CostType) *)(MEMORY_DATA + sizeof(Neuron_Struct::flagType) + sizeof(Neuron_Struct::h));
}

decltype(Neuron_Struct::h) &Neuron::heuristic() {
    return *(decltype(Neuron_Struct::h) *)(MEMORY_DATA + sizeof(Neuron_Struct::flagType));
}

const int Neuron::weight() { return x * x + y * y; }

const uint32_t Neuron::total() { return cost() + heuristic(); }

const bool Neuron::empty() { return !total(); }

NavMesh::NavMesh(int lwidth, int lheight) {
    Neuron *p;
    if (!lwidth || !lheight) throw std::runtime_error("Width or Height is zero!");

    if (clnavmesh != nullptr) {
        throw std::runtime_error("Multi use NavMesh source");
    }
    clnavmesh = this;

    this->widthSpace = lwidth;
    this->heightSpace = lheight;
    this->segments = new RoninEngine::AIPathFinder::Neuron[lheight = (lwidth * lheight)];
    this->worldScale = Vec2::one;
    auto lockedDiv = div(Mathf::max(lheight, 8), 8);  // add locked bits
    segmentOffset = lwidth = lockedDiv.quot + lockedDiv.rem;
    this->pmemory = GC::gc_malloc(lheight * NavMeshDataSizeMultiplier + lwidth);

    for (lwidth = 0; lwidth < widthSpace; ++lwidth) {
        for (lheight = 0; lheight < heightSpace; ++lheight) {
            p = &this->segments[lwidth * heightSpace + lheight];
            p->x = lwidth;
            p->y = lheight;
#ifdef _DEBUG
            p->p_flag = (static_cast<decltype(Neuron_Struct::flagType) *>(p->_data));
            p->p_h = (decltype(Neuron_Struct::h) *)((uint8_t *)p->_data + sizeof(Neuron_Struct::flagType));
            p->p_cost = (decltype(Neuron_Struct::CostType) *)((uint8_t *)p->_data + sizeof(Neuron_Struct::flagType) +
                                                              sizeof(Neuron_Struct::h));
#endif
        }
    }

    clear();
}

NavMesh::~NavMesh() {
    delete[] this->segments;
    GC::gc_free(this->pmemory);

    if (clnavmesh == this) {
        clnavmesh = nullptr;
    }
}

void NavMesh::randomGenerate(int flagFilter) {
    std::uint32_t lhs, rhs = segmentOffset;
    Neuron *p;
    clear(true);
    do {
        lhs = static_cast<std::uint32_t>(rand() & flagFilter);
        memcpy(reinterpret_cast<std::int8_t *>(pmemory) + segmentOffset - rhs, &lhs, Mathf::min(rhs, (std::uint32_t)sizeof(int)));
        rhs -= Mathf::min(rhs, static_cast<std::uint32_t>(sizeof(int)));
    } while (rhs > 0);
}

void NavMesh::stress() {
    NavResult result;
    Vec2Int first, next = {static_cast<int>(widthSpace - 1), static_cast<int>(heightSpace - 1)};
    // TODO: next a strees
    find(result, NavMethodRule::NavigationIntelegency, first, next);
}

void NavMesh::clear(bool clearLocks) {
    std::uint32_t length = widthSpace * heightSpace * NavMeshDataSizeMultiplier;
    std::uint32_t leftoffset;
    if (!clearLocks) {
        leftoffset = this->segmentOffset;
    } else {
        leftoffset = 0;
        length += this->segmentOffset;
    }
    memset(reinterpret_cast<std::uint8_t *>(pmemory) + leftoffset, 0, length);
}

void NavMesh::fill(bool fillLocks) {
    std::uint32_t length = widthSpace * heightSpace * NavMeshDataSizeMultiplier;
    std::uint32_t leftoffset;
    if (!fillLocks) {
        leftoffset = this->segmentOffset;
    } else {
        leftoffset = 0;
        length += this->segmentOffset;
    }
    memset(reinterpret_cast<std::uint8_t *>(pmemory) + leftoffset, 0xff, length);
}

Neuron *NavMesh::neuron(const int &x, const int &y) {
    Neuron *result = nullptr;
    if (x >= 0 && x < widthSpace && y >= 0 && y < heightSpace) result = &segments[x * heightSpace + y];
    return result;
}

const Vec2Int NavMesh::getDisposition(const Vec2 &worldPoint) {
    Vec2Int p;
    p.x = Mathf::ceil(widthSpace / 2 + worldPoint.x / worldScale.x);
    p.y = Mathf::ceil(heightSpace / 2 - worldPoint.y / worldScale.y);
    return p;
}

void NavMesh::find(NavResult &navResult, NavMethodRule method, Runtime::Vec2 worldPointFirst, Runtime::Vec2 worldPointLast) {
    find(navResult, method, this->getDisposition(worldPointFirst), getDisposition(worldPointLast));
}

Neuron *NavMesh::neuron(const Vec2 &vector2) {
    Vec2Int p;
    p.x = Mathf::ceil(widthSpace / 2 + vector2.x / worldScale.x);
    p.y = Mathf::ceil(heightSpace / 2 - vector2.y / worldScale.y);
    return neuron(p);
}
Neuron *NavMesh::neuron(const Runtime::Vec2 &vector2, Runtime::Vec2Int &outPoint) {
    outPoint.x = Mathf::ceil(widthSpace / 2 + vector2.x / worldScale.x);
    outPoint.y = Mathf::ceil(heightSpace / 2 - vector2.y / worldScale.y);
    return neuron(outPoint);
}
Neuron *NavMesh::neuron(Runtime::Vec2Int point) { return neuron(point.x, point.y); }
const Runtime::Vec2 NavMesh::PointToWorldPosition(const Runtime::Vec2Int &point) {
    return PointToWorldPosition(point.x, point.y);
}
const Runtime::Vec2 NavMesh::PointToWorldPosition(Neuron *path) { return PointToWorldPosition(path->x, path->y); }
const Runtime::Vec2 NavMesh::PointToWorldPosition(const int &x, const int &y) {
    Vec2 vec2(widthSpace / 2.f, heightSpace / 2.f);
    vec2.x = (x - vec2.x) * worldScale.x;
    vec2.y = -(y - vec2.y) * worldScale.y;
    return vec2;
}

void NavMesh::load(const NavMeshData &navmeshData) {
    if (!navmeshData.widthSpace || !navmeshData.heightSpace)
        throw std::runtime_error("Argument param, width or height is empty");

    if (navmeshData.navmesh == nullptr) throw std::runtime_error("Data is null");

    if (this->pmemory == navmeshData.navmesh) throw std::runtime_error("pmemory == navmesh data, unflow effect");

    if (this->segments != nullptr) delete[] this->segments;

    if (this->pmemory != nullptr) GC::gc_free(this->pmemory);

    this->widthSpace = navmeshData.widthSpace;
    this->heightSpace = navmeshData.heightSpace;
    this->segments = new Neuron[navmeshData.widthSpace * navmeshData.heightSpace];
    this->pmemory = navmeshData.navmesh;
}

void NavMesh::save(NavMeshData *navmeshData) {
    if (navmeshData == nullptr) return;

    navmeshData->widthSpace = this->widthSpace;
    navmeshData->heightSpace = this->heightSpace;
    navmeshData->navmesh = this->pmemory;
}

std::list<Vec2> NavMesh::findSpaces(const Neuron *neuron, int radiusInBlocks) {
    // TODO: Поиск пустых ячеек рядом
    return {};
}
std::list<Vec2> NavMesh::findSpaces(const int &x, const int &y, int radiusInBlocks) {
    return findSpaces(neuron(x, y), radiusInBlocks);
}

void NavMesh::find(NavResult &navResult, NavMethodRule method, Vec2Int first, Vec2Int last) {
    find(navResult, method, neuron(first), neuron(last), NavAlgorithm::AStar);
}

void NavMesh::find(NavResult &navResult, NavMethodRule method, Neuron *firstNeuron, Neuron *lastNeuron,
                   NavAlgorithm algorithm) {
    navResult.map = this;
    navResult.firstNeuron = firstNeuron;
    navResult.lastNeuron = lastNeuron;
    navResult.algorithm = algorithm;

    if (firstNeuron == nullptr || lastNeuron == nullptr) {
        navResult.status = NavStatus::Undefined;
        return;
    }
    if (firstNeuron->locked() || lastNeuron->locked()) {
        navResult.status = NavStatus::Locked;
        return;
    }

    enum { FLAG_OPEN_LIST = 1, FLAG_CLOSED_LIST = 2, FLAG_TILED_LIST = 4 };

    // list<Neuron*> closed;
    std::list<Neuron *> finded;
    Neuron *current;
    decltype(navResult.RelativePaths)::iterator iter, p1, p2;
    navResult.RelativePaths.clear();
    navResult.RelativePaths.emplace_back(firstNeuron);
    firstNeuron->heuristic() = AlgorithmUtils::DistancePhf(firstNeuron, lastNeuron);

    //Перестройка
    navResult.status = NavStatus::Opened;
    while (!navResult.RelativePaths.empty()) {
        iter = navResult.RelativePaths.begin();  // get the best Neuron
        current = iter.operator*();
        if (current == lastNeuron) {
            break;
        }

        current->flag() = FLAG_CLOSED_LIST;  // change to closing list
        navResult.RelativePaths.erase(iter);

        // Avail
        AlgorithmUtils::AvailPoints(this, method, current, lastNeuron, &finded);
        for (iter = std::begin(finded); iter != std::end(finded); ++iter) {
            if (!(*iter)->flag())  // free path
            {
                (*iter)->flag() = FLAG_OPEN_LIST;
                (*iter)->cost() = current->cost() + 1;
                (*iter)->heuristic() = AlgorithmUtils::DistancePhf((*iter), lastNeuron);

                navResult.RelativePaths.emplace_back((*iter));
                p1 = std::begin(navResult.RelativePaths);
                p2 = std::end(navResult.RelativePaths);

                for (; p1 != p2;) {
                    if ((*p1)->total() > (*iter)->total()) {
                        navResult.RelativePaths.emplace(p1, (*iter));
                        break;
                    }
                    ++p1;
                }
                if (p1 == p2) navResult.RelativePaths.emplace_back((*iter));
            }
        }

        finded.clear();  // clear data
    }

    Neuron *pathIsNot = nullptr;
    current = lastNeuron;
    navResult.RelativePaths.clear();
    navResult.RelativePaths.emplace_back(current);
    while (current != firstNeuron) {
        if (current == pathIsNot) {
            navResult.status = NavStatus::Closed;
            break;
        }
        pathIsNot = current;
        AlgorithmUtils::AvailPoints(this, method, current, firstNeuron, &finded, -1, FLAG_OPEN_LIST | FLAG_CLOSED_LIST);
        for (auto position : finded) {
            if ((position->cost() && position->cost() < current->cost()) || position == firstNeuron) {
                current = position;
                navResult.RelativePaths.emplace_front(current);
                current->flag() = FLAG_TILED_LIST;
            }
        }

        finded.clear();
    }
}

int AlgorithmUtils::DistancePhf(Neuron *a, Neuron *b) {
    return Mathf::pow(a->x - b->x, 2) + Mathf::pow(a->y - b->y, 2);  // a->x * a->y + b->x * b->y;
}

int AlgorithmUtils::DistanceManht(Neuron *a, Neuron *b) { return Mathf::abs(b->x - a->x) + Mathf::abs(b->y - a->y); }

auto AlgorithmUtils::GetMinCostPath(std::list<Neuron *> *paths) -> decltype(std::begin(*paths)) {
    int min = std::numeric_limits<int>::max();
    auto result = begin(*paths);
    for (auto i = result; i != std::end(*paths); ++i) {
        int g = (*i)->total();
        if (g <= min) {
            min = g;
            result = i;
        }
    }
    return result;
}

int GetMatrixMethod(NavMethodRule method, std::int8_t **matrixH, std::int8_t **matrixV) {
    static std::int8_t PLUS_H_POINT[]{-1, 1, 0, 0};
    static std::int8_t PLUS_V_POINT[]{0, 0, -1, 1};

    static std::int8_t M_SQUARE_H_POINT[]{-1, 0, 1, -1, 1, -1, 0, 1};
    static std::int8_t M_SQUARE_V_POINT[]{-1, -1, -1, 0, 0, 1, 1, 1};

    static std::int8_t M_CROSS_H_POINT[]{-1, 1, -1, 1};
    static std::int8_t M_CROSS_V_POINT[]{-1, -1, 1, 1};

    switch (method) {
        case RoninEngine::AIPathFinder::PlusMethod:
            *matrixH = PLUS_H_POINT;
            *matrixV = PLUS_V_POINT;
            return sizeof(PLUS_H_POINT);
            break;
        case RoninEngine::AIPathFinder::SquareMethod:
            *matrixH = M_SQUARE_H_POINT;
            *matrixV = M_SQUARE_V_POINT;
            return sizeof(M_SQUARE_H_POINT);
            break;
        case RoninEngine::AIPathFinder::CrossMethod:
            *matrixH = M_CROSS_H_POINT;
            *matrixV = M_CROSS_V_POINT;
            return sizeof(M_CROSS_H_POINT);
            break;
    }
    return 0;
}

void AlgorithmUtils::AvailPoints(NavMesh *map, NavMethodRule method, Neuron *arrange, Neuron *target,
                                 std::list<Neuron *> *pathTo, std::size_t maxCount, int filterFlag) {
    Neuron *it = nullptr;
    int i = 0, c;
    std::int8_t *matrixH;
    std::int8_t *matrixV;
    switch (method) {
        case RoninEngine::AIPathFinder::NavMethodRule::NavigationIntelegency: {
            // TODO: Написать интелектуальный пойск путей для достижения лучших
            // результатов.
            // TODO: Приводить вектор направление для лучшего достижения.
            // TODO: Выводить оптимальный результат, чтобы было меньше итерации
            // TODO: Вывести итог и анализ скорости.
            c = GetMatrixMethod(NavMethodRule::SquareMethod, &matrixH, &matrixV);
            do {
                it = map->neuron(arrange->x + matrixH[i], arrange->y + matrixV[i]);
                if (it && !it->locked() && (filterFlag == ~0 || it->flag() & filterFlag)) {
                    if (it->x == target->x || it->y == target->y) {
                        if (it == target) {
                            i = c;
                            pathTo->clear();
                        }
                        pathTo->emplace_front(it);
                    } else
                        pathTo->emplace_back(it);
                }
                // next step
            } while (maxCount != pathTo->size() && i++ != c);
        } break;
        default:

            c = GetMatrixMethod(method, &matrixH, &matrixV);
            for (; i != c; ++i) {
                it = map->neuron(arrange->x + matrixH[i], arrange->y + matrixV[i]);
                if (it && !it->locked() && (filterFlag == ~0 || it->flag() & filterFlag)) {
                    pathTo->emplace_back(it);
                    if (maxCount == pathTo->size() || it == target) break;
                }
            }

            break;
    }
}

}  // namespace RoninEngine::AIPathFinder
#undef MEMORY_DATA
