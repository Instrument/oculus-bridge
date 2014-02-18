#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/Camera.h"
#include "cinder/gl/Texture.h"
#include "cinder/Json.h"
#include <boost/format.hpp>

#include "Resources.h"

#include "WebSocketServer.h"
#include "Oculus.h"
#include "OVR.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class OculusSocketServerApp : public AppNative {
  public:
	void                setup();
    void                shutdown();
	void                update();
	void                draw();
    void                prepareSettings( Settings *settings );
    
  private:
    
    void                sendHMDConfig();
	void                onConnect();
	void                onDisconnect();

    bool                mSocketConnected;
    bool                mOculusConnected;
    int                 mServerPort;

    OculusRef           mOculusVR;
    WebSocketServer     mServer;
   
    ci::Quatf           orientation;
    ci::Vec3f           acceleration;
    gl::Texture         mBackgroundTexture;
};

void OculusSocketServerApp::prepareSettings( Settings *settings ){
    settings->setTitle("Oculus Bridge");
    settings->setFrameRate(60);
    settings->setWindowSize(400, 145);
    settings->setResizable(false);
}

void OculusSocketServerApp::setup()
{
    mServerPort = 9005;

    string configPath = getAppPath().string() + "/config.json";
    
    if(fs::exists(configPath)) {
        JsonTree::ParseOptions options;
        options.ignoreErrors(true);

        try{
            JsonTree config = JsonTree( loadFile( configPath ));
            
            for( JsonTree::ConstIter cIt = config.begin(); cIt != config.end(); ++cIt )
            {
                if( "port" == (*cIt).getKey()){
                    mServerPort = std::stoi((*cIt).getValue());
                    console() << "Port found: " << mServerPort << std::endl;
                }
            }
            
        } catch(JsonTree::Exception ex){
            console() << "Unable to parse config file." << std::endl;
        }
    } else {
        console() << "No config file found." << std::endl;
    }

    mOculusConnected = false;
    mSocketConnected = false;
    
    mServer.addConnectCallback( &OculusSocketServerApp::onConnect, this );
	mServer.addDisconnectCallback( &OculusSocketServerApp::onDisconnect, this );
   
    mServer.listen(mServerPort);
    mOculusVR = Oculus::create();
    
    mBackgroundTexture = gl::Texture( loadImage( loadResource( RES_BACKGROUND_TEX ) ) );
}

void OculusSocketServerApp::shutdown(){
    if(mSocketConnected){
        console() << "Closing socket connection." << std::endl;
        mServer.close();
    }
    
    if(mOculusVR){
        console() << "Disposing of Oculus." << std::endl;
        mOculusVR->destroy();
    }
    console() << "Done with shutdown()" << std::endl;
}

void OculusSocketServerApp::onConnect(){

    // Only accept connections from localhost
    if( mServer.getClientAddress() != "localhost"){
        mServer.close();
    }
    
    mSocketConnected = true;
    
    std::cout << "Connected..\n";
    
    sendHMDConfig();
}

void OculusSocketServerApp::sendHMDConfig(){

    if(mOculusVR && mOculusVR->isConnected() && mSocketConnected) {
        
        float fov = mOculusVR->getFov();
        float ipd = mOculusVR->getIPD();
        float lensDistance = mOculusVR->getLensSeparationDistance();
        float eyeToScreen = mOculusVR->getEyeToScreenDistance();
        Vec4f distortionValues = mOculusVR->getDistortionParams();
        Vec2f screenSize = mOculusVR->getScreenSize();
        Vec2f screenResolution = mOculusVR->getScreenResolution();
        
        boost::format fmt("{ \"m\" : \"config\", \"fov\" : %f, \"interpupillaryDistance\" : %f, \"eyeToScreen\" : %f, \"lensDistance\" : %f, \"distortion\" : [%f,%f,%f,%f], \"screenSize\" : [%f,%f], \"screenResolution\" : [%f,%f] }");
        
        fmt % fov;
        fmt % ipd;
        fmt % eyeToScreen;
        fmt % lensDistance;
        fmt % distortionValues.x;
        fmt % distortionValues.y;
        fmt % distortionValues.z;
        fmt % distortionValues.w;
        fmt % screenSize.x;
        fmt % screenSize.y;
        fmt % screenResolution.x;
        fmt % screenResolution.y;
        
        mServer.write(fmt.str());
    }
}

void OculusSocketServerApp::onDisconnect()
{
    std::cout << "Disonnected..\n";
    mSocketConnected = false;
}

void OculusSocketServerApp::update()
{
    mOculusVR->handleMessages();
    
    if( mOculusVR && mOculusVR->isConnected() && mSocketConnected ) {
       
        boost::format fmt("{ \"m\" : \"update\", \"o\" : [%f,%f,%f,%f], \"a\" : [%f,%f,%f] }");
        
        orientation = mOculusVR->getOrientation();
        acceleration = mOculusVR->getAcceleration();
        
        fmt % orientation.w;
        fmt % orientation.v.x;
        fmt % orientation.v.y;
        fmt % orientation.v.z;

        fmt % acceleration.x;
        fmt % acceleration.y;
        fmt % acceleration.z;

        mServer.write(fmt.str());
    }
    
    if(mOculusVR && mOculusVR->isConnected()){
        if(!mOculusConnected){
            sendHMDConfig();
            mOculusConnected = true;
        }
    } else {
        mOculusConnected = false;
    }
    
    mServer.poll();
}

void OculusSocketServerApp::draw()
{
    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
    
    glColor3f(1.0, 1.0, 1.0);
    gl::draw(mBackgroundTexture);
    
    glColor3f(0.16078431372549, 0.83921568627451, 0.88235294117647);
    if(mSocketConnected) {
        gl::drawSolidRect( Rectf(358, 30, 370, 42) );
    }

    if(mOculusVR && mOculusVR->isConnected()) {
        gl::drawSolidRect( Rectf(358, 104, 370, 116) );
    }
}

CINDER_APP_NATIVE( OculusSocketServerApp, RendererGl )
