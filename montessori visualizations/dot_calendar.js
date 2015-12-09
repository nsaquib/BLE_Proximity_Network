
	var PL = ["Washing Hands","Walking a Line","Flower Arranging","Unrolling","Watering a Plant"]
	var M = ["Hundred Board","Short Bead Stair","Color Tablets","Baric Tablets"]
	var S = ["Thermal Bottles","Red Rods","Trinomial Cube"]
	var L = ["Sand Tray","Sandpaper Letters","Stamp Game"]
         
	var index = 0
    var PLData = PL["undefined"]
    
    var colorDictionary = {
    	"PL":"#4B77DE",
    	"S":"#EEA927",
    	"M":"#E2402A",
    	"L":"#aaa"
    }
  
	var mScale = d3.select("#materialScale").append("svg").attr("width",405).attr("height", 40)
    var mScaleData =  []
	for(var mData =0; mData <11; mData++){  mScaleData.push(mData)}
    mScale.selectAll("rect").data(mScaleData).enter().append("rect")
        .attr("x",function(d){return d*40})
        .attr("y",28)
        .attr("width",2)
        .attr("height",10)
        .attr("fill","#aaa")
    mScale.append("rect") 
        .attr("x",0)
        .attr("y",38)
        .attr("width",402)
        .attr("height",2)
        .attr("fill","#aaa")
    mScale.append("text").text("least").attr("x",0).attr("y",24).attr("fill","#aaa")
    mScale.append("text").text("most").attr("x",374).attr("y",24).attr("fill","#aaa")
    
    newGraphGroup(PL,colorDictionary["PL"],"PL")
    newGraphGroup(L,colorDictionary["L"],"L")
    newGraphGroup(M,colorDictionary["M"],"M")
    newGraphGroup(S,colorDictionary["S"],"S")
 
	var weeks = []
	for(var week =0; week <10; week++){ weeks.push(week)}
	//chart.append("text")
	//.text("week:")
	//.attr("x",150)
	//.attr("y",10)
	//.attr("text-anchor","end")
	//.attr("fill", "#aaa")
	//
	//chart.selectAll("text .weekLabel")
	//.data(weeks)
	//.enter()
	//.append("text")
	//.attr("class","weekLabel")
	//.attr("class",function(d){return d})
	//.attr("x",function(d){return d*30+165})
	//.attr("y",10)
	//.text(function(d){return d+1})
	//.attr("fill", "#aaa")

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
function newGraphGroup(data,color,section){
    var sectionLabels = {"PL":"Practical Life","L":"Language","M":"Mathematics","S":"Sensorial"}
	var chart = d3.select("#"+section+"bar").append("svg").attr("width",400).attr("height", 30)
    chart.append("rect").attr("x",0)
    .attr("y",10)
    .attr("width",Math.random()*400+100)
    .attr("height",20)
    .attr("fill",color)
    .on("click",function(){
       // d3.selectAll("."+section).remove()
        toggle(section)
    })
	var weekRow = d3.select("#"+section).append("svg").attr("width",400).attr("height", 30)
    
    weekRow .append("text").text("Week:").attr("x",115).attr("y",25).attr("text-anchor","end")        
    .attr("fill","#555")

	var weeks = []
	for(var week =-1; week <10; week++){ weeks.push(week)}
    weekRow.selectAll("text").data(weeks).enter().append("text").attr("x",function(d){return d*25+143}).attr("y",25).text(function(d){return d+1})
        .attr("fill","#555")
    
    chart.append("text").text(sectionLabels[section]).attr("x",10).attr("y",25).attr("fill","#fff")
    for(var i in data){
       newGraph(data,i,color,section)
    }
    jQuery('#'+section).toggle('show');
    
}
function toggle(section){
        jQuery('#'+section).toggle('show');
}
function getRandomInt(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}
function newGraph(data,index,color,section){
	var chart = d3.select("#"+section).append("svg").attr("width",400).attr("height", 40).attr("class",section)
    var weeks = Array.apply(null, Array(10)).map(function (_, i) {return i;});
    
    var arr = []
    while(arr.length < 10){
      var randomnumber=Math.ceil(Math.random()*11)
      var found=false;
      for(var i=0;i<arr.length;i++){
    	if(arr[i]==randomnumber){found=true;break}
      }
      if(!found)arr[arr.length]=randomnumber;
    }
    //console.log(index+20)
    
    chart.selectAll("circle")
        .data(arr)
        .enter()
        .append("circle")
        .attr("cx",function(d,i){return (i+1)*25+120})
        .attr("cy",parseInt(index)+20)
        .attr("r",function(d,i){return d+1})
        .attr("fill",color)
        .attr("class",section)
        .attr("opacity",0.8)
        .on("mouseover",function(d){
            d3.select(this).attr("opacity",1)
    		var imageIndex = data[index].length%12
            console.log(imageIndex)
    		d3.select("#materialImage").html(
     		 "MONDAY:<br/><br/>TUESDAY<br/><br/>WEDNESDAY<br/><br/>THURSDAY<br/><br/>FRIDAY"
    		)
    		d3.select("#notes").html("WEEKLY PROGRESS:<br/> Vim ad equidem periculis similique."+
            "adipiscing eu eam, ne vel debet dicant reprimique. Est laudem menandri et solet signiferumque in sit."
    		)
        })
        .on("mouseout",function(){
            d3.select(this).attr("opacity",0.8)
            d3.select("#notes").html("")
            d3.select("#materialImage").html("")
        })
    chart
        .append("text")
        .text(data[index])
        .attr("x",115)    
        .attr("y",parseInt(index)+25)
        .attr("text-anchor","end")
        .attr("class",section)
        .attr("fill","#555")
    	.on("mouseover",function(d){
    		d3.select(this).attr("opacity",1)
    		d3.select("#notes").html("Vim ad equidem periculis similique. Eum aliquip discere propriae te,"+
            " mel ut timeam equidem legimus, mea id volumus volutpat. Sumo vocent "+
            "adipiscing eu eam, ne vel debet dicant reprimique. Est laudem menandri et solet signiferumque in sit.")
    		var imageIndex = data[index].length%12
            console.log(imageIndex)
    		d3.select("#materialImage").html("ABOUT THE MATERIAL:<br/>"+
     		 "<img src=\"materials/"+imageIndex+".jpg\" alt=\Mountain View\" style=\"width:300px;height:300px\"><br/>"
    		)
    	})
        .on("mouseout",function(d){
            d3.select("#notes").html("")
            d3.select("#materialImage").html("")
        })
    
}

