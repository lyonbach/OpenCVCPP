workspace "DocumentScanner"
architecture "x64"
configurations
{
    "Debug",
    "Release",
}

project "DocumentScanner"
    location "."
    kind "WindowedApp" 
    language "C++"
    targetdir (".")

    files
    {
        "DocumentScanner.cpp"
    }

    links
    {
        "opencv_core",
        "opencv_highgui",
        "opencv_imgproc",
        "opencv_imgcodecs"
    }

    libdirs { "/usr/local/lib" }

    includedirs
    {
        "/usr/local/include"
    }

    

    filter "configurations:Debug"
    defines "FLE_DEBUG"
    symbols "On"

    filter "configurations:Release"
    defines "FLE_RELEASE"
    optimize "On"
