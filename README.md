# SearchiQ

A full-stack document search engine built around a native C++ core, wrapped with a Node.js API, and presented through a polished vanilla frontend.

The project now has three layers working together:

- `src/cpp/`: the original search engine, TF-IDF ranking, autocomplete, and CLI/API mode
- `src/api/`: an Express server that keeps the compiled C++ process alive and exposes REST endpoints
- `public/`: a responsive browser UI with autocomplete, search results, and a developer dashboard

## Highlights

- Native C++ inverted index and TF-IDF ranking
- `--api` mode for machine-friendly JSON output
- Express wrapper with child process management, timeouts, and CORS
- Responsive frontend with autocomplete, results view, and stats modal
- Clean repo structure with separated native, API, and UI concerns

## Project Structure

```text
.
├── public/
│   ├── app.js
│   ├── index.html
│   └── styles.css
├── src/
│   ├── api/
│   │   └── server.js
│   └── cpp/
│       ├── InvertedIndex.cpp
│       ├── InvertedIndex.h
│       ├── PerformanceTracker.cpp
│       ├── PerformanceTracker.h
│       ├── Preprocessor.cpp
│       ├── Preprocessor.h
│       ├── SearchEngine.cpp
│       ├── SearchEngine.h
│       ├── TFIDFCalculator.cpp
│       ├── TFIDFCalculator.h
│       └── main.cpp
├── compile.sh
├── download_dataset.sh
├── package.json
└── README.md
```

## Architecture

### 1. Native search core

The C++ engine:

- loads documents recursively from a dataset directory
- preprocesses text
- builds an inverted index
- ranks matches with TF-IDF
- supports `search`, `autocomplete`, `suggest`, `stats`, and `performance`

When launched with `--api`, it reads commands from `stdin` and returns minified JSON on `stdout`.

### 2. Backend wrapper

The Express server:

- starts `./search_engine <dataset> --api`
- sends commands through `stdin`
- reads one JSON line per response from `stdout`
- serializes concurrent requests through a single child process
- exposes REST endpoints for the frontend

### 3. Frontend

The frontend:

- shows a search-first landing page
- requests autocomplete suggestions with debounce
- renders search results dynamically
- includes loading, empty, and error states
- includes a Developer Dashboard modal for index stats and latest search timing

## API Endpoints

The Node server exposes:

- `GET /api/search?q=your query`
- `GET /api/autocomplete?prefix=pre`
- `GET /api/stats`
- `GET /health`

## Quick Start

### 1. Download the dataset

```bash
./download_dataset.sh
```

This downloads and extracts the `20news-18828` dataset into the project root.

### 2. Compile the C++ engine

```bash
./compile.sh
```

### 3. Install Node dependencies

```bash
npm install
```

### 4. Start the full app

```bash
npm start
```

Then open [http://localhost:3000](http://localhost:3000).

## Development Commands

Build the native binary:

```bash
npm run build:native
```

Start the API and frontend host:

```bash
npm run dev
```

Run the C++ CLI directly:

```bash
./search_engine ./20news-18828
```

Run the C++ engine in API mode:

```bash
./search_engine ./20news-18828 --api
```

## Environment Variables

Optional backend configuration:

```bash
PORT=3000
SEARCH_DATASET_PATH=20news-18828
SEARCH_ENGINE_PATH=./search_engine
SEARCH_TIMEOUT_MS=5000
AUTOCOMPLETE_TIMEOUT_MS=3000
STATS_TIMEOUT_MS=3000
```

## Notes

- The Developer Dashboard reads document, term, and posting counts from `GET /api/stats`.
- The displayed search execution time is based on the measured frontend request duration for the latest search.
- The repo currently ignores generated binaries, object files, datasets, and `node_modules` via `.gitignore`.

## UI Direction

The frontend theme uses:

- warm editorial backgrounds instead of flat white
- glassmorphism-inspired search surfaces
- a distinctive `SearchiQ` wordmark
- soft grid/orb background layers
- a compact developer dashboard for observability

## Next Ideas

- return native query timing directly from the C++ API for stricter measurement
- add pagination and result counts by document path
- add search history and saved queries
- package the frontend as a SPA build if you want a larger product shell
