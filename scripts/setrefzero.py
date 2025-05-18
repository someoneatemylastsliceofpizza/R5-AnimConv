file = 'anims/ref.smd'
print(file)

with open(file, 'r') as f:
    content = f.read()
    
def process_smd(input_smd):
    with open(input_smd, 'r') as f:
        lines = f.readlines()

    output_lines = []

    in_nodes = False
    in_skeleton = False
    nodes_raw = []
    skeleton_raw = []

    for line in lines:
        stripped = line.strip()
        if stripped == 'nodes':
            in_nodes = True
            output_lines.append(stripped)
        elif stripped == 'end' and in_nodes:
            in_nodes = False
            output_lines.append('end\n')
        elif in_nodes:
            nodes_raw.append(stripped)

        elif stripped == 'skeleton':
            in_skeleton = True
            output_lines.append(stripped)
        elif stripped == 'end' and in_skeleton:
            in_skeleton = False
            output_lines.append('end\n')
        elif in_skeleton:
            skeleton_raw.append(stripped)
        elif not in_nodes and not in_skeleton:
            output_lines.append(stripped)
            
    return [nodes_raw, skeleton_raw]

rawref = process_smd('anims/ref.smd')

hdr = '\n'.join(rawref[0])
anim = rawref[1][1:]

for bone in range(len(anim)):
    if bone not in [ 64, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 158, 159, 160, 161, 162, 163, 164, 166, 167, 168, 169, 170, 177, 178, 179, 180, 181, 182, 183, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197]: # custom/weapon bones
        rawv = anim[bone].split()[1:]
        calcrot = ' '.join("0.000000" for i in range(3))
        anim[bone] = f"{bone}  {' '.join(rawv[:3])}  {calcrot}"

with open('anims/ref.smd', 'w') as out:
    out.write("version 1\n")
    out.write("nodes\n")
    out.write(hdr)
    out.write("\nend\n")
    out.write("skeleton\n")
    out.write("time 0\n")
    out.write("\n".join(j for j in anim))
    out.write("\nend\n")
