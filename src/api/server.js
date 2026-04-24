const express = require("express");
const cors = require("cors");
const path = require("path");
const { spawn } = require("child_process");
const readline = require("readline");

const app = express();

const PORT = Number(process.env.PORT || 3000);
const SEARCH_TIMEOUT_MS = Number(process.env.SEARCH_TIMEOUT_MS || 5000);
const AUTOCOMPLETE_TIMEOUT_MS = Number(process.env.AUTOCOMPLETE_TIMEOUT_MS || 3000);
const STATS_TIMEOUT_MS = Number(process.env.STATS_TIMEOUT_MS || 3000);
const DICTIONARY_TIMEOUT_MS = Number(process.env.DICTIONARY_TIMEOUT_MS || 15000);
const DATASET_PATH = process.env.SEARCH_DATASET_PATH || "20news-18828";
const SEARCH_ENGINE_PATH = path.resolve(process.env.SEARCH_ENGINE_PATH || "./search_engine");
const PUBLIC_DIR = path.resolve(__dirname, "../../public");

app.use(cors());
app.use(express.static(PUBLIC_DIR));

class SearchEngineClient {
  constructor(options) {
    this.executablePath = options.executablePath;
    this.datasetPath = options.datasetPath;
    this.child = null;
    this.lineReader = null;
    this.ready = null;
    this.pending = [];
    this.currentRequest = null;
    this.stderrBuffer = [];
    this.isStopping = false;
  }

  async start() {
    if (this.ready) {
      return this.ready;
    }

    this.isStopping = false;
    this.ready = new Promise((resolve, reject) => {
      const child = spawn(this.executablePath, [this.datasetPath, "--api"], {
        stdio: ["pipe", "pipe", "pipe"],
        cwd: process.cwd(),
      });

      this.child = child;
      this.stderrBuffer = [];

      const handleSpawnError = (error) => {
        this.ready = null;
        reject(new Error(`Failed to start search engine: ${error.message}`));
      };

      child.once("error", handleSpawnError);

      child.once("spawn", () => {
        child.removeListener("error", handleSpawnError);
        this.attachProcessListeners(child);
        resolve();
      });
    });

    return this.ready;
  }

  attachProcessListeners(child) {
    this.lineReader = readline.createInterface({
      input: child.stdout,
      crlfDelay: Infinity,
    });

    this.lineReader.on("line", (line) => {
      this.handleStdoutLine(line);
    });

    child.stderr.on("data", (chunk) => {
      const text = chunk.toString();
      if (this.stderrBuffer.length >= 20) {
        this.stderrBuffer.shift();
      }
      this.stderrBuffer.push(text.trim());
    });

    child.on("exit", (code, signal) => {
      const errorMessage = this.buildExitMessage(code, signal);
      this.failCurrentAndQueued(new Error(errorMessage));
      this.cleanupChild();

      if (!this.isStopping) {
        this.ready = null;
      }
    });
  }

  buildExitMessage(code, signal) {
    const stderrText = this.stderrBuffer.filter(Boolean).join(" | ");
    const baseMessage = `Search engine exited unexpectedly (code=${code}, signal=${signal || "none"})`;
    return stderrText ? `${baseMessage}: ${stderrText}` : baseMessage;
  }

  cleanupChild() {
    if (this.lineReader) {
      this.lineReader.close();
      this.lineReader = null;
    }
    this.child = null;
  }

  failCurrentAndQueued(error) {
    if (this.currentRequest) {
      clearTimeout(this.currentRequest.timer);
      this.currentRequest.reject(error);
      this.currentRequest = null;
    }

    while (this.pending.length > 0) {
      const request = this.pending.shift();
      clearTimeout(request.timer);
      request.reject(error);
    }
  }

  async stop() {
    this.isStopping = true;

    if (!this.child) {
      this.ready = null;
      return;
    }

    try {
      this.child.stdin.write("exit\n");
    } catch (error) {
      // Ignore write errors during shutdown.
    }

    this.child.kill();
    this.cleanupChild();
    this.ready = null;
  }

  async sendCommand(command, timeoutMs) {
    await this.start();

    return new Promise((resolve, reject) => {
      const request = {
        command,
        resolve,
        reject,
        timer: setTimeout(() => {
          if (this.currentRequest === request) {
            this.currentRequest = null;
          } else {
            this.pending = this.pending.filter((item) => item !== request);
          }

          reject(new Error(`Timed out waiting for search engine response to "${command}"`));
        }, timeoutMs),
      };

      this.pending.push(request);
      this.flushQueue();
    });
  }

  flushQueue() {
    if (this.currentRequest || this.pending.length === 0 || !this.child) {
      return;
    }

    this.currentRequest = this.pending.shift();

    try {
      this.child.stdin.write(`${this.currentRequest.command}\n`);
    } catch (error) {
      clearTimeout(this.currentRequest.timer);
      this.currentRequest.reject(new Error(`Failed to write to search engine stdin: ${error.message}`));
      this.currentRequest = null;
      this.ready = null;
    }
  }

  handleStdoutLine(line) {
    if (!this.currentRequest) {
      return;
    }

    const request = this.currentRequest;
    this.currentRequest = null;
    clearTimeout(request.timer);

    try {
      const parsed = JSON.parse(line);
      request.resolve(parsed);
    } catch (error) {
      request.reject(new Error(`Invalid JSON from search engine: ${line}`));
    }

    this.flushQueue();
  }
}

const searchClient = new SearchEngineClient({
  executablePath: SEARCH_ENGINE_PATH,
  datasetPath: DATASET_PATH,
});

function getRequiredQueryParam(req, res, key) {
  const value = typeof req.query[key] === "string" ? req.query[key].trim() : "";
  if (!value) {
    res.status(400).json({ error: `Missing required query parameter: ${key}` });
    return null;
  }
  return value;
}

function getPositiveInteger(value, fallback) {
  const parsed = Number.parseInt(String(value ?? ""), 10);
  if (Number.isNaN(parsed) || parsed <= 0) {
    return fallback;
  }
  return parsed;
}

app.get("/api/search", async (req, res) => {
  const query = getRequiredQueryParam(req, res, "q");
  if (!query) {
    return;
  }

  try {
    const result = await searchClient.sendCommand(`search ${query}`, SEARCH_TIMEOUT_MS);
    res.json(result);
  } catch (error) {
    res.status(504).json({ error: error.message });
  }
});

app.get("/api/autocomplete", async (req, res) => {
  const prefix = getRequiredQueryParam(req, res, "prefix");
  if (!prefix) {
    return;
  }

  try {
    const result = await searchClient.sendCommand(`autocomplete ${prefix}`, AUTOCOMPLETE_TIMEOUT_MS);
    res.json(result);
  } catch (error) {
    res.status(504).json({ error: error.message });
  }
});

app.get("/api/stats", async (_req, res) => {
  try {
    const result = await searchClient.sendCommand("stats", STATS_TIMEOUT_MS);
    res.json(result);
  } catch (error) {
    res.status(504).json({ error: error.message });
  }
});

app.get("/api/dictionary", async (req, res) => {
  const page = getPositiveInteger(req.query.page, 1);
  const limit = getPositiveInteger(req.query.limit, 100);

  try {
    const result = await searchClient.sendCommand("dictionary", DICTIONARY_TIMEOUT_MS);

    if (!Array.isArray(result)) {
      res.status(502).json({ error: "Invalid dictionary response from search engine." });
      return;
    }

    const totalItems = result.length;
    const totalPages = totalItems === 0 ? 0 : Math.ceil(totalItems / limit);
    const safePage = totalPages === 0 ? 1 : Math.min(page, totalPages);
    const startIndex = (safePage - 1) * limit;
    const endIndex = startIndex + limit;
    const items = result.slice(startIndex, endIndex);

    res.json({
      page: safePage,
      limit,
      total_items: totalItems,
      total_pages: totalPages,
      items,
    });
  } catch (error) {
    res.status(504).json({ error: error.message });
  }
});

app.get("/health", async (_req, res) => {
  try {
    await searchClient.start();
    res.json({ ok: true });
  } catch (error) {
    res.status(500).json({ ok: false, error: error.message });
  }
});

app.get("*", (_req, res) => {
  res.sendFile(path.join(PUBLIC_DIR, "index.html"));
});

const server = app.listen(PORT, async () => {
  try {
    await searchClient.start();
    console.log(`Search API listening on http://localhost:${PORT}`);
  } catch (error) {
    console.error(error.message);
    process.exit(1);
  }
});

async function shutdown() {
  server.close(async () => {
    await searchClient.stop();
    process.exit(0);
  });
}

process.on("SIGINT", shutdown);
process.on("SIGTERM", shutdown);
