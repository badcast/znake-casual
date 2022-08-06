#include "terrain2deditor.h"

using namespace RoninEngine;

using namespace RoninEngine::Runtime;
namespace ai = RoninEngine::AIPathFinder;
struct {
    Color defaultNeuron = Color(0xfff6f723);
    Color defaultBackground = Color(0xff1e1e1e);
    Color deafultFore = Color(0xff90791C);
    Color line = Color::red;
    Color pointStart = Color::red;
} navMeshColorSchemes;
RoninEngine::UI::GUI* ui;
uid slider;
ai::NavMesh* navMesh;
void Terrain2DEditor::start()
{
    ui = this->ui;
    Primitive::CreateCamera2D();
    navMesh = new ai::NavMesh(1000, 1000);
    navMesh->worldScale /= 6;

    slider = this->ui->Push_Slider(0.5, { 0, 50 });
    ui->Push_Label("Terrain 2D editor", Vec2Int(0, 25));
}

void Terrain2DEditor::update()
{
    Transform* t = Camera::mainCamera()->transform();
    t->position(Vec2::MoveTowards(t->position(), t->position() + input::get_axis(), TimeEngine::deltaTime()));

    float v = Math::max(0.05f, *(float*)ui->getResources(slider));
    navMesh->worldScale = Vec2::one * v;
}

void Plot()
{
    Vec2 lastPoint;
    AIPathFinder::Neuron* p;
    Runtime::Vec2Int p1, p2;
    Resolution res;
    Color prev, next;
    int yDefault;

    res = Application::getResolution();
    prev = Gizmos::getColor();
    Gizmos::setColor(next = navMeshColorSchemes.defaultNeuron);
    p1 = navMesh->WorldPointToPoint(Camera::ScreenToWorldPoint(Vec2::minusOne));
    p2 = navMesh->WorldPointToPoint(Camera::ViewportToWorldPoint(Vec2::one));
    yDefault = p1.y;
    // select draw from viewport neurons
    while (p1.x <= p2.x) {
        while (p1.y <= p2.y) {
            p = navMesh->GetNeuron(p1);
            lastPoint = navMesh->PointToWorldPosition(p1);
            if (!p || navMesh->neuronLocked(p1)) {
                next.r = 255;
                next.g = 0;
                next.b = 0;
            } else {
                next.r = 53;
                next.g = navMesh->neuronGetTotal(p1) ? 200 : 0;
                next.b = 246;
            }
            Gizmos::setColor(next);
            Gizmos::DrawTriangle(lastPoint, navMesh->worldScale.x - (navMesh->worldScale.x * 0.1f), navMesh->worldScale.y - navMesh->worldScale.y * 0.1f, true);
            ++p1.y;
        }
        p1.y = yDefault;
        ++p1.x;
    }
    Gizmos::setColor(prev);

    static std::uint32_t maxTotal = 0;
    static float upplow = 0;
    if (TimeEngine::time() > upplow) {
        maxTotal = 0;
        upplow = TimeEngine::time() + 1;
    }
    auto totalC = navMesh->getCachedSize();
    maxTotal = std::max(maxTotal, totalC);

    Gizmos::DrawTextOnPosition(Camera::ScreenToWorldPoint(Vec2::zero), "Slider value: " + (std::to_string(*(float*)ui->getResources(slider))) + ", Cached " + std::to_string(totalC) + " (" + std::to_string(maxTotal) + ")");
}

void Terrain2DEditor::onDrawGizmos()
{
    static Vec2 alpha = (Vec2::up + Vec2::right) * 2;
    static float angle = 0;
    ai::NavResult result;
    Vec2 first = Camera::ViewportToWorldPoint(Vec2::half);
    Vec2 last = Vec2::RotateAround(first, alpha, angle * Math::Deg2Rad);

    if (input::isMouseDown()) {
        auto ner = navMesh->neuronGetPoint(navMesh->GetNeuron(Camera::ScreenToWorldPoint(input::getMousePointF())));
        navMesh->neuronLock(ner, !navMesh->neuronLocked(ner));
    }

    if (TimeEngine::frame() % 10 == 0) {
        angle += 13;

        if (angle > 360)
            angle -= 360;

        navMesh->Randomize(323232);
    } else
        navMesh->Clear();
    navMesh->Find(result, ai::NavMethodRule::NavigationIntelegency, first, last);

    // Draw nav mesh
    Plot();

    Gizmos::setColor(Color::red);
    Gizmos::DrawCircle(last, 0.2f);

    Gizmos::setColor(Color::red);
    if (result.status == ai::NavStatus::Opened) {
        first = navMesh->PointToWorldPosition(result.firstNeuron);
        Gizmos::setColor(navMeshColorSchemes.pointStart);
        Gizmos::DrawCircle(first, navMesh->worldScale.x);
        Gizmos::setColor(navMeshColorSchemes.line);
        for (auto n = ++result.RelativePaths.begin(); n != result.RelativePaths.end(); ++n) {
            Gizmos::DrawLine(first, navMesh->PointToWorldPosition(*n));
            first = navMesh->PointToWorldPosition(*n);
        }
    }
}
