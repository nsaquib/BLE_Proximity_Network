// Word tree by Jason Davies, http://www.jasondavies.com/
(function(exports) {

exports.wordtree = wordtree;

function wordtree() {
  var width,
      height,
      p = 5,
      maxDepth = 10,
      trailing0 = 15,
      trailing1 = 30,
      cull = .01,
      reverse = false,
      token = wordtreeToken,
      tokens = [],
      prefix = [],
      tokenIndex,
      root,
      phraseLine = false,
      event = d3.dispatch("prefix");

  var scale = d3.scale.sqrt().range([0, 25]);

  var layout = d3.layout.partition()
        .sort(wordtreeSort)
        .value(function(d) { return d.count; }),
      diagonal = d3.svg.diagonal()
        .projection(function(d) { return [project(d.y), d.x]; })
        .source(function(d) {
          return {y: d.source.x + (d.source.length ? d.source.length + p : 0), x: d.source.y};
        })
        .target(function(d) {
          return {y: d.target.x - (d.target.length ? p : 0), x: d.target.y};
        }),
      mostFrequent,
      duration = 0;

  function wordtree(svg) {
    var count = root ? root.count : 0;
    if (!count) return;

    collapse(root, height / root.count);
    var nodes = fix(layout.nodes(root));
    scale.domain(oneLine(root) ? [root.count - 1, root.count + 1] : [0, root.count]);

    var token = svg.selectAll("text")
        .data(nodes, nodePath);
    token
        .attr("font-size", size)
        .attr("dy", ".3em")
        .each(function(d) {
          d.length = this.getComputedTextLength();
        });
    fixLengths(nodes);
    token.transition()
        .duration(duration)
        //.style("opacity", 1)
        .attr("x", function(d) { return project(d.x); })
        .attr("y", function(d) { return d.y; });
    var tokenEnter = token.enter().append("text");
    var tspan = token.selectAll("tspan")
        .data(function(d) { return d.tokens.map(function(t) { return {data: d, token: t}; }); });
    tspan.enter().append("tspan")
        .text(function(d, i) { return (i && d.token.whitespace ? "\u00a0" : "") + d.token.token; });
    tspan.on("click", function(d) {
          var e = d3.event,
              keyword;
          if (e.ctrlKey || e.altKey || e.shiftKey) {
            svg.call(wordtree.prefix([d.token.token]));
            keyword = d.token.token;
            d.data = root;
          } else {
            keyword = fullPrefix(d.data);
            expandNode(d.data);
            wordtree(svg);
          }
          event.prefix({keyword: keyword, tree: d.data});
        });
    tokenEnter
        //.style("opacity", 1e-6)
        .attr("font-size", size)
        .attr("dy", ".3em")
        .each(function(d) {
          d.length = this.getComputedTextLength();
        })
        .each(function(d) {
          var p = d.parent;
          d.x = p ? p.x + (p.length || 0) + 50 : 0;
        })
        .attr("x", function(d) { return project(d.x); })
        .attr("y", function(d) { return d.y; })
        .attr("visibility", "hidden");
    token
        .attr("text-anchor", reverse ? "end" : null)
      .transition().delay(duration)
        .attr("visibility", null);
    token.exit().remove();

    // Update the linksâ€¦
    var link = svg.selectAll("path.link")
        .data(layout.links(nodes), function(d) { return nodePath(d.source) + ":" + nodePath(d.target); });

    // Transition links to their new position.
    link.attr("visibility", null)
      .transition()
        .duration(duration)
        .attr("d", diagonal);

    // Enter any new links at the parent's previous position.
    link.enter().insert("path", "a")
        .attr("class", "link")
        .attr("d", diagonal)
        .attr("visibility", "hidden")
      .transition().delay(duration)
        .attr("visibility", null);

    link.exit().remove();

    duration = 500;
    uncollapse(root);
  }

  wordtree.tokens = function(_) {
    if (!arguments.length) return tokens;
    tokens = _;
    tokenIndex = {};
    for (var i = 0, n = tokens.length, t; i < n; ++i) {
      t = token.call(wordtree, tokens[i], i);
      (tokenIndex.hasOwnProperty(t) ? tokenIndex[t] : tokenIndex[t] = []).push(i);
    }
    return wordtree;
  };

  wordtree.prefix = function(_) {
    if (!arguments.length) return prefix;
    prefix = _.slice();
    root = build(tokens, reverse ? prefix[prefix.length - 1] : prefix[0], reverse ? prefix.reverse() : prefix);
    return wordtree;
  };

  wordtree.root = function() {
    return root;
  };

  wordtree.reverse = function(_) {
    if (!arguments.length) return reverse;
    reverse = !!_;
    return wordtree;
  };

  wordtree.width = function(_) {
    if (!arguments.length) return width;
    layout.size([layout.size()[0], width = +_]);
    return wordtree;
  };

  wordtree.height = function(_) {
    if (!arguments.length) return height;
    layout.size([height = +_, layout.size()[1]]);
    return wordtree;
  };

  wordtree.phraseLine = function(_) {
    if (!arguments.length) return phraseLine;
    phraseLine = !!_;
    return wordtree;
  };

  return d3.rebind(d3.rebind(wordtree, event, "on"), layout, "sort");

  function size(d) {
    var s = d.count / root.count * height;
    if (!d.children || !d.children.length) return Math.max(0, Math.min(15, s - 1));
    return (s > 30 ? 30 + scale(d.count - 30 * root.count / height) : s - 1) | 0 + "px";
  }

  function build(tokens, start, rest) {
    var positions = tokenIndex[start.toLowerCase()];
    if (!positions) return null;
    var n = positions.length;
    if (!n) return null;
    var hash = {},
        root = {tokens: [tokens[positions[0]]], children: [], index: positions[0]};
    for (var i = 0; i < n; i++) {
      var j = positions[reverse ? n - i - 1 : i],
          max = Math.min(tokens.length, j + maxDepth),
          min = Math.max(0, j - maxDepth),
          parent = root,
          key = [start];
      while (reverse ? --j > min : ++j < max) {
        var token = tokens[j];
        if (newline(token)) break;
        key.push(token.token.toLowerCase());
        var k = key.join(" "),
            node = hash[k];
        if (node) parent = node;
        else {
          parent.children.push(node = {tokens: [token], children: [], index: j});
          hash[k] = node;
          parent = node;
        }
      }
    }
    revalue(root);
    filter(root, maxDepth - 1);
    if (rest && rest.length) {
      rest = hash[rest.join(" ")];
      if (rest) expandNode(rest.node || rest);
    }
    return root;
  }

  function filter(d, depth) {
    if (depth <= 0) {
      d.children = [];
      return;
    }
    var t = d.count * cull,
        count = 0;
    d.children = d.children.filter(function(d) {
      if (d.count > t) {
        filter(d, depth - 1);
        return true;
      }
    });
  }

  function project(x) {
    return reverse ? width - x : x;
  }

  function revalue(d, parent) {
    var v = 0,
        n = d.children.length;
    if (!n) {
      v = 1;
      expandTrailing(d);
    } else if (n === 1) {
      var child = d.children[0];
      (reverse ? d.tokens.unshift : d.tokens.push).apply(d.tokens, child.tokens);
      d.children = child.children;
      if (d.index < 0) d.index = child.index;
      child.node = d;
      return revalue(d, parent);
    } else {
      for (var i = 0; i < n; ++i) v += revalue(d.children[i], d);
    }
    // Reset length.
    d.length = 0;
    d.parent = parent;
    return d.count = v;
  }

  function fullPrefix(d) {
    var s = [];
    do {
      (reverse ? s.push : s.unshift).call(s, d3.merge(d.tokens.map(function(d) { return [d.whitespace ? " " : "", d.token]; })));
    } while (d = d.parent);
    s[0] = s[0].slice(1);
    return d3.merge(s).join("");
  }

  function expandTrailing(d) {
    if (reverse) {
      var j = d.tokens[0].index,
          j0 = j - trailing0,
          j1 = j0 - trailing1;
      while (--j >= 0 && !newline(tokens[j + 1]) && (j >= j0 || !sentenceSeparator(tokens[j]))) d.tokens.unshift(tokens[j]);
    } else {
      var j = d.tokens[d.tokens.length - 1].index,
          j0 = j + trailing0,
          j1 = j0 + trailing1;
      while (++j < tokens.length && !newline(tokens[j]) && (j < j0 || !sentenceSeparator(tokens[j - 1]))) d.tokens.push(tokens[j]);
    }
  }

  function newline(token) {
    return phraseLine && /[\r\n]/.test(token.whitespace);
  }

  function sentenceSeparator(token) {
    return /[\.!?]/.test(token.token);
  }

  function collapse(d, dy) {
    if (d.count * dy < 5) {
      if (d.children && d.children.length > 1) {
        d._collapsed = d.children;
        d.children = [d.children[0]]; // TODO choose longest branch
      }
    } else if (d._collapsed && !d._children) {
      d.children = d._collapsed;
    }
    if (d.children) d.children.forEach(function(d) { collapse(d, dy); });
  }
}

function uncollapse(d) {
  if (d._collapsed && !d._children) d.children = d._collapsed;
  if (d.children) d.children.forEach(uncollapse);
}

function fix(d) {
  d.forEach(function(d) {
    d.y = d.x + d.dx / 2;
    d.x = d.parent ? d.parent.x + 100 : 0;
  });
  return d;
}

function fixLengths(d) {
  d.forEach(function(d) {
    var p = d.parent;
    d.x = p ? p.x + (p.length || 0) + 50 : 0;
  });
  return d;
}

function expandNode(d) {
  expand(d);
  while (d.parent) {
    if (!d.parent._children) {
      d.parent._children = d.parent.children;
      d.parent.children = [d];
      d.parent._count = d.parent.count;
    }
    d.parent.count = d.count;
    d = d.parent;
  }
}

function expand(d) {
  if (d._children) {
    d.children = d._children;
    delete d._children;
    d.count = d._count;
    delete d._count;
  }
  if (d.children) d.children.forEach(expand);
}

function nodePath(d) {
  var path = [];
  do {
    path.unshift(d.index + "-" + d.tokens.length);
    d = d.parent;
  } while (d);
  return path.join("/");
}

function oneLine(d) {
  return !d.children || !d.children.length || d.children.length === 1 && oneLine(d.children[0]);
}

function wordtreeToken(d) { return d.token.toLowerCase(); }
function wordtreeSort(a, b) { return b.count - a.count; }

})(this);