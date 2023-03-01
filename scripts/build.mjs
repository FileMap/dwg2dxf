import util from 'util';
import path from 'path';

import fs from 'fs-extra';
import rimrafCallback from 'rimraf';

const BASE_PATH = process.cwd();
const LIBS_PATH = path.join(BASE_PATH, 'libs');

const rimraf = util.promisify(rimrafCallback);

async function cloneLib(origin, commit, name) {
    await fs.mkdir(LIBS_PATH, { recursive: true });
    await rimraf(LIBS_PATH);
    await fs.mkdir(LIBS_PATH, { recursive: true });

    cd(LIBS_PATH);
    await $`git clone --single-branch --filter=blob:none ${origin}`;

    cd(path.join(LIBS_PATH, name));
    await $`git reset --hard ${commit}`;
    await $`git submodule update --init --recursive`;
    await $`mkdir build`;

    cd(path.join(LIBS_PATH, name, 'build'));
    await $`cmake .. -DCMAKE_BUILD_TYPE=Release`;

    cd(BASE_PATH);
}


await cloneLib('https://github.com/LibreDWG/libredwg', '4340d0bcabc298ae1dca706040bf6998e59911c2', 'libredwg');

await $`CC=gcc yarn prebuildify --strip --napi`;
