var renderer, camera, scene, element;
var point;
var aspectRatio, windowHalf;

var referenceCube;
var accelerationIndicator;
var oculusBridge;


function initScene() {
  clock = new THREE.Clock();
  mouse = new THREE.Vector2(0, 0);

  windowHalf = new THREE.Vector2(window.innerWidth / 2, window.innerHeight / 2);
  aspectRatio = window.innerWidth / window.innerHeight;
  
  scene = new THREE.Scene();  

  camera = new THREE.PerspectiveCamera(45, aspectRatio, 1, 10000);

  camera.position.set(120, 160, 120);
  camera.lookAt(scene.position);

  // Initialize the renderer
  renderer = new THREE.WebGLRenderer({antialias:true});
  renderer.setClearColor(0x161616);
  renderer.setSize(window.innerWidth, window.innerHeight);

  element = document.getElementById('viewport');
  element.appendChild(renderer.domElement);
}


function initLights(){
  point = new THREE.DirectionalLight( 0xffffff, 1, 0, Math.PI, 1 );
  point.position.set( -250, 250, 150 );
  
  scene.add(point);
}


function initGeometry(){
  var floorMaterial = new THREE.MeshBasicMaterial( { color:0x515151, wireframe:true, transparent:true, opacity:0.5 } );
  var floorGeometry = new THREE.PlaneGeometry(200, 200, 10, 10);
  var floor = new THREE.Mesh(floorGeometry, floorMaterial);
  floor.rotation.x = -Math.PI / 2;

  scene.add(floor);

  var material = new THREE.MeshLambertMaterial({ color: 0x29d6e1, emissive:0x297d67});
  referenceCube = new THREE.Mesh( new THREE.CubeGeometry(90, 60, 50), material);

  scene.add(referenceCube);

  createAccelerometerUI();
}


function createAccelerometerUI(){

  // build a grid for scale
  var material = new THREE.LineBasicMaterial({ color: 0x515151 });

  var geometry = new THREE.Geometry();
  for(var i = -10; i < 11; i+=2){
    geometry.vertices.push( new THREE.Vector3( i*10, 0, -100 ) );
    geometry.vertices.push( new THREE.Vector3( i*10, 30, -100 ) );
  }

  geometry.vertices.push( new THREE.Vector3( -100, 10, -100 ) );
  geometry.vertices.push( new THREE.Vector3( 100, 10, -100 ) );

  geometry.vertices.push( new THREE.Vector3( -100, 20, -100 ) );
  geometry.vertices.push( new THREE.Vector3( 100, 20, -100 ) );

  var line = new THREE.Line( geometry, material, THREE.LinePieces );
  scene.add( line );

  // add indicators for accelerometer values
  var accelerationValues = [
    new THREE.Mesh( new THREE.CubeGeometry(1,10,1), new THREE.MeshLambertMaterial({color:0xff0000, emissive:0x600000}) ),
    new THREE.Mesh( new THREE.CubeGeometry(1,10,1), new THREE.MeshLambertMaterial({color:0x00ff00, emissive:0x006000}) ),
    new THREE.Mesh( new THREE.CubeGeometry(1,10,1), new THREE.MeshLambertMaterial({color:0x0000ff, emissive:0x000060}) )
  ];

  accelerationIndicator = new THREE.Object3D();
  accelerationIndicator.position.set(0,5,-100);
  for(var i = 0; i < accelerationValues.length; i++){
    accelerationValues[i].position.set(0, i*10, 0);
    accelerationIndicator.add(accelerationValues[i]);
  }
  scene.add(accelerationIndicator);
}


function onResize() {
  windowHalf = new THREE.Vector2(window.innerWidth / 2, window.innerHeight / 2);
  aspectRatio = window.innerWidth / window.innerHeight;
 
  camera.aspect = aspectRatio;
  camera.updateProjectionMatrix();
 
  renderer.setSize(window.innerWidth, window.innerHeight);
}


function bridgeConfigUpdated(config){
  var stats = document.getElementById("stats");
  
  stats.innerHTML = "Display Configuration<hr>";

  // Show all the parameters in the config object.
  for(var itm in config){
    var row = document.createElement("div");
    var label = document.createElement("label");
    var value = document.createElement("span");

    label.innerHTML = itm;
    value.innerHTML = config[itm];
    
    row.appendChild(label);
    row.appendChild(value);
    stats.appendChild(row);
  }
}


function bridgeAccelerationUpdated(accel) {
  // scale values so 1g = 20 world units
  accelerationIndicator.children[0].position.x = (accel.x * 1.02040816326531) * 2;
  accelerationIndicator.children[1].position.x = (accel.y * 1.02040816326531) * 2;
  accelerationIndicator.children[2].position.x = (accel.z * 1.02040816326531) * 2;
}

function bridgeOrientationUpdated(quat) {
  referenceCube.quaternion.set(quat.x, quat.y, quat.z, quat.w);
 }

function bridgeConnected(){
  var stats = document.getElementById("stats");
  stats.innerHTML = "Bridge Connected!";
  document.getElementById("logo").className = "";
}

function bridgeDisconnected(){
  var stats = document.getElementById("stats");
  stats.innerHTML = "Bridge Disconnected.";
  document.getElementById("logo").className = "offline"
}


function animate() {
  requestAnimationFrame(animate);
  render();
}


function render() { 
  renderer.render(scene, camera);
}


function init(){

  window.addEventListener('resize', onResize, false);

  initScene();
  initGeometry();
  initLights();
    
  // Create the bridge object and attempt to connect.

  oculusBridge = new OculusBridge({
    onOrientationUpdate  : bridgeOrientationUpdated,
    onAccelerationUpdate : bridgeAccelerationUpdated,
    onConfigUpdate       : bridgeConfigUpdated,
    onConnect            : bridgeConnected,
    onDisconnect         : bridgeDisconnected
  });

  oculusBridge.connect();
}


window.onload = function() {
  init();
  animate();
}