//
//  Oculus.h
//  OculusSocketServer
//
//  Created by Ben Purdy on 7/15/13.
//


#ifndef __OculusSocketServer__Oculus__
#define __OculusSocketServer__Oculus__

#include <iostream>
#include "OVR.h"

using namespace OVR;

typedef std::shared_ptr< class Oculus > OculusRef;

class Oculus : public MessageHandler
{
public:
    // ! Returns an empty ptr if we can't initialize correctly the HMD device
    static OculusRef create();
    ~Oculus();
    
    void        destroy();
    
    float       getIPD() const;
    float       getFov();
    float       getEyeToScreenDistance() const;
    float       getProjectionCenterOffset();
    float       getDistortionScale();
    ci::Vec4f   getDistortionParams() const;
    ci::Quatf   getOrientation();
    ci::Vec3f   getAcceleration();
    float       getLensSeparationDistance();
    ci::Vec2f   getScreenSize();
    ci::Vec2f   getScreenResolution();
    
    virtual void    OnMessage(const Message&);
    void            handleMessages();
    
    bool        isConnected() { return mHMD && mSensorDevice && !mHMD->IsDisconnected(); };
    
protected:
    Oculus();
        
    OVR::Ptr<OVR::DeviceManager>    mManager;
    OVR::Ptr<OVR::HMDDevice>        mHMD;
    OVR::HMDInfo                    mHMDInfo;
    OVR::SensorFusion*              mSensorFusion;
    OVR::Ptr<OVR::SensorDevice>     mSensorDevice;
    OVR::Util::Render::StereoConfig mStereoConfig;

    bool                            mIsConnected;

    struct DeviceStatusNotificationDesc
    {
        DeviceHandle    Handle;
        MessageType     Action;
        
        DeviceStatusNotificationDesc():Action(Message_None) {}
        DeviceStatusNotificationDesc(MessageType mt, const DeviceHandle& dev) : Handle(dev), Action(mt) {}
    };
    
    Array<DeviceStatusNotificationDesc> DeviceStatusNotificationsQueue;
};

#endif /* defined(__OculusSocketServer__Oculus__) */
