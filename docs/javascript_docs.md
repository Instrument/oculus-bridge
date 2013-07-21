OculusBridge.js
===============


## Configuraion Options

The OculusBridge object accepts a single configuration object that may contain any of the following properties.  All configuration values are optional.


`onConnect` - **function**, a callback that will be invoked when a connection is estabilshed.

`onDisconnect` - **function**, a callback invoked when the connection is lost.

`onConfigUpdate` - **function**, a callback invoked when configuration data is recieved from the server.  A single argument is passed to this callback, it is a map of the metrics for the head mounted display.  These metrics are needed when rendering any images for use on the Oculus Rift.

`onOrientationUpdate` - **function**, invoked whenever new orientation values are sent from the server (around 60 Hz).  The four quaternion values are passed to the callback as individual arguments.

`debug` - **boolean**, default is false, this parameter 

`port` - **number**, default is 9000, but can be changed if needed.

`address` - **string**, default is "localhost".

`retryInterval` - **number**, default is 1, the number of seconds to wait between connection attempts.


## Using the Callbacks

	var bridge = new OculusBridge( {
		"onConnect" : function() { 
			console.log("we are connected!");
		},
		"onConfigurationUpdate" : function(config) {
			console.log("Field of view: " + config.FOV);
		},
		"onOrientationUpdate" : function(quatValues){
			var values = [quatValues.x, quatValues.y, quatValues.z, quatValues.w];
			console.log("Orientation: " + values.join(", "));
		}
	});

	bridge.connect();


## Methods

`connect` - attempt to connect to the bridge server.

`disconnect` - disconnect from the bridge server

`isConnected` - returns a boolean indicating connection status

`getOrientation` - an alternative to using the `onOrientationUpdate` callback, this method returns the last known quaternion values sent from the server.

`getConfiguration` - returns the metrics for the head mounted display.  If a connection has not been made, these will default to the metrics for the development model.