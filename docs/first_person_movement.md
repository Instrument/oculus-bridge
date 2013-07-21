First Person Controls
=====================

Using the Oculus Rift to control the direction of movement makes it much easier and more natural to explore virtual environments.  The following example code illustrates a method of tracking the view direction along the x/z plane, making it possible for the user to move in the direction that they are facing.

In this example, we keep track of a base "body angle" which is combined with the orientation of the Oculus Rift.  By keeping track of the body angle, we can use keyboard or gamepad controls to modify the body angle in conjunction with the user's head movement.  Without a system like this, the user would potentailly have to physically rotate their body all the way around in order to navigate in the virtual environment.

This technique is exactly how the `first_person.html` example works, and is modeled after the behavior from the Tuscany demo (included with the Oculus SDK) as well as other first person games that use the Oculus Rift (HalfLife 2, etc).

**Note:** this example is based on integration with THREE.js, but any library that implements quaternions and vectors would work with this technique.


## Initial Setup

As stated before, this technique is used to move the player along a particular plane, and makes the assumption that the player's virtual body will always be "standing" on that plane.  In this example, we use the XZ plane for the "ground" and the Y axis is the up/down axis.

In our code, we must keep track of the angle that the player is facing if they are not turning their head.  We'll call this `bodyAngle` and it will be modified via the keyboard controls to turn left or right.

For movement in the XZ plane, we will use another value which we'll call `viewAngle` and will represent the current direction that the player is actually looking, along the XZ plane.  The `viewAngle` value represents a combination of the player's `bodyAngle` and the angle of their head as reported from the Oculus Bridge.


## What Direction Are They Facing?

The following code will determine the current `viewAngle` when the `onOrientationUpdate` callback is invoked.
	
	function bridgeOrientationUpdated(quatValues) {
	
		var bodyAxis = new THREE.Vector3(0, 1, 0);

		// make a quaternion for the the body angle rotated about the Y axis
		var bodyQuat = new THREE.Quaternion();
	  	bodyQuat.setFromAxisAngle(bodyAxis, bodyAngle);
	
		// make a quaternion for the current orientation of the Rift
		var riftQuat = new THREE.Quaternion(quatValues.x, quatValues.y, quatValues.z, quatValues.w);
	
		// multiply the body rotation by the Rift rotation.
		bodyQuat.multiply(riftQuat);
	
	
		// Make a vector pointing along the Z axis and rotate it 
		// according to the combined look+body angle.
	  	var xzVector = new THREE.Vector3(0, 0, 1);
	  	xzVector.applyQuaternion(bodyQuat);
	
	  	// Compute the X/Z angle based on the combined look/body angle.
	  	viewAngle = Math.atan2(xzVector.z, xzVector.x) + Math.PI;
	
	  	// Update the camera so it matches the current view orientation
	  	camera.quaternion.copy(bodyQuat);
	}
	
## Handling Movement

Once you know the body angle, and view angle, you can move the player around in your virtual space quite easily.  The following example assumes `bodyPosition` is a `THREE.Vector3` and that the current keyboard states are stored in an array called `keys`.  

This example function also uses a time delta value to scale the movement and rotation increments, which is a very standard way to ensure smooth motion in interative applications like this.


	function updateInput(deltaTime) {
	
		var step        = 25 * deltaTime; // 25 units per second
		var turn_speed  = (55 * deltaTime) * Math.PI / 180; // 55 degrees per second
		
		// Forward/backward
		
		if(keys[FORWARD]){
			bodyPosition.x += Math.cos(viewAngle) * step;
			bodyPosition.z += Math.sin(viewAngle) * step;
		}
		if(keys[BACKWARD]){
			bodyPosition.x -= Math.cos(viewAngle) * step;
			bodyPosition.z -= Math.sin(viewAngle) * step;
		}
		
		// Turn - all you have to do is change bodyAngle
		
		if(keys[TURN_RIGHT]){ // E
			bodyAngle += turn_speed;
		}   
		
		if(keys[TURN_LEFT]){ // Q
			bodyAngle -= turn_speed;
		}
		
		// Straif - move the player 90 degrees offset to the current view angle.
		
		if(keys[STRAIF_LEFT]){
			bodyPosition.x -= Math.cos(viewAngle + Math.PI/2) * step;
			bodyPosition.z -= Math.sin(viewAngle + Math.PI/2) * step;
		}   
		
		if(keys[STRAIF_RIGHT]){
			bodyPosition.x += Math.cos(viewAngle+Math.PI/2) * step;
			bodyPosition.z += Math.sin(viewAngle+Math.PI/2) * step;
		} 
	}