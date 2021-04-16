import sys
import glob

SHADERS_FOLDER = sys.argv[1]
SHADERS_OUTPUT = sys.argv[2]

outfile = open(f"{SHADERS_OUTPUT}/t_shaders.h", "w")

shaders = []

for shdrfile in glob.glob(f"{SHADERS_FOLDER}/*"):
    fname = shdrfile.split("/")[-1]
    shadername, shadertype = fname.split(".")
    shf = open(shdrfile, "r")
    shader = {}
    shader["name"] = shadername
    shader["type"] = shadertype
    shader["text"] = shf.readlines()
    shf.close()
    shaders.append(shader)

    
outfile.write("#pragma once\n\n")
for shader in shaders:
    outfile.write(f"static const char* {shader['name']}_{shader['type']} = ")
    for linenum in range(len(shader['text'])):
        cLine = shader['text'][linenum].strip()
        if cLine != "":
            if linenum == len(shader['text']) - 1:
                outfile.write(f'"{cLine}\\n";\n\n')
            else:
                outfile.write(f'"{cLine}\\n"\n')
    
outfile.close()