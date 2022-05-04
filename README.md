AIPac

--------------
Plain English:
--------------

The Big Picture
---------------

A neural net controls PacMan's movements through the maze and you can control the types of inputs going into the neural net. 

There are 3 different representations (called AIType in the parameters file) for the maze:  Windowed, Global, and Vector.

1) In the Windowed representation, a window around PacMan with a specified radius is considered.  For a radius of 4, this results in four tiles in each direction around him, plus the two tiles in each direction that he occupies, or 4+4+2 (4 to the right, 4 to the left, 2 for PacMan) left/right and up/down. This gives 100 total tiles for a radius of 4.  Larger radius values make this number go up quickly:  radius = 5 gives 144, radius = 6 gives 196.  I also have the tiles that PacMan covers removed from this set, so these quoted numbers minus PacMan's 4 are the final count of inputs.

Determining inputs is somewhat complex.  For dots and walls, you can have 1 or 2 inputs per tile depending on how the DotsWallsOnly and DotWallSingleInput parameters are set.  If DotWallSingleInput is set to 1, there will be one input with a -1 for a wall, 0 for empty, and 1 for a dot.  If DotWallSingleInput is set to 0 and DotsWallsOnly is set to 1, you'll get one input indicating a dot is present/absent.  If DotsWallOnly is set to -1, dots will be ignored and an input will indicate the presence/absence of a wall.  If DotsWallOnly is set to 0, you'll get one input for dot presence/absence and one input for wall presence/absence.

For power pellets, if PelletInputs is set to 1, you'll get one input per tile indicating the presence/absence of a power pellet.

Ghosts can have 1 or 2 inputs per tile .  If SingleGhostInput is set to 1, you get 1 input that is either -1 (hunting ghost), 0 (no ghost on that tile), or 1 (blue ghost).   If SingleGhostInput is 0, you'll get two inputs, one indicating presence/absence of a hunting ghost and one indicating the presence/absence of a blue ghost.

The result is, for a radius of 4 you have 96 tiles.  You could have as many as 5 inputs per tile (2 for ghosts, 1 for power pellets, and 2 for dots/walls) or as few as 1 (ignore ghosts and power pellets and one input for dots/walls combined).

Last of all, if the window extends outside the boundaries of the maze, each input will be set to 0.
  

2)  The global representation greatly reduces the inputs, but also greatly reduces the information available.  The neural nets can get the (X,Y) position of the ghosts and their state (hunting, blue, dead), (X,Y) position of the power ups and their state (available or already eaten), PacMan's (X,Y) position, walls directly around PacMan, the number of dots on each side of PacMan, and the direction in which the most dots can be found.  A maximum of 38 inputs can be used, and again you can control which ones are used from the parameters file.


3)  The Vector representation is an experiment that I did on changing the nature of the inputs.  Walls are treated as they were with the Windowed representation with a 2 radius window giving 96 inputs.  PowerUps can provide 2 inputs - the distance and angle to the powerup if it is available.  If it has already been eaten, the inputs are (0,0).  Each ghost can provides 3 inputs - angle, distance and state of the ghost.  The entire field of dots can supply 2 inputs - the distance and angle to the centroid of all the dots remaining.


4) The Console representation tries to represent how a human player might see the board.  Inputs are developed in layers with each layer consisting of 968 inputs representing each tile in the 32 X 29 game board.  The layers are:  walls (each tile having a wall segment receives a -1), dots (each tile having a dot receives a 1), powerups (each tile having a powerup receives a 50), ghosts (each tile having a ghost in hunt mode receives a -200 while blue ghosts receive 200), and PacMan (each tile with a PacMan receives a 1).  All tiles not covered by one of the items described receives a 0.  The values used approximately reflect the score values of each game item.  

Two methods of combining these neurons are available:  AllLayers concatenates all the nodes sequentially in the order described above creating a vector of 4640 inputs when ghosts are present, 3712 with no ghosts.  Flatten combines the walls, dots, powerups, and ghosts into one layer.  The layers for walls, dots, and powerups are actually complementary and have no collisions.  Ghosts are added last in order to ensure the presence of ghosts in any state is represented.  The PacMan layer is kept independent and concatenated to the end of the flattened layers creating 1856 inputs.


--------------


At each step that PacMan takes, the neural network is evaluated. The net has 4 outputs, one for each direction (up, down, left, right). The output with the highest value gives the direction that PacMan takes.

In the absence of ghosts, the game map is deterministic so yes, PacMan will take the same path when using the same neural network. The ghosts have a strong random component to their movements, so adding these will cause some random variation in PacMan's movement from run to run.

The Neural Nets are evolved using an Evolutionary Computation process. The fitness I'm using is the score that PacMan gets for one run through the maze. The time one run can take is limited (you can also control this in the parameters file). The goal is to maximize the score that PacMan gets.  In the absence of ghosts, and thus a deterministic PacMan, one evaluation is performed.  In the presence of ghosts, multiple evaluations are made for any single neural net and the results evaluated by averaging, taking the minimum or the median.  This helps to prevent a lucky run from dominating the score for that network.

As of 26 March 2017, I have added the ability to penalize the fitness for the number of connections in the network.  When PenalizeLinks is set to 1, the score is divided by the number of linkages between neurons found in the network.  The hope is to encourage accurate but less complex networks.

I've found it most interesting to look at PacMan's navigation early in the process and compare that to what happens after it has run for a while. No surprise that early in the process PacMan's movements are more or less random - often he just drifts one direction or the other, hits a wall, and stops. Then after it has run for a while and his score is over 1000, his behavior is much more interesting. So far the maximum score I've seen (with no ghosts) is about 2600 out of a maximum of 3040.


How to get PacMan to run through the maze
-----------------------------------------

Yes, you can opt to have the PacMan run after each generation using the best neural net thus far. To get this to work, while one of the application's windows are activated, press the 'B' key.  The title bar of the game window will say "Active" if it will have a moving PacMan after this generation is done, otherwise it will say "Sleeping."  After the current evolutionary step, PacMan will become active and run through the maze under the control of the current best neural net. You can turn it off - go back to having PacMan not run - by pressing the 'B' key again.

BE PATIENT after you press the 'B' key as the evolutionary step will have to complete before PacMan runs and sometimes this will take a while depending on how many inputs you have and how large the population is. Also, if you get impatient and hit the 'B' key a second time, you will end up toggling the display off so you'll be back where you started. Just hit the 'B' key once and look for "Active" or "Sleeping" in the title bar for that window.



----------------
Original README:
----------------
AIPac was developed in order to examine (play with) evolutionary training of a PacMan controller.  I've tried to develop the simulator in as open a form as possible in order to allow modifications to the maze, the graphics, etc.  I make no warranty on the quality of the code.  I refer to AIPac and PMAI somewhat interchangably, so be forewarned.

The files/distributions are as follows:

AIPac_README.txt - this file


AIPac_win32exe.zip:
This contains just an exectuable with the minimum files necessary to get it to work.  The files are:
	PMAI.exe - the executable
	AIPac.param - a parameters file within which you can control the behavior of the executable.  I've tried to make it self-explanatory.
	Resources/ - this directory contains the graphics and mazes I used for the simulator.  It should be fairly flexible to allow changes, but it may not be intuitive.
	
	
AIPac_code_win32.zip
	This contains all the code used to develop the application along with the Resources listed above.  I used Code::Blocks as my IDE, so I retained the project as-is.  My version of Code::Blocks is 4639 which is current as of late 2007.  I also used GCC 3.4 to compile the code.  I'm certain you can use other IDEs (such as MSVC++), but I've opted for open source.
	NEAT/ - This directory contains the code for evolutionary control of neural networks.  NEAT is Neuro-Evolution through Augmenting Topologies, developed by Dr. Kenneth Stanley.  (http://www.cs.ucf.edu/~kstanley/)  This version of NEAT was developed by Mat Buckland of AI-Junkie fame (www.ai-junkie.com) and he has given me permission to include it with my distribution.  (Thanks, fup!)  I have used his code with minimal modifications, and as a result, it is a bit of a hack, especially my modified CController.cpp file. 
	Resources/  - same as above.  The graphics and maze maps necessary for the executable.
	Utilities/  - my collection of programming utility classes used in this project.  I tend to use these a lot in my projects.
	
	
	
-------------------	
Additional Details:
-------------------

Here are some additional details related to AIPac that might come in useful.  I've tried to put the details in the order of interest and those pieces that will get you up and running the fastest.

Remember that the Resource directory needs to be present.  This has all the graphics, mazes, etc. needed for the simulator.  You can control most (all, I think) of the details of where the various files are found within the parameters files, AIPac.param, but I recommend leaving things as they are until you're familiar with the program.

While the simulator is running, if you press the 'B' key, this will toggle displaying the current best PacMan controller after each generation.  Remember that there will be a delay in displaying the best controller due to the fact that generations take a while to process.

Currently, I have the parameters set up for a rather lengthy evolution step.  If you want to see some action more quickly, try changing the following parameters in AIPac.param:

Replicates	- Typically I have this set to 5 which means that each PacMan controller will be tested 5 times and a measure of performance assessed across the multiples.  Set this to a smaller number and things will run faster.  The purpose of this is to account for the random nature of the ghosts.  If the ghosts are not present, set this to 1 since that type of simulation is deterministic.

PopSize 	- Obviously, reducing population size will make things faster, although potentially less interesting.

AIType		- I have this set to Windowed which defines a window around PacMan, as described in the readme file.  Setting this to Global will be a very different type of AI.  Specifically, the inputs defined in the section following AIType setting will be used rather than a window around PacMan.  This will shrink the number of inputs significantly, but I've found it to be less interesting.

I've tried to be as descriptive as possible in the AIPac.param file.  It should be largely self-explanatory.


If there are other questions or comments, please leave a message in the forum.


---------------	
Update Details:
---------------

v0.1.8 updates - March 2017

Added a console representation for the neural network.



v0.1.7 updates - March 2017
---------------------------

Six years gone and I've just now gotten back into this.  At this stage, I've only added a few things.

The most significant addition is to add on option to penalize the fitness by the number of links in the network.  If PenalizeLinks is set to 1, then PacMan's score is divided by the number of links or connections between neurons in the network.

With the addition of PenalizeLinks, I thought it would be nice to see the number of links, so I added the number of links to the monitor window as well as the number of neurons.  These are displayed for the best genome found thus far.

I should also point out that 6 years later, I was able to open the project in the latest version of CodeBlocks, compile, and run with no problems.  It might be possible that you need to ensure the various g++ libraries match architectures (32-bit vs 64-bit), but my test worked fine on Windows 7 with CodeBlocks 16 and a MinGW install of g++.


v0.1.6 updates - January 2011
-----------------------------

A major bug that I had shelved and forgotten was fixed:
Occasionally (often) the program would hang and become non-responsive.  This was found to be due to the way the gate of the ghost cage was controlled.  With no ghosts or just one ghost, no problems, but with multiple ghosts the case in which ghosts were using the gate simultaneously led to the possibility that the gate would close while a ghost was part way through.  This appears to be fixed now.

Code was added to the shutdown procedure to ensure the process was halted when any window was closed (using the window's X or close button).  This seemed to be more of a problem for Play mode.

README (this file) was updated and expanded to better describe the intention and operation of the application.

I found that more often than not, the score obtained during visualization did not match the score obtained during training.  In my efforts to fix this, I found that the NEAT code improperly used pre-increment operators.  In particular, this was done while parsing/updating vectors such that the first element in the vector was not processed.  I changed all instances of pre-increment operators in loops to post-increment operators, which fixed a few other oddities (number of bars in the species indicator bar not matching listed number of species, inconsistent outputs from neural nets, etc.).  The heart of the problem was found to be the way I was handling updates of PacMan's speed in WorldController::MovePacMan().  During training, the flag 'update' was always true but during visualization, it would be false 75% of the time.  I removed the dependency on this flag and allow the updates to always happen just as in training.  This seems to have solved the problem.  Much of the code I used for debugging consists of writing out log files.  I left this code in place for future reference should it be needed, but commented it out. 


v0.1.5 updates
--------------

Vector-based inputs implemented.  Walls are input as a window.  Pellet inputs provide a distance and angle to each power pellet, ghost inputs provide distance, angle, and state (-1 = hunting, 1 = blue) to each ghost, dot inputs show distance and angle to the centroid of all remaining dots.  All vectors assume PacMan as the origin.

EnforceIntersections was added to the control parameters.  If set to 1, PM will only make a directional change at an intersection.  If set to 0, PM can change direction at will.  There's a bit of a speed up with this as well as enforcing smooth paths.

**Verify proper scoring for dots - the possible maximum score should be 284 dots * 10.  (Ghosts and Pellets seem to be OK.)**
I found that at certain intersections mulitple dots are removed simultaneously due to a perceived overlap of the bounding box and dot tiles, but only one produced a score.  A modification was made to count how many dots are removed for any particular PacMan location and the score adjusted appropriately.  MapController::ModifyTileByPixel

The visualization delay was linked to the parameters file.  Larger numbers cause the visualization to run more slowly so that it is easier to watch.  This does not affect the actual GA portion, just the visual of PacMan running through the maze.  The same delay is linked to the playable mode option.

A user playable option is now available.  The parameter Mode can be set to Evolve to run an evolutionary simulation, or to Play for a Keyboard playable version.  Use the arrow keys to move PacMan around the maze.  All the other options remain the same.  

**Fix the gate on the ghost cage - it doesn't redisplay and it may not be reset.**
  A forced redraw of the entire maze was implemented any time the gate is opened or closed.  This does not appear to slow the process whatsoever and in fact I can force a redraw at every frame without any noticable performance losses.  This is a much easier fix than to implement specific drawing and erasing of the gate, or modification of the maze graphics.



v0.1.4 Updates
--------------

A few bugs were fixed.  Specifically, some issues with whether PowerPellets and Ghosts were visible or not were addressed.  

The ability to ignore walls or dots was added to the Windowed AI.  In the parameters file the following keywords control this as follows:

DotWallSingleInput - If set to 1, the input will be -1 for a wall, 0 if empty, and 1 for a dot.  If set to 0, there will be two sets of inputs - one for dots one for walls.

DotsWallsOnly - If set to 0, both dots and walls will be used for inputs.  If set to -1, only the walls will be seen.  If set to 1, only the dots will be seen.



v0.1.3 UPDATES:
---------------

Windows now update during the evolution step thus preventing an freezing of the application during one epoch.  

The console window now tells you whether PacMan will run or not after the current epoch and has (almost) instant feedback from pressing 'B'.

Small optimizations to speed up the simulation.

Ghosts now allowed to reevaluate their direction after PacMan eats PowerPellet.  Hopefully this will discourage chance collisions forcing PacMan to be more active in pursuit.

When the console window was running a spontaneous crash would occur the seemed to stem from the Windows message queue.  A DoEvents() function was implemented to allow window events to be processed while the Console was active.  This seems to have solved the problem.




	
 
	
	
	
	
