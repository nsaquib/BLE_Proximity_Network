var config = {
	timeline:{
		timer:null,
		width:1200,
		height:400,
		margin:20,
		xScale:d3.scale.linear().domain([0,3500]).range([20,800]),
		barWidth:10,
		sliderWidth:20
	}
}
var global = {
	data:null
}

function passData(select_object) {
	child = select_object.options[select_object.selectedIndex].value;
	console.log(child);
	if (child == 'dash') {
		queue()
		.defer(d3.json, "data/dash.json")
		.await(dataDidLoad);
	}
	else if (child == 'uriah') {
		queue()
		.defer(d3.json, "data/uriah.json")
		.await(dataDidLoad);
	}
	else if (child == 'dario') {
		queue()
		.defer(d3.json, "data/dario.json")
		.await(dataDidLoad);
	}
	else if (child == 'beatrice') {
		queue()
		.defer(d3.json, "data/wildflower_2/json/beatrice.json")
		.await(dataDidLoad);
	}
}

var heatmapInstance = h337.create({
    container: document.getElementById('heatMap'),
	backgroundColor: "none",

	 radius:50,
	 maxOpacity: .7,
	 minOpacity: .05
  });

function dataDidLoad(error,data1) {
	//make new data object and add different files(paths) to it
	$("#timeline-controls .stop").hide()
	initTimeLine()
	
	//var allInterval = data1.values.length-1
	
	global.data =data1
	allData = {}
	allInterval  = 500
	allInOneInterval = formatData(data1,allData,allInterval)
	console.log(allInOneInterval[0])
	//allInOneInterval = formatData(data3,allData,data3.values.length-1)
	//console.log(allData)
	heatmapInstance.setData(allInOneInterval[0]);

	var newData = {}
	//interval is preset but also changes with timeline dragging?
	var interval = 400
//	var start = 0
//	var end = 1800
//		data =formatData(data1,newData,interval)
//		data =formatData(data2,newData,interval)
//		data =formatData(data3,newData,interval)
	animationData =formatData(data1,newData,interval)
	animate(animationData)
}

var utils = {
range: function(start, end) {
	var data = []

	for (var i = start; i < end; i++) {
		data.push(i)
	}

	return data
}
}
var table = {
	group: function(rows, fields) {
		var view = {}
		var pointer = null

		for(var i in rows) {
			var row = rows[i]

			pointer = view
			for(var j = 0; j < fields.length; j++) {
				var field = fields[j]

				if(!pointer[row[field]]) {
					if(j == fields.length - 1) {
						pointer[row[field]] = []
					} else {
						pointer[row[field]] = {}
					}
				}

				pointer = pointer[row[field]]
			}

			pointer.push(row)
		}

		return view
	},

	filter: function(view, callback) {
		var data = []

		for(var i in view) {
			var list = view[i]
			if(callback(list, i)) {
				data = data.concat(list)
			}
		}

		return data
	}
}
Object.size = function(obj) {
    var size = 0, key;
    for (key in obj) {
        if (obj.hasOwnProperty(key)) size++;
    }
    return size;
};

function formatData(data, newData, interval){
	//console.log(data)
	var interval = interval
	var totalLength = Object.size(data)
	//console.log(totalLength)
	//var data = table.group(data,["frame"])
	//console.log(data)
	var intervalFrame = 0
	var xScale = d3.scale.linear().domain([0,7000]).range([50,1800])
	var yScale = d3.scale.linear().domain([0,1100]).range([30,230])
	for(var frame = 1; frame+interval < totalLength; frame++){

		newData[frame-1]={}
		newData[frame-1]["data"]=[]
		
		
		for(var i =0; i<interval; i++){
			var frameSize = data[frame+i].length
			for(var cordSet = 0; cordSet < frameSize; cordSet++){
				var currentFrame = parseInt(frame)+i
				newData[frame-1]["data"].push({"x": xScale(data[currentFrame][cordSet][0]),"y": yScale(data[currentFrame][cordSet][1])})
			}
		}

	}
	return newData
}

function objectSize(obj) {
    var size = 0, key;
    for (key in obj) {
        if (obj.hasOwnProperty(key)) size++;
    }
    return size;
};

function timelineControlStop() {
	$("#timeline-controls .play").show()
	$("#timeline-controls .stop").hide()
	clearInterval(config.timer)
}

function initTimeLine(){
	var height = config.timeline.height-120
	var width = config.timeline.width-110
	var margin =  config.timeline.margin
	
	var timeline = d3.select("#timeline").append("svg").attr("width",width)
	var xScale = config.timeline.xScale
	
	var xAxis = d3.svg.axis().scale(xScale)//.tickSize(1)//.ticks(16).tickFormat(d3.format("d"))

	timeline.append("g")
		//.attr("transform", "translate(0," + (height) + ")")
		//}.call(xAxis);
	
	var timelineBar = timeline
		.append("rect")
		.attr("class", "timeline-item")
	    .attr("x", 20)
		.attr("y", 20)
		.attr("width",width)
		.attr("height", 20)
		.attr("fill","#eee")
		
	var slider = timeline.append("rect")
		.attr("class", "slider")
		.attr("x", 20)
		.attr("y", 20)
		.attr("width", width)
		.attr("height", 20)
		.attr("fill", "#000")
		.attr("opacity", 0.3)
		.call(d3.behavior.drag()
			.on("dragstart", function() {
				d3.event.sourceEvent.stopPropagation();
				d3.select(this).property("drag-offset-x", d3.event.sourceEvent.x - this.getBoundingClientRect().left)
			})
			.on("drag", function(d, e) {
				timelineControlStop()

				var x = d3.event.x - d3.select(this).property("drag-offset-x")
				var w = parseFloat(d3.select(this).attr("width"))

				if(x <= 20) {
					x = 20
				}

				if((x + w) >= width) {
					x = width - w
				}
				d3.select(this).attr("x", x)
				updateHandleLocations()
				//TODO:updateMaps()
				updateHeatmap()
			})
		)
	var leftHandle = timeline.append("rect")
		.attr("class", "handle-left")
		.attr("x", 20)
		.attr("y", 20)
		.attr("width", 10)
		.attr("height", 20)
		.attr("fill", "#444")
		.attr("opacity", 0.3)
		.call(d3.behavior.drag()
			.on("dragstart", function() {
				d3.event.sourceEvent.stopPropagation();
			})
			.on("drag", function() {
				timelineControlStop()

				var x = d3.event.x - (d3.select(this).attr("width") / 2)
				if(x <= 20) {
					x = 20
				}
				if(x >= rightHandlePosition()) {
					x = rightHandlePosition()
				}
				d3.select(this).attr("x", x)
				updateSliderLocation()
				//TODO:updateMaps()
				updateHeatmap()
			})
		)
	var rightHandle = timeline.append("rect")
		.attr("class", "handle-right")
		.attr("x", width-10)
		.attr("y", 20)
		.attr("width", 10)
		.attr("height", 20)
		.attr("fill", "#444")
		.attr("opacity", 0.3)
		.call(d3.behavior.drag()
			.on("dragstart", function() {
				d3.event.sourceEvent.stopPropagation();
			})
			.on("drag", function() {
				timelineControlStop()

				var x = d3.event.x - (d3.select(this).attr("width") / 2)

				if(x <= leftHandlePosition()) {
					x = leftHandlePosition()
				}

				if(x >= width) {
					x = width
				}

				d3.select(this).attr("x", x)
				updateSliderLocation()
				//TODO: updateMaps()
				heatmapInstance.repaint()
				updateHeatmap()
			})
		)
		
}
function leftHandlePosition() {
	return parseFloat(d3.select("#timeline").select(".handle-left").attr("x"))
}
function rightHandlePosition() {
	return parseFloat(d3.select("#timeline").select(".handle-right").attr("x"))
}
function updateSliderLocation() {
	var startX = leftHandlePosition()
	var endX = rightHandlePosition()
	var slider = d3.select("#timeline .slider")
	slider.attr("width", endX - startX)
	slider.attr("x", startX)
	
}
function updateHandleLocations() {
	var leftHandle = d3.select("#timeline .handle-left")
	var rightHandle = d3.select("#timeline .handle-right")

	var slider = d3.select("#timeline .slider")
	var startX = parseFloat(slider.attr("x")) - config.timeline.barWidth
	var endX = parseFloat(slider.attr("x")) + parseFloat(slider.attr("width"))

	leftHandle.attr("x", startX)
	rightHandle.attr("x", endX)
}
function updateSliderRange(startYear, endYear) {
	var xScale = config.timeline.xScale

	var startX = xScale(startYear)
	var endX = xScale(endYear)

	var slider = d3.select("#timeline .slider")
	slider.attr("width", endX - startX)
	slider.attr("x", startX)

	updateHandleLocations()
}
//var  animationAtFrame = 0
function animate(data) {
	var size = objectSize(data);
	$("#timeline-controls .play").click(function() {
		$("#timeline-controls .play").hide()
		$("#timeline-controls .stop").show()
		
		var animationAtFrame = Math.floor(config.timeline.xScale.invert(leftHandlePosition()))
		var sliderRange = 40
		config.timer = setInterval(function(){
			 
			 
			 updateSliderRange(animationAtFrame, animationAtFrame+sliderRange)
			 heatmapInstance.setData(data[animationAtFrame]);
			 animationAtFrame +=1
			 // updateHeatmap()
			 if(animationAtFrame==size-1){
			 	console.log("stop")
				 timelineControlStop()
			 }
		 }, 20);
		 
		 if(animationAtFrame == size){
			 timelineControlStop
		 }
  $("#timeline-controls .stop").click(timelineControlStop)
})
}
function updateHeatmap(){
	var xScale = config.timeline.xScale
	var startFrame = Math.floor(xScale.invert(leftHandlePosition()))
	var endFrame = Math.floor(xScale.invert(rightHandlePosition()))
	var data = global.data
	var currentData = table.filter(table.group(data,["frame"]),function(list,frame){
		//console.log([frame,startFrame,endFrame])
		frame = parseInt(frame)
		return(frame >= startFrame && frame <=endFrame)
	})
	var newData = {}
	var interval = endFrame - startFrame
	var currentData = formatData(currentData,newData,interval)
	heatmapInstance.setData(currentData[0]);
}
