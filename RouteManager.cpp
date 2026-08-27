#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "RouteManager.h"

#include <cmath>
#include <algorithm>
#include <iostream>

#include <osg/Depth>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/ShapeDrawable>
#include <osg/Material>

#include <osgEarth/GeoData>
#include <osgEarth/SpatialReference>

namespace
{
    const double PI = 3.14159265358979323846;
}

RouteManager::RouteManager()
    : _currentIndex(0)
    , _currentHeading(0.0)
    , _speedKmh(200.0f)
    , _targetAltitude(1200.0f)
    , _isRunning(false)
{
    _sceneRoot = new osg::Group();
    _sceneRoot->setName("RouteManagerRoot");

    osg::StateSet* ss = _sceneRoot->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
    ss->setMode(GL_BLEND, osg::StateAttribute::ON);
    ss->setRenderBinDetails(10, "RenderBin");

    loadDefaultRoute();
}

void RouteManager::init(osgEarth::MapNode* mapNode)
{
    _mapNode = mapNode;

    buildAircraft();
    rebuildRouteGraphics();
    resetRoute();
}

// ---------------------------------------------------------------
// Koordinat donusumu
// ---------------------------------------------------------------
bool RouteManager::toWorld(double lat, double lon, double alt, osg::Vec3d& out) const
{
    if (!_mapNode.valid())
        return false;

    const osgEarth::SpatialReference* srs = _mapNode->getMapSRS();
    if (!srs)
        return false;

    osgEarth::GeoPoint gp(srs->getGeographicSRS(), lon, lat, alt,
        osgEarth::ALTMODE_ABSOLUTE);
    return gp.toWorld(out);
}

// ---------------------------------------------------------------
// Ucak: kirmizi koni
// ---------------------------------------------------------------
void RouteManager::buildAircraft()
{
    if (_aircraftXform.valid())
        return;

  
    osg::ref_ptr<osg::Cone> cone = new osg::Cone(osg::Vec3(0, 0, 0), 0.35f, 1.0f);
    osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(cone.get());
    sd->setColor(osg::Vec4(1.0f, 0.15f, 0.15f, 1.0f));

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable(sd.get());


    osg::StateSet* ss = geode->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
    osg::ref_ptr<osg::Material> mat = new osg::Material();
    mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.15f, 0.15f, 1.0f));
    mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.4f, 0.05f, 0.05f, 1.0f));
    ss->setAttributeAndModes(mat.get(), osg::StateAttribute::ON);

    
    _aircraftScaleXform = new osg::MatrixTransform();
    _aircraftScaleXform->setMatrix(osg::Matrix::scale(_aircraftScale, _aircraftScale, _aircraftScale));
    _aircraftScaleXform->addChild(geode.get());


    _aircraftRotate = new osg::MatrixTransform();
    _aircraftRotate->addChild(_aircraftScaleXform.get());

    _aircraftXform = new osgEarth::GeoTransform();
    _aircraftXform->addChild(_aircraftRotate.get());

    if (_mapNode.valid() && _mapNode->getTerrain())
        _aircraftXform->setTerrain(_mapNode->getTerrain());

    _sceneRoot->addChild(_aircraftXform.get());
}

void RouteManager::setAircraftScale(float s)
{
    _aircraftScale = s;
    if (_aircraftScaleXform.valid())
        _aircraftScaleXform->setMatrix(osg::Matrix::scale(s, s, s));
}

// ---------------------------------------------------------------
// Rota grafigi: kirmizi cizgi + yesil noktalar
// ---------------------------------------------------------------
void RouteManager::rebuildRouteGraphics()
{
    if (!_mapNode.valid())
        return;

    if (_lineXform.valid())
    {
        _sceneRoot->removeChild(_lineXform.get());
        _lineXform = nullptr;
    }
    if (_pointXform.valid())
    {
        _sceneRoot->removeChild(_pointXform.get());
        _pointXform = nullptr;
    }

    if (_waypoints.empty())
        return;

    osg::ref_ptr<osg::Vec3dArray> worldPts = new osg::Vec3dArray();
    for (const RoutePoint& wp : _waypoints)
    {
        osg::Vec3d w;
        if (toWorld(wp.lat, wp.lon, wp.alt, w))
            worldPts->push_back(w);
    }

    if (worldPts->empty())
        return;


    const osg::Vec3d origin = (*worldPts)[0];

    osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array();
    for (const osg::Vec3d& w : *worldPts)
        verts->push_back(osg::Vec3(w - origin));

    // --- kirmizi cizgi ---
    // Tek waypoint varken cizgi cizilmez.
    if (verts->size() >= 2)
    {
        osg::ref_ptr<osg::Geometry> lineGeom = new osg::Geometry();
        lineGeom->setUseVertexBufferObjects(true);
        lineGeom->setVertexArray(verts.get());

        osg::ref_ptr<osg::Vec4Array> lineColor = new osg::Vec4Array();
        lineColor->push_back(osg::Vec4(1.0f, 0.1f, 0.1f, 1.0f));
        lineGeom->setColorArray(lineColor.get(), osg::Array::BIND_OVERALL);

        lineGeom->addPrimitiveSet(new osg::DrawArrays(
            verts->size() > 2 ? GL_LINE_LOOP : GL_LINE_STRIP, 0, verts->size()));

        osg::StateSet* lss = lineGeom->getOrCreateStateSet();
        lss->setAttributeAndModes(new osg::LineWidth(3.0f), osg::StateAttribute::ON);
        lss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        lss->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false),
            osg::StateAttribute::ON);

        osg::ref_ptr<osg::Geode> lineGeode = new osg::Geode();
        lineGeode->addDrawable(lineGeom.get());

        _lineXform = new osg::MatrixTransform();
        _lineXform->setName("RouteLine");
        _lineXform->setMatrix(osg::Matrix::translate(origin));
        _lineXform->addChild(lineGeode.get());
        _sceneRoot->addChild(_lineXform.get());
    }

    // --- yesil noktalar ---
    {
        osg::ref_ptr<osg::Geometry> pointGeom = new osg::Geometry();
        pointGeom->setUseVertexBufferObjects(true);
        pointGeom->setVertexArray(verts.get());

        osg::ref_ptr<osg::Vec4Array> ptColor = new osg::Vec4Array();
        ptColor->push_back(osg::Vec4(0.1f, 1.0f, 0.2f, 1.0f));
        pointGeom->setColorArray(ptColor.get(), osg::Array::BIND_OVERALL);

        pointGeom->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, verts->size()));

        osg::StateSet* pss = pointGeom->getOrCreateStateSet();
        pss->setAttributeAndModes(new osg::Point(12.0f), osg::StateAttribute::ON);
        pss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        pss->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false),
            osg::StateAttribute::ON);

        osg::ref_ptr<osg::Geode> ptGeode = new osg::Geode();
        ptGeode->addDrawable(pointGeom.get());

        _pointXform = new osg::MatrixTransform();
        _pointXform->setName("RoutePoints");
        _pointXform->setMatrix(osg::Matrix::translate(origin));
        _pointXform->addChild(ptGeode.get());
        _sceneRoot->addChild(_pointXform.get());
    }
}

// ---------------------------------------------------------------
// Waypoint yonetimi
// ---------------------------------------------------------------
void RouteManager::addWaypoint(double lat, double lon, double alt)
{
    _waypoints.push_back({ lat, lon, alt });
    rebuildRouteGraphics();

    if (_waypoints.size() == 1)
        resetRoute();
}

void RouteManager::removeWaypoint(size_t index)
{
    if (index >= _waypoints.size())
        return;

    _waypoints.erase(_waypoints.begin() + index);

   
    if (_waypoints.empty())
        _currentIndex = 0;
    else if (_currentIndex >= _waypoints.size())
        _currentIndex = _waypoints.size() - 1;

    rebuildRouteGraphics();
}

void RouteManager::clearWaypoints()
{
    _waypoints.clear();
    _isRunning = false;
    _currentIndex = 0;
    rebuildRouteGraphics();
}

void RouteManager::loadDefaultRoute()
{
    _waypoints = {
        { 39.9334, 32.8597, 1000.0 },   // Ankara
        { 39.9500, 32.8800, 1200.0 },
        { 40.1281, 32.9951, 1500.0 },
        { 39.7825, 32.8025, 1100.0 }
    };
    rebuildRouteGraphics();
    resetRoute();
}

void RouteManager::resetRoute()
{
    _currentIndex = 0;
    _isRunning = false;

    if (!_waypoints.empty())
    {
        _currentLat = _waypoints[0].lat;
        _currentLon = _waypoints[0].lon;
        _currentAlt = _waypoints[0].alt;
        _currentHeading = 0.0;
    }

    // Ucagi baslangic noktasina tasi
    update(0.0);
}

// ---------------------------------------------------------------
// Simulasyon adimi
// ---------------------------------------------------------------
void RouteManager::update(double deltaTime)
{
    if (!_waypoints.empty() && _isRunning && _waypoints.size() >= 2 && deltaTime > 0.0)
    {
        const RoutePoint& target = _waypoints[(_currentIndex + 1) % _waypoints.size()];

        double dLat = target.lat - _currentLat;
        double dLon = target.lon - _currentLon;
        double dist = std::sqrt(dLat * dLat + dLon * dLon);

        if (dist < 0.001)
        {
            _currentIndex = (_currentIndex + 1) % _waypoints.size();
        }
        else
        {
            double step = (_speedKmh / 3600.0) * deltaTime * 0.009;
            step = std::min(step, dist);
            _currentLat += (dLat / dist) * step;
            _currentLon += (dLon / dist) * step;

            
            _currentHeading = std::atan2(dLon, dLat) * (180.0 / PI);
            if (_currentHeading < 0.0)
                _currentHeading += 360.0;
        }

        _currentAlt += (_targetAltitude - _currentAlt) * 0.05;
    }

    
    if (_aircraftXform.valid() && _mapNode.valid())
    {
        const osgEarth::SpatialReference* srs = _mapNode->getMapSRS();
        if (srs)
        {
            osgEarth::GeoPoint gp(srs->getGeographicSRS(),
                _currentLon, _currentLat, _currentAlt,
                osgEarth::ALTMODE_ABSOLUTE);
            _aircraftXform->setPosition(gp);
        }
    }

    if (_aircraftRotate.valid())
    {
        const double hdgRad = _currentHeading * PI / 180.0;
        _aircraftRotate->setMatrix(
            osg::Matrix::rotate(-osg::PI_2, osg::Vec3d(1, 0, 0)) *
            osg::Matrix::rotate(-hdgRad, osg::Vec3d(0, 0, 1)));
    }
}
