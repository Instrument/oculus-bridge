oculus-bridge
=============


A simple desktop app and javaScript library to link the Oculus Rift with the web.

__Important Bits:__ 

- Host App: [Windows](https://github.com/Instrument/oculus-bridge/blob/master/app/build/oculus-bridge-windows.zip?raw=true) | [Mac OSX](https://github.com/Instrument/oculus-bridge/blob/master/app/build/oculus-bridge-osx.zip?raw=true)
- [JS library](https://github.com/Instrument/oculus-bridge/tree/master/web/build)
- [Documentation](https://github.com/Instrument/oculus-bridge/tree/master/docs) 


The goal of this project is to provide a flexible, simple way to access the tracking data and display configuration for the Oculus Rift for use with webGL or any other browser-based content.  This is not the first websocket based utility for the Oculus Rift, but the cross platform code base and modularity of the JavaScript has been built to support changes in the hardware and ease of integration on the JavaScript side.


## Requirements

- An Oculus Rift
- A browser that supports websockets
- The OculusBrige host application : [Windows](https://github.com/Instrument/oculus-bridge/blob/master/app/build/oculus-bridge-windows.zip?raw=true) | [Mac OSX](https://github.com/Instrument/oculus-bridge/blob/master/app/build/oculus-bridge-osx.zip?raw=true)


## Basic Usage

- Download and launch the OculusBrige host application
- Open one of the example pages:
	- [Basic Connection Test](http://instrument.github.io/oculus-bridge/examples/connect.html)
	- [First Person Example](http://instrument.github.io/oculus-bridge/examples/first_person.html)
	- [CSS Shader Example](http://instrument.github.io/oculus-bridge/examples/css_shaders.html)
- Plug in your Oculus Rift (you probably already did this already..)

The order in which these things occur isn't important.  The app and JavaScript are built to handle loosing connectivity, having the hardware connected and disconnected randomly at runtime, etc.


## Browser/Platform Support

##### JavaScript library

The JavaScript library has been tested with the following browsers:

- Chrome 28
- Safari 6
- Firefox 22

Example content is currently using THREE.js r59


##### OculusBridge Application

The bridge application has been compiled for MacOS Mountain Lion and Windows 7.


## Using the JavaScript Library

To add Oculus Rift support to your web project, download and include [OculusBridge.min.js](https://github.com/Instrument/oculus-bridge/tree/master/web/build) in your HTML.  The bridge is purely a way to access data from the rift hardware and does not make any assumptions about the renderer.  It is especially easy to use the bridge with THREE.js but there is no specific dependancy on THREE.js at this time.  Check out the documentation and example code for more detailed explaination.

	// rotate a THREE.js object based on the orientation of the Oculus Rift

	var bridge = new OculusBridge({
		"onOrientationUpdate" : function(quatValues) {
			giantSquid.quaternion.set(quatValues.x, quatValues.y, quatValues.z, quatValues.w);
		}
	});

	bridge.connect();


## Documentation

- [OculusBridge JavaScript](https://github.com/Instrument/oculus-bridge/blob/master/docs/javascript_docs.md)
- [Implementing typical FPS movement](https://github.com/Instrument/oculus-bridge/blob/master/docs/first_person_movement.md)
- [Building the host app](https://github.com/Instrument/oculus-bridge/blob/master/docs/build_instructions.md)

## Credits

Thanks to the following libraries and people who make this project possible.  Credit and copyright belong to the respective parties:

- [Cinder](http://libcinder.org/) - [license](https://github.com/cinder/Cinder/blob/master/docs/COPYING)
- [Websocket++](http://www.zaphoyd.com/websocketpp) - [license](https://github.com/zaphoyd/websocketpp/blob/master/COPYING)
- [Websocket++ Cinder Block](https://github.com/BanTheRewind/Cinder-WebSocketPP) 
- [THREE.js](http://threejs.org/) - [license](https://github.com/mrdoob/three.js/blob/master/LICENSE)
- [RiftCamera](https://github.com/troffmo5/OculusStreetView) for THREE.js
- [Oculus Rift SDK](http://developer.oculusvr.com)