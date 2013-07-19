oculus-bridge
=============

A simple desktop app and javascript library to link the Oculus Rift with the web.

The goal of this project is to provide a flexible, modular way to access the tracking data and display metrics for the Oculus Rift for use with webGL or any other browser-based content.  This is not the first websocket based utility for the Oculus Rift, but the cross platform code base and modularity of the javascript has been built to support changes in the hardware and ease of integration on the javascript side.


Usage
=====

Do these three things:

- Download and run the host app
- Open a web page that makes use of the OculusBridge javascript library
- Plug in your Oculus Rift (you probably already did this already but if not, now would be a good time..)

The order that these things occur isn't important.  The app and javascript are built to handle loosing connectivity, having the hardware connected and disconnected randomly at runtime, etc.



Using the JavaScript Library
============================

Include OculusBridge.js in your project, there are currently no external dependancies.  The bridge is purely a way to access the rift hardware and does not make any assumptions about the renderer, or that there even IS a renderer.


// Example: rotate a THREE.js object based on the orientation of the Oculus Rift

var bridge = OculusBridge({
	"onOrientationUpdate" : function(w, x, y, z) {
		myRandomObject.quaternion.set(w, x, y, z);
	}
}
});

bridge.connect();



Configuraion Options
====================

The following properties can be set when instantiating the bridge, all properties are optiona.

onConnect - function, a callback that will be invoked when a connection is estabilshed.
onDisconnect - function, a callback invoked when the connection is lost.
onConfigUpdate - function, a callback invoked when configuration data is recieved from the server.
onOrientationUpdate - function, invoked whenever new orientation values are sent from the server (around 60 hertz)
debug - boolean, true will cause debug data to be logged to the javscript console.
port - number, default is 9000, but can be changed if needed.
address - string, optional. default is localhost.
retryInterval - number, default is 1, the number of seconds to wait between connection attempts.

Example:

var bridge = OculusBridge( {
	"onConnect" : function() { 
		console.log("hooray!");
	},
	"onConfigurationUpdate" : function(config) {
		console.log("Field of view: " + config.FOV);
	}
});

bridge.connect();



Methods
=======

connect - attempt to connect to the bridge server.
disconnect - disconnect from the bridge server
isConnected - returns a boolean indicating connection status
getOrientation - returns an array of numbers representing the last known w, x, y, and z quaternion values sent from the server.

