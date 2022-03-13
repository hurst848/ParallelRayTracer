--// HOW TO USE THE RAY TRACER //--

// HOW TO RUN //
go to release folder
run the exe


// Menu Options //

1. Default Settings
	Renders a scene using some defaul values used in the creation of the ray tracer.
2. Network MultiThreading (NON-FUCNTIONAL)
	Would start a server for render clients to connect to and initalize a render over 
	the network, but it remains non functional due to time constraints. 
	DO NOT SELECT THIS OPTION
3. Thread Number Testing Suite
	Runs a testing suite designed to test how long, at various resolutions, it takes to 
	render a scene with a varible amount of threads (1-50 threads). Outputs 3 individual
	csv files for the results at each tested resolution.
4. AA Sample Testing Suite
	Runs a testing suite designed to test how long it takes to render a scene with a varible
	value of sampling per pixel in the anti alliasing process (Values 1 - 225). Outputs one csv f
	file containing the timing data, as well as all the generated frames in bitmap format 
	for qualatative anyalisis. The output is stored in a created directory called "AASampleTest".
5. Scatter Depth Testing Suite
	Runs a testing suite designed to test how long it takes to render a scene with a varible
	value of maximum depth in the ray scattering process (Values 1 - 225). Outputs one csv
	file containing the timing data, as well as all the generated frames in bitmap format 
	for qualatative anyalisis. The output is stored in a created directory called "ScatterDepthTesting".
6. Job Size Testing Suite
	Runs a testing suite designed to test how long, at various resolutions, it takes to 
	render a scene with a varible amount of pixels in each job (tested values defined in code). 
	Outputs 3 individual csv files for the results at each tested resolution.
7. Custom Render Setup
	Runs a render given user input parameters, instructions for use can be found bellow.


// Custom Render Setup //

Following are step by step instructions on how to make a custom scene, and use the ray tracer to
render it to the screen:

1. Create the Scence File
	- Some values for creating a custom scene may be difficult to aquire as the custom render
	  configurator is based of a serializeation of an in code generated scene.
	- The scene file should follow this format:

~res:
640, 480
~camera:
2.666667, 0.000000, 0.000000
-1.333333, -1.000000, -1.000000
0.000000, 0.000000, 0.000000
0.000000, 2.000000, 0.000000
~Scene:
~Sphere:
100.000000
0.000000, -100.500000, -1.000000
~Lambertian:
0.800000, 0.800000, 0.000000
~Sphere:
0.500000
0.000000, 0.000000, -1.000000
~Lambertian:
0.700000, 0.300000, 0.300000
~Sphere:
0.500000
-1.000000, 0.000000, -1.000000
~Metal:
0.800000, 0.800000, 0.800000
~Sphere:
0.500000
1.000000, 0.000000, -1.000000
~Metal:
0.800000, 0.600000, 0.200000

	- The value in res refurs to the resolution that the scene will be rendered at. For optimal
	  results, use the bellow list of resolutions for guarenteed functionallity, as they have been
	  tested to work with the default job size of 10x10 used in the custom setup:
		- 320, 240
		- 640, 480
		- 1280, 720
		- 1920, 1080
		- 2560, 1440
		- 3840, 2160
	- The values for the camera are as followed in order as listed in the above example:
		- Camera's horizontal value
			- x = aspectRatio * 2.0f
			- y = 0
			- z = 0
		- Camera's lowerLeftCorner value
			- For 16:9 aspect ratios:
				- -1.777778, -1.000000, -1.000000
			- For 4:3 aspect ratios:
				- -1.333333, -1.000000, -1.000000
		- Camera's origin value
			- Any valid vec3
		- Camera's vertical value
			- x = 0
			- y = 2.0f
			- z = 0
	- To start declaring a scene, use the ~Scene tag
	- To declare a Sphere, use the ~sphere tag and assign the following values:
		- Sphere's radius value
			- Any valid positive float
		- Sphere's origin
			- Any Valid vec3
		- Sphere's Material
			- Specifiy "~Lambertian:" or "~Metal:"
				- Specify any valid vec3 as a colour value

2. Select the Custom Render option
	- Select option 7 in the main menu

3. Input Valid file path to created Scene file
	- Path format should follow the example bellow:
		- C:\\Users\\username\\Desktop\\SceneFile.txt

4. Input valid samples per pixel value

5. Input valid depth value 
		