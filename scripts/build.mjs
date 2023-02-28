import util from 'util';
import path from 'path';

import fs from 'fs-extra';
import rimrafCallback from 'rimraf';

const BASE_PATH = process.cwd();

const rimraf = util.promisify(rimrafCallback);

async function cloneLib(origin, commit, name) {
    const targetPath = path.join(BASE_PATH, 'libs', name);

    await fs.mkdir(targetPath, { recursive: true });
    await rimraf(targetPath);
    await fs.mkdir(targetPath, { recursive: true });

    cd(targetPath);
    await $`git init`;
    await $`git remote add origin ${origin}`;
    await $`git fetch origin --depth=1 ${commit}`;
    await $`git reset --hard FETCH_HEAD`;
    await $`mkdir build`;
    cd(targetPath+'/build');
    await $`cmake ..  -DCMAKE_BUILD_TYPE=Release`;
    

    cd(BASE_PATH);
}


await cloneLib('https://github.com/LibreDWG/libredwg', '4340d0bcabc298ae1dca706040bf6998e59911c2', 'libredwg');

await $`yarn prebuildify --strip --napi`;
