# ISPASS 2024 Tutorial Docker Container

## Building
podman build -t sstsimulator/ispass2024 .

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
```

## Codespaces
Edit `sst-tutorials/.devcontainer/ispass2024/devcontainer.json`.

Launch the container:
1. Navigate to https://github.com/sstsimulator/sst-tutorials/
2. Click `Code` dropdown
3. Click `...`
4. Click `New with options...`
5. Change `Dev container configuration` dropdown to `ispass2024`
6. `Create codespace`
