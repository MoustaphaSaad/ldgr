project "ldgr"
	language "C++"
	kind "SharedLib"
	targetdir (bin_path .. "/%{cfg.platform}/%{cfg.buildcfg}/")
	location  (build_path .. "/ldgr/")

	files {"include/**.h", "src/**.cpp"}

	includedirs {
		"include/",
		cpprelude_path .. "/include"
	}

	links {"cpprelude"}

	if os.istarget("linux") then

		buildoptions {"-std=c++14", "-Wall", "-fno-rtti", "-fno-exceptions"}
		linkoptions {"-pthread"}

		filter "configurations:debug"
			linkoptions {"-rdynamic"}

	elseif os.istarget("windows") then

		if os.getversion().majorversion == 10.0 then
			systemversion(win10_sdk_version())
		end

		buildoptions {"/utf-8"}

		filter "configurations:debug"
			links {"dbghelp"}
	end

	filter "configurations:debug"
		targetsuffix "d"
		defines {"DEBUG", "LDGR_DLL"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG", "LDGR_DLL"}
		optimize "On"

	filter "platforms:x86"
		architecture "x32"

	filter "platforms:x64"
		architecture "x64"