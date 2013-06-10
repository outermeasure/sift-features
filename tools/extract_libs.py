"""
Script used for extracting a list of debug and release
filenames from a directory filed with .lib files

usage:
	extract_libs <path>
"""
import sys
import os

def isLibFilename( filename ):
	return filename[-4:] == ".lib"

def isDebugFilename( filename ):
	return filename[-5] == "d"

def printList( title, list ):
	print title
	for file in release_libs[:-1]:
		print "\"" + file[:-4] + "\"" + ","
	print "\"" + release_libs[-1][:-4] + "\""

	print ""

release_libs = []
debug_libs = []

if __name__ == "__main__":
	for file in os.listdir( sys.argv[1] ):
		if isLibFilename( file ):
			if isDebugFilename( file ):
				debug_libs.append( file )
			else:
				release_libs.append( file )

	printList( "Release Libraries", release_libs )
	printList( "Debug Libraries", debug_libs )