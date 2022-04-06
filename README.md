# Starbound Patch Helper

The Starbound Patch Helper is a tool intended to help automate the creation of patch files. It can be configured to ignore Starbound specific quarks in order to be more widely useful. It was made in my spare time over several weekends. Bugs, crashes, freezes, and other issues are entirely possible.

# What it does

Pull specific values out of "source" JSON files and write them to "intermediary" JSON files. This is entirely dependent on how the configs are set up and the "source" files supplied.

Create JSON patches from the "source" and "intermediary" files depending on configs. It is generally expected that some "intermediary" files will be modified before doing this, but some patch types may be generated without changing any of them.

# Usage

When ran directly it will prompt for inputs. It can also be run from the command line. Parameters can be used to entirely skip the need for user interaction.

# Supported non-standard JSON and JSON Patch features

Most of this is supported only because of how Starbound handles things, but some features are intentionally utilized elsewhere.
All of this was done without altering the underlying JSON parser. All of it can be disabled or ignored.

- Comments (`// ` and `/* */`) in JSON files.
- Newlines in JSON values.
- Patch operation sets to allow one patch file to separately fail or succeed patch applications.
- Inverse test operations to see if a value exists or not without regard for what the value is.

# Future plans

- Pre-built binaries for Linux.
- Pre-built binaries for Mac. I don't have a Mac. It will require cross compiling.
- More configuration options.
- More detailed crashes, including printing the path of a read file when parsing fails.
- Support for settings paths other than the default relative ones when running from the command line.
- Better documentation.
