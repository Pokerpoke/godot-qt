# Embedding godot in qt

## Build godot first

- windows

```powershell
./script/build_godot.ps1
```

## Build godot-qt

```shell
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=<your qt path> -DCMAKE_BUILD_TYPE=Debug -G "Ninja"
ninja
```