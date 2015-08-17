# Podtique Embedded Webserver

Configuration of a Podtique player is done via web browser pointed at the device.

The web application is written in node.js and Express, and uses dbus to communicate with the podtique radio process. It uses SocketIO to push updates to the client (e.g. Wi-Fi SSID list changes).

Express routes requests. EJS is the templating engine for web pages. There is a REST API for some operations, like configuring the radio spectrum. The REST API is implemented in app.js

The server attempts to determine the Wi-Fi state and available netowrks, and configures the radio’s Wi-Fi. This is currently unsupported on OS X.

## Launching the Server

### Configure DBUS

DBUS operates slightly differently between OS X and Linux.

On OS X, you need to have set the `DBUS_LAUNCHD_SESSION_BUS_SOCKET` environment variable. This is actually set by launchd, using the technique described [here](https://github.com/brianmcgillion/DBus/blob/master/README.launchd).

On Linux, the app depends on the `DBUS_SESSION_BUS_ADDRESS` environment variable being set. I’m not entirely clear on the right way to set this. It’s discussed [here](http://dbus.freedesktop.org/doc/dbus-launch.1.html#automatic_launching).

### Configure the Server

The server is configured via `podtique/ui/config.js`. This file looks for environment variables.

<table>
	<tr><th>Variable</th><th>Use</th></tr>
	<tr><td>PODTIQUE_CONTENT</td><td>Path to the content files (MP3s)</td></tr>
	<tr><td>RADIO_SPECTRUM_PATH</td><td>Path to the spectrum.json file describing the station playlists.</td></tr>
</table>

### Launch the Server

Change to the `podtique/ui` directory, and type

	$ node app.js

