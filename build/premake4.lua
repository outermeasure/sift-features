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
		rootdir.."/sources/include",
		libraries_dir.."/include"
	}
	
	libdirs{
		libraries_dir.."/lib"
	}


	flags {
		"StaticRuntime"
	}
	
	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

	configuration "Debug"
		debugdir (rootdir.."/binaries")
		defines { "DEBUG" }
		targetname "siftd"
		flags {"Symbols"} -- enable debug information
		links{
			"opencv_calib3d231d",
			"opencv_contrib231d",
			"opencv_core231d",
			"opencv_features2d231d",
			"opencv_flann231d",
			"opencv_gpu231d",
			"opencv_haartraining_engined",
			"opencv_highgui231d",
			"opencv_imgproc231d",
			"opencv_legacy231d",
			"opencv_ml231d",
			"opencv_objdetect231d",
			"opencv_ts231d",
			"opencv_video231d"
		}

	configuration "Release"
		debugdir (rootdir.."/binaries")
		defines { "NDEBUG" }
		targetname "sift"
		links{
			"opencv_calib3d231",
			"opencv_contrib231",
			"opencv_core231",
			"opencv_features2d231",
			"opencv_flann231",
			"opencv_gpu231",
			"opencv_haartraining_engine",
			"opencv_highgui231",
			"opencv_imgproc231",
			"opencv_legacy231",
			"opencv_ml231",
			"opencv_objdetect231",
			"opencv_ts231",
			"opencv_video231"
		}