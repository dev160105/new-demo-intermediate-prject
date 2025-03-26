#ifndef MAP_EVENTS_H
#define MAP_EVENTS_H

enum class EventType {
    MapLoaded,
    OverlayLoaded,
    OverlayClicked,
    DimensionsConfirmed,
    PathDraw,
    TowerPlaced
};

#endif