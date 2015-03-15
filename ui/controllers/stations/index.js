var ejs = require("ejs");
var fs = require("fs");



function
render(inReq, inResp, inTemplate)
{
	var path = __dirname + "/" + inTemplate + ".ejs";
	console.log("Reading template " + path);
	fs.readFile(path, function(inErr, inData)
	{
		var html = ejs.render(inData.toString(), { req: inReq, resp: inResp, filename: path });
		inResp.send(html);
	});
}

exports.index =
function(inReq, inResp)
{
	console.log("stations index");
	render(inReq, inResp, "home.html");
};
