#include "NavMesh.h"
#include "pch.h"
#include <algorithm>

namespace RoninEngine::AIPathFinder {
Neuron::Neuron() : Neuron(0, 0) {}
Neuron::Neuron(const Disposition &p) : Neuron(p.x, p.y) {}
Neuron::Neuron(const int &x, const int &y) {
  this->x = x;
  this->y = y;
  this->_data = nullptr;
  this->m_lock = false;

#ifdef _DEBUG
  this->p_flag = nullptr;
  this->p_h = nullptr;
  this->p_cost = nullptr;
#endif // _DEBUG
}

const bool Neuron::locked() { return this->m_lock; }
void Neuron::lock(const bool state) { m_lock = state; }

decltype(Neuron_Struct::flagType) &Neuron::flag() {
  return (*static_cast<uint8_t *>(_data));
}

decltype(Neuron_Struct::CostType) &Neuron::cost() {
  return *(
      decltype(Neuron_Struct::CostType) *)((uint8_t *)_data +
                                           sizeof(Neuron_Struct::flagType) +
                                           sizeof(Neuron_Struct::h));
}
decltype(Neuron_Struct::h) &Neuron::heuristic() {
  return *(decltype(Neuron_Struct::h) *)((uint8_t *)_data +
                                         sizeof(Neuron_Struct::flagType));
}

const int Neuron::weight() { return x * x + y * y; }

const uint32_t Neuron::total() { return cost() + heuristic(); }

const bool Neuron::empty() { return total() == 0; }

NavMesh::NavMesh(std::size_t Width, std::size_t Height) {
  if (!Width || !Height)
    throw std::out_of_range("Width or Height is zero!");
  this->widthSpace = Width;
  this->heightSpace = Height;
  this->segments = new RoninEngine::AIPathFinder::Neuron[Width * Height];
  this->worldScale = Vec2(1, 1) / 2;
  this->pmemory = GC::gc_malloc(widthSpace * heightSpace *
                                (sizeof(Neuron_Struct::flagType) +
                                 sizeof(Neuron_Struct::CostType) +
                                 sizeof(Neuron_Struct::h)));
  Neuron *p;
  for (Width = 0; Width < widthSpace; ++Width) {
    for (Height = 0; Height < heightSpace; ++Height) {
      p = &this->segments[Width * heightSpace + Height];
      p->x = Width;
      p->y = Height;
      p->_data = ((uint8_t *)pmemory) + (Width * heightSpace + Height) *
                                            (sizeof(Neuron_Struct::flagType) +
                                             sizeof(Neuron_Struct::CostType) +
                                             sizeof(Neuron_Struct::h));
#ifdef _DEBUG
      p->p_flag = (static_cast<decltype(Neuron_Struct::flagType) *>(p->_data));
      p->p_h = (decltype(Neuron_Struct::h) *)((uint8_t *)p->_data +
                                              sizeof(Neuron_Struct::flagType));
      p->p_cost = (decltype(
          Neuron_Struct::CostType) *)((uint8_t *)p->_data +
                                      sizeof(Neuron_Struct::flagType) +
                                      sizeof(Neuron_Struct::h));
#endif
    }
  }
  clear();
}

NavMesh::~NavMesh() {
  delete[] this->segments;
  GC::gc_free(this->pmemory);
}

void NavMesh::randomGenerate() {
  int w, h;
  Neuron *p;
  clear();
  for (w = 0; w < widthSpace; ++w) {
    for (h = 0; h < heightSpace; ++h) {
      p = &this->segments[w * heightSpace + h];
      p->lock(Random::range(0, 100) < 25);
    }
  }
}

void NavMesh::clear() {
  memset(pmemory, 0,
         widthSpace * heightSpace *
             (sizeof(Neuron_Struct::flagType) +
              sizeof(Neuron_Struct::CostType) + sizeof(Neuron_Struct::h)));
}

void NavMesh::clear(bool clearLockedData) {
  int len;
  clear();

  if (clearLockedData) {
    for (len = widthSpace * heightSpace; len;)
      this->segments[--len].m_lock = false;
  }
}

Neuron *NavMesh::neuron(const int &x, const int &y) {
  Neuron *result = nullptr;
  if (x >= 0 && x < widthSpace && y >= 0 && y < heightSpace)
    result = &segments[x * heightSpace + y];
  return result;
}

Neuron *NavMesh::neuron(const Vec2 &vector2) {
  Disposition p;
  p.x = widthSpace / 2 + vector2.x / worldScale.x + 0.5f;
  p.y = heightSpace / 2 - vector2.y / worldScale.y + 0.5f;
  return neuron(p);
}
Neuron *NavMesh::neuron(const Vec2 &vector2, Disposition &p) {
  p.x = widthSpace / 2 + vector2.x / worldScale.x + 0.5f;
  p.y = heightSpace / 2 - vector2.y / worldScale.y + 0.5f;
  return neuron(p);
}
Neuron *NavMesh::neuron(Disposition point) { return neuron(point.x, point.y); }
const Vec2 NavMesh::PointToWorldPosition(Disposition point) {
  return PointToWorldPosition(point.x, point.y);
}
const Vec2 NavMesh::PointToWorldPosition(Neuron *path) {
  return PointToWorldPosition(path->x, path->y);
}
const Vec2 NavMesh::PointToWorldPosition(const int &x, const int &y) {
  Vec2 vec2(widthSpace / 2.f, heightSpace / 2.f);
  vec2.x = (x - vec2.x) * worldScale.x;
  vec2.y = -(y - vec2.y) * worldScale.y;
  return vec2;
}

list<Vec2> NavMesh::findSpaces(const Neuron *neuron, int radiusInBlocks) {
  // TODO: Поиск пустых ячеек рядом
  return {};
}
list<Vec2> NavMesh::findSpaces(const int &x, const int &y, int radiusInBlocks) {
  return findSpaces(neuron(x, y), radiusInBlocks);
}
NavResult NavMesh::find(NavMethodRule method, Neuron *firstNeuron,
                        Neuron *lastNeuron, list<Neuron *> *result,
                        NavAlgorithm algorithm) {
  NavResult pinfo;
  pinfo.map = this;
  pinfo.firstNeuron = firstNeuron;
  pinfo.lastNeuron = lastNeuron;
  pinfo.algorithm = algorithm;
  pinfo.RelativePaths = result;
  pinfo.status = NavStatus::Undefined;

  if (firstNeuron == nullptr || lastNeuron == nullptr || result == nullptr)
    return pinfo;

  if (firstNeuron->locked() || lastNeuron->locked()) {
    pinfo.status = NavStatus::Fail;
    return pinfo;
  }

  constexpr int FLAG_OPEN_LIST = 1;
  constexpr int FLAG_CLOSED_LIST = 2;
  constexpr int FLAG_TILED_LIST = 4;

  // list<Neuron*> closed;
  list<Neuron *> finded;
  Neuron *current;
  decltype(result->begin()) iter, p1, p2;
  result->clear();
  result->emplace_back(firstNeuron);
  firstNeuron->heuristic() =
      AlgorithmUtils::DistanceManht(firstNeuron, lastNeuron);

  //Перестройка
  pinfo.status = NavStatus::Opened;
  while (!result->empty()) {
    iter = result->begin(); // get the best Neuron
    current = iter.operator*();
    if (current == lastNeuron) {
      break;
    }

    current->flag() = FLAG_CLOSED_LIST; // change to closing list
    result->erase(iter);

    // Avail
    AlgorithmUtils::AvailPoints(this, method, current, lastNeuron, &finded);
    for (iter = std::begin(finded); iter != std::end(finded); ++iter) {
      if (!(*iter)->flag()) // free path
      {
        (*iter)->flag() = FLAG_OPEN_LIST;
        (*iter)->cost() = current->cost() + 1;
        (*iter)->heuristic() =
            AlgorithmUtils::DistanceManht((*iter), lastNeuron);

        result->emplace_back((*iter));
        p1 = std::begin(*result);
        p2 = std::end(*result);

        for (; p1 != p2;) {
          if ((*p1)->total() > (*iter)->total()) {
            result->emplace(p1, (*iter));
            break;
          } else
            ++p1;
        }
        if (p1 == p2)
          result->emplace_back((*iter));
      }
    }

    finded.clear(); // clear data
  }

  Neuron *pathIsNot = nullptr;
  current = lastNeuron;
  result->clear();
  result->emplace_back(current);
  while (current != firstNeuron) {
    if (current == pathIsNot) {
      pinfo.status = NavStatus::Closed;
      break;
    }
    pathIsNot = current;
    AlgorithmUtils::AvailPoints(this, method, current, firstNeuron, &finded, -1,
                                FLAG_OPEN_LIST | FLAG_CLOSED_LIST);
    for (auto position : finded) {
      if ((position->cost() && position->cost() < current->cost()) ||
          position == firstNeuron) {
        current = position;
        result->emplace_front(current);
        current->flag() = FLAG_TILED_LIST;
      }
    }

    finded.clear();
  }

  return pinfo;
}

int AlgorithmUtils::DistancePhf(Neuron *a, Neuron *b) {
  return Mathf::pow(a->x - b->x, 2) +
         Mathf::pow(a->y - b->y, 2); // a->x * a->y + b->x * b->y;
}

int AlgorithmUtils::DistanceManht(Neuron *a, Neuron *b) {
  return Mathf::abs(b->x - a->x) + Mathf::abs(b->y - a->y);
}

auto AlgorithmUtils::GetMinCostPath(list<Neuron *> *paths)
    -> decltype(std::begin(*paths)) {
  int min = INT_MAX;
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

int GetMatrixMethod(NavMethodRule method, int8_t **matrixH, int8_t **matrixV) {
  static int8_t PLUS_H_POINT[]{-1, 1, 0, 0};
  static int8_t PLUS_V_POINT[]{0, 0, -1, 1};

  static int8_t M_SQUARE_H_POINT[]{-1, 0, 1, -1, 1, -1, 0, 1};
  static int8_t M_SQUARE_V_POINT[]{-1, -1, -1, 0, 0, 1, 1, 1};

  static int8_t M_CROSS_H_POINT[]{-1, 1, -1, 1};
  static int8_t M_CROSS_V_POINT[]{-1, -1, 1, 1};

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

void AlgorithmUtils::AvailPoints(NavMesh *map, NavMethodRule method,
                                 Neuron *arrange, Neuron *target,
                                 list<Neuron *> *pathTo, std::size_t maxCount,
                                 int filterFlag) {
  Neuron *it = nullptr;
  int i = 0, c;
  int8_t *matrixH;
  int8_t *matrixV;
  switch (method) {
  case RoninEngine::AIPathFinder::NavigationIntelegency: {
    // TODO: Написать интелектуальный пойск путей для достижения лучших
    // результатов.
    // TODO: Приводить вектор направление для лучшего достижения.
    // TODO: Выводить оптимальный результат, чтобы было меньше итерации
    // TODO: Вывести итог и анализ скорости.
    c = GetMatrixMethod(NavMethodRule::SquareMethod, &matrixH, &matrixV);
    do {
      it = map->neuron(arrange->x + matrixH[i], arrange->y + matrixV[i]);
      if (it && !it->locked() &&
          (filterFlag == static_cast<int>(0x80000000) ||
           (it->flag() & filterFlag))) {
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
      if (it && !it->locked() &&
          (filterFlag == 0x80000000 || (it->flag() & filterFlag))) {
        pathTo->emplace_back(it);
        if (maxCount == pathTo->size() || it == target)
          break;
      }
    }

    break;
  }
}

} // namespace RoninEngine::AIPathFinder
