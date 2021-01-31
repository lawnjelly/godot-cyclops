# godot-cyclops 0.10
Portal occlusion culling for Godot Engine 3.2
Jan 31st 2021

Cyclops is a simple room / portal occlusion culling system for Godot Engine, written as a module. It must be compiled with the engine (soon I hope to make some precompiled builds).

## History
I originally wrote LPortal a year and a half ago, which is a fully featured rooms / portals system, however I decided to rewrite a simplified cut down version that focuses on being easy to use. I'm aiming to make Cyclops pretty much fully usable without any scripting, all from the Godot IDE.

Quite a lot is cut out from the original LPortal, most significantly, Cyclops does not fully support dynamic lights and shadows. It is designed to be used with baked lighting, such as lightmaps. Luckily, we have recently merged JFons more capable lightmapper into Godot core, so this has become a more practical undertaking. In addition, you can also use this module in conjunction with my lightmapping module, LLightmap. Indeed you are likely to get the best results in terms of performance using LLightmap rather than the core lightmapper.

## Components of Cyclops
Cyclops introduces 3 new Godot nodes, LRoom, LPortal and LRoomManager.

### LRoom
The most basic unit of the system is the room. In a first person shooter this would roughly correspond to real world rooms. In an outdoor game a room might be part of a valley for instance. There is one caveat - rooms must be convex. If you want to make a concave area you should build it out of two or more convex LRooms.

The convex hull forming the room is defined by the geometry, in this case MeshInstances, that you place as children and grandchildren of the room. The convex hull is used to decide when an object (such as the camera) is within a room, because it can be done simply with a series of plane checks.

The MeshInstances you place within rooms should be _static_, that is, they are not expected to move as you play the game level.

Rooms start their lives simply as a Spatial. But they have a very specific naming convention, they should start with the prefix `room_`, followed by the name that want to give the room. e.g. `room_lounge`, or `room_kitchen`. They must go through a conversion process (see later) to turn them into LRooms that have been prepared for rendering.

### LPortal
When you are in a room, you can see pretty much everything that is in the room in front of you. The fun comes when we want to look into adjacent rooms. Instead of defining occluders, such as walls, to block visibility, we take the opposite approach, and define portals, which are like doors or windows that we can see through. All other walls of the room are considered opaque.

A portal is essentially just a convex polygon, with all the points approximately in the same plane. In many cases a Godot plane can be used as a portal. Portals start their life as a simple MeshInstance, with another naming convention: Their prefix should be `portal_` followed by the room that they link (see) into. e.g. `portal_kitchen`.

Portals should be children of rooms. Typically there will be one or more portals leading out of each room. Incidentally, you only need to create one portal between each pair of linked rooms. The system will automatically make it two way.

Portals must also be converted to become ready for use - they are converted from MeshInstances to LPortals.

### LRoomManager
Finally the LRoomManager is the brains of the whole operation. It contains the settings you wish to use, and enables you to convert the level to prepare it for use, either in the editor or at runtime. In order for the system to work, the room manager needs you to assign some nodes:

* `Rooms` - this is the roomlist, the Spatial you have decided to hang your rooms as children from.
* `Camera` - this is the camera which occlusion will be calculated from.
* `Dobs` - this stands for dynamic objects. This allows dynamic objects to be occluded in the system, see below.

## Dobs





