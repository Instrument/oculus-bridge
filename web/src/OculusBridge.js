var OculusBridge = function(config) {

	// Websocket
	var socket;

	var retryOnDisconnect 	= true;
	var websocketAddress 	= config.hasOwnProperty("address") ? config["address"] : "localhost";
	var websocketPort 		= config.hasOwnProperty("port") ? config["port"] : 9000;
	var retryInterval 		= config.hasOwnProperty("retryInterval") ? config["retryInterval"] : 1;
	var debugEnabled		= config.hasOwnProperty("debug") ? config["debug"] : false;

	// Quaternion values
	var x = 0;
	var y = 0;
	var z = 0;
	var w = 0;

	// Defaults
	var HMD_Params = {
		FOV 					: 125.871,

		hScreenSize				: 0.14976,
		vScreenSize				: 0.0935,
		vScreenCenter			: 0.0935 / 2,

		eyeToScreenDistance		: 0.041,

		lensSeparationDistance	: 0.067,
		interpupillaryDistance	: 0.0675,

		hResolution				: 1280,
		vResolution				: 720,

		distortionK				: [1, .22, .24, 0],
		chromaAbParameter		: [0.996, -0.004, 1.014, 0]
	}

	// Callback handlers.
	var callbacks = {
		onOrientationUpdate : null,
		onConfigUpdate : null,
		onConnect : null,
		onDisconnect : null
	};

	// hook up any callbacks specified in the config object
	for(var cb in callbacks){
		if(typeof(config[cb]) == "function"){
			callbacks[cb] = config[cb];
		}
	}


	var updateOrientation = function(data) {

		if(data["o"] && (data["o"].length == 4)) {
			
			x = Number(data["o"][1]);
			y = Number(data["o"][2]);
			z = Number(data["o"][3]);
			w = Number(data["o"][0]);

			if(callbacks["onOrientationUpdate"]) {
				callbacks["onOrientationUpdate"](w, x, y, z);
			}
		}
	}

	var updateConfig = function(data) {
		HMD_Params.hScreenSize				= data["screenSize"][0];
		HMD_Params.vScreenSize				= data["screenSize"][1];
		HMD_Params.vScreenCenter			= data["screenSize"][1] / 2;

		HMD_Params.eyeToScreenDistance		= data["eyeToScreen"];

		HMD_Params.lensSeparationDistance	= data["lensDistance"];
		HMD_Params.interpupillaryDistance	= data["interpupillaryDistance"];

		HMD_Params.hResolution				= data["screenResolution"][0];
		HMD_Params.vResolution				= data["screenResolution"][1];

		HMD_Params.distortionK				= [ data["distortion"][0], data["distortion"][1], data["distortion"][2], data["distortion"][3] ];

		HMD_Params.FOV						= data["fov"];

		if(callbacks["onConfigUpdate"]) {
			callbacks["onConfigUpdate"]( HMD_Params );
		}
	}


	var connect = function() {
		
		retryOnDisconnect = true;
		
		var socketURL = "ws://" + websocketAddress + ":" + websocketPort + "/";
		
		// attempt to open the socket connection
	 	socket = new WebSocket(socketURL); 

		debug("Attempting to connect: " + socketURL);
	 	

	 	// hook up websocket events //

		socket.onopen = function(){
			debug("Connected!")

			if(callbacks["onConnect"]) {
				callbacks["onConnect"]();
			}
		}

		socket.onerror = function(e){
			debug("Socket error.");
		}
		
		socket.onmessage = function(msg) {
			
			var data = JSON.parse( msg.data );

			var message = data["m"];

			switch(message){
				case "config" :
					updateConfig(data);
				break;

				case "orientation":
					updateOrientation(data);
				break;

				default:
					debug("Unknown message received from server: " + msg.data);
					disconnect();
				break;
			}

		}
		
		socket.onclose = function() {
			if(callbacks["onDisconnect"]) {
				callbacks["onDisconnect"]();
			}

			if(retryOnDisconnect){
				debug("Connection failed, retrying in 1 second...");
				window.setTimeout( reconnect, retryInterval * 1000 );
			}
		}
	}

	var debug = function(message){
		if(debugEnabled){
			console.log("OculusBridge: " + message);
		}
	}

	var reconnect = function(){
		connect();
	}

	var disconnect = function(){
		retryOnDisconnect = false;
		socket.close();
	}

	var getOrientation = function(){
		return [w, x, y, z];
	}

	var isConnected = function(){
		return socket.readyState == 1;
	}

	return {
		"isConnected" : isConnected,
		"disconnect" : disconnect,
		"connect" : connect,
		"getOrientation" : getOrientation
	}
};