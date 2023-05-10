
# Marquee: Scrolling Display of Useful Information

*Marquee* is an application that can display weather, 3D Printer status, messages, and other information on a monochrome LED Matrix panel. It is based on the [WebThingApp library](https://github.com/jpasqua/WebThingApp), which itself is based on the [WebThing library](https://github.com/jpasqua/WebThing) library. To understand how those libraries work in more detail, refer to their documentation. 

This application demonstrates the following:

* The framework to follow to create new apps
* How to create and use persistent settings for your app
* Screens
	* How to use screens that come with WebThingApp
	* How to create custom screens
	* How to get numeric input from the user
* Clients
	* How to use clients that come with WebThingApp
	* How to create custom clients
* How to use the plugin framework

## Dependencies

### Libraries

To build this application you will need to douwnload and install all of the libraries required by [WebThingApp library](https://github.com/jpasqua/WebThingApp) and [WebThing library](https://github.com/jpasqua/WebThing). In addition, you will need:

* [BPA3DPrintClients](https://github.com/jpasqua/BPA3DPClients): master


### Services

In addition to the services used by [WebThingApp library](https://github.com/jpasqua/WebThingApp) and [WebThing library](https://github.com/jpasqua/WebThing), *Marquee* may require API keys to connect to your 3D Printers

* **OctoPrint**:
	* If you are monitoring an OctoPrint-connected printer, you'll need the API key for it, which can be found in the OctoPrint Web UI.

<a name="structure"></a>
## File Structure 

Let's look at the files and directory structure of a typical `WebThingApp`. In this case the app is called `Marquee` and files often have the prefix `MQ`. Here is an overview of the directory structure.

````
Marquee
├── MQDataSupplier.[h,cpp]
├── MQSettings.[h,cpp]
├── MQWebUI.[h,cpp]
├── Marquee.ino
├── MarqueeApp.[h,cpp]
├── ...
├── data
│   ├── <html files that are specific to this app>
│   ├── plugins
│   │    └── 1_aio
│   └── wt
│   │   └── <html files from the WebThing library>
│   └── wta
│       └── <html files from the WebThingApp library>
└── src
    ├── hardware
    │   └── HWConfig.h
    └── screens
        ├── SplashScreen.[h,cpp]
        ├── HomeScreen.[h,cpp]
        └── ...
````

Description:

* The `Marquee` directory contains the source code for the app itself:

	* `Marquee.ino`: This is a bridge between the Arduino `setup()` and `loop()` functions and the app initialization and operation. It is boilerplate and there is no app-specific code in this module.
	* `MarqueeApp`: This is the core of the application. It is a subclass of `WTAppImpl` and `WTApp` which are part of the `WebThingApp` library.
	* `MQSettings `: Defines, internalizes, and externalizes the settings that are specific to this app.
	* `MQWebUI`: Provides app-specific web pages and functions. For example, the printer configuration page that lets your 3D printers and what to display. `WebThingApp` provides many other pages such as general settings, display settings, and weather settings.
	* `MQDataSupplier `: This module contains a dataSupplier function which gets plugged into the `DataBroker` to publish app specific information.
* The `data` directory contains contains all the files that will be written to the file system as part of the build process. There are four sets of files in the data directory:

	* At the root are HTML files that are used by any custom pages served up by the app's Web UI. For example, a custom home page. You may also place a settings.json file here if you want to have settings loaded into the app by default. Otherwise the user will need to configure the app settings when the device starts the first time.
	* The `plugins` subdirectory contains subdirectories for each plugin to be loaded. See the [plugin section](#plugins) for details.
	* The `wt` subdirectory contains HTML for the pages displayed by the `WebThing` Web UI pages. These are low level configuration items such as the hostname to use. You may also place a settings.json file here if you want to have settings loaded into the app by default. Otherwise the user will need to configure all the `WebThings` settings when the device starts the first time.
	* The `wta` subdirectory contains HTML for the pages displayed by the `WebUIHelper`. These are pages that are common to most apps like a page to configure display settings.


* The `src/screens` directory contains code for app-specific screens. Most apps will use the common screens provided by `WebThingApp`, but they will also typically provide at least one custom screen.

## Building and Using Marquee

To build *Marquee*, follow the instructions in the ["Building a WebThingApp"](../../README.md#building) section of the `WebThingApp` documention. This covers both the hardware and software aspects of the build process.

Once complete, you can set up your device following the instructions in the ["Setting up your device"](../../README.md#preparation) section. This will get you 90% of the way through the process, but you still need to configure settings that are specific to *Marquee*.

