# Fluvit

##### A hydraulic erosion simulation

Gahwon Lee

![ ](https://thumbs.gfycat.com/AmpleCavernousAngelwingmussel-size_restricted.gif)

### Compiling

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j8
```

### How to Run
* `./fluvit -h` for help
* `./fluvit path/to/png` to load heightmap
* `./fluvit` for random terrain


### Controls

Right click and WASD to fly

1,2,3 = different ways to visualize terrain

p to toggle playback

### Credits

* http://graphics.cs.brown.edu/games/IsoHeightfield/mcguiresibley04iso.pdf
* https://hal.inria.fr/inria-00402079/document
* http://old.cescg.org/CESCG-2011/papers/TUBudapest-Jako-Balazs.pdf
* 