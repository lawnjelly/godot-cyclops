# godot-cyclops 0.10
Portal occlusion culling for Godot Engine 3.2
Jan 31st 2021

Cyclops is a simple room / portal occlusion culling system for Godot Engine, written as a module. It must be compiled with the engine (soon I hope to make some precompiled builds).

## History
I originally wrote LPortal a year and a half ago, which is a fully featured rooms / portals system, however I decided to rewrite a simplified cut down version that focuses on being easy to use. I'm aiming to make Cyclops pretty much fully usable without any scripting, all from the Godot IDE.

Quite a lot is cut out from the original LPortal, most significantly, Cyclops does not fully support dynamic lights and shadows. It is designed to be used with baked lighting, such as lightmaps. Luckily, we have recently merged JFons more capable lightmapper into Godot core, so this has become a more practical undertaking. In addition, you can also use this module in conjunction with my lightmapping module, LLightmap. Indeed you are likely to get the best results in terms of performance using LLightmap rather than the core lightmapper.

