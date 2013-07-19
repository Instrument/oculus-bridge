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
    gl::Texture         mBackgroundTexture;
};


void OculusSocketServerApp::prepareSettings( Settings *settings ){
    settings->setTitle("Oculus Bridge");
    settings->setFrameRate(60);
    settings->setWindowSize(256, 145);
    settings->setResizable(false);
}

void OculusSocketServerApp::setup()
{
    mServerPort = 9000;
    
    string configPath = getAppPath().string() + "/../config.json";
    
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
    if( mOculusVR  ) {
        orientation = mOculusVR->getOrientation();

        boost::format fmt("{ \"m\" : \"orientation\", \"o\" : [%f,%f,%f,%f] }");
        
        fmt % orientation.w;
        fmt % orientation.v.x;
        fmt % orientation.v.y;
        fmt % orientation.v.z;

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
    mOculusVR->handleMessages();
}

void OculusSocketServerApp::draw()
{
    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
    
    glColor3f(1.0, 1.0, 1.0);
    gl::draw(mBackgroundTexture);
    
    glColor3f(0.43921568627451, 0.89803921568627, 0.62352941176471);

    if(mSocketConnected) {
        gl::drawSolidRect( Rectf(214, 30, 226, 42));
    }    
    if(mOculusVR && mOculusVR->isConnected()) {
        gl::drawSolidRect( Rectf(214, 104, 226, 116));
    }
}

CINDER_APP_NATIVE( OculusSocketServerApp, RendererGl )
