local common = { }

local glob = require "tundra.syntax.glob"

Build {
	Units = function()
		Program {
			Name = "m68k",
			Sources = { glob.Glob { Dir = "src", Extensions = { ".c", ".h" } } }
		}

		Default "m68k"
	end,
	SyntaxExtensions = { "tundra.syntax.glob" },
	Configs = {
		Config { Name = "macosx-clang", Inherit = common, Tools = { "clang-osx" }, DefaultOnHost = "macosx" },
		Config { Name = "macosx-gcc", Inherit = common, Tools = { "gcc-osx" } },
		Config { Name = "win32-msvc", Inherit = common, Tools = { { "msvc-winsdk"; TargetArch = "x86" } } },
		Config { Name = "win64-msvc", Inherit = common, Tools = { { "msvc-winsdk"; TargetArch = "x64" } } },
		Config { Name = "linux-gcc", Inherit = common, Tools = { "gcc" } },
	},
}