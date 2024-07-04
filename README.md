# Null
Null Engine


# Architecture

GameEngine/
├── Core/
│   ├── MainLoop.cpp
│   ├── GameObject.cpp
│   ├── Component.cpp
│   └── SceneManager.cpp
├── Input/
│   ├── InputManager.cpp
│   └── InputMappings.cpp
├── Rendering/
│   ├── Renderer.cpp
│   ├── Shader.cpp
│   ├── Texture.cpp
│   └── Camera.cpp
├── Physics/
│   ├── PhysicsEngine.cpp
│   └── CollisionDetection.cpp
├── Audio/
│   ├── AudioEngine.cpp
│   └── SoundManager.cpp
├── Resources/
│   ├── ResourceLoader.cpp
│   └── AssetManager.cpp
├── Scripting/
│   ├── ScriptEngine.cpp
│   └── ScriptBindings.cpp
├── Utilities/
│   ├── Math.cpp
│   ├── Debug.cpp
│   ├── Config.cpp
│   └── Serialization.cpp
└── Networking/
    ├── NetworkManager.cpp
    └── Sync.cpp
