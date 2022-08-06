#include "terrain2deditor.h"

using namespace RoninEngine::Runtime;
namespace ai = RoninEngine::AIPathFinder;
struct {
    Color defaultNeuron = Color(0xfff6f723);
    Color defaultBackground = Color(0xff1e1e1e);
    Color deafultFore = Color(0xff90791C);
    Color line = Color::red;
    Color pointStart = Color::red;
} navMeshColorSchemes;

ai::NavMesh navMesh(1000, 1000);
void Terrain2DEditor::start() {
    CreateGameObject()->addComponent<Camera2D>();
    navMesh.worldScale /= 6;
}

void Terrain2DEditor::update()
{
    Transform * t = Camera::mainCamera()->transform();
    t->position(Vec2::MoveTowards(t->position(), t->position()+input::get_axis(), Time::deltaTime()));
}

void Plot() {
    Vec2 lastPoint, a, b;
    AIPathFinder::Neuron* p;
    Runtime::Vec2Int p1, p2;
    Resolution res;
    Color prev, next;
    int yDefault;
    float max = 0;

    res = Application::getResolution();
    prev = Gizmos::getColor();
    Gizmos::setColor(next=navMeshColorSchemes.defaultNeuron);
    p1 = navMesh.WorldPointToPoint(Camera::ScreenToWorldPoint(Vec2::minusOne));
    p2 = navMesh.WorldPointToPoint(Camera::ViewportToWorldPoint(Vec2::one));
    yDefault = p1.y;
    //select draw from viewport neurons
    while (p1.x <= p2.x) {
        while (p1.y <= p2.y) {
            p = navMesh.GetNeuron(p1);
            lastPoint = navMesh.PointToWorldPosition(p1);
            if (!p || navMesh.neuronLocked(p1)) {
                next.r = 255;
                next.g = 0;
                next.b = 0;
            } else {
                next.r = 53;
                next.g = navMesh.neuronGetTotal(p1) ? 200 : 0;
                next.b = 246;
            }
            Gizmos::setColor(next);
            Gizmos::DrawFill(lastPoint, navMesh.worldScale.x - (navMesh.worldScale.x * 0.1f),
                             navMesh.worldScale.y - navMesh.worldScale.y * 0.1f);
            ++p1.y;
        }
        p1.y = yDefault;
        ++p1.x;
    }
    Gizmos::setColor(prev);
}

void Terrain2DEditor::onDrawGizmos() {
    static Vec2 alpha = (Vec2::up + Vec2::right) * 2;
    static float angle = 0;

    ai::NavResult result;
    Vec2 first = Camera::ViewportToWorldPoint(Vec2::half);
    Vec2 last = Vec2::RotateAround(first, alpha, angle * Mathf::Deg2Rad);

    if (input::isMouseDown()) {
        auto ner = navMesh.neuronGetPoint(navMesh.GetNeuron(Camera::ScreenToWorldPoint(input::getMousePointF())));
        navMesh.neuronLock(ner, !navMesh.neuronLocked(ner));
    }

    if (Time::frame() % 10 == 0) {
        angle += 13;
        if (angle > 360) angle -= 360;
        navMesh.randomGenerate(323232);
    }
    else
        navMesh.clear();
    navMesh.find(result, ai::NavMethodRule::NavigationIntelegency, first, last);

    //Draw nav mesh
    Plot();

    Gizmos::setColor(Color::red);
    Gizmos::DrawCircle(last, 0.2f);

    Gizmos::setColor(Color::red);
    if (result.status == ai::Opened) {
        first = navMesh.PointToWorldPosition(result.firstNeuron);
        Gizmos::setColor(navMeshColorSchemes.pointStart);
        Gizmos::DrawCircle(first, navMesh.worldScale.x);
        Gizmos::setColor(navMeshColorSchemes.line);
        for (auto n = ++result.RelativePaths.begin(); n != result.RelativePaths.end(); ++n) {
            Gizmos::DrawLine(first, navMesh.PointToWorldPosition(*n));
            first = navMesh.PointToWorldPosition(*n);
        }
    }
}
