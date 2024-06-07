
#ifndef _APP_SIGNAL_HPP_
#define _APP_SIGNAL_HPP_

#include <utility>
#include <boost/any.hpp>
#include <boost/signals2.hpp>
#include "app/matrix.hpp"


enum EventType {
    k2DWindowLevelChanged,
    k3DTransFuncChanged,
    kDetectorConnected,
    kNotifyMessage,
    kPopMessage,
    kPreRecon2dDone,
    kPreRecon3dDone,
    kProgressUpdated,
    kProjLoaded,
    kProjPrepared,
    kReconDone,
    kReconLayerModified,
    kRenderModeModified,
    kRenderVolumeRequired,
    kScanStarted,
    kTick,
};
struct AppEvent {
    EventType type;
    boost::any p1;
    boost::any p2;
    boost::any p3;
    boost::any p4;
    boost::any p5;
};

// TODO: unify signals to AppSignal with std::variant / boost::variant
typedef boost::signals2::signal<void(long long elapsed_ms)> TickSignal;

typedef boost::signals2::signal<void(AppEvent)> AppSignal;

// detector id, sequential id, image data
typedef boost::signals2::signal<void(int, int, MatPtr)> NewProjSignal;

#endif
