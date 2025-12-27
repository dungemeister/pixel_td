# Pixel TD

2D Pixel Tower Defence probe "game" with DOD and ECS like architecture

## Dependencies

* **SDL3 v3.2.20** for backend render
* **SDL_image v3.2.0** for sprite handling
* **SDL3_ttf v3.1.0** for font handling

Dependencies added to the project with .so and includes and linked via cmake functionality.


## Building

Build system of current project is Cmake. To build project use default cmake workflow as follow:

```bash
    mkdir build
    cd build
    cmake -S ..
```

## Controls

|Button|Meaning|
|:---|:---:|
|SPACE|Pause the Game|
|LMC on tower|Display tower info|
|LMC on enemy|Display enemy info|

## Images

Some demo images

![DemoGameplay](assets/demo/demo1.png)
![DemoGameplay](assets/demo/demo2.png)
![DemoGameplay](assets/demo/demo3.png)

## Tests



