
#Zesmi#
##Zeff's Extremely Scalable Minecraft Implementation#

###Aim#
Aiming to develop a distributed minecraft implementation like a load balanced website.  One machine takes a request and routes though to controllers who route to data processor who formulates response and passes back to the connection handler.

Intending to make the controllers and child the same process except in configuration if there are any children configured the process becomes a master instead, if there are no children the master also acts as a child.

Splitting the service into multiple processes like this will take better advantage of multicore cpus and allow for scaling to cloud computers in very large instances.  The bottleneck is likely to be interprocess communication though expect that in a cloud environment you would add gigabit interfaces to connect directly to important systems to a node.</p>

###Dependencies#
At this point it's only for windows though I'm trying to prepare for multiplatform support in the source.

###Install#
Download and compiled all objects into a shared library (except UnitTests.cpp)  UnitTests cpp compiles on its own and links to the dll.

###License#
[GPLv3](https://github.com/zeffrin/Zesmi/blob/master/Documentation/gpl-3.0.txt)

###Documentation#
[[Components]]

###Author#
Ben Stephens

###Contact#
Ben Stephens ([mailto://zeffrin@gmail.com](zeffrin@gmail.com))

###Download#
You can download this project in either [http://github.com/zeffrin/Zesmi/zipball/master](zip) or [tar](http://github.com/zeffrin/Zesmi/tarball/master) formats.

You can also clone the project with [Git](http://git-scm.com) by running:
$ git clone git://github.com/zeffrin/Zesmi*

get the source code on GitHub : [zeffrin/Zesmi](http://github.com/zeffrin/Zesmi)

