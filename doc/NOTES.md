# ofoscape

## To-do

- [x] Support for iChannel textures.
- [ ] Scene loading logic.
- [x] Pass FBO into scenes, instead of pre-allocating.
- [ ] Put this on Git.
- [ ] Make loadTextures caching (retrieve already loaded images by path).
- [x] Bind user-defined variables to shader.
- [ ] Fullscreen/UI toggle functionality.
- [ ] Shader hot-reload.
- [ ] Project hot-reload?
- [ ] Add MIDI support.

## Issues

- [x] Why does the `cwd` is at `bin`, and not `bin/data`? A: `ofDisableDataPath()`
- [ ] Add path checking before ofxShadertoy::load to avoid stray compilation errors.
- [x] Double-loading of scenes?
- [ ] Smoothing requires live update'ing of variables' states.

## Ideas

- Provide standardized OSC endpoints for built-in functions:
	- `/ofo/control/mouse <x> <y> <pressed>` (perhaps also a GUI toggle: "OSC Mouse")
	- `/ofo/scene/crossfade <val>`
	- `/ofo/scene/load <index> <deck>`
	- `/ofo/scene/next`: loads the next scene from the last loaded one
	- `/ofo/scene/prev`
	- Reject paths beginning with `/ofo` in input registration.

- Make a little dynamically generated "debug UI" for all inputs (view & set).
	- Make UI visibility toggeable via shortcut.
	- *Make a dynamic, scene-specific UI*, for all the bound variables (Synesthesia-like).

- Support other types of input variables? float, int, bool

## Notes-to-self

In OF, you basically should initialize everything after creating a window/context. Before that, some stuff will misteriously emit warnings or not work (GUI & OSC, respectively, for example).