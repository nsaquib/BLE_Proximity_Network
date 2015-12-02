
//TODO:

$(function() {
	// Window has loaded
	queue()
	//	.defer(d3.json, "static/geojson/lines.geojson")
		//.defer(d3.json, "static/geojson/stations.geojson")
		.defer(d3.json, data)
		.defer(d3.json, blocks)
		.defer(d3.json, water)
		.defer(d3.json, river)
	.await(dataDidLoad);
})

function dataDidLoad(error,data,blocks,water,river) {
	//console.log(line)	
	//console.log(stops)
	console.log(data)
	
	var svg = d3.select("#subway")
		.append("svg")
		.attr("width",mapWidth)
		.attr("height",mapHeight); 
	drawWater(water,svg,"none","#333","water")
//	drawWater(river,svg,"none","#000","river")
	//drawLineGraph("RED",data)
	d3.selectAll(".rolloverpath").attr("opacity",1)
	formatSubwayStopsByLine(stations,data,blocks,svg)
	drawLineGraph(initialLineToDraw,data)
	//drawAllBlocks(blocks,svg)
	
//	var lineColor = "red"
//	drawLineGraph(lineColor,stops_steve)
}
function capitalCase(string){
	string = string.toLowerCase()
	string = string.replace(/\w\S*/g, function(txt){return txt.charAt(0).toUpperCase() + txt.substr(1).toLowerCase();});
	string = string.replace(/'([a-z])/g, function (g) { return "'"+g[1].toUpperCase(); });
	string = string.replace("/", "-")
	string = string.replace(/-([a-z])/g, function (g) { return "-"+g[1].toUpperCase(); });
	
	return string
}
function undoHighlight(station){
	//console.log("undo "+station)
	d3.select("#chart-title").html("")
	d3.selectAll(".rollovers"+stripSpecialCharactersAndSpace(station)).attr("opacity",0)
}

function formatDetailedData(station,data){
	var blockgroups = data.stations[station].blockgroups
	var stationData = calculateIncomeData(blockgroups)
	var income = "$"+stationData[2]
	var quantity = stationData[0]
	var min = "$"+stationData[3]
	var max = "$"+stationData[4]
	if(income == "$0"){
		income = "Not Enough Data"
		max = "Not Enough Data"
		min = "Not Enough Data"
	}
	return "<strong>"+capitalCase(station).replace(wordToReplace, "")+"</strong><br/>"+quantity+" Blockgroups in 0.5 Mile Radius"+"<br/>Average Median Household Income: "+income+"<br/>Min Median Household Income: "+min+"<br/>Max Median Household Income: "+max
}

function drawWater(water,svg,fill,stroke,waterClass){
	var projection = d3.geo.mercator()
		.scale(scale)
		.center(center)
		.translate(translate)
	var path = d3.geo.path()
		.projection(projection)
	svg.selectAll("path")
        .data(water.features)
        .enter()
        .append("path")
		.attr("class",waterClass)
		.attr("d",path)
		.attr("stroke", stroke)
		.style("fill", fill)
	    .style("stroke-width", 1)
	    .style("opacity",.2)
}
function drawAllBlocks(blocks,svg){
	var projection = d3.geo.mercator()
		.scale(scale)
		.center(center)
		.translate(translate)
	//	console.log(currentBlocks)
	var path = d3.geo.path()
		.projection(projection)
	var svg = svg
	svg.selectAll("path")
        .data(blocks.features)
        .enter()
        .append("path")
		.attr("class","blockBoundaries")
		.attr("d",path)
		.attr("stroke", "#aaa")
		.style("fill", "none")
	    .style("stroke-width", .5)
	    .style("opacity",.5)
}
function drawBlocks(blocks,currentCoordinates,data){
	var stationName = currentCoordinates[0]
	var currentData = data.stations[stationName]["blockgroups"]
	var currentBlocks = []
	//console.log(blocks)
	d3.selectAll("#subway .blockBoundaries").remove()
	for(var i in currentData){
		currentBlock = currentData[i][0]
		currentBlocks.push(currentBlock)
	}
	var projection = d3.geo.mercator()
		.scale(scale)
		.center(center)
		.translate(translate)
	//	console.log(currentBlocks)
		
	var path = d3.geo.path()
		.projection(projection)
	var svg = d3.select("#subway svg")
	svg.selectAll("path")
        .data(blocks.features)
        .enter()
        .append("path")
		.attr("class","blockBoundaries")
		.attr("d",path)
		.attr("stroke", "#000")
		.style("fill", "none")
	    .style("stroke-width", .5)
	    .style("opacity", function(d){
			//console.log(d.properties.LINE);
			if(currentBlocks.indexOf(d["GEO_ID"])>-1){
				return .5
			}else{
				return .1			
			}
		})
}

function highlightCurrentStation(station,data){
	//console.log(station)
	d3.selectAll(".rollovers"+stripSpecialCharactersAndSpace(station)).attr("opacity",.4)	
	d3.select("#chart-title").html(formatDetailedData(station,data))

}
function stripSpecialCharactersAndSpace(inputString){
	var newString = inputString.replace(/[\.,-\/#!$%\^&\*;:{}=\-_`~()]/g,"")
	newString = newString.replace(/\s/g, '')
	newString = newString.replace(/['"]+/g, '')
	//console.log("class: "+newString)
	return newString
}
function calculateIncomeData(blockgroups){
	var sum = 0
	var populationTotal = 0
	var max = 0
	var min = 100000000000000
	for(var i in blockgroups){
		var blockgroup = blockgroups[i]
		//console.log(blockgroup)
		var income = parseFloat(blockgroup[1]["SE_T057_001"])
		if(income > max){
			max = income
		}
		if (income < min){
			min = income
		}
		var population = parseFloat(blockgroup[1]["SE_T001_001"])
		if(isNaN(population)){
			population = 0
		}
		
		var populationProportion = parseFloat(blockgroup[1]["pop_proportion"])
		
		if(isNaN(income)){
			income = 0
			population = 0
		}
		if(min == 100000000000000){
			min = 0
		}
		income = income*(population*populationProportion)
		sum +=income
		populationTotal = populationTotal+population*populationProportion
		var averageIncome = sum/populationTotal
		if(sum == 0){
			averageIncome =0
			max = 0
			min = 0
			populationTotal = 0
		}else{
			max = parseInt(max)
			min = parseInt(min)
			averageIncome = parseInt(averageIncome)
			populationTotal = parseInt(populationTotal)
		}
	}
	console.log(["income data function",blockgroups.length,populationTotal,sum/populationTotal,min,max])
	
	return [blockgroups.length,populationTotal,averageIncome,min,max]
}
/*function calculateLineWideAverage(graphData){
	
}*/
function calculateAverages(data){
	//console.log(data.lines)
	//console.log(data.stations["NAYLOR ROAD METRO STATION"])
	var incomeAverages = []
	//console.log(data.lines["WIL"])
	var allLinesIncomeSum = 0
	var allLinesPopSum = 0
	for(var line in data.lines){
		//console.log(data.lines[line])
		var lineName = data.lines[line]["line_name"]
		var lineIncomeSum = 0
		var linePopSum = 0
		var lineData = data.lines[line]
		var stations = lineData.stations
		for (var station in stations){
			var currentStation = stations[station][0]
			var blockGroups = data.stations[currentStation].blockgroups
			//console.log(blockGroups)
			for(var blockGroup in blockGroups){
				var currentBlockGroup = blockGroups[blockGroup][1]
				var population = parseFloat(currentBlockGroup["SE_T001_001"])
				var income = parseFloat(currentBlockGroup["SE_T057_001"])
				if (isNaN(income)){
					income = 0
					population = 1
				}
				var proportion = parseFloat(currentBlockGroup["pop_proportion"])
				
				var weightedPopulation = population*proportion
				var incomeTotal = income*weightedPopulation
				lineIncomeSum = lineIncomeSum+incomeTotal
				linePopSum = linePopSum+weightedPopulation
				lineAverage = lineIncomeSum/linePopSum
			}
		}
		incomeAverages.push([lineName,lineAverage,linePopSum])
		allLinesIncomeSum +=lineIncomeSum
		allLinesPopSum +=linePopSum
		allLinesAverage = allLinesIncomeSum/allLinesPopSum
		
	}
	incomeAverages.push(["all",allLinesAverage,allLinesIncomeSum])
	//console.log(incomeAverages)
	return incomeAverages
}
function calculateDistance(origin,coordinates) {
	var lat1 = origin[1]
	var lon1 = origin[0]
	var lat2 = coordinates[1]
	var lon2 = coordinates[0]
  var R = 6371; // Radius of the earth in km
  var dLat = deg2rad(lat2-lat1);  // deg2rad below
  var dLon = deg2rad(lon2-lon1); 
  var a = 
    Math.sin(dLat/2) * Math.sin(dLat/2) +
    Math.cos(deg2rad(lat1)) * Math.cos(deg2rad(lat2)) * 
    Math.sin(dLon/2) * Math.sin(dLon/2)
    ; 
  var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a)); 
  var d = R * c; // Distance in km
  return d;
}
function deg2rad(deg) {
  return deg * (Math.PI/180)
}

function formatLineData(lineColor,data){
	var currentData = data.lines[lineColor]["stations"]
	//console.log(lineColor)
	var orderedStations = data.lines[lineColor]["primary_routes"][0]
	//console.log([lineColor,orderedStations])
	var stationList = []
	var cummulativeDistance = 0
	//for each station, get groups and average data
	for(var station in orderedStations){
		var currentStation = orderedStations[station]
		var originStation = orderedStations[0]
		//console.log([originStation,currentStation])
		var blockgroups = data.stations[currentStation]["blockgroups"]
		//console.log(blockgroups)
		var coordinates = data.stations[currentStation]["coordinates"]
		if(station == 0){
			var origin = data.stations[originStation]["coordinates"]
		}else{
			var origin = data.stations[orderedStations[station-1]]["coordinates"]
		}
		var incomeData = calculateIncomeData(blockgroups)
		var income = incomeData[2]
		var minIncome = incomeData[3]
		var maxIncome = incomeData[4]
		var distance = calculateDistance(origin,coordinates)
		//added to space out the chart and labels
		
		if(distance < 0.8){
			distance = 0.8
		}
		cummulativeDistance = cummulativeDistance+distance
		//console.log([distance,cummulativeDistance])
		stationList.push([currentStation,income,cummulativeDistance,minIncome,maxIncome])
		
	}
	
	return stationList
}

function drawLineGraph(lineColor,data){
	var averages = calculateAverages(data)
	var graphData = formatLineData(lineColor,data)
	//console.log(averages)
	var color = lineColor
	//console.log(graphData[graphData.length-1])
	d3.select("#charts svg").remove()
	var margin = {top: 20, right: 20, bottom: 140, left: 50},
	    width = 780 - margin.left - margin.right,
	    height = 600 - margin.top - margin.bottom;
	var chartSvg = d3.select("#charts")
		.append("svg")
		.attr("width",width)
		.attr("height",height)
		
	var maxDistance = graphData[graphData.length-1][2]
	var maxIncomes = []
		for(var i in graphData){
			maxIncomes.push(graphData[i][4])
		}
	var maxIncome = Math.max.apply(null, maxIncomes);
	//	console.log(incomes)
	//	console.log(maxIncome)
	//var incomeScale = d3.scale.linear().domain([0,maxIncome]).range([height- margin.top,margin.bottom])
	var incomeScale = d3.scale.linear().domain([0,260000]).range([height- margin.top,margin.bottom])
	var coordinateScale = d3.scale.linear().domain([-.5,maxDistance]).range([margin.left,width - margin.right])	
	
	for(var i in averages){
		var averagesLineColor = averages[i][0]
		if(lineNameToLine[averagesLineColor] == lineColor){
			var averageIncomeLineColor = averages[i][1]
			//console.log(colorDictionary[lineNameToLine[lineColor]])
		}
	}
	
	var lineName = data.lines[lineColor]["line_name"].replace("Line", "")
	
	d3.select("#line-title").html("<strong>"+capitalCase(lineName)+" Line </strong><br/> Average Median Household Income: $"+parseInt(averageIncomeLineColor))
	chartSvg.append("text")
	.text("Weighted Line Average: $"+parseInt(averageIncomeLineColor))
	.attr("x",width)
	.attr("y",incomeScale(averageIncomeLineColor)-margin.bottom-3)
	.attr("text-anchor", "end")
	.attr("fill", colorDictionary[lineColor])
	//draw averages
	chartSvg.selectAll("rect")
	.data(averages)
	.enter()
	.append("rect")
	.attr("x",function(d){
		return margin.left-8
	})
	.attr("y",function(d){
		return incomeScale(d[1])-margin.bottom
	})
	.attr("width",width)
	.attr("height",2)
	.attr("fill",function(d){
		//return colorDictionary[d[0]]
		return colorDictionary[lineNameToLine[d[0]]]
	})
	.attr("opacity",0.3)
	//.on("mouseover",function(d){
		//console.log(["line average",d])
	//})
	for(var i in graphData){
		chartSvg.append("text")
			.attr("id","stationLabel")
			.text(function(d){
				return capitalCase(graphData[i][0]).replace(wordToReplace, "").replace(wordToReplace2, "")
			})
			.attr("x", function(d){
				return coordinateScale(graphData[i][2])
			})
			.attr("y", function(d,i){
				return height-margin.bottom-margin.top
			})
			.attr("dy", 5)
			.style("text-anchor","start")
	}
	//console.log(graphData)
	//chartSvg.selectAll("text")
	//	.data(graphData)
	//	.enter()
	//	.append("text")
	//	.attr("id","stationLabel")
	//	.attr("class", function(d){
	//		return stripSpecialCharactersAndSpace(d[0])
	//	})
	//	.attr("x", function(d){
	//		return coordinateScale(d[2])
	//	})
	//	.attr("y", function(d,i){
	//		return height-margin.bottom-margin.top
	//	})
	//	.attr("dy", 5)
	//	.text(function(d){
	//			return capitalCase(d[0]).replace("Metro Station", "")
	//	})
	//	.style("text-anchor","start")
	//	
//	chartSvg.append("text")
//	.text(graphData[0][0])
//	.attr("x",function(d){coordinateScale((graphData[0][2]))})
//	.attr("y",height-margin.bottom-margin.top)
//	.attr("fill", "#aaa")	
//	.attr("id","stationLabel")

	chartSvg.append("rect")
		.attr("y",height-margin.bottom-20)
		.attr("x",margin.left)
		.attr("width",width-margin.left-margin.right)
		.attr("height",1)
		.attr("fill","#aaa")
	
	var yAxis = d3.svg.axis()
	    .scale(incomeScale)
	    .orient("left")
		.tickValues([50000,100000,150000,200000,250000]);	
	chartSvg.append("g")
        .attr("class", "y axis")
        .call(yAxis)
		.attr("fill","#aaa")
        .attr("transform", "translate("+margin.left+","+-margin.bottom+")")
      chartSvg
		.append("text")
        .attr("x",0)
		.attr("y", -55)
		.attr("transform","rotate(90)")
        .style("text-anchor", "start")
        .text("Median Household Income ($)")
		.attr("fill","#aaa");
	//line graph 		
	var line = d3.svg.line()
		.x(function(d){
			return coordinateScale(d[2])
		})
		.y(function(d){
			var income = d[1]
			if(isNaN(income)){
				income = 0
			}
			return incomeScale(income)-margin.bottom
		})
	chartSvg.append("path")		
		.datum(graphData)
		.attr("class",color)
		.attr("d",line)
		.attr("fill","none")
		.style("stroke", colorDictionary[color])
		.style("stroke-width", 2)
		.style("opacity",1)
	//average line	
	//chartSvg.append("rect")
	//	.attr("class", "lineAverage")
	//	.attr("x", margin.left)
	//	.attr("y", incomeScale(lineAverage)-margin.bottom)
	//	.attr("width",width-margin.left)
	//	.attr("height",1)
	//	.attr("fill","#888")
	//	.style("opacity",.5)
	//chartSvg.append("text")
	//	.text(color+" Line Average")
	//	.attr("class", "lineAverage")
	//	.attr("x", width)
	//	.attr("y",incomeScale(lineAverage)-4-margin.bottom)
	//	.attr("text-anchor", "end")
	//	.attr("fill","#888")
	//	.style("opacity",.5)
	//chartSvg.append("text")
	//	.text("$"+ parseFloat(lineAverage).toFixed(2))
	//	.attr("class", "lineAverage")
	//	.attr("x", width)
	//	.attr("y",incomeScale(lineAverage)+12-margin.bottom)
	//	.attr("text-anchor", "end")
	//	.attr("fill","#888")
	//	.style("opacity",.5)	
	// circle average
	//min mark	
	chartSvg.selectAll("rect min")
		.data(graphData)
		.enter()
		.append("rect")
		.attr("class", "min")
		.attr("x", function(d){
			return coordinateScale(d[2])-4
		})
		.attr("y", function(d){
			return incomeScale(d[3])-margin.bottom
		})
		.attr("width", 8)
		.attr("height",1)
		.attr("fill",colorDictionary[color])
		.attr("opacity",.5)
		
	chartSvg.selectAll("rect max")
		.data(graphData)
		.enter()
		.append("rect")
		.attr("class", "max")
		.attr("x", function(d){
			return coordinateScale(d[2])-4
		})
		.attr("y", function(d){
			return incomeScale(d[4])-margin.bottom
		})
		.attr("width", 8)
		.attr("height",1)
		.attr("fill",colorDictionary[color])
		.attr("opacity",.5)
		
	chartSvg.selectAll("rect range")
		.data(graphData)
		.enter()
		.append("rect")
		.attr("class", "range")
		.attr("x", function(d){
			return coordinateScale(d[2])-.5
		})
		.attr("y", function(d){
			return incomeScale(d[4])-margin.bottom
		})
		.attr("width",1)
		.attr("height",function(d){
			return incomeScale(d[3])-incomeScale(d[4])
		})
		.attr("fill",colorDictionary[color])
		.attr("opacity",.5)
		
	  var tip = d3.tip()
	    .attr('class', 'chart-tip')
	    .offset([-4, 0])
	 chartSvg.call(tip)
			
	chartSvg.selectAll("circle average")
		.data(graphData)
		.enter()
		.append("circle")
		.attr("class", "average")
		.attr("class",function(d){
			return stripSpecialCharactersAndSpace(d[0])
		})
		.attr("cx", function(d){
			return coordinateScale(d[2])
		})
		.attr("cy", function(d){
			return incomeScale(d[1])-margin.bottom
		})
		.attr("r", 3)
		.attr("fill",colorDictionary[color])
	console.log(graphData)	
	chartSvg.selectAll("circle average")
		.data(graphData)
		.enter()
		.append("circle")
		//.attr("class", "rollovers")
		.attr("class",function(d){
			return "rollovers"+stripSpecialCharactersAndSpace(d[0])
		})
		.attr("cx", function(d){
			return coordinateScale(d[2])
		})
		.attr("cy", function(d){
			return incomeScale(d[1])-margin.bottom
		})
		.attr("r",8)
		.attr("fill",colorDictionary[color])
		.attr("opacity",0)	
		.on("mouseover",function(d){
			var tipText = "$"+ parseInt(d[1])
			tip.html(tipText)
			tip.show()
			d3.select(this).attr("opacity",.5)
			//console.log(d[0])
			highlightCurrentStation(d[0],data)
		})
		.on("mouseout", function(d){
			tip.hide()
			d3.select(this).attr("opacity",0)
			undoHighlight(d[0])
		})
	
}

function buildStationLineDictionary(data){
	//console.log(data.lines)
	var stationLineDictionary = {}
	for(var line in data.lines){
		var currentLine = data.lines[line]
		//console.log(currentLine.stations)
		for(var station in currentLine.stations){
			var currentStation = currentLine.stations[station]
			var stationName = currentStation[0]
			var stationRoutes = currentStation[1]["ROUTE"]
			stationLineDictionary[stationName]=stationRoutes
		}
	}
	return stationLineDictionary
}
function formatSubwayStopsByLine(stops,data,blocks,svg){
	var stationLineDictionary = buildStationLineDictionary(data)	
	var projection = d3.geo.mercator()
		.scale(scale)
		.center(center)
		.translate(translate)
	
	var stationData = data.lines
	var stationLocationData = data.stations
	var index = 0
	//console.log(stationLocationData)
	for(var route in stationData){
		index +=1
		var offset = offsetDictionary[route]
		//console.log(stationData[route])
		var currentRoute = stationData[route]["primary_routes"][0]
		//console.log(currentRoute)
		var color = route
		drawSubwayLines(currentRoute,data,svg,color,offset)

		for(var station in currentRoute){
			var currentStation = currentRoute[station]
			var currentStationCoordinates = [currentStation,data.stations[currentStation].coordinates]
			//console.log(data.lines)
			var routes = stationLineDictionary[currentStation].split(",")
			for(var route in routes){
				var fill = colorDictionary[routes[route].replace(/\s+/g, '')]
				var radius = 3
				//console.log([radius,fill])
				drawSubwayStops(blocks,currentStationCoordinates,data,svg,color,radius,offset)
			}
		}
	}
	
}
function drawSubwayLines(route,data,svg,color,offset){
	//	console.log(route)
	//	console.log(data)
	var routeLine = []
	for(var station in route){
		var currentStation = route[station]
		var coordinates = data.stations[currentStation].coordinates
		//console.log(coordinates)
		routeLine.push(coordinates)
	}
	var projection = d3.geo.mercator()
		.scale(scale)
		.center(center)
		.translate(translate)
		
	var line = d3.svg.line()
	    .x(function(d) { 
			return parseInt(projection(d)[0])+offset[0]; 
		})
	    .y(function(d) { 
			//console.log(["y",projection(d)[1]]);
			return parseInt(projection(d)[1])+offset[1]; 
		});
		//var tip = d3.tip().attr('class', 'd3-tip').html(function(d) { return d;});
	//	var clicked = null;
//below is invisible path for rollovers
	svg.selectAll("path ."+color)
        .data(routeLine)
        .enter()
        .append("path")
		.attr("class","subwayLine"+" "+color)
		.attr("d",line(routeLine))
		.attr("fill","none")
		.style("stroke-width", .8)
		.style("stroke",colorDictionary[color])
		.style("stroke-opacity",0)
		.transition()
		.duration(2000)
		.style("stroke-opacity",1)

	svg.selectAll("path ."+color)
	    .data(routeLine)
	    .enter()
	    .append("path")
		.attr("class","rolloverpath")
		.attr("d",line(routeLine))
		.attr("fill","none")
		.style("stroke-width", 20)
		.style("stroke",colorDictionary[color])
		.style("opacity", 0)
		.style("stroke-linecap", "round")
		.on("click",function(d){
			var lineColor = color
			drawLineGraph(lineColor,data)
			//d3.selectAll(".selected").classed("selected",false).attr("class","rolloverpath "+color)
			//d3.selectAll(".rolloverpath").style("opacity",0)
			//d3.select(this).attr("class","selected").style("opacity",0.2)
			//d3.select("#chart-title").html("Rollover stations to see detailed data.")
		})
		.on("mouseover",function(d){
			//d3.select(this).style("opacity",0.2)
			drawLineGraph(fill,data)
		})
		.on("mouseout",function(d){
			//d3.selectAll(".rolloverpath").style("opacity",0)
			//d3.selectAll(".selected").style("opacity",.2)
		})		


}
function drawSubwayStops(blocks,currentCoordinates,data,svg,fill,radius,offset){
	var projection = d3.geo.mercator()
		.scale(scale)
		.center(center)
		.translate(translate)
	 
	svg.append("circle")
		.attr("cx", projection(currentCoordinates[1])[0]+offset[0])
		.attr("cy", projection(currentCoordinates[1])[1]+offset[1])
		.attr("r",3)
		.attr("class",function(d){
			return stripSpecialCharactersAndSpace(currentCoordinates[0])
		})
	    .style("fill",colorDictionary[fill])
		.attr("opacity", .8)
		//.attr("stroke","#fff")
		
		
	var mapTip = d3.tip()
		.attr('class', 'map-tip')
		.offset([-4, 0])
	
	svg.call(mapTip)
	svg.append("circle")
		.attr("cx", projection(currentCoordinates[1])[0]+offset[0])
		.attr("cy", projection(currentCoordinates[1])[1]+offset[1])
		.attr("r",10)
		.attr("class",function(d){
			return "rollovers"+stripSpecialCharactersAndSpace(currentCoordinates[0])
		})
	    .style("fill",colorDictionary[fill])
		.attr("opacity", 0)
		.on("mouseover", function(){
			drawLineGraph(fill,data)
			d3.select(this).attr("opacity",.2)
			//drawBlocks(blocks,currentCoordinates,data)
			currentStation = currentCoordinates[0]
			highlightCurrentStation(currentStation,data)
			var tipText = capitalCase(currentStation).replace(wordToReplace, "")
			mapTip.html(tipText)
			mapTip.show()
			//console.log(fill)
			//d3.selectAll("."+fill).attr("opacity",.2)
			
		//	var stationData = displayDataByStation(station,data)
			//d3.select("#station_rollover").html("stop:"+d.properties.STATION+"</br> line:"+d.properties.LINE+"</br>"+stationData)			
		})
		.on("mouseout",function(d){
			undoHighlight(currentStation)
			mapTip.hide()
			//d3.selectAll(".rolloverpath").attr("opacity",0)
			//d3.selectAll(".selected").style("opacity",.2)
			
			//d3.selectAll("path .rolloverpath ."+fill).attr("opacity",0)
		})
		.on("click",function(d){
		//	d3.selectAll(".selected").classed("selected",false).attr("class","rolloverpath")
			d3.selectAll(".rolloverpath").style("opacity",0)
			//drawLineGraph(fill,data)
		//	console.log(fill)
		})
}
