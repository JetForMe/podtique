var ejs = require("ejs");
var fs = require("fs");
var config = require("../../config");


function
render(inReq, inResp, inTemplate, inSpectrum)
{
	var path = __dirname + "/" + inTemplate + ".ejs";
	console.log("Reading template " + path);
	fs.readFile(path, function(inErr, inData)
	{
		var html = ejs.render(inData.toString(), { req: inReq, resp: inResp, filename: path, stations: inSpectrum });
		inResp.send(html);
	});
}

exports.index =
function(inReq, inResp)
{
	console.log("stations index");
	console.log("spectrum file: " + config.spectrumPath);
	fs.readFile(config.spectrumPath, 'utf8', function(inErr, inData)
	{
		if (inErr) throw inErr;
		var spectrum = JSON.parse(inData);
		console.log("Spectrum: " + spectrum);
		render(inReq, inResp, "home.html", spectrum);
	});
};
