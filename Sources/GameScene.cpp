#include "pch.h"

#include "Camera2D.h"
#include "GameObject.h"
#include "GameScene.h"
#include "Gizmos.h"
#include "NavMesh.h"
#include "ObjectParser.h"
#include "Player.h"
#include "Sprite.h"
#include "SpriteRenderer.h"
#include "inputSystem.h"

using namespace RoninEngine::Runtime;
using namespace RoninEngine::AIPathFinder;

namespace RoninEngine {
constexpr int MaxPopulation = 1000;
constexpr int InitPopuplation = 1000;
constexpr int NavMeshMagnitude = 250;
constexpr float NavMeshWorldScale = 0.32;

UI::uid g_buttons;
UI::uid b_mainMenu;
UI::uid b_generateNav;
UI::uid b_clearNav;
UI::uid b_showGizmo;
UI::uid b_add, b_sub;
UI::uid t_infoblock;
UI::uid t_speedLabel;

RoninEngine::UI::GUI *gui;
AIPathFinder::NavMesh *navMesh = nullptr;
list<AIPathFinder::Neuron *> paths;

Vec2 from, to;
bool showGizmosLayer = false;
int arranged = 0;
Neuron *last = nullptr;
float changeDrawPointPer;
decltype(paths.end()) _endIter;
Player *player;

void ui_event_handler(const uint8_t &id, void *userData) {

  if (b_mainMenu == id) {
    Application::LoadLevel(GC::gc_alloc<MainMenu>());
  } else if (b_generateNav == id || id == b_clearNav) {
    paths.clear();
    arranged = 0;
    if (b_generateNav == id)
      navMesh->randomGenerate();
    else
      navMesh->clear(true);
  } else if (b_showGizmo == id) {
    showGizmosLayer = !showGizmosLayer;
  } else if (b_add == id) {
    player->playerCamera->aspectRatio += Vec2::half / 2;
  } else if (b_sub == id) {
    player->playerCamera->aspectRatio -= Vec2::half / 2;
  }
}

void GameScene::awake() {
  gui = this->ui;

  auto display = Application::display();

  GameObject *playerObj = CreateGameObject("Player");
  player = playerObj->Add_Component<Player>();

  player->playerCamera->visibleBorders = true;
  player->playerCamera->visibleGrids = true;

  g_buttons = ui->Create_Group();
  b_mainMenu =
      this->ui->Push_Button("Main Menu", {25, 25}, g_buttons);
  b_generateNav =
      this->ui->Push_Button("generate Nav Mesh", {300, 25}, g_buttons);
  b_clearNav = this->ui->Push_Button("flush Nav Mesh", {550, 25}, g_buttons);
  b_showGizmo = this->ui->Push_Button("show GIZMOS", {25, 90}, g_buttons);

  b_add = this->ui->Push_Button("+", {display.w - 32, display.h - 64, 32, 32});
  b_sub = this->ui->Push_Button("-", {display.w - 32, display.h - 32, 32, 32});

  t_infoblock =
      this->ui->Push_Label("", {display.w / 2 - 130, display.h / 2 + 25}, 5);
  t_speedLabel = this->ui->Push_Label("", {0, 70}, 5);
  gui->Register_Callback(ui_event_handler, nullptr);
}

SpriteRenderer *target;
GameObject *testObj;
vector<tuple<Transform *, float, list<Neuron *>>> ais;
void GameScene::start() {
  SpriteRenderer *spriteRenderer;
  Sprite *spr;
  GameObject *floor = CreateGameObject("Floor");
  Texture *texture;

  testObj = CreateGameObject("Population");
  spriteRenderer = testObj->Add_Component<SpriteRenderer>();
  spriteRenderer->renderType = SpriteRenderType::Simple;
  spriteRenderer->tileRenderPresent = SpriteRenderTile::Fixed;
  spriteRenderer->flip = {1, 1};

  GC::gc_alloc_texture_from(
      &texture, GC::resource_bitmap("test", FolderKind::GFX));
  GC::gc_alloc_sprite_with(&spr, texture);

  spriteRenderer->setSprite(spr);
  testObj->transform()->position(Vec2::zero);
  testObj->Get_Component<Renderer>()->zOrder = 1; // layer

  spriteRenderer = floor->Add_Component<SpriteRenderer>();
  spriteRenderer->renderType = SpriteRenderType::Tile;
  spriteRenderer->tileRenderPresent = SpriteRenderTile::Fixed;
  spriteRenderer->size = {NavMeshMagnitude, NavMeshMagnitude};
  spriteRenderer->size *= NavMeshWorldScale;

  GC::gc_alloc_texture_from(
      &texture, GC::resource_bitmap("floor", FolderKind::TEXTURES));
  GC::gc_alloc_sprite_with(&spr, texture);
  spriteRenderer->setSprite(spr);

  GC::gc_alloc_lval(navMesh, static_cast<std::size_t>(NavMeshMagnitude), static_cast<std::size_t>(NavMeshMagnitude));
  navMesh->worldScale = Vec2::one * NavMeshWorldScale;

  changeDrawPointPer = 0;
  target = CreateGameObject()->Add_Component<SpriteRenderer>();
  target->zOrder = 1;

  GC::gc_alloc_texture_from(
      &texture, GC::resource_bitmap("target", FolderKind::GFX));
  GC::gc_alloc_sprite_with(&spr, texture);
  target->setSprite(spr);

  list<Neuron *> c;
  for (size_t i = 0; i < Mathf::Min(MaxPopulation, InitPopuplation); ++i) {
    auto t = Instantiate(testObj)->transform();
    t->position(
        navMesh->PointToWorldPosition(Random::range(0, NavMeshMagnitude),
                                      Random::range(0, NavMeshMagnitude)));

    ais.emplace_back(std::make_tuple(t, 0, c));
  }
}

void GameScene::update() {
  static bool lastLockVal = false;
  static int activeCount = 0;
  if (Camera::mainCamera() == nullptr) {
    return;
  }

  Vec2 p = Camera::mainCamera()->ScreenToWorldPoint(input::getMousePointF());
  Neuron *c = navMesh->neuron(p);
  from = testObj->transform()->position();

  if (input::get_key_down(SDL_SCANCODE_ESCAPE)) {
    if (!ui->Visible(g_buttons))
      ui->Show_Group(g_buttons);
    else
      ui->Close_Group(g_buttons);
  }

  if (Time::frame() % 25 == 0) {

    string s = input::get_key(SDL_SCANCODE_LSHIFT) ? "Speed x2" : "Speed x1";
    s.push_back(';');
    s += "Count ";
    s += to_string(ais.size());
    s += " Active ";
    s += to_string(activeCount);
    gui->Text(t_speedLabel, s);
  }
  if (c && c != last) {
    if (last)
      last->lock(lastLockVal);

    lastLockVal = c->locked();
    c->lock(true);
    char p[70];
    p[0] = 0;
    if (c->total()) {
      strcat(p, "cost: ");
      strcat(p, to_string(c->cost()).data());
      strcat(p, "; ");
      strcat(p, "h: ");
      strcat(p, to_string(c->heuristic()).data());
      strcat(p, "; ");
      ui->Visible(t_infoblock, true);
      ui->Text(t_infoblock, p);
    } else
      ui->Visible(t_infoblock, false);

    last = c;
  }
  if (target) {
    if (!gui->Focused_UI() && input::isMouseDown()) {

      p = input::getMousePointF();
      p = Camera::mainCamera()->ScreenToWorldPoint(p);
      paths.clear();
      auto a = navMesh->neuron(from);
      a->lock(false);
      auto b = navMesh->neuron(p);

      // navMesh->clear();
      testObj->transform()->position(navMesh->PointToWorldPosition(a));
      if (b) {
        b->lock(lastLockVal);
        target->transform()->position(navMesh->PointToWorldPosition(b));
      }
      navMesh->clear();
      auto status = navMesh
                        ->find(RoninEngine::AIPathFinder::NavMethodRule::
                                   NavigationIntelegency,
                               a, b, &paths,
                               RoninEngine::AIPathFinder::NavAlgorithm::AStar)
                        .status;
      if (status < AIPathFinder::NavStatus::Closed) {
        // todo: error
        arranged = 0;
        paths.clear();
      } else {
        arranged = 1;
        _endIter = begin(paths);
      }
      navMesh->clear();
    }

    float ang = target->transform()->angle();

    ang += 11;
    if (ang > 360)
      ang -= 360;

    target->transform()->angle(ang);
  }

  if (!paths.empty()) {
    int j;
    if (_endIter != std::end(paths) && Time::time() > changeDrawPointPer) {
      changeDrawPointPer = Time::time() + 0.001f;
      for (j = 0; _endIter != std::end(paths) &&
                  j < paths.size() / Mathf::sqrt(paths.size());
           ++j, ++_endIter)
        ;
    }

    Vec2 lp = navMesh->PointToWorldPosition(*begin(paths));
    auto iter = paths.cbegin();
    for (j = 0; iter != _endIter && j <= arranged; ++iter, ++j)
      lp = navMesh->PointToWorldPosition(*iter);
    to = lp;

    if (arranged && _endIter == std::end(paths)) {
      if (from != to) {
        testObj->transform()->LookAtLerp(to, 0.5f);
      }
      testObj->transform()->position(
          from =
              Vec2::MoveTowards(from, to,
                                input::get_key(SDL_SCANCODE_LSHIFT)  ? .2
                                : input::get_key(SDL_SCANCODE_LCTRL) ? .0005f
                                                                     : .05f));
      if (from == to) {
        if (paths.size() != arranged)
          ++arranged;
        else
          arranged = 0;
      }
    } else {
      // testObj->transform()->LookAt(p);
    }
  }

  activeCount = 0;
  for (auto &iter : ais) {
    auto *trans = get<Transform *>(iter);
    float &t = get<float>(iter);
    list<Neuron *> &rr = get<remove_reference<decltype(rr)>::type>(iter);

    if (t == 0)
      t = 1;

    if (!rr.empty()) {
      if (trans->position() == navMesh->PointToWorldPosition(rr.back())) {

      } else {
        Vec2 dir = navMesh->PointToWorldPosition(rr.front());
        if (trans->position() != dir)
          trans->LookAtLerp(dir, 0.5f);
        trans->position(
            Vec2::MoveTowards(trans->position(), dir,
                              input::get_key(SDL_SCANCODE_LSHIFT)  ? 2
                              : input::get_key(SDL_SCANCODE_LCTRL) ? .005f
                                                                   : .05f));
        if (trans->position() == dir) {
          rr.pop_front();
        }
        ++activeCount;
      }
    } else {
      auto result =
          navMesh->find(NavMethodRule::NavigationIntelegency,
                        navMesh->neuron(trans->position()),
                        navMesh->neuron(Random::range(0, NavMeshMagnitude),
                                        Random::range(0, NavMeshMagnitude)),
                        &rr, NavAlgorithm::AStar);
      if (result.status != NavStatus::Opened) {
        float a = trans->angle() + 1;
        if (a > 360)
          a -= 360;
        trans->angle(a);
        rr.clear();
      } else
        rr.pop_front();
      navMesh->clear();
    }
  }

  player->transform()->position(
      Vec2::MoveTowards(player->transform()->position(), from, 0.085f));
}

void GameScene::lateUpdate() {}

void GameScene::onDrawGizmos() {

  if (!showGizmosLayer)
    return;

  // draw nav path
  Gizmos::DrawNavMesh(navMesh);

  Gizmos::color.g = 200;
  Gizmos::color.r = 0;
  Gizmos::color.b = 0;

  if (!paths.empty()) {
    auto iter = paths.cbegin();
    int j;

    Vec2 lp = navMesh->PointToWorldPosition(*begin(paths));
    for (j = 0; iter != _endIter && j <= arranged; ++iter, ++j) {
      if (lp != navMesh->PointToWorldPosition(*iter))
        Gizmos::DrawLine(lp, navMesh->PointToWorldPosition(*iter));
      lp = navMesh->PointToWorldPosition(*iter);
    }
    to = lp;

    Gizmos::color.b = 200;
    for (; iter != _endIter; ++iter) {
      Gizmos::DrawLine(lp, navMesh->PointToWorldPosition(*iter));
      lp = navMesh->PointToWorldPosition(*iter);
    }
  }

  Gizmos::color = 0xff00ff00;
  Gizmos::DrawTriangle(testObj->transform()->position(), 1.f, 1.f);
  // Gizmos::DrawLine(testObj->transform()->position(),
  // testObj->transform()->position() + testObj->transform()->forward() * 2);
}

void GameScene::onUnloading() {
  testObj = nullptr;
  to = Vec2::zero;
  last = nullptr;
  GC::gc_unalloc(navMesh);
  paths.clear();
  ais.clear();
  ais.shrink_to_fit();
}
} // namespace RoninEngine
