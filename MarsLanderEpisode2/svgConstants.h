#ifndef __SVG_CONSTANTS_H__
#define __SVG_CONSTANTS_H__

#include <string>

static const std::string SVG_HTML_FILE_NAME = "visualDebugTool.html";
static const std::string POLYLINE_BEGIN = "	<polyline points = \"";
static const std::string POLYLINE_END = " />\n";
static const std::string STYLE_BEGIN = "style = \"";
static const std::string ATTRIBUTE_END = "\"";
static const std::string STROKE = "stroke:";
static const std::string STROKE_WIDTH = "stroke-width:";
static const std::string RGB_BEGIN = "rgb(";
static const std::string RGB_END = ")";
static const std::string FILL_NONE = "fill:none;";
static const std::string ONCLICK_DISPLAY_DATA = "onclick=\"displayData(event)\"";
static const std::string DATA_EVALUATION = "data-evaluation=\"";
static const std::string ID_BEGIN = "<g id=\"turn";
static const std::string ID_END = ">\n";
static const std::string GROUP_END = "</g>\n\n";
static const std::string DISPLAY_NONE = "style=\"display:none\"";

static const std::string FILE_START = "\
<? xml version = \"1.0\" encoding = \"UTF-8\"?>\n\
<svg xmlns = \"http://www.w3.org/2000/svg\" version = \"1.1\" width = \"1750\" height = \"750\" viewBox = \"0 0 7000 3000\" style = \"background-color:black\">\n\
";

static const std::string FILE_END = "\
</svg>\n\
\n\
<button onclick = \"showNextSim()\">Next Sim</button>\n\
<button onclick = \"showPreviousSim()\">Previous Sim</button>\n\
<button onclick = \"wholeSimulation(0)\">Whole Sim</button>\n\
\n\
<script>\n\
\n\
var turn = 0;\n\
var turnStr = \"turn\";\n\
function showNextSim() {\n\
	if (turn > 0) {\n\
		var previousTurnElementId = turnStr + (turn - 1);\n\
		document.getElementById(previousTurnElementId).style.display = \"none\";\n\
	}\n\
\n\
	var currentTurnElementId = turnStr + turn;\n\
	document.getElementById(currentTurnElementId).style.display = \"block\";\n\
	++turn;\n\
}\n\
\n\
function showPreviousSim() {\n\
	--turn;\n\
	var currentTurnElementId = turnStr + turn;\n\
	document.getElementById(currentTurnElementId).style.display = \"none\";\n\
\n\
	if (turn > 0) {\n\
		var previousTurnElementId = turnStr + (turn - 1);\n\
		document.getElementById(previousTurnElementId).style.display = \"block\";\n\
	}\n\
}\n\
\n\
function wholeSimulation(groupIdx) {\n\
	setTimeout(function() {\n\
		var currentTurnElementId = turnStr + groupIdx;\n\
		var element = document.getElementById(currentTurnElementId);\n\
\n\
		if (element === null) {\n\
			return;\n\
		}\n\
\n\
		if (groupIdx > 0) {\n\
			var previousTurnElementId = turnStr + (groupIdx - 1);\n\
			document.getElementById(previousTurnElementId).style.display = \"none\";\n\
		}\n\
\n\
		element.style.display =\"block\";\n\
\n\
		wholeSimulation(++groupIdx);\n\
	}, 500);\n\
}\n\
\n\
function clicked(evt) {\n\
	var e = evt.target;\n\
	var dim = e.getBoundingClientRect();\n\
	var x = evt.clientX - dim.left;\n\
	var y = evt.clientY - dim.top;\n\
	y = 3000 - y;\n\
	alert(\"x: \" + x + \" y:\" + y);\n\
}\n\
\n\
function displayData(event) {\n\
	var evaluation = event.target.getAttributeNS(null, \"data-evaluation\");\n\
	alert(\"Evaluation: \" + evaluation);\n\
}\n\
\n\
</script>\n\
";


#endif
