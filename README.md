# gr4-playground

## Setup and build

Using mormj's `gr4-oot-env-incubator`. This defaults to the /code directory

```
git clone https://github.com/haakov/gr4-playground
cd gr4-playground
docker run -it -v `pwd`:/code ghcr.io/mormj/gr4-oot-env-incubator:latest bash
```

Run the next commands from within the container to build the GR4 GUI backend:
```
mkdir backend/build
cd backend/build
cmake ..
make -j2
```

If you get the following error: `c++: fatal error: Killed signal terminated program cc1plus`, you have probably run out of memory. Try increasing the RAM and swap size of your Docker container, and reduce the thread count of your `make` command. `-j4` is too much for my 16 GB Mac Mini M4, but `-j2` works. Note: The `make` step will probably take at least 30 minutes, maybe more than an hour.

## Running the backend

From within the Docker, run the following command from the project root:

```
docker run -ti -p 8080:8080 gr4-playground "/code/build/main"
```

## Running the frontend

From the frontend directory, run python3 -m http.server

The backend will listen on port 8080.
