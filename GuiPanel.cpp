#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "GuiPanel.h"
#include "RouteManager.h"

#include <iostream>

#include <osg/Viewport>
#include <osgEarth/Map>
#include <osgEarth/ImageLayer>
#include <osgEarth/ElevationLayer>
#include <osg/Light>

#include <imgui.h>
#include <imgui_impl_opengl3.h>

// ---------------------------------------------------------------
// Yardimci siniflar
// ---------------------------------------------------------------
namespace
{
    struct GuiRealizeOp : public osg::Operation
    {
        GuiRealizeOp(GuiPanel* panel)
            : osg::Operation("GuiRealizeOp", false), _panel(panel) {}

        void operator()(osg::Object*) override
        {
            if (_panel.valid())
                _panel->initImGui();
        }

        osg::observer_ptr<GuiPanel> _panel;
    };

    struct GuiDrawCallback : public osg::Camera::DrawCallback
    {
        GuiDrawCallback(GuiPanel* panel) : _panel(panel) {}

        void operator()(osg::RenderInfo& renderInfo) const override
        {
            if (_panel.valid())
                _panel->drawImGui(renderInfo);
        }

        osg::observer_ptr<GuiPanel> _panel;
    };
}

// ---------------------------------------------------------------
GuiPanel::GuiPanel(RouteManager* routeManager)
    : _routeManager(routeManager)
{
}

GuiPanel::~GuiPanel()
{
    
    if (_initialized && ImGui::GetCurrentContext())
        ImGui::DestroyContext();
}

osg::Operation* GuiPanel::getRealizeOperation()
{
    if (!_realizeOp.valid())
        _realizeOp = new GuiRealizeOp(this);
    return _realizeOp.get();
}

osg::Camera::DrawCallback* GuiPanel::getDrawCallback()
{
    if (!_drawCb.valid())
        _drawCb = new GuiDrawCallback(this);
    return _drawCb.get();
}

void GuiPanel::initImGui()
{
    if (_initialized)
        return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    if (!ImGui_ImplOpenGL3_Init(nullptr))
    {
        std::cout << "[HATA] ImGui OpenGL3 backend baslatilamadi!" << std::endl;
        ImGui::DestroyContext();
        return;
    }

    ImGui::StyleColorsDark();
    _initialized = true;
}

void GuiPanel::drawImGui(osg::RenderInfo& renderInfo)
{
    if (!_initialized)
        return;

    ImGuiIO& io = ImGui::GetIO();

    const osg::Viewport* vp = renderInfo.getCurrentCamera()
        ? renderInfo.getCurrentCamera()->getViewport()
        : nullptr;

    if (vp && vp->width() > 0 && vp->height() > 0)
        io.DisplaySize = ImVec2((float)vp->width(), (float)vp->height());
    else
        io.DisplaySize = ImVec2(1280.0f, 720.0f);

    double dt = 1.0 / 60.0;
    if (const osg::FrameStamp* fs = renderInfo.getState()->getFrameStamp())
    {
        double now = fs->getSimulationTime();
        if (_lastSimTime > 0.0 && now > _lastSimTime)
            dt = now - _lastSimTime;
        _lastSimTime = now;
    }
    io.DeltaTime = (float)dt;


    if (_autoAdvance && _sky.valid())
    {
        _localHour += (float)(dt * _timeScale / 3600.0);
        while (_localHour >= 24.0f)
        {
            _localHour -= 24.0f;
            if (++_day > 28) { _day = 1; if (++_month > 12) { _month = 1; ++_year; } }
        }
        applyDateTime();
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    buildUi();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// ---------------------------------------------------------------
bool GuiPanel::handle(const osgGA::GUIEventAdapter& ea,
    osgGA::GUIActionAdapter& aa)
{
    if (!_initialized || !ImGui::GetCurrentContext())
        return false;

    ImGuiIO& io = ImGui::GetIO();

    switch (ea.getEventType())
    {
    case osgGA::GUIEventAdapter::PUSH:
    case osgGA::GUIEventAdapter::RELEASE:
    case osgGA::GUIEventAdapter::MOVE:
    case osgGA::GUIEventAdapter::DRAG:
    {
        io.AddMousePosEvent(ea.getX(), ea.getWindowHeight() - ea.getY());

        if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH ||
            ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)
        {
            const bool down = (ea.getEventType() == osgGA::GUIEventAdapter::PUSH);
            if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
                io.AddMouseButtonEvent(0, down);
            if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
                io.AddMouseButtonEvent(1, down);
            if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
                io.AddMouseButtonEvent(2, down);
        }
        break;
    }
    case osgGA::GUIEventAdapter::SCROLL:
    {
        if (ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP)
            io.AddMouseWheelEvent(0.0f, 1.0f);
        else if (ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_DOWN)
            io.AddMouseWheelEvent(0.0f, -1.0f);
        break;
    }
    case osgGA::GUIEventAdapter::KEYDOWN:
    case osgGA::GUIEventAdapter::KEYUP:
    {
        const bool down = (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN);
        const int key = ea.getKey();

        // Metin girisi icin yazdirabilir karakterler
        if (down && key >= 32 && key < 127)
            io.AddInputCharacter((unsigned int)key);

        if (key == osgGA::GUIEventAdapter::KEY_BackSpace)
            io.AddKeyEvent(ImGuiKey_Backspace, down);
        else if (key == osgGA::GUIEventAdapter::KEY_Delete)
            io.AddKeyEvent(ImGuiKey_Delete, down);
        else if (key == osgGA::GUIEventAdapter::KEY_Return ||
            key == osgGA::GUIEventAdapter::KEY_KP_Enter)
            io.AddKeyEvent(ImGuiKey_Enter, down);
        else if (key == osgGA::GUIEventAdapter::KEY_Tab)
            io.AddKeyEvent(ImGuiKey_Tab, down);
        else if (key == osgGA::GUIEventAdapter::KEY_Left)
            io.AddKeyEvent(ImGuiKey_LeftArrow, down);
        else if (key == osgGA::GUIEventAdapter::KEY_Right)
            io.AddKeyEvent(ImGuiKey_RightArrow, down);
        break;
    }
    default:
        break;
    }

    return io.WantCaptureMouse || io.WantCaptureKeyboard;
}

// ---------------------------------------------------------------
// Panel icerigi
// ---------------------------------------------------------------
void GuiPanel::buildUi()
{
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 640), ImGuiCond_FirstUseEver);

    ImGui::Begin("IHA Kontrol & Katman Paneli");

    drawTelemetrySection();
    drawRouteControlSection();
    drawWaypointSection();
    drawLayerSection();
    drawElevationSection();
    drawSkySection();

    ImGui::End();
}

void GuiPanel::drawTelemetrySection()
{
    if (!_routeManager.valid())
        return;

    if (!ImGui::CollapsingHeader("Telemetri", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::Text("Enlem  (Lat) : %.6f°", _routeManager->getCurrentLat());
    ImGui::Text("Boylam (Lon) : %.6f°", _routeManager->getCurrentLon());
    ImGui::Text("Yukseklik    : %.1f m", _routeManager->getCurrentAlt());
    ImGui::Text("Yon (Heading): %.1f°", _routeManager->getCurrentHeading());

    const size_t total = _routeManager->getTotalWaypoints();
    if (total > 0)
        ImGui::Text("Hedef        : %zu / %zu",
            _routeManager->getCurrentIndex() + 1, total);

    ImGui::TextColored(
        _routeManager->isRunning() ? ImVec4(0.2f, 1.0f, 0.3f, 1.0f)
        : ImVec4(1.0f, 0.7f, 0.2f, 1.0f),
        "Durum        : %s",
        _routeManager->isRunning() ? "UCUSTA" : "BEKLEMEDE");
}

void GuiPanel::drawRouteControlSection()
{
    if (!_routeManager.valid())
        return;

    if (!ImGui::CollapsingHeader("Ucus Kontrolu", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (_routeManager->isRunning())
    {
        if (ImGui::Button("Durdur", ImVec2(110, 0)))
            _routeManager->pauseRoute();
    }
    else
    {
        if (ImGui::Button("Baslat", ImVec2(110, 0)))
            _routeManager->startRoute();
    }

    ImGui::SameLine();
    if (ImGui::Button("Sifirla", ImVec2(110, 0)))
        _routeManager->resetRoute();

    float speed = _routeManager->getSpeedKmh();
    if (ImGui::SliderFloat("Hiz (km/h)", &speed, 50.0f, 900.0f, "%.0f"))
        _routeManager->setSpeedKmh(speed);

    float alt = _routeManager->getTargetAltitude();
    if (ImGui::SliderFloat("Hedef irtifa (m)", &alt, 100.0f, 12000.0f, "%.0f"))
        _routeManager->setTargetAltitude(alt);

    float scale = _routeManager->getAircraftScale();
    if (ImGui::SliderFloat("Koni boyutu (m)", &scale, 200.0f, 20000.0f, "%.0f"))
        _routeManager->setAircraftScale(scale);
}

void GuiPanel::drawWaypointSection()
{
    if (!_routeManager.valid())
        return;

    if (!ImGui::CollapsingHeader("Rota / Waypoint", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::TextDisabled("Yeni koordinat ekle");

    ImGui::SetNextItemWidth(150.0f);
    ImGui::InputDouble("Enlem##in", &_inputLat, 0.001, 0.01, "%.6f");

    ImGui::SetNextItemWidth(150.0f);
    ImGui::InputDouble("Boylam##in", &_inputLon, 0.001, 0.01, "%.6f");

    ImGui::SetNextItemWidth(150.0f);
    ImGui::InputDouble("İrtifa##in", &_inputAlt, 50.0, 500.0, "%.1f");

    const bool valid = (_inputLat >= -90.0 && _inputLat <= 90.0 &&
        _inputLon >= -180.0 && _inputLon <= 180.0);

    if (!valid)
        ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Gecersiz koordinat!");

    ImGui::BeginDisabled(!valid);
    if (ImGui::Button("Waypoint Ekle", ImVec2(150, 0)))
        _routeManager->addWaypoint(_inputLat, _inputLon, _inputAlt);
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Konumu Al", ImVec2(110, 0)))
    {
        _inputLat = _routeManager->getCurrentLat();
        _inputLon = _routeManager->getCurrentLon();
        _inputAlt = _routeManager->getCurrentAlt();
    }

    ImGui::Separator();

    if (ImGui::Button("Rotayi Olustur", ImVec2(150, 0)))
        _routeManager->loadDefaultRoute();

    ImGui::SameLine();
    if (ImGui::Button("Rotayi Temizle", ImVec2(110, 0)))
        _routeManager->clearWaypoints();

    ImGui::Separator();
    ImGui::TextDisabled("Mevcut waypoint'ler");

    const std::vector<RoutePoint>& wps = _routeManager->getWaypoints();
    if (wps.empty())
    {
        ImGui::TextDisabled("(bos)");
        return;
    }

    if (ImGui::BeginChild("wplist", ImVec2(0, 130), true))
    {
        for (size_t i = 0; i < wps.size(); ++i)
        {
            ImGui::PushID((int)i);

            const bool isCurrent = (i == _routeManager->getCurrentIndex());
            if (isCurrent)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.3f, 1.0f));

            ImGui::Text("%zu) %.4f, %.4f  %.0fm", i + 1,
                wps[i].lat, wps[i].lon, wps[i].alt);

            if (isCurrent)
                ImGui::PopStyleColor();

            ImGui::SameLine();
            if (ImGui::SmallButton("Sil"))
            {
                _routeManager->removeWaypoint(i);
                ImGui::PopID();
                break;
            }

            ImGui::PopID();
        }
    }
    ImGui::EndChild();
}

void GuiPanel::drawLayerSection()
{
    if (!_mapNode.valid() || !_mapNode->getMap())
        return;

    if (!ImGui::CollapsingHeader("Harita Katmanlari", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    osgEarth::Map* map = _mapNode->getMap();
    osgEarth::LayerVector layers;
    map->getLayers(layers);

    int layerIdx = 0;
    for (osgEarth::Layer* layer : layers)
    {
        osgEarth::ImageLayer* imgLayer = dynamic_cast<osgEarth::ImageLayer*>(layer);
        if (!imgLayer)
            continue;

        ImGui::PushID(layerIdx++);

        bool visible = imgLayer->getVisible();
        if (ImGui::Checkbox("##vis", &visible))
            imgLayer->setVisible(visible);

        ImGui::SameLine();
        ImGui::Text("%s", imgLayer->getName().c_str());

        float opacity = imgLayer->getOpacity();
        ImGui::SetNextItemWidth(160.0f);
        if (ImGui::SliderFloat("Baskinlik", &opacity, 0.0f, 1.0f, "%.2f"))
            imgLayer->setOpacity(opacity);

        ImGui::PopID();
    }

    if (layerIdx == 0)
        ImGui::TextDisabled("(goruntu katmanı yok)");
}

void GuiPanel::drawElevationSection()
{
    if (!_mapNode.valid() || !_mapNode->getMap())
        return;

    if (!ImGui::CollapsingHeader("Yukseklik (Elevation)", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    osgEarth::Map* map = _mapNode->getMap();
    osgEarth::ElevationLayerVector elevLayers;
    map->getLayers(elevLayers);

    if (elevLayers.empty())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.2f, 1.0f),
            "Haritada yukseklik katmanı yok.");
        ImGui::TextWrapped(
            "Arazi duz gorunuyorsa sebebi bu. UAVViewer.earth dosyasina "
            "srtm_data icin bir elevation katmani eklemen gerekiyor.");
        return;
    }

    int idx = 0;
    for (osgEarth::ElevationLayer* layer : elevLayers)
    {
        if (!layer)
            continue;

        ImGui::PushID(idx++);

        bool visible = layer->getVisible();
        if (ImGui::Checkbox("##elevvis", &visible))
            layer->setVisible(visible);

        ImGui::SameLine();

        const std::string name = layer->getName().empty()
            ? std::string("(isimsiz katman)") : layer->getName();
        ImGui::Text("%s", name.c_str());

        ImGui::SameLine();
        ImGui::TextDisabled(layer->isOpen() ? "[acik]" : "[kapali]");

        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::TextDisabled("Kamera yukseklige gore arazi kabarır;");
    ImGui::TextDisabled("etkiyi görmek için yakınlas ve yana bak.");
}

// ---------------------------------------------------------------
// Gokyuzu: mevsim ve saat
// ---------------------------------------------------------------
void GuiPanel::applyDateTime()
{
    if (!_sky.valid())
        return;

    // SkyNode UTC bekliyor; Turkiye UTC+3.
    double utcHour = _localHour - 3.0;
    int y = _year, m = _month, d = _day;

    if (utcHour < 0.0)
    {
        utcHour += 24.0;
        if (--d < 1) { d = 28; if (--m < 1) { m = 12; --y; } }
    }

    _sky->setDateTime(osgEarth::DateTime(y, m, d, utcHour));
}

void GuiPanel::drawSkySection()
{
    if (!_sky.valid())
        return;

    if (!ImGui::CollapsingHeader("Gunes: Mevsim ve Saat",
        ImGuiTreeNodeFlags_DefaultOpen))
        return;

    bool changed = false;

    // --- mevsim hazir ayarlari ---
    // Gundonumu ve ekinoks tarihleri: gunes yuksekligindeki fark
    // en belirgin bu tarihlerde.
    const char* seasons[] = {
        "Ilkbahar (21 Mart)",
        "Yaz (21 Haziran)",
        "Sonbahar (23 Eylul)",
        "Kis (21 Aralik)"
    };

    ImGui::SetNextItemWidth(220.0f);
    if (ImGui::Combo("Mevsim", &_seasonPreset, seasons, IM_ARRAYSIZE(seasons)))
    {
        switch (_seasonPreset)
        {
        case 0: _month = 3;  _day = 21; break;
        case 1: _month = 6;  _day = 21; break;
        case 2: _month = 9;  _day = 23; break;
        case 3: _month = 12; _day = 21; break;
        }
        changed = true;
    }

   
    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::SliderInt("Ay", &_month, 1, 12))
        changed = true;

    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::SliderInt("Gün", &_day, 1, 28))
        changed = true;

    ImGui::Separator();

    // --- saat ---
    const int hh = (int)_localHour;
    const int mm = (int)((_localHour - hh) * 60.0f);
    ImGui::Text("Yerel saat: %02d:%02d  (UTC+3)", hh, mm);

    ImGui::SetNextItemWidth(240.0f);
    if (ImGui::SliderFloat("##saat", &_localHour, 0.0f, 24.0f, "%.2f saat"))
        changed = true;

    // Hizli secimler
    if (ImGui::SmallButton("Safak")) { _localHour = 6.0f;  changed = true; }
    ImGui::SameLine();
    if (ImGui::SmallButton("Ogle")) { _localHour = 12.0f; changed = true; }
    ImGui::SameLine();
    if (ImGui::SmallButton("Gun batimi")) { _localHour = 19.5f; changed = true; }
    ImGui::SameLine();
    if (ImGui::SmallButton("Gece")) { _localHour = 23.0f; changed = true; }

    ImGui::Separator();

    // --- zamanin akmasi ---
    ImGui::Checkbox("Saati otomatik ilerlet", &_autoAdvance);
    if (_autoAdvance)
    {
        ImGui::SetNextItemWidth(200.0f);
        ImGui::SliderFloat("Hız (x)", &_timeScale, 10.0f, 3600.0f, "%.0f");
        ImGui::TextDisabled("1 saniye = %.1f dakika simulasyon", _timeScale / 60.0f);
    }

    // --- gece karanligi ---
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::SliderFloat("Gece aydinligi", &_minAmbient, 0.0f, 0.5f, "%.2f"))
    {
        
        if (osg::Light* sun = _sky->getSunLight())
            sun->setAmbient(osg::Vec4(_minAmbient, _minAmbient, _minAmbient, 1.0f));
    }

    ImGui::TextDisabled("Gölgeler güneşin açısına göre oluşur:");
    ImGui::TextDisabled("sabah/akşam saatleri en belirgin.");

    if (changed)
        applyDateTime();
}
