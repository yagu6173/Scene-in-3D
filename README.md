# CSCI 5299 Computer Graphics - Homework 2: Scene-in-3D

## Program Overview

This program renders a 3D scene viewed in three projection modes: orthogonal, 
perspective, and first-person.

The scene features a grassy landscape with a highway and river crossing it, 
colorful houses, trees, and an airplane flying overhead. The first-person mode 
lets you walk along the highway and observe the surroundings at ground level.

## How to run

### In one step  
$unzip hw2.zip && make clean && make && ./hw2

### Step by step
1. `unzip`: $unzip hw2.zip
2. `build`: $make all or $make
3. `run`: $./hw2
4. `clean`: $ make clean  


## Keyboard Controls

### First Person  
* `f/F`: Move forward
* `b/B`: Move backward
* `Up/Down/Left/Right` arrow key: Look up/down/left/right

### Orthogonal/Perspective  
* `Up/Down` arrow key: Increase/decrese angle of elevation 
* `Left/Right` arrow key: Increase/decrese angle of azimuth

### Common keys  
* `0`: Reset view angles to default value
* `a/A`: turn axes on/off
* `Ese`: exit the window

## Implementation Details

- Orthographic and perspective share the same `th`/`ph` view angles for 
  consistency — rotation is consistent from one to the other
- First-person eye position is maintained as `(FV_Ex, FV_Ey, FV_Ez)` and 
  updated incrementally on each forward/backward step by legnth of 0.1 
- Door and window Z-fighting with the wall is resolved with `glPolygonOffset`


## Hours taken

- 6 hours in learning the concept of perspective and first person view
- 6 hours in coding, testing and improving the code


## Links
* Canvas: https://canvas.colorado.edu/courses/135873/assignments/2723109?module_item_id=7393123
* Github Repo: https://github.com/yagu6173/Scene-in-3D
