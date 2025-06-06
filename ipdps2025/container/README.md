# IPDPS 2025 Tutorial Docker Container

## Building
```
./build-container.sh
```

## Running Locally

```
podman run -ti --rm  <id>
-t: Open a terminal
-i: Attach to stdin
--rm: Delete the container and any anon volumes upon exit
```

## Pushing
```
podman login --username sstbuilder docker.io
podman push sstbuilder/ipdps2025
```

## Codespaces
### Editing the codespace
Go to `sst-tutorials/.devcontainer/ipdps2025/devcontainer.json`.

### Launch the container:
1. Navigate to https://github.com/sstsimulator/sst-tutorials/
2. Click `Code` dropdown
3. Click `...`
4. Click `New with options...`
5. Change `Dev container configuration` dropdown to `ipdps2025`
6. `Create codespace`
