#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <vector>

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/MatrixTransform>

#include <osgEarth/MapNode>
#include <osgEarth/GeoTransform>

struct RoutePoint
{
    double lat;
    double lon;
    double alt;
};

class RouteManager : public osg::Referenced
{
public:
    RouteManager();

    // mapNode'u baglar ve sahne grafigini kurar.
    void init(osgEarth::MapNode* mapNode);

    // Ucak konisi, rota cizgisi ve waypoint noktalarini tasiyan dugum.
    // main.cpp bunu rootGroup'a ekliyor.
    osg::Group* getSceneRoot() { return _sceneRoot.get(); }

    void update(double deltaTime);

    // --- rota kontrolu ---
    void startRoute() { _isRunning = true; }
    void pauseRoute() { _isRunning = false; }
    void resetRoute();

    // --- waypoint yonetimi ---
    void addWaypoint(double lat, double lon, double alt);
    void removeWaypoint(size_t index);
    void clearWaypoints();
    void loadDefaultRoute();

    const std::vector<RoutePoint>& getWaypoints() const { return _waypoints; }

    // --- durum sorgulari ---
    bool isRunning() const { return _isRunning; }
    size_t getCurrentIndex() const { return _currentIndex; }
    size_t getTotalWaypoints() const { return _waypoints.size(); }

    double getCurrentLat() const { return _currentLat; }
    double getCurrentLon() const { return _currentLon; }
    double getCurrentAlt() const { return _currentAlt; }
    double getCurrentHeading() const { return _currentHeading; }

    float getSpeedKmh() const { return _speedKmh; }
    void setSpeedKmh(float speed) { _speedKmh = speed; }

    float getTargetAltitude() const { return _targetAltitude; }
    void setTargetAltitude(float alt) { _targetAltitude = alt; }

    // Kirmizi koninin buyuklugu (metre). Yuksekten bakinca gorunsun diye
    // varsayilan olarak buyuk.
    float getAircraftScale() const { return _aircraftScale; }
    void setAircraftScale(float s);

protected:
    virtual ~RouteManager() {}

private:
    // Rota degisince cizgi ve noktalari bastan olusturur.
    void rebuildRouteGraphics();

    // Ucak konisini olusturur (bir kez).
    void buildAircraft();

    // lat/lon/alt -> ECEF dunya koordinati
    bool toWorld(double lat, double lon, double alt, osg::Vec3d& out) const;

    osg::observer_ptr<osgEarth::MapNode> _mapNode;

    osg::ref_ptr<osg::Group> _sceneRoot;
    // Cizgi ve noktalari tasiyan transformlar. Sahneden cikarabilmek icin
    // dogrudan bunlara referans tutuyoruz (Geode'a degil).
    osg::ref_ptr<osg::MatrixTransform> _lineXform;    // kirmizi rota cizgisi
    osg::ref_ptr<osg::MatrixTransform> _pointXform;   // yesil waypoint noktalari
    osg::ref_ptr<osgEarth::GeoTransform> _aircraftXform;
    osg::ref_ptr<osg::MatrixTransform> _aircraftRotate;
    osg::ref_ptr<osg::MatrixTransform> _aircraftScaleXform;

    std::vector<RoutePoint> _waypoints;
    size_t _currentIndex;

    double _currentLat = 0.0;
    double _currentLon = 0.0;
    double _currentAlt = 0.0;
    double _currentHeading;

    float _speedKmh;
    float _targetAltitude;
    float _aircraftScale = 3000.0f;
    bool _isRunning;
};