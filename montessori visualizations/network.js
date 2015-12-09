var config = {
	timeline: {
		timer: null
	},
  play:false,
  graph:null,
  nodes:["0","1","2","3","4","5","6","7","8","9","10","11","13","14","15","16"]

}
var aNames = {"0":"Lyra",
"1":"Mila",
"2":"Penny",
"3":"Giorgio",
"4":"Ije",
"5":"Leo",
"6":"Yusuf",
"7":"Inaaya",
"8":"Oscar",
"9":"Gustav",
"10":"Rachel",
"11":"Jay",
"12":"Aiden",
"13":"Oliver",
"14":"Alec",
"15":"Missie (Teacher)",
"16":"Kanan (Teacher)"}
d3.json("Aster_2days.json", function(error, graphData) {
  var chordData = formatForChord(graphData)
  config.graph = new myGraph("#graph",chordData);
  drawNodes()
  //buttons (label, colorIndex, timestart, data)
  drawNetwork(0,graphData,20,graphData)
  clearInterval(config.timeline.timer)

  var timeIntervals={"dropoff":0,"dispersion":990,"encounters":2900,"small groups":3630}
  for(var i in timeIntervals){
    drawButton("button",i,2,timeIntervals[i],graphData)
  }    
  playStopButton(graphData)
})

function playStopButton(graphData){
  var playButton = d3.select("#play").append("svg").attr("width",80).attr("height",30)
  playButton.append("rect").attr("x",0).attr("y",0).attr("height",24).attr("width",50)
  .attr("fill","#fff")
  .attr("stroke","#aaa")
  playButton.append("text").attr("x",10).attr("y",16).text("play").attr("fill","#aaa") 
  
  playButton.on("click",function(){
    if(config.play == false){
      console.log("false")
      playButton.selectAll("text").remove()
      playButton.append("text").attr("x",10).attr("y",16).text("stop").attr("fill","#aaa").attr("class","playStop")
      config.play = true
      var currentTime = config.timeline.timer
      drawNetwork(currentTime,graphData)
    }else{
      console.log("true")
      playButton.selectAll("text").remove()
      playButton.append("text").attr("x",10).attr("y",16).text("play").attr("fill","#aaa").attr("class","playStop")
      config.play = false  
      clearInterval(config.timeline.timer)
    }
  })
}

function myGraph(el,graphData) {
    // Add and remove elements on the graph object
    this.addNode = function (id) {
        nodes.push({"id":id});
        update();
    }

    this.removeNode = function (id) {
        var i = 0;
        var n = findNode(id);
        while (i < links.length) {
            if ((links[i]['source'] === n)||(links[i]['target'] == n)) links.splice(i,1);
            else i++;
        }
        var index = findNodeIndex(id);
        if(index !== undefined) {
            nodes.splice(index, 1);
            update();
        }
    }
    this.removeLink = function(sourceId, targetId){
      var sourceNode = findNode(sourceId);
      var targetNode = findNode(targetId);
      for (var i = 0; i < links.length; i++) {
                      if (links[i]['source'] == sourceNode && links[i]['target'] == targetNode) {
                          links.splice(i, 1);
                          update();
                      }
                  }
    }
    this.addLink = function (sourceId, targetId) {
        var sourceNode = findNode(sourceId);
        var targetNode = findNode(targetId);

        if((sourceNode !== undefined) && (targetNode !== undefined)) {
            links.push({"source": sourceNode, "target": targetNode});
            update();
        }
    }

    var findNode = function (id) {
        for (var i=0; i < nodes.length; i++) {
            if (nodes[i].id === id)
                return nodes[i]
        };
    }

    var findNodeIndex = function (id) {
        for (var i=0; i < nodes.length; i++) {
            if (nodes[i].id === id)
                return i
        };
    }

    // set up the D3 visualisation in the specified element
    var w = 600,
        h = 400;

    var vis = this.vis = d3.select(el).append("svg:svg")
        .attr("width", w)
        .attr("height", h);

    var force = d3.layout.force()
        .gravity(.01)
        .distance(60)
        .charge(-100)
        .chargeDistance(200)
        .friction(0.4)
        .size([w, h]);

    var nodes = force.nodes(),
        links = force.links();

    var update = function () {

        var link = vis.selectAll("line.link")
            .data(links, function(d) { return d.source.id + "-" + d.target.id; });

        link.enter().insert("line")
            .attr("class", "link");

        link.exit().remove();

        var node = vis.selectAll("g.node")
            .data(nodes, function(d) { return d.id;})

        var nodeEnter = node.enter().append("g")
            .attr("class", "node")
            .call(force.drag)
            .on("click",function(d){
              drawChord(d.id,graphData)
            });

        nodeEnter.append("circle")
            .attr("class", "circle")
            .attr("cx", "0px")
            .attr("cy", "0px")
            .attr("r", "6px")
            .attr("fill",function(d){
                if(d.id == "15" || d.id == "16"){
                    return "red"
                }else{
             return "#48AA6D"
                }
            })
            .attr("opacity",0.6)
        nodeEnter.append("circle")
            .attr("class", "circle")
            .attr("cx", "0px")
            .attr("cy", "0px")
            .attr("r", "14px")
            .attr("fill",function(d){
                if(d.id == "15" || d.id == "16"){
                    return "red"
                }else{
             return "#48AA6D"
                }
            })
            .attr("opacity",0.1)
            
var wNames = {"0":"Elliot",
"1":"Neve",
"2":"Uriah",
"3":"Dash",
"4":"Beatrice",
"5":"Sofi",
"6":"Riella",
"7":"Eleanor",
"8":"Dario",
"9":"Elliana",
"10":"Sylas",
"11":"Veylan",
"12":"Mary (Teacher)",
"13":"Erin (Teacher)"}
var aNames = {"0":"Lyra",
"1":"Mila",
"2":"Penny",
"3":"Giorgio",
"4":"Ije",
"5":"Leo",
"6":"Yusuf",
"7":"Inaaya",
"8":"Oscar",
"9":"Gustav",
"10":"Rachel",
"11":"Jay",
"12":"Aiden",
"13":"Oliver",
"14":"Alec",
"15":"Missie (Teacher)",
"16":"Kanan (Teacher)"}
        nodeEnter.append("text")
            .attr("class", "nodetext")
            .attr("dx", 8)
            .attr("dy", ".35em")
            .text(function(d) {return aNames[d.id]})

        node.exit().remove();

        force.on("tick", function() {
          link.attr("x1", function(d) { return d.source.x; })
              .attr("y1", function(d) { return d.source.y; })
              .attr("x2", function(d) { return d.target.x; })
              .attr("y2", function(d) { return d.target.y; });

          node.attr("transform", function(d) { return "translate(" + d.x + "," + d.y + ")"; });
        });
        // Restart the force layout.
        force.start();
    }

    // Make it all go
    update();
}

function formatForChord(graphData){
  var chordData = {}
  for (var l in config.nodes){
    var node = config.nodes[l]
    chordData["_"+node]={}
  }
  
  for(var k in config.nodes){
    var node = config.nodes[k]
      for(var i in graphData){
        var links = graphData[i].links
        for(var j in links){
          var link = links[j]
          var source = link.source
          var target = link.target
          var value = parseInt(link.value)
          if(value <85){
            if(node == source){
              if(target in Object.keys(chordData["_"+node])){
                chordData["_"+node]["_"+target]+=1 
              }else{
                chordData["_"+node]["_"+target]=1
              }              
            }else if(node == target){
              if(source in Object.keys(chordData["_"+node])){
                chordData["_"+node]["_"+source]+=1
              }
              else{
                chordData["_"+node]["_"+source]=1
              }
            }
          }
         
        }
      }
  }
  
  return chordData
}

function drawChord(nodeId,chordData){
  d3.select("#chartTitle").html(aNames[nodeId])
  
  var data = chordData["_"+nodeId]
  var dataArray = []
  for(var id in data){
    if(isNaN(data[id])!=true){
    dataArray.push([id,data[id]])}
  }
  d3.select("#chart svg").remove()
  //console.log(dataArray)
  var chart = d3.select("#chart").append("svg").attr("width",400).attr("height",200)
  chart.selectAll("rect")
  .data(dataArray)
  .enter()
  .append("rect")
  .attr("x",85)
  .attr("y",function(d,i){return i*20})
  .attr("width",function(d){return d[1]+20})
  .attr("height",10)
  .attr("fill",function(d){
      if(nodeId == "15" || nodeId == "16"){
          return "red"
      }else{
   return "#48AA6D"
      }
  })
  .attr("opacity",.6)
  
  chart.selectAll("text")
  .data(dataArray)
  .enter()
  .append("text")
  .attr("x",80)
  .attr("y",function(d,i){return i*20+10})
  .attr("font-size","12px")
  .attr("text-anchor","end")
  .text(function(d){return aNames[d[0].replace("_","")]})
}

function drawNodes(){
  var graph = config.graph
  var nodes = config.nodes
  for(var i in nodes){
    graph.addNode(nodes[i])
  }
}
function drawNetwork(starttime,graphData){
  
  
  var threshold = 85
  var graph = config.graph
  //sort by time
  var allChanges = []
  for (var l in graphData){
    linkChanges = graphData[l].links    
    var currentSweepChanges = []
    for(var k in linkChanges){
      change = linkChanges[k]
      if(change.value<threshold){
        currentSweepChanges.push(change)
      }
    }  
    if (currentSweepChanges.length==0) {
      allChanges.push(["NA",l])      
    }else{
      allChanges.push([currentSweepChanges,l])      
    }
  }
 // console.log(allChanges.length)
  
  var i = starttime
  var allLinks = []
  for(var s =0; s<17;s++){
      for(var t=0; t<17; t++){
          allLinks.push([s,t])
      }
  }
  config.timeline.timer  = setInterval(function(){ 
     var timePassedBeforeStart = Math.round(allChanges[0][1])
     var timeInSeconds= Math.round(allChanges[i][1])-timePassedBeforeStart+9*60*60
     var clock = formatSecondsAsTime(timeInSeconds)
     d3.select("#time").html("\<span style=\"color:#aaa; font-size:24px;\">"+clock+"\</span>")
      
      
      
     if(i >1){
   for(var r in allLinks){
   graph.removeLink(allLinks[r][0],allLinks[r][1])
   }
   
         
       var oldChanges = allChanges[i-1][0]
       if(oldChanges!="NA"){
       for(var j in oldChanges){
         graph.removeLink(oldChanges[j].source,oldChanges[j].target)
           
          // console.log(oldChanges[j])
       }}
     }
    
      var currentChanges = allChanges[i][0]
     if(currentChanges!="NA"){
     if(currentChanges.length >1){
       for(var j in currentChanges){
         graph.addLink(currentChanges[j].source,currentChanges[j].target)
       }
     }}

     // graph.addLink(changes[i]["addLink"][0],changes[i]["addLink"][1]);
     // graph.removeLink(changes[i]["addLink"][0],changes[i]["addLink"][1]);
  
      //graph.removeNode(changes[i]["removeNode"])
      //graph.addNode(changes[i]["addNode"])
     if(timeInSeconds > 12.45*60*60){
     //  console.log("stop")
       clearInterval(config.timeline.timer);
     }
    i=i+1
   //  console.log(i)
  }, 20);
}
function formatSecondsAsTime(secs, format) {
		var hr  = Math.floor(secs / 3600);
		var min = Math.floor((secs - (hr * 3600))/60);
		var sec = Math.floor(secs - (hr * 3600) -  (min * 60));

		if (hr < 10)   { hr    = "0" + hr; }
		if (min < 10) { min = "0" + min; }
		if (sec < 10)  { sec  = "0" + sec; }
		//if (hr)            { hr   = "00"+hr; }

		if (format != null) {
			var formatted_time = format.replace('hh', hr);
			formatted_time = formatted_time.replace('h', hr*1+""); // check for single hour formatting
			formatted_time = formatted_time.replace('mm', min);
			formatted_time = formatted_time.replace('m', min*1+""); // check for single minute formatting
			formatted_time = formatted_time.replace('ss', sec);
			formatted_time = formatted_time.replace('s', sec*1+""); // check for single second formatting
			return formatted_time;
		} else {
			return hr + ':' + min;
		}
	}
function drawButton(divName,label,colorIndex,timeStart,graphData){
  var className = label.replace(/\s/g, '');
  var colors = ["#53AE32","#1C3818","#48AA6D","#72993B","#357443","#3A5B1B","#338326"]
  var button = d3.select("#"+divName).append("svg").attr("width",label.length*8+14).attr("height",20)
  button
  .append("rect")
  .attr("x",0)
  .attr("y",0)
  .attr("width",label.length*8+10)
  .attr("class",className)
  .attr("height",20)
  .attr("fill","#fff")
  .attr("stroke",colors[colorIndex])
  .on("click",function(){
    d3.selectAll("rect").attr("fill","#fff").attr("opacity",1)
    d3.select(this).attr("fill",colors[colorIndex]).attr("opacity",.3)
  })
  
  button.append("text")
  .text(label)
  .attr("fill",colors[colorIndex])
  .attr("font-size","14px")
  .attr("letter-spacing","1px")
  .attr("x",6)
  .attr("y",14)
  .attr("class",className)
  .on("click",function(){
    d3.selectAll("rect").attr("fill","#fff").attr("opacity",1)
    d3.select("."+className).attr("fill",colors[colorIndex]).attr("opacity",.3)
    
  })  
    
  button.on("click",function(){
    clearInterval(config.timeline.timer)
    drawNetwork(timeStart,graphData)
     config.play = true
     d3.select("#play svg text")
      .remove()
      d3.select("#play svg").append("text").attr("x",10).attr("y",16).text("stop").attr("fill","#aaa").attr("class","playStop")
  })
  return button
}