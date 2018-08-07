import { spawn } from 'child_process';
import { shell } from 'electron';
import readline from 'readline';

const DONE_READING = Symbol();

export const WEBSITE_URL = 'http://35.241.33.112';
export const API_SERVER_URL = 'http://35.190.92.101/v1';

export async function* call(args) {
    const fullArgs = ['-m', 'hlt_client', '--json'].concat(args);
    const subprocess = spawn('python', fullArgs);
    const rl = readline.createInterface({
        input: subprocess.stdout,
        crlfDelay: Infinity,
    });

    let iterating = true;
    let currentResolve = null;
    let currentReject = null;
    const makePromise = () => new Promise((resolve, reject) => {
        currentResolve = resolve;
        currentReject = reject;
    });
    let currentPromise = makePromise();

    rl.on('line', (line) => {
        const result = JSON.parse(line);
        currentResolve(result);
        currentPromise = makePromise();
    });
    rl.on('close', () => {
        iterating = false;
        currentReject(DONE_READING);
    });

    while (iterating) {
        try {
            const value = await currentPromise;
            yield value;
        }
        catch (e) {
            if (e !== DONE_READING) {
                console.error(e);
            }
            return;
        }
    }
}

export function openBrowserTab(url) {
    shell.openExternal(url);
}

export function fetchApi(apiKey, endpoint, options) {
    if (!options) {
        options = {};
    }
    // Don't mutate options given to us
    options = Object.assign({}, options);
    if (!options.headers) {
        options.headers = {};
    }
    options.headers = Object.assign({}, options.headers, {
        'X-Api-Key': apiKey,
    });
    return window.fetch(`${API_SERVER_URL}/${endpoint}`, options);
}
