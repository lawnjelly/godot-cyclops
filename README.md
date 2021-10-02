## NOTE - Cyclops is deprecated as of Godot 3.4, Rooms & Portals is now available as part of the standard Godot engine.

# godot-cyclops 0.12
Portal occlusion culling for Godot Engine 3.2 _(Feb 2nd 2021)_

Cyclops is a simple room / portal occlusion culling system for Godot Engine, written as a module. It must be compiled with the engine (soon I hope to make some precompiled builds).

## History
I originally wrote LPortal a year and a half ago, which is a fully featured rooms / portals system, however I decided to rewrite a simplified cut down version that focuses on being easy to use. I'm aiming to make Cyclops pretty much fully usable without any scripting, all from the Godot IDE.

Quite a lot is cut out from the original LPortal, most significantly, Cyclops does not fully support dynamic lights and shadows. It is designed to be used with baked lighting, such as lightmaps. Luckily, we have recently merged JFons more capable lightmapper into Godot core, so this has become a more practical undertaking. In addition, you can also use this module in conjunction with my lightmapping module, LLightmap. Indeed you are likely to get the best results in terms of performance using LLightmap rather than the core lightmapper.

## Installation
1) Use the instructions here to download and compile Godot: \
https://docs.godotengine.org/en/3.2/development/compiling/index.html

2) Download this repository, and create a new folder in the modules directory called 'cyclops'. Unzip the repository into this folder.

3) Recompile. Note that for exports you will need to compile the export template for the platform you want to export to (e.g. android if you are intending to export to android).

## Tutorial
https://www.youtube.com/watch?v=6O1j6CMVdGA

## Components of Cyclops
Cyclops introduces 3 new Godot nodes, LRoom, LPortal and LRoomManager.

### LRoom
The most basic unit of the system is the room. In a first person shooter this would roughly correspond to real world rooms. In an outdoor game a room might be part of a valley for instance. There is one caveat - rooms must be convex. If you want to make a concave area you should build it out of two or more convex LRooms.

The convex hull forming the room is usually defined by the geometry, in this case MeshInstances, that you place as children and grandchildren of the room. You can also optionally specify the convex hull manually by having a MeshInstance child with the name prefix `bound_`. The convex hull is used to decide when an object (such as the camera) is within a room, because it can be done simply with a series of plane checks.

The MeshInstances you place within rooms should be _static_, that is, they are not expected to move as you play the game level.

Rooms start their lives simply as a Spatial. But they have a very specific naming convention, they should start with the prefix `room_`, followed by the name that want to give the room. e.g. `room_lounge`, or `room_kitchen`. They must go through a conversion process (see later) to turn them into LRooms that have been prepared for rendering.

### LPortal
When you are in a room, you can see pretty much everything that is in the room in front of you. The fun comes when we want to look into adjacent rooms. Instead of defining occluders, such as walls, to block visibility, we take the opposite approach, and define portals, which are like doors or windows that we can see through. All other walls of the room are considered opaque.

A portal is essentially just a convex polygon, with all the points approximately in the same plane. In many cases a Godot plane can be used as a portal. Portals start their life as a simple MeshInstance, with another naming convention: Their prefix should be `portal_` followed by the room that they link (see) into. e.g. `portal_kitchen`.

Portals should be children of rooms. Typically there will be one or more portals leading out of each room. Incidentally, you only need to create one portal between each pair of linked rooms. The system will automatically make it two way.

Portals must also be converted to become ready for use - they are converted from MeshInstances to LPortals.

_Note : Godot does not allow two or more nodes in the scene tree to share the same name. This is a problem when you want more than one portal to lead into a room. This is solved using a wildcard in the name. Anything including and after the character '*' will be ignored. So you can have e.g. `portal_lounge*1`, `portal_lounge*2`, and this will be interpretted as `portal_lounge` in both cases._

### LRoomManager
Finally the LRoomManager is the brains of the whole operation. It contains the settings you wish to use, and enables you to convert the level to prepare it for use, either in the editor or at runtime. In order for the system to work, the room manager needs you to assign some nodes:

* `Rooms` - this is the roomlist, the Spatial you have decided to hang your rooms as children from.
* `Camera` - this is the camera which occlusion will be calculated from.
* `Dobs` - this stands for dynamic objects. This allows dynamic objects to be occluded in the system, see below.

## Dobs
To make things as simple as possible, you can assign a branch of the scene tree under which you will place all your dynamic (moving) MeshInstances. They will automatically be found and occluded as part of the system. It is as simple as that. The only gotcha is that this branch should NOT be in one of the rooms (which are for static objects only).

## Typical Usage
Most of the effort involved is in setting up your scene. You should group your static objects into rooms, children of the node you want to be your roomlist. Rooms will typically contain walls, floors, ceilings, tables, chairs etc. Maybe there is no ceiling if you can see the sky above.
You would then place portal meshes over windows / doors. This can be done either in the Godot IDE, or in a modelling package such as Blender. Simple plane MeshInstances will often do the job. Remember to name the rooms and portals correctly, as described earlier.

Make sure to save your work, and keep backups. It is often useful to work on an unconverted scene (i.e. before LRooms and LPortals have been converted), and do conversion as you go along, just for testing and final exports.

To convert the scene for occlusion culling, select the LRoomManager in the IDE, and a new button at the top of the 3d window should appear, titled `Convert Rooms`. Press this. This will log some output text to describe the process (look for any errors), and hopefully you should end up with some LRooms and LPortals.

You can preview the occlusion system in the editor. Make sure the camera you want to use is assigned (in the room manager), and switch the LRoomManager to `Active` in the inspector. Now if you move or rotate the camera, you should see objects occlusion culled. If you deactivate the room manager they will all be shown again.

## Final versions in game
You can either load your scene file for the final game prior, or post conversion, however, you will always need to run convert again when you load the level, because some of the necessary data is not saved, and needs to be created immediately prior to use. You would do this in game by calling the `rooms_convert` function of the LRoomManager.

One last thing, when changing level in your game, be sure to call `rooms_clear` prior to unloading the previous level, and before closing the game (although you may be able to get away without this last step). This both clears out the data and memory used by the previous level, but it is also important to do because Cyclops maintains unsafe pointers to objects in memory, for speed, and clearing these will prevent any risk of crashes.

