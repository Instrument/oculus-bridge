oculus-bridge
=============


A simple desktop app and javascript library to link the Oculus Rift with the web.

__Important Bits:__ 

- Host App for [Windows](https://github.com/Instrument/oculus-bridge/blob/master/app/build/oculus-bridge-windows.zip?raw=true) or [Mac OSX](https://github.com/Instrument/oculus-bridge/blob/master/app/build/oculus-bridge-osx.zip?raw=true)
- [JS library](https://github.com/Instrument/oculus-bridge/tree/master/web/build)
- [Documentation](https://github.com/Instrument/oculus-bridge/tree/master/docs) 


The goal of this project is to provide a flexible, simple way to access the tracking data and display configuration for the Oculus Rift for use with webGL or any other browser-based content.  This is not the first websocket based utility for the Oculus Rift, but the cross platform code base and modularity of the javascript has been built to support changes in the hardware and ease of integration on the javascript side.


## Requirements

- An Oculus Rift
- A browser that supports websockets
- The OculusBrige host application - [Windows](https://github.com/Instrument/oculus-bridge/blob/master/app/build/oculus-bridge-windows.zip?raw=true) or [Mac OSX](https://github.com/Instrument/oculus-bridge/blob/master/app/build/oculus-bridge-osx.zip?raw=true)


## Basic Usage

- Download and launch the [OculusBrige host application](https://github.com/Instrument/oculus-bridge/tree/master/app/build)
- Open a web page that makes use of the [OculusBridge javascript library](https://github.com/Instrument/oculus-bridge/tree/master/web/build)
- Plug in your Oculus Rift (you probably already did this already..)

The order in which these things occur isn't important.  The app and javascript are built to handle loosing connectivity, having the hardware connected and disconnected randomly at runtime, etc.


## Browser/Platform Support

##### Javascript library

The javascript library has been tested with the following browsers:

- Chrome 28
- Safari 6
- Firefox 22

##### OculusBridge Application

The bridge application has been compiled for MacOS Mountain Lion and Windows 7.


## Using the JavaScript Library

To add Oculus Rift support to your web project, download and include [OculusBridge.min.js](https://github.com/Instrument/oculus-bridge/tree/master/web/build) in your HTML.  The bridge is purely a way to access data from the rift hardware and does not make any assumptions about the renderer.  It is especially easy to use the bridge with THREE.js but there is no specific dependancy on THREE.js at this time.

More extensive [examples](https://github.com/Instrument/oculus-bridge/tree/master/examples) and [documentation](https://github.com/Instrument/oculus-bridge/tree/master/docs) can be found in this repo, but here's a quick bit of javascript that illustrates how to connect to the bridge, and match the rotation of an object with the head tracking data.


	// rotate a THREE.js object based on the orientation of the Oculus Rift

	var bridge = new OculusBridge({
		"onOrientationUpdate" : function(quatValues) {
			giantSquid.quaternion.set(quatValues.x, quatValues.y, quatValues.z, quatValues.w);
		}
	});

	bridge.connect();


## Credits

Thanks to the following libraries and people who make this project possible.  Credit and copyright belong to the respective parties:

- [Cinder](http://libcinder.org/) - [license](https://github.com/cinder/Cinder/blob/master/docs/COPYING)
- [Websocket++](http://www.zaphoyd.com/websocketpp) - [license](https://github.com/zaphoyd/websocketpp/blob/master/COPYING)
- [Websocket++ Cinder Block]() - 
- [THREE.js](http://threejs.org/) - [license](https://github.com/mrdoob/three.js/blob/master/LICENSE)
- [RiftCamera](https://github.com/troffmo5/OculusStreetView) for THREE.js
- [Oculus Rift SDK](http://developer.oculusvr.com)
