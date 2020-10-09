# Plugin Template

Is this folder you will find a simple plugin example for the rp-can-low-level binding.

The json file defines a few signals that may be found on the can bus.
The cpp file implements a custom decoder, take it as an example
to understand how to make your own decoder.

## Prerequisites

Install the rp-can-low-level and the devel version if you haven't yet:
```bash
dnf install rp-can-low-level rp-can-low-level-devel
```

## Generate your plugin

First, install the generator:
```bash
dnf install rp-low-level-can-generator
```

Generate your plugin:
```bash
can-config-generator -m you_config_file.json -o generated-plugin.cpp
```
## Build and Install your plugin

To be able to build your plugin, change ```set(TARGET_NAME generated_plugin)``` in
the CMakeLists.txt with your plugin's name.

At the root of your plugin project (replace "generated-plugin" by the name of
your plugin).

```bash
mkdir build && cd build
cmake ..
make generated-plugin
make install_generated-plugin
```

Now activate the plugin in the can-low-level.
To do so, edit the control-rp-can-low-level.json file to
make it look like the example down below. This file should be located in
/usr/local/rp-can-low-level/etc/.
```json
{
	"$schema": "",
	"metadata": {
		"uid": "Low Can",
		"version": "2.0",
		"api": "low-can",
		"info": "Low can Configuration"
	},
	"config": {
		"active_message_set": 0,
		"dev-mapping": {
			"hs": "can0",
			"ls": "can0",
			"j1939": "can0"
		},
		"diagnostic_bus": "hs"
	},
	"plugins": [
		{
			"uid": "generated-plugin",
			"info": "custom generated plugin",
			"libs": "generated-plugin.ctlso"
		}
	]
}
```

Congratulations, your plugin is now ready to be used.
