# ofoscape

## To-do

- [x] Support for iChannel textures.
- [x] Scene loading logic (`/next`).
- [x] Pass FBO into scenes, instead of pre-allocating.
- [x] Put this on Git.
- [ ] Make loadTextures caching (retrieve already loaded images by path).
- [x] Bind user-defined variables to shader.
- [ ] Fullscreen/UI toggle functionality.
- [x] Shader hot-reload.
- [ ] Project hot-reload?
- [x] Add MIDI support.
- [x] Add global/macro variables for shaders.
- [x] Variable refactoring.
- [ ] Manager API cleanup (too many public scene-related API points).
- [ ] Refactor 'slot' to 'deck'.
- [ ] Skip rendering when scene invisible? Add as option/setting!
- [ ] Add support for "iMouse.x", "...y", "...z" (and others?).

## Issues

- [x] Why does the `cwd` is at `bin`, and not `bin/data`? A: `ofDisableDataPath()`
- [ ] Add path checking before ofxShadertoy::load to avoid stray compilation errors.
- [x] Double-loading of scenes?
- [ ] Smoothing requires live update'ing of variables' states.
- [ ] `/prev` scene loading?
- [x] LPF bug: input value should be kept separately.

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

- Support other types of input variables? float, int, bool...

- "4-channel mixer" layout + crossfader + assignable FX layer.
	- Create a "ofoShaderEffect" object: accepts N-textures and parameters, and affects/blends them.

## Notes-to-self

- Using [ofxMidi](https://github.com/danomatika/ofxMidi) under `/addons`.

- In OF, you basically should initialize everything after creating a window/context. Before that, some stuff will misteriously emit warnings or not work (GUI & OSC, respectively, for example).

## OF

### Bugs

- Had to patch `ofRunApp` to call `ofExitCallback`, otherwise the application wouldn't know that it's shutting down.

- `ofParameter<double>.addListener` fails to compile (but works fine for `float`). 