var config = {
	timeline:{
		timer:null,
		width:1000,
		height:100,
		margin:20,
		xScale:d3.scale.linear().domain([0,4000]).range([20,980]),
		barWidth:10,
		sliderWidth:20
	}
}
var global = {
	data:null
}

$(function() {
	queue()
	.defer(d3.json, "data/heatmap_testdata.json")
	.defer(d3.json, "data/heatmap_testdata2.json")
	.defer(d3.json, "data/heatmap_testdata3.json")
	.defer(d3.json, "data/heatmap_testdata4.json")
	
	.await(dataDidLoad);
})

var heatmapInstance = h337.create({
    container: document.getElementById('heatMap'),
	backgroundColor: '#fff',
    gradient: {
       // enter n keys between 0 and 1 here
       // for gradient color customization
       '.2': '#eee',
       '.95': '#aaa',
       '1': '#000'
     },
	 radius:20,
	 maxOpacity: .9,
	 minOpacity: .1
  });

function dataDidLoad(error,data1,data2,data3,data4) {
	//make new data object and add different files(paths) to it
	$("#timeline-controls .stop").hide()
	initTimeLine()
	
	//var allInterval = data1.values.length-1
	
	var allData = {}
	var allInterval = 3000
	var joinedData = data1.values.concat(data2.values).concat(data3.values)
	
	var currentData = table.filter(table.group(joinedData,["frame"]),function(list,frame){
		frame = parseInt(frame)
		return(frame >= 0 && frame <= allInterval )
	})
	global.data = currentData
	
	
	allInOneInterval = formatData(currentData,allData,allInterval-1)
	//allInOneInterval = formatData(data3,allData,data3.values.length-1)
	//console.log(allData)
	heatmapInstance.setData(allInOneInterval[0]);

	var newData = {}
	//interval is preset but also changes with timeline dragging?
	var interval = 1000
//	var start = 0
//	var end = 1800
//		data =formatData(data1,newData,interval)
//		data =formatData(data2,newData,interval)
//		data =formatData(data3,newData,interval)
	animationData =formatData(joinedData,newData,interval)
	console.log(animationData)
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

function formatData(data, newData,interval){
	var interval = interval
	var totalLength = Object.size(data)
	//console.log(totalLength)
	//var data = table.group(data,["frame"])
	//console.log(data)
	for(var frame = 0; frame + interval < totalLength; frame++){
		
		
		if(newData[frame]==undefined){
			newData[frame] = {}
		//	console.log(newData[frame])
			newData[frame]["data"]=[]
		//	console.log(newData)			
			for(var i =1; i < interval; i++){
				var currentFrame = parseInt(frame)+i
				//console.log(data[20])
				var x = data[currentFrame].x
				var y = data[currentFrame].y
				newData[frame]["data"].push({"x":x,"y":y})
			}
		}else{
			for(var i =1; i < interval; i++){
				var currentFrame = parseInt(frame)+i
				var x = data[currentFrame][0].x
				var y = data[currentFrame][0].y
				newData[frame]["data"].push({"x":x,"y":y})
			}
		}
		
		
		//counter = {}
		//newData[frame]["data"].forEach(function(obj){
		//	var key = JSON.stringify(obj)
		//	counter[key] = (counter[key] || 0)+1
		//})
		//var outputData = []
		//outputData[0]=[]
		//outputData[0]["data"]=[]
		//for (var key in counter){
		//	var value = counter[key]
		//	var x =parseInt(key.split(",")[0].replace("[",""))
		//	var y = parseInt(key.split(",")[1].replace("]",""))
		//	outputData[0]["data"].push({"x":x,"y":y,"value":value})
		//}
		
	}
	//console.log(outputData))
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
	var height = config.timeline.height-100
	var width = config.timeline.width
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
		.attr("fill", "#E1883B")
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
		.attr("fill", "#E1883B")
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
		 }, 2);
		 
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
