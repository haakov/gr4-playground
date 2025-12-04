# gr4-playground

## Setup and build

Using mormj's `gr4-oot-env-incubator`. This defaults to the /code directory

```
git clone https://github.com/haakov/gr4-playground
cd gr4-playground
docker build -t gr4-playground .
```

## Running the backend

From within the Docker, run the following command from the project root:

```
docker run -ti -p 8080:8080 gr4-playground "/code/build/main"
```

## Running the frontend

From the frontend directory, run `python3 -m http.server`

The backend will listen on port 8080.
