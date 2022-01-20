var body = document.body;
body.style.margin = 0;

var div = document.getElementById('canvas-div');
var canvas = document.getElementById('chart');
canvas.height = div.offsetHeight;
canvas.width = div.offsetWidth;

var x_center = div.offsetWidth/2
var y_center = div.offsetWidth/2

var ctx = canvas.getContext("2d");

// sand background
var sand = new Image();
sand.src = "static/images/sand.jpg"
var pat = ctx.createPattern(sand, "repeat")
ctx.save();
ctx.beginPath();
ctx.fillStyle = pat;
ctx.arc(x_center, y_center, div.offsetWidth/2-5, 0, 2 * Math.PI);
ctx.fill();
ctx.strokeStyle = 'white';
ctx.lineWidth = 5;
ctx.stroke();




// sand leds
var leds = ctx.createRadialGradient(x_center, y_center, 0, x_center, y_center, div.offsetWidth/2+50)
leds.addColorStop(0, "rgba(255,0,255,0.1)")
leds.addColorStop(1, "rgba(255,0,255,0.3)")
ctx.fillStyle = leds;
ctx.fill();