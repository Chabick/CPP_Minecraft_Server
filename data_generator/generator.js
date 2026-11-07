/*
first generate all content from server.jar with this cmds:
"java -DbundlerMainClass="net.minecraft.data.Main -jar server.jar"
"java -DbundlerMainClass="net.minecraft.data.Main -jar server.jar --all"

after running this generator, all files are created under /generated_files
 */

const fs = require('node:fs');

fs.readFile('generated/reports/blocks.json', 'utf8', (err, data) => {
    if (err) throw err;
    let blocks = JSON.parse(data);

    let content = "";
    content += "#ifndef MCSERVER_BLOCKTYPES_H\n";
    content += "#define MCSERVER_BLOCKTYPES_H\n\n";
    content += "#include <cstdint>\n\n";
    content += "namespace MC {\n";
    content += "    enum class BlockType : uint32_t {\n";

    for (let [key, data] of Object.entries(blocks)) {
        content += "        " + key.replace("minecraft:", "") + " = ";

        for (let state of blocks[key]?.states) {
            if (state?.default) {
                content += state?.id;
                break;
            }
        }

        content += ",\n";
    }

    content += "    };\n";
    content += "};\n\n";
    content += "#endif";

    fs.writeFile('generated_files/BlockTypes.h', content, { flag: 'w+'}, err => {
        if (err) throw err;
    });

    content = "";
    content += "#ifndef MCSERVER_BLOCKSTATES_H\n";
    content += "#define MCSERVER_BLOCKSTATES_H\n\n";
    content += "#include <cstdint>\n\n";
    content += "namespace MC {\n";
    content += "    enum class BlockStates : uint32_t {\n";

    for (let [key, data] of Object.entries(blocks)) {
        for (let state of blocks[key]?.states) {
            //TODO: boolean properties arent correct
            //TODO: add a spacer between properties
            content += "        " + key.replace("minecraft:", "") +
                (state?.properties?.face ?? "") +
                (state?.properties?.facing ?? "") +
                (state?.properties?.rotation ?? "") +
                (state?.properties?.powered ?? false ? "powered" : "") +
                (state?.properties?.hinge ?? "") +
                (state?.properties?.half ?? "") +
                (state?.properties?.axis ?? "") +
                (state?.properties?.stage ?? "") +
                (state?.properties?.side_chain ?? "") +
                (state?.properties?.shape ?? "") +
                (state?.properties?.distance ?? "") +
                ((state?.properties?.open ?? false) ? "open" : "") +
                ((state?.properties?.attached ?? false) ? "attached" : "") +
                ((state?.properties?.persistent ?? false) ? "persistent" : "") +
                ((state?.properties?.waterlogged ?? false) ? "waterlogged" : "") +
                ((state?.properties?.west ?? false) ? "west" : "") +
                ((state?.properties?.south ?? false) ? "south" : "") +
                ((state?.properties?.north ?? false) ? "north" : "") +
                ((state?.properties?.east ?? false) ? "east" : "") +
                ((state?.properties?.in_wall ?? false) ? "in_wall" : "") + " = " + state?.id
            ;
            content += ",\n";
        }
    }

    content += "    };\n";
    content += "};\n\n";
    content += "#endif";

    fs.writeFile('generated_files/BlockStates.h', content, { flag: 'w+'}, err => {
        if (err) throw err;
    });

});