# gr4-playground

## Setup and build

Using mormj's `gr4-oot-env-incubator`. This defaults to the /code directory

```
git clone https://github.com/haakov/gr4-playground
cd gr4-playground
docker run -it -v `pwd`:/code ghcr.io/mormj/gr4-oot-env-incubator:latest bash
```

Run the next commands from within the container:
```
mkdir build
cd build
cmake ..
make -j2
```


