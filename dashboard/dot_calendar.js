$(function() {
	// Window has loaded

	queue()
	.defer(d3.csv, dotCalendar)
	.await(dataDidLoad);
})
function dataDidLoad(error, data) {
	var formated = formatData(data)
	var chart = d3.select("#chart").append("svg").attr("width",600).attr("height",1200)
	var index = 0
	for(var i in formated){
		index = index+1
		console.log([i,index,formated[i]])
		drawGraph(formated,i, chart,parseInt(index))
	}
	var weeks = []
	for(var week =0; week <10; week++){ weeks.push(week)}
	chart.append("text")
	.text("week:")
	.attr("x",150)
	.attr("y",10)
	.attr("text-anchor","end")
	.attr("fill", "#aaa")
	
	chart.selectAll("text .weekLabel")
	.data(weeks)
	.enter()
	.append("text")
	.attr("class","weekLabel")
	.attr("class",function(d){return d})
	.attr("x",function(d){console.log(d); return d*30+165})
	.attr("y",10)
	.text(function(d){return d+1})
	.attr("fill", "#aaa")
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

function formatData(data,section){
	var currentSection = section
	var dataByMaterial = table.group(data,["activity"])
	//var dataByMaterial = table.filter(data,["section"])
	return dataByMaterial
}
var colorDictionary = {
	"PL":"#4B77DE",
	"S":"#EEA927",
	"M":"#E2402A",
	"L":"#aaa"
}

function drawGraph(data,material, svg, y){
//	console.log(data)
	var rScale = d3.scale.linear().domain([0,10]).range([2,10])
	svg.selectAll("circle .dots")
	.data(data[material])
	.enter()
	.append("circle")
	.attr("class", "dots")
	.attr("class",function(d){return d.activity})
	.attr("cy", function(d){ return parseInt(y)*30})
	.attr("cx", function(d){return parseInt(d.week)*30+140})
	.attr("r", function(d){ return rScale(d.value)})
	.attr("opacity",.5)
	.attr("fill",function(d){ return colorDictionary[d.section]})
	.on("mouseover",function(d){
		d3.select(this).attr("opacity",1)
		d3.select("#notes").html(
			"<strong>Progress<br/><br/>Activity:</strong> "+d.activity+"<br/><strong>Week: </strong>"+d.week+"<br/><strong>Estimated Hours Spent:</strong>"+d.value+"<br/><br/><br/><strong>Notes and Observations:</strong><br/>"+d.notes)
		var imageIndex = parseInt(y)%12
		
		d3.select("#materialImage").html(
 		 "<img src=\"materials/"+imageIndex+".jpg\" alt=\Mountain View\" style=\"width:300px;height:300px\"><br/>"
		)
		d3.select("#materialNotes").html(
			"Section: "+sectionDictionary[d.section]+"<br/>"+
			"Focus: "+skillNotesArray[imageIndex]+"<br/>"+"<br/>"+
			"Description: "+materialNotesArray[imageIndex]+"<br/>"
			+"... read more"
		)
	})
	.on("mouseout",function(){
		d3.select(this).attr("opacity",.5)
		
	})
	var materialNotesArray = ["This is one of our favorites","This material gives the basic sense of color and space, once the material is mastered, the child can move onto the second in this series","in the early childhood classroom, this is enjoyed sensorially, as the the child uses his or her tactile and visual discrimination to order the objects","whether this material is enjoyed by the youngest children practicing their carrying or the most advanced, the material combines all the sensorial materials at one time","this is often the most coveted material in the classroom. it's primary function is to develop visual and tactiel discrimitation","this is a material for good gross motor control","each object in this series differs in weight and thickness, and each sequence creates an orderly staircase","a frequent partner to the pink tower, this shares some qualities with its upright partner"]
	var skillNotesArray = ["coordination and carrying", "tactile discrimination","visual and tactiel discrimination","motor skills","counting","order"]
	var sectionDictionary = {"S":"Sensorial","L":"Language","M":"Mathematics","PL":"Practical Life"}
	svg.selectAll("text .labels")
	.data(data[material])
	.enter()
	.append("text")
	.attr("class","labels")
	.attr("class",function(d){return d.activity})
	.attr("x",150)
	.attr("y", function(d){ return parseInt(y)*30+4})
	.text(function(d){return d.activity})
	.attr("fill",function(d){ return colorDictionary[d.section]})
	.attr("opacity",.1)
	.attr("text-anchor","end")
	.on("mouseover",function(d){
		var imageIndex = parseInt(y)%12
		d3.select("#materialImage").html(
 		 "<img src=\"materials/"+imageIndex+".jpg\" alt=\Mountain View\" style=\"width:300px;height:300px\"><br/>"
		)
		d3.select("#materialNotes").html(
			"Section: "+sectionDictionary[d.section]+"<br/>"+
			"Focus: "+skillNotesArray[imageIndex%3+1]+"<br/>"+"<br/>"+
			"Description: "+materialNotesArray[imageIndex]+"<br/>"
			+"... read more"
		)
		d3.select(this).attr("opacity",1)
	})
	.on("mouseout",function(){
		d3.select(this).attr("opacity",.2)
		
	})
}