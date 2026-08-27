#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <osg/Camera>
#include <osg/OperationThread>
#include <osg/RenderInfo>
#include <osg/observer_ptr>
#include <osg/ref_ptr>

#include <osgGA/GUIEventHandler>
#include <osgEarth/MapNode>
#include <osgEarth/Sky>

#include "RouteManager.h"

class GuiPanel : public osgGA::GUIEventHandler
{
public:
    GuiPanel(RouteManager* routeManager);

    void setMapNode(osgEarth::MapNode* mapNode) { _mapNode = mapNode; }

    // Gokyuzu dugumu: tarih/saat kontrolleri bunun uzerinden calisiyor.
    // SkyNode olusturulamadiysa panelde ilgili bolum gizlenir.
    void setSkyNode(osgEarth::SkyNode* sky) { _sky = sky; }

    // ImGui context'i ve OpenGL backend'i burada kuruluyor.
    // viewer.setRealizeOperation(...) ile kaydedilir; realize
    // operation'lar GL context current iken calisir.
    osg::Operation* getRealizeOperation();

    // Cizim OSG'nin draw asamasinda, yine context current iken.
    // viewer.getCamera()->setFinalDrawCallback(...) ile kaydedilir.
    osg::Camera::DrawCallback* getDrawCallback();

    bool handle(const osgGA::GUIEventAdapter& ea,
        osgGA::GUIActionAdapter& aa) override;

    // Yardimci siniflardan cagriliyor; disaridan cagirma.
    void initImGui();
    void drawImGui(osg::RenderInfo& renderInfo);

protected:
    virtual ~GuiPanel();

private:
    void buildUi();
    void drawTelemetrySection();
    void drawRouteControlSection();
    void drawWaypointSection();
    void drawLayerSection();
    void drawElevationSection();
    void drawSkySection();
    void applyDateTime();

    osg::ref_ptr<RouteManager> _routeManager;
    osg::observer_ptr<osgEarth::MapNode> _mapNode;
    osg::observer_ptr<osgEarth::SkyNode> _sky;

    osg::ref_ptr<osg::Operation> _realizeOp;
    osg::ref_ptr<osg::Camera::DrawCallback> _drawCb;

    // Koordinat girisi icin gecici alanlar
    double _inputLat = 39.9334;
    double _inputLon = 32.8597;
    double _inputAlt = 1200.0;

    // --- tarih / saat ---
    int _year = 2026;
    int _month = 6;          // Haziran: yaz gunesi yuksek
    int _day = 21;
    float _localHour = 15.0f; // Turkiye yerel saati (UTC+3)
    bool _autoAdvance = false;
    float _timeScale = 60.0f; // gercek 1 sn = kac simulasyon saniyesi
    float _minAmbient = 0.10f;
    int _seasonPreset = 1;

    bool _initialized = false;
    double _lastSimTime = 0.0;
};