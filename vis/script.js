// Extend the prototype
$(document).ready(function() {

var proto = $.circleProgress.defaults,
    superInitWidget = proto.initWidget,
    superDrawFrame = proto.drawFrame;

$.extend(proto, {
    dotRadius: 10,
    
    initWidget: function() {
        superInitWidget.call(this);
        this.dotOffset = this.dotRadius - this.getThickness() / 2;
        this.radius -= this.dotOffset;
    },
    
    drawFrame: function(v) {
        this.ctx.save();
        this.ctx.clearRect(0, 0, this.size, this.size);
        this.ctx.translate(this.dotOffset, this.dotOffset);
        superDrawFrame.call(this, v);
        this.drawDot();
        this.ctx.restore();
    },
    
    drawDot: function() {
        var ctx = this.ctx,
            sa = this.startAngle,
            r = this.radius,
            rd = r - this.getThickness() / 2,
            x = r + rd * Math.cos(sa),
            y = r + rd * Math.sin(sa);

        ctx.save();
        ctx.fillStyle = this.arcFill;
        ctx.beginPath();
        ctx.arc(x, y, this.dotRadius, 0, 2 * Math.PI);
        //ctx.fill();
        ctx.restore();
    }
});
    
});

$('#circle').circleProgress({
	dotRadius: 7, // new attribute
	value: 0,
	size: 235,
	thickness: 15,
	startAngle: -Math.PI / 2,
	lineCap: 'butt',
	fill: {
		gradient: ['#f31959', '#2667a5']
	}
});

var socket = io.connect('http://livinglab.powerprojects.se:5000');
var number_of_people = $('#people');
var power_usage = $('#keywords');
var key;
var counter = 0;

socket.on('connect', function() {
	
	socket.emit('subscribe',{topic: "app/powerUsage/#"});
	socket.emit('subscribe',{topic: "app/numPeople/#"});
		
	socket.on('mqtt', function (msg) {
		
		if (msg.topic == "app/powerUsage") {
			var obj = JSON.parse(msg.payload);
			key = Math.round(obj.value*50000);
			switch(counter){
				case 0:
					$('#keywords').text(text[counter] /*+ Math.round(key/5)*/ + ' W');
					break;
				case 1:
					$('#keywords').text(text[counter] /*+ key*/ + ' W');
					break;
				case 2:
					$('#keywords').text(text[counter] /*+ key*/ + ' W');
					break;
				case 3:
					$('#keywords').text(text[counter] /*+ Math.round(key/8)*/ + ' W');
					break;		
				case 4:
					$('#keywords').text(text[counter] + key + ' W');
					break;	
			}
			//power_usage.text(Math.round(obj.value*50000) + ' W');
			$('#circle').circleProgress('value', obj.value);			
		}
		else if(msg.topic == "app/numPeople") {
			var obj = JSON.parse(msg.payload);
			number_of_people.text('Antal personer: ' + obj.value);
		}
		else
			console.log("Received msg from topic: " +msg);
	});
});

var text = ["El/person: ", "Närvaroel: ", "Frånvaroel: ", "El/arbetstimma: ", "Elförbrukning: "];

setInterval(change, 5000);
function change() {
	switch(counter){
		case 0:
			$('#keywords').text(text[counter] /*+ key/5*/ + ' W');
			break;
		case 1:
			$('#keywords').text(text[counter] /*+ key*/ + ' W');
			break;
		case 2:
			$('#keywords').text(text[counter] /*+ key*/ + ' W');
			break;
		case 3:
			$('#keywords').text(text[counter] /*+ key/8*/ + ' W');
			break;		
		case 4:
			$('#keywords').text(text[counter] + key + ' W');
			break;	
	}
	counter++;
	if(counter >= text.length) { counter = 0; }
}