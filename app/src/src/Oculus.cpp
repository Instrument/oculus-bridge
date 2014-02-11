//
//  Oculus.cpp
//  OculusSocketServer
//
//  Created by Ben Purdy on 7/15/13.
//
//  This class is a simple wrapper to get configuration and tracking data from the
//  Oculus Rift (based on the Cinder block by https://github.com/simongeilfus minus
//  the rendering stuff, plus some help from the example projects in the Oculus SDK)
//

#include "cinder/app/AppNative.h"
#include "cinder/Utilities.h"
#include "Oculus.h"

using namespace ci;

OculusRef Oculus::create()
{
    OculusRef newDevice( new Oculus() );
    return newDevice;
}


Oculus::~Oculus()
{
    destroy();
}


void Oculus::destroy(){
    RemoveHandlerFromDevices();

    // Clean up
    if(mSensorDevice){
        mSensorDevice.Clear();
    }
    if(mHMD){
        mHMD.Clear();
    }
    if(mManager){
        mManager.Clear();
    }
}

Oculus::Oculus()
{
    mIsConnected = false;
    
    // Init OVR
    OVR::System::Init( OVR::Log::ConfigureDefaultLog( OVR::LogMask_All ) );
    
    // Create Manager and Oculus Handle
    mManager = *OVR::DeviceManager::Create();
    
    // route messages for connect/disconnect of the device.
    mManager->SetMessageHandler(this);
    
    // try to create an HMD
    mHMD     = *mManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
    
    // hook up the sensors and start calibration if a device is present.
    if( mHMD ){
        mIsConnected = true;
        
        if (mHMD->GetDeviceInfo(&mHMDInfo))
        {
            mStereoConfig.SetHMDInfo( mHMDInfo );
        }
        
        mSensorDevice = *mHMD->GetSensor();
        
        if (mSensorDevice){
            mSensorFusion = new SensorFusion();
            mSensorFusion->AttachToSensor(mSensorDevice);
        } else {
            app::console() << "No Sensor found.\n";
        }
    } else {
        app::console() << "No Oculus found.\n";
    }
    
    
}

float Oculus::getIPD() const
{
    return mStereoConfig.GetIPD();
}

float Oculus::getFov()
{
    return mStereoConfig.GetYFOVDegrees();
}

float Oculus::getEyeToScreenDistance() const
{
    return mStereoConfig.GetEyeToScreenDistance();
}

float Oculus::getProjectionCenterOffset()
{
    return mStereoConfig.GetProjectionCenterOffset();
}

float Oculus::getDistortionScale()
{
    return mStereoConfig.GetDistortionScale();
}

Vec4f Oculus::getDistortionParams() const
{
    return Vec4f( mHMDInfo.DistortionK[0], mHMDInfo.DistortionK[1], mHMDInfo.DistortionK[2], mHMDInfo.DistortionK[3] );
}

ci::Quatf Oculus::getOrientation()
{
    return ci::Quatf( mSensorFusion->GetOrientation().w, mSensorFusion->GetOrientation().x, mSensorFusion->GetOrientation().y, mSensorFusion->GetOrientation().z );
}

Vec3f Oculus::getAcceleration()
{
    return Vec3f( mSensorFusion->GetAcceleration().x, mSensorFusion->GetAcceleration().y, mSensorFusion->GetAcceleration().z );
}

float Oculus::getLensSeparationDistance() {
    return mHMDInfo.LensSeparationDistance;
}

Vec2f Oculus::getScreenSize(){
    return Vec2f( mHMDInfo.HScreenSize, mHMDInfo.VScreenSize);
}

Vec2f Oculus::getScreenResolution(){
    return Vec2f( mHMDInfo.HResolution, mHMDInfo.VResolution);
}





/**
* Mesasge handling code, adapted from the Tuscany demo code included with the Oculus SDK
**/

// Handler for messages, when messages come in they're put into a queue to be handled later.
void Oculus::OnMessage(const Message& msg) {
    if (msg.Type == Message_DeviceAdded || msg.Type == Message_DeviceRemoved)
    {
        if (msg.pDevice == mManager)
        {
            const MessageDeviceStatus& statusMsg =
            static_cast<const MessageDeviceStatus&>(msg);
            
            { // limit the scope of the lock
                Lock::Locker lock(mManager->GetHandlerLock());
                DeviceStatusNotificationsQueue.PushBack(DeviceStatusNotificationDesc(statusMsg.Type, statusMsg.Handle));
            }
            
            switch (statusMsg.Type)
            {
                case OVR::Message_DeviceAdded:
                    app::console() << "DeviceManager reported device added." << std::endl;
                    break;
                    
                case OVR::Message_DeviceRemoved:
                    app::console() << "DeviceManager reported device removed." << std::endl;
                    break;
                    
                default: OVR_ASSERT(0); // unexpected type
            }
        }
    }
}

// Process any messages in the queue.
void Oculus::handleMessages() {
    bool queueIsEmpty = false;
    
    while (!queueIsEmpty)
    {
        DeviceStatusNotificationDesc desc;
        {
            Lock::Locker lock(mManager->GetHandlerLock());
            if (DeviceStatusNotificationsQueue.GetSize() == 0)
                break;
            desc = DeviceStatusNotificationsQueue.Front();
            
            // We can't call Clear under the lock since this may introduce a dead lock:
            // this thread is locked by HandlerLock and the Clear might cause
            // call of Device->Release, which will use Manager->DeviceLock. The bkg
            // thread is most likely locked by opposite way:
            // Manager->DeviceLock ==> HandlerLock, therefore - a dead lock.
            // So, just grab the first element, save a copy of it and remove
            // the element (Device->Release won't be called since we made a copy).
            
            DeviceStatusNotificationsQueue.RemoveAt(0);
            queueIsEmpty = (DeviceStatusNotificationsQueue.GetSize() == 0);
        }
        
        bool wasAlreadyCreated = desc.Handle.IsCreated();
        
        if (desc.Action == Message_DeviceAdded)
        {
            switch(desc.Handle.GetType())
            {
                case Device_Sensor:
                    if (desc.Handle.IsAvailable() && !desc.Handle.IsCreated())
                    {
                        if (!mSensorDevice)
                        {
                            mSensorDevice = *desc.Handle.CreateDeviceTyped<SensorDevice>();
                            mSensorFusion->AttachToSensor(mSensorDevice);
                            mIsConnected = true;
                        }
                        else if (!wasAlreadyCreated)
                        {
                            app::console() << "A new SENSOR has been detected, but it is not currently used." << std::endl;
                        }
                    }
                    break;
                case Device_HMD:
                {
                    OVR::HMDInfo info;
                    desc.Handle.GetDeviceInfo(&info);
                    
                    if (strlen(info.DisplayDeviceName) > 0 && (!mHMD || !info.IsSameDisplay(mHMDInfo)))
                    {
                        if (!mHMD || !desc.Handle.IsDevice(mHMD))
                            mHMD = *desc.Handle.CreateDeviceTyped<HMDDevice>();

                            // update stereo config with new HMDInfo
                            if (mHMD && mHMD->GetDeviceInfo(&mHMDInfo))
                            {
                                mStereoConfig.SetHMDInfo(mHMDInfo);
                            }
                    }
                    break;
                }
                default:;
            }
        }
        else if (desc.Action == Message_DeviceRemoved)
        {
            if (desc.Handle.IsDevice(mSensorDevice))
            {
                mSensorFusion->AttachToSensor(NULL);
                mSensorDevice.Clear();
                mIsConnected = false;
            }
            else if (desc.Handle.IsDevice(mHMD))
            {
                if (mHMD && !mHMD->IsDisconnected())
                {
                    mHMD = mHMD->Disconnect(mSensorDevice);
                    
                    // This will initialize TheHMDInfo with information about configured IPD,
                    // screen size and other variables needed for correct projection.
                    // We pass HMD DisplayDeviceName into the renderer to select the
                    // correct monitor in full-screen mode.
                    if (mHMD && mHMD->GetDeviceInfo(&mHMDInfo))
                    {
                        mStereoConfig.SetHMDInfo(mHMDInfo);
                    }
                }
            }
        }
        else
            OVR_ASSERT(0); // unexpected action
    }
}