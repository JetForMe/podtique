var ejs = require("ejs");
var fs = require("fs");
var util = require("util");


function
render(inReq, inResp, inTemplate, inParams)
{
	var params = inParams || {};
	var path = __dirname + "/" + inTemplate + ".ejs";
	console.log(module.filename + " reading template " + path);
	fs.readFile(path, function(inErr, inData)
	{
		params["req"] = inReq;
		params["resp"] = inResp;
		params["filename"] = path;
		var html = ejs.render(inData.toString(), params);
		inResp.send(html);
	});
}

exports.index =
function(inReq, inResp)
{
	console.log("settings index");
	
	//	Figure out the network settings…
	
	var params = {};
	
	var os = require('os');
	var interfaces = os.networkInterfaces();
	Object.keys(interfaces).forEach(function(inInterfaceName)
	{
		interfaces[inInterfaceName].forEach(function(inInterface)
		{
			//console.log(inInterfaceName + ": " + inInterface.address + " " + inInterface.family.toLowerCase() + " " + inInterface.internal);
			if ('ipv4' !== inInterface.family.toLowerCase() || inInterface.internal)
			{
				return;
			}
			console.log(inInterfaceName + ": " + inInterface.address + " " + inInterface.family);
			params["addr"] = inInterface.address;
		});
	});
	
	params["ssid"] = "TsunamiN";
	render(inReq, inResp, "home.html", params);
};
