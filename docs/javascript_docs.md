OculusBridge.js
===============


## Configuraion Options

The OculusBridge object accepts a single configuration object that may contain any of the following properties.  All configuration values are optional.


`onConnect` - **function**, a callback that will be invoked when a connection is estabilshed.

`onDisconnect` - **function**, a callback invoked when the connection is lost.

`onConfigUpdate` - **function**, a callback invoked when configuration data is recieved from the server.  A single argument is passed to this callback, it is a map of the metrics for the head mounted display.  These metrics are needed when rendering any images for use on the Oculus Rift.

`onOrientationUpdate` - **function**, invoked whenever new orientation values are sent from the server (around 60 Hz).  A single object is passed to this callback, containing the quaternion values in the same format as the `getOrientation` method.

`onAccelerationUpdate` - **function**, just like the orientation update, this function is invoked whenever new acceleration values are sent from the server (around 60 Hz).  A single object is passed to this callback with the same format as the `getAcceleration` method.

`debug` - **boolean**, default is false, this parameter 

`port` - **number**, default is 9005, changing this value is not advised if you are planning to make your content public since anyone else using the bridge app will not be expecting the non-default port.

`address` - **string**, default is "localhost".  IMPORTANT: currently the bridge app will refuse any websocket connection that doesn't originate from localhost.  This may change in the future but until then you should leave this parameter alone unless you're going to make a custom build of the app.

`retryInterval` - **number**, default is 1, the number of seconds to wait between connection attempts.


## Using the Callbacks

	var bridge = new OculusBridge( {
		"onConnect" : function() { 
			console.log("we are connected!");
		},
		"onDisconnect" : function() {
			console.log("good bye Oculus.");
		},
		"onConfigUpdate" : function(config) {
			console.log("Field of view: " + config.FOV);
		},
		"onOrientationUpdate" : function(quatValues){
			var values = [quatValues.x, quatValues.y, quatValues.z, quatValues.w];
			console.log("Orientation: " + values.join(", "));
		}
		"onAccelerationUpdate" : function(accelValues){
			var values = [accelValues.x, accelValues.y, accelValues.z];
			console.log("Acceleration: " + values.join(", "));
		}
	});

	bridge.connect();


## Methods

`connect` - attempt to connect to the bridge server.

`disconnect` - disconnect from the bridge server

`isConnected` - returns a boolean indicating connection status

`getOrientation` - an alternative to using the `onOrientationUpdate` callback, this method returns the last known quaternion values sent from the server.

Example orientation object:

	{
		"x" : 0.2329875,
		"y" : 1.1288273,
		"z" : 0.1837934,
		"w" : 0.0439387
	}
	
`getAcceleration` - an alternative to using the `onAccelerationUpdate` callback, this method returns the last known accelerometer values sent from the server.  Acceleration values represent standard G's with the y-axis representing up and down.  In other words, if the headset is sitting motionless on a level surface you will see 9.8 in the y axis with zeros for x and z.

Example acceleration object:

	{
		"x" : 0,
		"y" : 9.8,
		"z" : 0
	}
	
`getConfiguration` - an alternative to using the `onConfigUpdate` callback, this method returns the metrics for the head mounted display.  If a connection has not been made, the values will default to the metrics for the currently released development hardware.

Example configuration object:

	{
		"FOV" 						: 125.871,
		"hScreenSize"				: 0.14976,
		"vScreenSize"				: 0.0935,
		"vScreenCenter"				: 0.0468,
		"eyeToScreenDistance"		: 0.041,
		"lensSeparationDistance"	: 0.067,
		"interpupillaryDistance"	: 0.0675,
		"hResolution"				: 1280,
		"vResolution"				: 720,
		"distortionK"				: [1, .22, .24, 0],
		"chromaAbParameter"			: [0.996, -0.004, 1.014, 0]
	}