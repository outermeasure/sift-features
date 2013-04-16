newoption{
	trigger = "destination", 
	description = "Destination folder"
}

-- Root directory of project checkout relative to this .lua file
rootdir = ".."
libraries_dir = rootdir.."/libraries"

-- Create the solution and make it active
solution "sift-features"
	configurations{ "Debug", "Release" }
	location( _OPTIONS["destination"] )


-- Create the application project
project "sift-features"
	language "C++"
	kind "WindowedApp"
	targetdir( rootdir.."/binaries" )
    
	files{
		rootdir.."/sources/include/**.h",
		rootdir.."/sources/src/**.cpp"
	}
	
	includedirs{
		rootdir.."/source/include",
		libraries_dir.."/include"
	}
	
	libdirs{
		libraries_dir.."/lib"
	}


	flags {
		"StaticRuntime"
	}
	
	configuration "Debug"
		debugdir (rootdir.."/binaries")
		defines { "DEBUG" }
		targetname "siftd"
		flags {"Symbols"} -- enable debug information
		links{
			"libjpegd",
			"libpngd",
			"libtiffd",

			-- a library that reads and writes OpenEXR images
			"IlmImfd",

			-- JasPer Project is an open-source initiative to provide a free software-based reference implementation of the codec specified in the JPEG-2000 Part-1 standard
			"libjasperd",

			"opencv_core245d",
			"opencv_imgproc245d",
			"opencv_highgui245d",
			"opencv_ml245d",
			"opencv_video245d",
			"zlibd",

			"comctl32"
		}

	configuration "Release"
		debugdir (rootdir.."/binaries")
		defines { "NDEBUG" }
		targetname "sift"
		links{
			"libjpeg",
			"libpng",
			"libtiff",
			"IlmImf",
			"libjasper",
			"opencv_core245",
			"opencv_imgproc245",
			"opencv_highgui245",
			"opencv_ml245",
			"opencv_video245",
			"zlib",
			"comctl32"
		}