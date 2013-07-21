oculus-bridge
=============

A simple desktop app and javascript library to link the Oculus Rift with the web.

The goal of this project is to provide a flexible, modular way to access the tracking data and display configuration for the Oculus Rift for use with webGL or any other browser-based content.  This is not the first websocket based utility for the Oculus Rift, but the cross platform code base and modularity of the javascript has been built to support changes in the hardware and ease of integration on the javascript side.



## Requirements

- A browser that supports websockets
- An Oculus Rift



## Basic Usage

- Download and launch the bridge application
- Open a web page that makes use of the OculusBridge javascript library
- Plug in your Oculus Rift (you probably already did this already..)

The order that these things occur isn't important.  The app and javascript are built to handle loosing connectivity, having the hardware connected and disconnected randomly at runtime, etc.



## JavaScript Library

To add Oculus Rift support to your web project, simply include `OculusBridge.js` in your HTML.  The bridge is purely a way to access the rift hardware and does not make any assumptions about the renderer.  It is especially easy to use the bridge with THREE.js but there is no specific dependancy on THREE.js at this time.

For example, here is how you would hook the orientation of the Oculus Rift up so it controls the orientation of an object in a THREE.js scene.  The following code would be run during initialization.


	// rotate a THREE.js object based on the orientation of the Oculus Rift

	var bridge = new OculusBridge({
		"onOrientationUpdate" : function(quat) {
			giantSquid.quaternion.set(quat.x, quat.y, quat.z, quat.w);
		}
	}
	});

	bridge.connect();




