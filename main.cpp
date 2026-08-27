#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <iostream>

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/LightSource>
#include <osg/Light>

#include <osgEarth/MapNode>
#include <osgEarth/EarthManipulator>
#include <osgEarth/Sky>

#include "RouteManager.h"
#include "GuiPanel.h"

int main(int argc, char** argv)
{
    // osgEarth baslatmasi hicbir pencere/viewer olusturulmadan EN BASTA
    osgEarth::initialize();

    osgViewer::Viewer viewer;
    viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
    viewer.setUpViewInWindow(100, 100, 1280, 720);

    osg::ref_ptr<osg::Node> loadedNode = osgDB::readNodeFile("UAVViewer.earth");
    if (!loadedNode.valid())
    {
        std::cout << "[HATA] UAVViewer.earth dosyasi okunamadi!" << std::endl;
        std::cout << "       Calisma dizini proje kokunu gosteriyor mu?" << std::endl;
        return -1;
    }

    osg::ref_ptr<osgEarth::MapNode> mapNode = osgEarth::MapNode::get(loadedNode.get());
    if (!mapNode.valid())
    {
        std::cout << "[HATA] Dosyada MapNode bulunamadi!" << std::endl;
        return -1;
    }

    osg::ref_ptr<osg::Group> rootGroup = new osg::Group();

    // ---- Gokyuzu: gunes, ay ve yildizlar ----
    osg::ref_ptr<osgEarth::SkyNode> sky = osgEarth::SkyNode::create();
    if (sky.valid())
    {
        sky->setMoonVisible(true);
        sky->setStarsVisible(true);
        sky->setSunVisible(true);

        // Baslangic tarihi/saati. Panelden degistirilebiliyor.
        // (yerel 15:00 = UTC 12:00)
        sky->setDateTime(osgEarth::DateTime(2026, 6, 21, 12.0));

        // Gece tamamen zifiri olmasin. Bu surumde SkyNode'da
        // setMinimumAmbient yok; gunes isiginin ambient bilesenini
        // dogrudan ayarliyoruz.
        if (osg::Light* sun = sky->getSunLight())
            sun->setAmbient(osg::Vec4(0.10f, 0.10f, 0.10f, 1.0f));

        sky->addChild(mapNode.get());
        rootGroup->addChild(sky.get());
        sky->attach(&viewer, 0);
    }
    else
    {
        
        std::cout << "[UYARI] SkyNode olusturulamadi, basit isiklandirma kullaniliyor."
            << std::endl;

        rootGroup->addChild(mapNode.get());

        osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
        osg::Light* sunLight = lightSource->getLight();
        sunLight->setLightNum(0);
        sunLight->setPosition(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f));
        sunLight->setAmbient(osg::Vec4(0.4f, 0.4f, 0.4f, 1.0f));
        sunLight->setDiffuse(osg::Vec4(1.0f, 1.0f, 0.95f, 1.0f));

        osg::StateSet* ss = rootGroup->getOrCreateStateSet();
        lightSource->setStateSetModes(*ss, osg::StateAttribute::ON);
        ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
        ss->setMode(GL_LIGHT0, osg::StateAttribute::ON);
        rootGroup->addChild(lightSource.get());
    }

    // ---- Rota yoneticisi ----
    osg::ref_ptr<RouteManager> routeManager = new RouteManager();
    routeManager->init(mapNode.get());

    // Ucak konisi, kirmizi rota cizgisi ve yesil noktalar
    rootGroup->addChild(routeManager->getSceneRoot());

    // ---- GUI ----
    osg::ref_ptr<GuiPanel> guiPanel = new GuiPanel(routeManager.get());
    guiPanel->setMapNode(mapNode.get());
    guiPanel->setSkyNode(sky.get());   // mevsim/saat kontrolleri icin

    viewer.setSceneData(rootGroup.get());

    osg::ref_ptr<osgEarth::Util::EarthManipulator> manip =
        new osgEarth::Util::EarthManipulator();
    viewer.setCameraManipulator(manip.get());

    viewer.addEventHandler(guiPanel.get());

    viewer.setRealizeOperation(guiPanel->getRealizeOperation());

    viewer.realize();

    viewer.getCamera()->setFinalDrawCallback(guiPanel->getDrawCallback());

    {
        osgEarth::Viewpoint vp;
        vp.focalPoint() = osgEarth::GeoPoint(
            mapNode->getMapSRS()->getGeographicSRS(),
            routeManager->getCurrentLon(),
            routeManager->getCurrentLat(),
            routeManager->getCurrentAlt());
        vp.heading()->set(0.0, osgEarth::Units::DEGREES);
        vp.pitch()->set(-45.0, osgEarth::Units::DEGREES);
        vp.range()->set(120000.0, osgEarth::Units::METERS);
        manip->setViewpoint(vp);
    }

    double lastTime = viewer.getFrameStamp()
        ? viewer.getFrameStamp()->getSimulationTime() : 0.0;

    while (!viewer.done())
    {
        double currentTime = viewer.getFrameStamp()->getSimulationTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        routeManager->update(deltaTime);

        viewer.frame();
    }

    return 0;
}
