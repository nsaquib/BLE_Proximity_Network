      //*******************************************************************
      //  CREATE MATRIX AND MAP
      //*******************************************************************
      d3.csv(csv, function (error, data) {
        var mpr = chordMpr(data);
		//console.log("this is the csv you are visualizing:"+csv)
        mpr
          .addValuesToMap('term1')
          .setFilter(function (row, a, b) {
            return (row.term1 === a.name && row.term2 === b.name)
          })
          .setAccessor(function (recs, a, b) {
            if (!recs[0]) return 0;
            return +recs[0].count;
          });
        drawChords(mpr.getMatrix(), mpr.getMap(), data);
		d3.select(".foodsTab").attr("color", "#ffaaff")
		d3.select("#map").style("visibility", "hidden")
		d3.select("#foods").style("visibility", "visible")
		d3.select("#cuisines").style("visibility", "hidden") 
		//drawMap( data)
		
      });
	  
	
	  function formatData(data){
		  var foodsDictionary ={}
		  var restaurantsDictionary ={}
		  var cuisinesDictionary ={}
		  var coordinatesDictionary ={}
		  
		  for(var i in data){
			  var terms = [data[i].term1, data[i].term2]
			  var foods = data[i].foods
			  var restaurants = data[i].restaurant
			  var cuisines = data[i].type
			  var coordinates = data[i].coordinates
			  //console.log(restaurants, cuisines, coordinates)
			  foodsDictionary[terms] = foods
			  restaurantsDictionary[terms] = restaurants
			  cuisinesDictionary[terms] = cuisines
			  coordinatesDictionary[terms] = coordinates
		  }
		  //console.log(foodsDictionary)
		  return [foodsDictionary, restaurantsDictionary, coordinatesDictionary, cuisinesDictionary]
	  }
      //*******************************************************************
      //  DRAW THE CHORD DIAGRAM
      //*******************************************************************
      function drawChords (matrix, mmap, data) {
		 
		 // console.log(restaurantsDictionary)
		var foodsDictionary = formatData(data)[0]
		var restaurantsDictionary = formatData(data)[1]
		var cuisinesDictionary = formatData(data)[3]
		var coordinatesDictionary = formatData(data)[2]
		//console.log(coordinatesDictionary)
		
        var w =400, h = 400, r1 = h / 2, r0 = r1 -120;
		var totalMenuItems = 58255
        var fill = d3.scale.ordinal()
            .domain(d3.range(16))
            .range(["#59D984",
"#537E4C",
"#75DF40",
"#53992E",
"#B1DF6C",
"#A6DFA1",
"#8EAE6C"]);
        
        var chord = d3.layout.chord()
            .padding(.1)
            .sortGroups(d3.descending)
            .sortSubgroups(d3.descending)
            .sortChords(d3.descending);

        var arc = d3.svg.arc()
            .innerRadius(r0)
            .outerRadius(r0 + 5);

        var svg = d3.select("#container").append("svg:svg")
            .attr("width", w)
            .attr("height", h)
          .append("svg:g")
            .attr("id", "circle")
            .attr("transform", "translate(" + w / 2 + "," + h / 2 + ")");

            svg.append("circle")
                .attr("r", r0 + 5);

        var rdr = chordRdr(matrix, mmap);
        chord.matrix(matrix);
		var opacityScale = d3.scale.sqrt().domain([0,.4]).range([0.2,.5])
		var fontScale = d3.scale.sqrt().domain([0,maxForScale]).range([12,14])
		
        var g = svg.selectAll("g.group")
            .data(chord.groups())
          .enter().append("svg:g")
            .attr("class", "group")
            .on("mouseover", mouseover)
            .on("mouseout", function (d) { d3.select("#tooltip").style("visibility", "hidden") });
		
		g.append("svg:path")
            .style("stroke", "none")
            .style("fill", function(d) { 
				//return "#aaa"
				return fill(d.index); 
			})
			//.style("opacity", function(d){return opacityScale(d.value)})
            .attr("d", arc);
            var aNames = {"0":"Lyra","1":"Mila",
            "2":"Penny","3":"Giorgio",
            "4":"Ije","5":"Leo",
            "6":"Yusuf","7":"Inaaya",
            "8":"Oscar","9":"Gustav",
            "10":"Rachel","11":"Jay",
            "12":"Aiden","13":"Oliver",
            "14":"Alec","15":"Missie (Teacher)",
            "16":"Kanan (Teacher)"}
        g.append("svg:text")
            .each(function(d) { d.angle = (d.startAngle + d.endAngle) / 2; })
            .attr("dy", ".35em")
            .style("font-family", "serif")
            .style("font-size", function(d){return 12})
            .attr("text-anchor", function(d) { 
				return d.angle > Math.PI ? "end" : null; })
            .attr("transform", function(d) {
              return "rotate(" + (d.angle * 180 / Math.PI - 90) + ")"
                  + "translate(" + (r0 +8) + ")"
                  + (d.angle > Math.PI ? "rotate(180)" : "");
            })
            .text(function(d) { return aNames[rdr(d).gname]; });

          var chordPaths = svg.selectAll("path.chord")
                .data(chord.chords())
              .enter().append("svg:path")
                .attr("class", "chord")
               .style("stroke", function(d) {return fill(d.index); return "#ddd" })
                .style("fill", function(d) { 
					return fill(d.source.index); 
					//return "#000"
				})
				.attr("opacity",1)
                .attr("d", d3.svg.chord().radius(r0))
                .on("mouseover", function (d,i) {					
                  d3.select("#tooltip")
                    .style("visibility", "visible")
                    .html(chordTip(rdr(d)));
					d3.selectAll("path").attr("opacity",.2)
					d3.select(this).attr("opacity", 1)
                    //.style("top", function () { return (d3.event.pageY - 20)+"px"})
                    //.style("left", function () { return (d3.event.pageX - 100)+"px";})
                })
				.on("mouseout", function(d){
					d3.selectAll("path").attr("opacity",1)
				})
				.on("click", function (d,i) {					
                  d3.select("#tooltip")
                    .style("visibility", "visible")
                    .html(chordTip(rdr(d)))
					 })
			//	.style("opacity", function(d){return opacityScale(d.target.value)})
                //.on("mouseout", function (d) { d3.select("#tooltip").style("visibility", "hidden") });

	
          function chordTip (d) {
  			//console.log(d)			  
            var p = d3.format(".2%"), q = d3.format(",.3r")
          }

          function groupTip (d) {
            var p = d3.format(".1%"), q = d3.format(",.3r")
			//console.log(d)
            return ""
			//return  p(d.gvalue/totalMenuItems)+" or "+ q(d.gvalue) +" items has "+ d.gname 
               // + p(d.gvalue/d.mtotal) + " of Matrix Total (" + q(d.mtotal) + ")"
          }

          function mouseover(d, i) {
            d3.select("#tooltip")
              .style("visibility", "visible")
              .html(groupTip(rdr(d)))
              //.style("top", function () { return (d3.event.pageY - 20)+"px"})
              //.style("left", function () { return (d3.event.pageX - 130)+"px";})
             chordPaths.classed("fade", function(p) {
              return p.source.index != i
                  && p.target.index != i;
            });
          }
      }

	